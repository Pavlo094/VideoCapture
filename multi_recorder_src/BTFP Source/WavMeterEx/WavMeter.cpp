#include <streams.h>
#include "wavmeter.h"
#include <malloc.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string.h>

using namespace std;

// AMOVIESETUP_FILTER
const AMOVIESETUP_FILTER sudWavMeter =
{
    &CLSID_WavMeterEx,		  // GUID of the filter.
    L"Wav Meter Ex",			  // Name of the filter
    MERIT_DO_NOT_USE,         // dwMerit
    0,                        // nPins
    0                         // lpPin
};

// Global data
CFactoryTemplate g_Templates[]= {
	{L"Wav Meter", &CLSID_WavMeterEx, CWavMeterExFilter::CreateInstance, NULL, &sudWavMeter},
};

// g_cTemplates
int g_cTemplates = sizeof(g_Templates) / sizeof(g_Templates[0]);

// disable warning
#pragma warning(disable:4355)

//
// Filter constructor
CWavMeterExFilter::CWavMeterExFilter(LPUNKNOWN pUnk, HRESULT *phr):
	CTransformFilter(NAME("Wav Meter Filter"), pUnk, CLSID_WavMeterEx)
{
	// Set default 2 channels. We'll pick up the real channels later, when connecting.
	m_channels = 2;
	// Set the peaks to a default minimum value and volume to 1.0
	for(int i=0; i < PEAKS; i++)
	{
		m_Peaks[i] = PEAK_VALUE;
		m_Volumes[i] = 1.0;
		m_Mutes[i] = FALSE;
	}

	*phr = S_OK;
}
//
// Destructor
CWavMeterExFilter::~CWavMeterExFilter()
{
}
//
// Create instance used by COM to create instance of us
CUnknown * WINAPI CWavMeterExFilter::CreateInstance(LPUNKNOWN pUnk, HRESULT * phr)
{
	// Create new instance
	return new CWavMeterExFilter(pUnk, phr);
}
//
// NonDelegatingQueryInterface
//
// We expose IWavMeter interface here
STDMETHODIMP CWavMeterExFilter::NonDelegatingQueryInterface(REFIID riid, void **ppv)
{
	CheckPointer(ppv, E_POINTER);

	// Check if it is looking for our interface
	if(riid == IID_IWavMeterEx)
	{
		// Get the interface
		return GetInterface((IWavMeterEx*)this, ppv);
	}
	else
	{
		// Forward it to our base class
		return CTransformFilter::NonDelegatingQueryInterface(riid, ppv);
	}
}
HRESULT CWavMeterExFilter::DecideBufferSize(IMemAllocator *pAlloc, ALLOCATOR_PROPERTIES *pProperties)
{
	HRESULT hr = NOERROR;

    // Is the input pin connected
    if(m_pInput->IsConnected() == FALSE)
    {
        return E_UNEXPECTED;
    }

    CheckPointer(pAlloc,E_POINTER);
    CheckPointer(pProperties,E_POINTER);

    pProperties->cBuffers = 1;
    pProperties->cbAlign  = 1;

    // Get input pin's allocator size and use that
    ALLOCATOR_PROPERTIES InProps;
    IMemAllocator * pInAlloc = NULL;

    hr = m_pInput->GetAllocator(&pInAlloc);
    if(SUCCEEDED(hr))
    {
        hr = pInAlloc->GetProperties(&InProps);
        if(SUCCEEDED(hr))
        {
            pProperties->cbBuffer = InProps.cbBuffer;
        }
        pInAlloc->Release();
    }

    if(FAILED(hr))
        return hr;

    ASSERT(pProperties->cbBuffer);

    // Ask the allocator to reserve us some sample memory, NOTE the function
    // can succeed (that is return NOERROR) but still not have allocated the
    // memory that we requested, so we must check we got whatever we wanted

    ALLOCATOR_PROPERTIES Actual;
    hr = pAlloc->SetProperties(pProperties,&Actual);
    if(FAILED(hr))
    {
        return hr;
    }

    ASSERT(Actual.cBuffers == 1);

    if(pProperties->cBuffers > Actual.cBuffers ||
        pProperties->cbBuffer > Actual.cbBuffer)
    {
        return E_FAIL;
    }

    return NOERROR;
}
//
// The main method where the actual samples transform is done
HRESULT CWavMeterExFilter::Transform(IMediaSample *pIn, IMediaSample *pOut)
{
	// Check the pointer
	CheckPointer(pIn, E_POINTER);

	// Copy buffers
	Copy(pIn, pOut);

	// Define buffer pointer
	BYTE* pBuffer = NULL;
	// Get the actual buffer length
	long lBufferLength = pOut->GetActualDataLength();
	// Get the buffer pointer
	pOut->GetPointer(&pBuffer);

	// Now calculate peaks and set volume
	CalculatePeaks(pBuffer, lBufferLength);

	return NOERROR;
}
//
// Copy
//
// Make destination an identical copy of source
//
HRESULT CWavMeterExFilter::Copy(IMediaSample *pSource, IMediaSample *pDest) const
{
    CheckPointer(pSource,E_POINTER);   
    CheckPointer(pDest,E_POINTER);   

    // Copy the sample data

    BYTE *pSourceBuffer, *pDestBuffer;
    long lSourceSize = m_pInput->CurrentMediaType().GetSampleSize();//pSource->GetActualDataLength();

#ifdef DEBUG
    long lDestSize = m_pOutput->CurrentMediaType().GetSampleSize();//pDest->GetSize();
    ASSERT(lDestSize >= lSourceSize);
#endif

	 long lDataLength = pSource->GetActualDataLength();

    pSource->GetPointer(&pSourceBuffer);
    pDest->GetPointer(&pDestBuffer);

    CopyMemory( (PVOID) pDestBuffer,(PVOID) pSourceBuffer,lDataLength);

    // Copy the sample times

    REFERENCE_TIME TimeStart, TimeEnd;
    if (NOERROR == pSource->GetTime(&TimeStart, &TimeEnd)) {
        pDest->SetTime(&TimeStart, &TimeEnd);
    }

    LONGLONG MediaStart, MediaEnd;
    if (pSource->GetMediaTime(&MediaStart,&MediaEnd) == NOERROR) {
        pDest->SetMediaTime(&MediaStart,&MediaEnd);
    }

    // Copy the Sync point property

    HRESULT hr = pSource->IsSyncPoint();
    if (hr == S_OK) {
        pDest->SetSyncPoint(TRUE);
    }
    else if (hr == S_FALSE) {
        pDest->SetSyncPoint(FALSE);
    }
    else {  // an unexpected error has occured...
        return E_UNEXPECTED;
    }

    // Copy the media type

    AM_MEDIA_TYPE *pMediaType;
    pSource->GetMediaType(&pMediaType);
    pDest->SetMediaType(pMediaType);
    DeleteMediaType(pMediaType);

    // Copy the preroll property

    hr = pSource->IsPreroll();
    if (hr == S_OK) {
        pDest->SetPreroll(TRUE);
    }
    else if (hr == S_FALSE) {
        pDest->SetPreroll(FALSE);
    }
    else {  // an unexpected error has occured...
        return E_UNEXPECTED;
    }

    // Copy the discontinuity property

    hr = pSource->IsDiscontinuity();
    if (hr == S_OK) {
    pDest->SetDiscontinuity(TRUE);
    }
    else if (hr == S_FALSE) {
        pDest->SetDiscontinuity(FALSE);
    }
    else {  // an unexpected error has occured...
        return E_UNEXPECTED;
    }

    // Copy the actual data length

   
    pDest->SetActualDataLength(lDataLength);

    return NOERROR;

} // Copy
//
// In check i9nput type we tell DirectShow what media types we are accepting
HRESULT CWavMeterExFilter::CheckInputType(const CMediaType* pMediaType)
{
	// Check, just in case
	//
	CheckPointer(pMediaType, E_POINTER);

	// Major type must be audio
	//
	if(pMediaType->majortype != MEDIATYPE_Audio)
	{
		return VFW_E_TYPE_NOT_ACCEPTED;
	}

	// Invalid format block we don't want, too
	//
	if(pMediaType->formattype != FORMAT_WaveFormatEx)
	{
		return VFW_E_TYPE_NOT_ACCEPTED;
	}

	// Get wave format.
	//
	WAVEFORMATEX *pwfx = (WAVEFORMATEX*)pMediaType->pbFormat;

	// Reject compressed audio
	//
	if(pwfx->wFormatTag != WAVE_FORMAT_PCM &&
		pwfx->wFormatTag != WAVE_FORMAT_EXTENSIBLE)
	{
		return VFW_E_TYPE_NOT_ACCEPTED;
	}

	// We accept only 16 bit audio
	//
	if(pwfx->wBitsPerSample != 16)
	{
		return VFW_E_TYPE_NOT_ACCEPTED;
	}

	// Only 48000Hz
	if(pwfx->nSamplesPerSec != 48000)
	{
		return VFW_E_TYPE_NOT_ACCEPTED;
	}

	// Accept only 2,3, or 6 channels
	/*if(!(pwfx->nChannels == 1 ||
		pwfx->nChannels == 2 ||
		pwfx->nChannels == 6))
	{
		return VFW_E_TYPE_NOT_ACCEPTED;
	}*/

	// Keep the number of channels
	m_channels = pwfx->nChannels;

	// Everything ok.
	return S_OK;
}
//
// We only support transform if input and output formats are identical
HRESULT CWavMeterExFilter::CheckTransform(const CMediaType *mtIn, const CMediaType *mtOut)
{
	return CheckInputType(mtIn);
}
//
// This is called on the output pin. The media type we support on the output pin
// is exactly the same as in the input pin.
HRESULT CWavMeterExFilter::GetMediaType(int iPosition, CMediaType *pMediaType)
{
	// This method should only be called if the input pin is connected
	ASSERT(m_pInput->IsConnected());

	// Only one output media type we support, which is the input type.
	if(iPosition < 0)
	{
		// Invalid index
		return E_INVALIDARG;
	}
	else if(iPosition == 0)
	{
		// The input connection media type
		return m_pInput->ConnectionMediaType(pMediaType);
	}

	// We offer no more media types
	return VFW_S_NO_MORE_ITEMS;
}
//
// This is where we actually calculate peaks and set volume
HRESULT CWavMeterExFilter::CalculatePeaks(PBYTE pb, long cb)
{
	// Get the number of samples(only 16 bit audio)
	long samples = cb/2;

	// Get the short amplitude of the buffer
	short *pBuffer = (short *)pb;
	// define peaks
	short Peaks[PEAKS];
	// set default 0
	for(int i = 0;i < PEAKS;i++)
		Peaks[i]=0;
	// set count to 0
	int counter = 0;

	double volumes[PEAKS];
	BOOL mutes[PEAKS];
	// In case client is changing volume in this instance - lock
	m_VolumeCritSec.Lock();
	for(int i=0; i < PEAKS; i++)
	{
		volumes[i] = m_Volumes[i];
		mutes[i] = m_Mutes[i];
	}
	m_VolumeCritSec.Unlock();

	long calculatePeaksOn = samples/m_channels;
	while(samples--)
	{
		// check which channel
		short channel = (short)counter%m_channels;

		// Set the volume increase on the specified channel
		*pBuffer = (short)((*pBuffer) * volumes[channel]);

		// Now get the peaks
		short value = (*pBuffer);
		

		// Check if it is bigger then the last peak value
		if(Peaks[channel] < abs(value))
			Peaks[channel] = (short)abs(value);

		// Should we mute this channel.
		if(mutes[channel] == TRUE)
		{
			*pBuffer = 0;
		}

		// increment counter
		counter++;
		// Get the next buffer
		pBuffer++;
	}

	CalculatePeaksDb(Peaks);

	//// Lock the critical section here. The client might be reading m_Peaks or setting m_Volumes from different thread.
	//m_CritSec.Lock();

	//// Calculate the dB value.
	//for(int i=0; i < m_channels; i++)
	//{
	//	// Check if zero
	//	if(m_Peaks[i] == 0)
	//	{
	//		m_Peaks[i] = PEAK_VALUE;
	//	}
	//	else
	//	{
	//		// dB scale.
	//		m_Peaks[i] = 20 * log10((double)Peaks[i]/32768);
	//	}
	//}

	//// Unlock the critical section.
	//m_CritSec.Unlock();

	return S_OK;
}
void CWavMeterExFilter::CalculatePeaksDb(const short* Peaks)
{
	// Lock the critical section here. The client might be reading m_Peaks or setting m_Volumes from different thread.
	m_CritSec.Lock();

	// Calculate the dB value.
	for(int i=0; i < m_channels; i++)
	{
		// Check if zero
		if(m_Peaks[i] == 0)
		{
			m_Peaks[i] = PEAK_VALUE;
		}
		else
		{
			// dB scale.
			m_Peaks[i] = 20 * log10((double)Peaks[i]/32768);
		}
	}

	// Unlock the critical section.
	m_CritSec.Unlock();
}
//
// Get last peaks of the audio stream
STDMETHODIMP CWavMeterExFilter::GetPeaks(double Peaks[])
{
	// Lock the critical section
	m_CritSec.Lock();
	// Set values for the cient
	for(int i = 0; i < PEAKS; i++)
	{
		Peaks[i] = m_Peaks[i];
	}
	m_CritSec.Unlock();

	return S_OK;
}
//
// Set volume for each channel
STDMETHODIMP CWavMeterExFilter::SetVolume(double Volumes[])
{
	// Lock the volume critical section
	m_VolumeCritSec.Lock();
	// Set volume values
	for(int i=0; i < PEAKS; i++)
	{
		m_Volumes[i] = Volumes[i];
	}
	m_VolumeCritSec.Unlock();

	return S_OK;
}
//
// Set mute for each channel
STDMETHODIMP CWavMeterExFilter::SetMute(BOOL Mutes[])
{
	// Lock the volume critical section
	m_VolumeCritSec.Lock();
	// Set volume values
	for(int i=0; i < PEAKS; i++)
	{
		m_Mutes[i] = Mutes[i];
	}
	m_VolumeCritSec.Unlock();

	return S_OK;
}

////////////////////////////////////////////////////////////////////////
//
// Exported entry points for registration and unregistration 
// (in this case they only call through to default implementations).
//
////////////////////////////////////////////////////////////////////////

STDAPI DllRegisterServer()
{
	return AMovieDllRegisterServer2(TRUE);
}

STDAPI DllUnregisterServer()
{
	return AMovieDllRegisterServer2(FALSE);
}

//
// DllEntryPoint
//
extern "C" BOOL WINAPI DllEntryPoint(HINSTANCE, ULONG, LPVOID);

BOOL APIENTRY DllMain(HANDLE hModule, 
                      DWORD  dwReason, 
                      LPVOID lpReserved)
{
	return DllEntryPoint((HINSTANCE)(hModule), dwReason, lpReserved);
}