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
    &CLSID_WavMeter,		  // GUID of the filter.
    L"Wav Meter",			  // Name of the filter
    MERIT_DO_NOT_USE,         // dwMerit
    0,                        // nPins
    0                         // lpPin
};

// Global data
CFactoryTemplate g_Templates[]= {
	{L"Wav Meter", &CLSID_WavMeter, CWavMeterFilter::CreateInstance, NULL, &sudWavMeter},
};

// g_cTemplates
int g_cTemplates = sizeof(g_Templates) / sizeof(g_Templates[0]);

// disable warning
#pragma warning(disable:4355)

//
// Filter constructor
CWavMeterFilter::CWavMeterFilter(LPUNKNOWN pUnk, HRESULT *phr):
	CTransInPlaceFilter(NAME("Wav Meter Filter"), pUnk, CLSID_WavMeter, phr, false)
{
	// Set default 2 channels. We'll pick up the real channels later, when connecting.
	m_channels = 2;
	// Set the peaks to a default minimum value and volume to 1.0
	for(int i=0; i < PEAKS; i++)
	{
		m_Peaks[i] = PEAK_VALUE;
		m_Volumes[i] = 1.0;
	}
}
//
// Destructor
CWavMeterFilter::~CWavMeterFilter()
{
}
//
// Create instance used by COM to create instance of us
CUnknown * WINAPI CWavMeterFilter::CreateInstance(LPUNKNOWN pUnk, HRESULT * phr)
{
	// Create new instance
	return new CWavMeterFilter(pUnk, phr);
}
//
// NonDelegatingQueryInterface
//
// We expose IWavMeter interface here
STDMETHODIMP CWavMeterFilter::NonDelegatingQueryInterface(REFIID riid, void **ppv)
{
	CheckPointer(ppv, E_POINTER);

	// Check if it is looking for our interface
	if(riid == IID_IWavMeter)
	{
		// Get the interface
		return GetInterface((IWavMeter*)this, ppv);
	}
	else
	{
		// Forward it to our base class
		return CTransformFilter::NonDelegatingQueryInterface(riid, ppv);
	}
}
//
// The main method where the actual samples transform is done
HRESULT CWavMeterFilter::Transform(IMediaSample *pIn)
{
	// Check the pointer
	CheckPointer(pIn, E_POINTER);

	// Define buffer pointer
	BYTE* pBuffer = NULL;
	// Get the actual buffer length
	long lBufferLength = pIn->GetActualDataLength();
	// Get the buffer pointer
	pIn->GetPointer(&pBuffer);

	// Now calculate peaks and set volume
	CalculatePeaks(pBuffer, lBufferLength);

	return NOERROR;
}
//
// In check i9nput type we tell DirectShow what media types we are accepting
HRESULT CWavMeterFilter::CheckInputType(const CMediaType* pMediaType)
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
	if(!(pwfx->nChannels == 1 ||
		pwfx->nChannels == 2 ||
		pwfx->nChannels == 6))
	{
		return VFW_E_TYPE_NOT_ACCEPTED;
	}

	// Keep the number of channels
	m_channels = pwfx->nChannels;

	// Everything ok.
	return S_OK;
}
//
// We only support transform if input and output formats are identical
HRESULT CWavMeterFilter::CheckTransform(const CMediaType *mtIn, const CMediaType *mtOut)
{
	return CheckInputType(mtIn);
}
//
// This is called on the output pin. The media type we support on the output pin
// is exactly the same as in the input pin.
HRESULT CWavMeterFilter::GetMediaType(int iPosition, CMediaType *pMediaType)
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
HRESULT CWavMeterFilter::CalculatePeaks(PBYTE pb, long cb)
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
	// In case client is changing volume in this instance - lock
	m_VolumeCritSec.Lock();
	for(int i=0; i < PEAKS; i++)
	{
		volumes[i] = m_Volumes[i];
	}
	m_VolumeCritSec.Unlock();

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

		// increment counter
		counter++;
		// Get the next buffer
		pBuffer++;
	}

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

	return S_OK;
}
//
// Get last peaks of the audio stream
STDMETHODIMP CWavMeterFilter::GetPeaks(double Peaks[])
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
STDMETHODIMP CWavMeterFilter::SetVolume(double Volumes[])
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