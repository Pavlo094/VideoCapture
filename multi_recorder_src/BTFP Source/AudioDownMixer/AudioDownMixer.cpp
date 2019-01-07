#include <streams.h>
#include "AudioDownMixer.h"


// AMOVIESETUP_FILTER
const AMOVIESETUP_FILTER sudAudioDownMixer =
{
    &CLSID_AudioDownMixer,		  // GUID of the filter.
    L"Audio DownMixer",			  // Name of the filter
    MERIT_DO_NOT_USE,         // dwMerit
    0,                        // nPins
    0                         // lpPin
};

// Global data
CFactoryTemplate g_Templates[]= {
	{L"Audio DownMixer", &CLSID_AudioDownMixer, CAudioDownMixer::CreateInstance, NULL, &sudAudioDownMixer},
};

// g_cTemplates
int g_cTemplates = sizeof(g_Templates) / sizeof(g_Templates[0]);

// disable warning
#pragma warning(disable:4355)

//
// Filter constructor
CAudioDownMixer::CAudioDownMixer(LPUNKNOWN pUnk, HRESULT *phr):
	CTransformFilter(NAME("Audio DownMixer"), pUnk, CLSID_AudioDownMixer),
	m_Channels(2),
	m_SampleRate(48000),
	m_BitsPerSample(16)
{
	*phr = S_OK;
}
//
// Destructor
CAudioDownMixer::~CAudioDownMixer()
{
}
//
// Create instance used by COM to create instance of us
CUnknown * WINAPI CAudioDownMixer::CreateInstance(LPUNKNOWN pUnk, HRESULT * phr)
{
	// Create new instance
	return new CAudioDownMixer(pUnk, phr);
}
HRESULT CAudioDownMixer::DecideBufferSize(IMemAllocator *pAlloc, ALLOCATOR_PROPERTIES *pProperties)
{
	// Is the input pin connected
    if (m_pInput->IsConnected() == FALSE) {
        return E_UNEXPECTED;
    }

    ASSERT(pAlloc);
    ASSERT(pProperties);
    HRESULT hr = NOERROR;

	#define NUMBER_OF_BUFFERS 20

    pProperties->cBuffers = NUMBER_OF_BUFFERS;
	//pProperties->cbAlign = 1;

	//But we need input pin's allocator buffer size and the input type 
	//to determine the buffer size required for us

	CMediaType input_mt;
	hr = m_pInput->ConnectionMediaType(&input_mt); if (FAILED(hr)) return hr;
	
	DWORD DestByteSizeOfSamples = m_BitsPerSample / 8;
	DWORD DestNumberOfChannels = m_Channels;
	DWORD SourceByteSizeOfSamples = ((WAVEFORMATEX*)input_mt.Format())->wBitsPerSample / 8;
	DWORD SourceNumberOfChannels = ((WAVEFORMATEX*)input_mt.Format())->nChannels;

    ALLOCATOR_PROPERTIES InProps;
    IMemAllocator * pInAlloc = NULL;
    hr = m_pInput->GetAllocator(&pInAlloc);
    if (SUCCEEDED (hr))
    {
        hr = pInAlloc->GetProperties (&InProps);
        if (SUCCEEDED (hr))
        {
            pProperties->cbBuffer = ((InProps.cbBuffer * DestByteSizeOfSamples * DestNumberOfChannels) / SourceByteSizeOfSamples) / SourceNumberOfChannels + 800 ; //+200 for safety
        }
        pInAlloc->Release();
    }

    if (FAILED(hr)) return hr;

    ASSERT(pProperties->cbBuffer);

	 // Ask the allocator to reserve us some sample memory, NOTE the function
    // can succeed (that is return NOERROR) but still not have allocated the
    // memory that we requested, so we must check we got whatever we wanted

    ALLOCATOR_PROPERTIES Actual;
    hr = pAlloc->SetProperties(pProperties,&Actual);
    if (FAILED(hr)) {
        return hr;
    }

    ASSERT( Actual.cBuffers == NUMBER_OF_BUFFERS );
	ASSERT( Actual.cbBuffer >= pProperties->cbBuffer );
	ASSERT( Actual.cbAlign == pProperties->cbAlign );

    if ( pProperties->cBuffers > Actual.cBuffers ||
         pProperties->cbBuffer > Actual.cbBuffer ||
		 pProperties->cbAlign  > Actual.cbAlign) {
                return E_FAIL;
    }
    return NOERROR;
}
//
// The main method where the actual samples transform is done
HRESULT CAudioDownMixer::Transform(IMediaSample *pIn, IMediaSample *pOut)
{
	// Check the pointer
	CheckPointer(pIn, E_POINTER);

	// Copy the sample times
    REFERENCE_TIME TimeStart, TimeEnd;
    if (NOERROR == pIn->GetTime(&TimeStart, &TimeEnd)) {
        pOut->SetTime(&TimeStart, &TimeEnd);
    }
    LONGLONG MediaStart, MediaEnd;
    if (pIn->GetMediaTime(&MediaStart,&MediaEnd) == NOERROR) {
        pOut->SetMediaTime(&MediaStart,&MediaEnd);
    }

    // Copy the Sync point property
    HRESULT hr = pIn->IsSyncPoint();
    if (hr == S_OK) {
        pOut->SetSyncPoint(TRUE);
    }
    else if (hr == S_FALSE) {
        pOut->SetSyncPoint(FALSE);
    }
    else {  // an unexpected error has occured...
        return E_UNEXPECTED;
    }

	// Copy the preroll property
    hr = pIn->IsPreroll();
    if (hr == S_OK) {
        pOut->SetPreroll(TRUE);
    }
    else if (hr == S_FALSE) {
        pOut->SetPreroll(FALSE);
    }
    else {  // an unexpected error has occured...
        return E_UNEXPECTED;
    }

    // Copy the discontinuity property
    hr = pIn->IsDiscontinuity();
    if (hr == S_OK) {
    pOut->SetDiscontinuity(TRUE);
    }
    else if (hr == S_FALSE) {
        pOut->SetDiscontinuity(FALSE);
    }
    else {  // an unexpected error has occured...
        return E_UNEXPECTED;
    }

	return DownMix(pIn, pOut);
}
//
// This is where the actual downmixing is done
HRESULT CAudioDownMixer::DownMix(IMediaSample *pIn, IMediaSample *pOut)
{
	BYTE* pSourceBuffer;
	long cbSourceSize = pIn->GetActualDataLength();
	pIn->GetPointer(&pSourceBuffer);

	BYTE* pDstBuffer;
	long cbDstSize = pOut->GetActualDataLength();
	long cbFullSize = pOut->GetSize();
	pOut->GetPointer(&pDstBuffer);

	// Get the number of samples(only 16 bit audio)
	long nrInputSamples = cbSourceSize/2;
	long nrOutputSamples = cbDstSize/2;
	long nrMaxOutputSamples = cbFullSize/2;

	// Get the short amplitude of the buffer
	short *pSource = (short *)pSourceBuffer;
	short *pDst = (short *)pDstBuffer;

	int nrInputChannels = ((WAVEFORMATEX *)m_pInput->CurrentMediaType().Format())->nChannels;
	short leftChannelTemp = 0;
	short rightChannelTemp = 0;
	int channelCount = 0;
	int nrOutputSamplesWritten = 0;
	while(nrInputSamples--)
	{
		if(channelCount%2 == 0)
		{
			leftChannelTemp += *pSource;
		}
		else
		{
			rightChannelTemp += *pSource;
		}
		channelCount++;
		pSource++;
		
		if(channelCount == nrInputChannels)
		{
			if(nrOutputSamplesWritten + 2 > nrMaxOutputSamples)
				break;

			channelCount = 0;
			leftChannelTemp /= (nrInputChannels/2);
			rightChannelTemp /= (nrInputChannels/2);

			// Normalize
			if(leftChannelTemp > 32767)
				leftChannelTemp = 32767;
			else if(leftChannelTemp < -32767)
				leftChannelTemp = -32767;
			if(rightChannelTemp > 32767)
				rightChannelTemp = 32767;
			else if(rightChannelTemp < -32767)
				rightChannelTemp = -32767;

			*pDst = leftChannelTemp;
			pDst++;
			*pDst = rightChannelTemp;
			pDst++;
			nrOutputSamplesWritten+=2;

			leftChannelTemp = 0;
			rightChannelTemp = 0;
		}
	}// while

	long cbActuaSize = nrOutputSamplesWritten * sizeof(short);
	ASSERT(cbFullSize >= cbActuaSize);
	pOut->SetActualDataLength(cbActuaSize);

	return S_OK;
}
//
// In check i9nput type we tell DirectShow what media types we are accepting
HRESULT CAudioDownMixer::CheckInputType(const CMediaType* pMediaType)
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

	// Accept only 2,3, 6 or 8 channels
	if(!(pwfx->nChannels == 1 ||
		pwfx->nChannels == 2 ||
		pwfx->nChannels == 6 ||
		pwfx->nChannels == 8))
	{
		return VFW_E_TYPE_NOT_ACCEPTED;
	}

	// Everything ok.
	return S_OK;
}
//
// We only support transform if input and output formats are identical
HRESULT CAudioDownMixer::CheckTransform(const CMediaType *mtIn, const CMediaType *mtOut)
{
	// The output type must come out from MakeOutputCMediaTypeFromParams for our params
	// I can transform only to THAT sample type
	CMediaType myoutput= MakeOutputCMediaTypeFromParams(mtIn);
	
	if (*mtOut==myoutput) return S_OK;
	else return VFW_E_TYPE_NOT_ACCEPTED;
}
// GetMediaType: I support one type: stereo PCM waveform 
// with the same bits and samplerate as the input one.
// This type is only available if my input is connected.
HRESULT CAudioDownMixer::GetMediaType(int iPosition, CMediaType *pMediaType)
{
	// Is the input pin connected?
    if (m_pInput->IsConnected() == FALSE) return E_UNEXPECTED;

	// This should never happen
    if (iPosition < 0) return E_INVALIDARG;

    // Do we have more items to offer? No, only one.
    if (iPosition > 0) return VFW_S_NO_MORE_ITEMS;

	*pMediaType = MakeOutputCMediaTypeFromParams();

    return NOERROR;
}

//Only call this if our input pin is connected (we need this for samplerate). 
//Or provide input type as second param
CMediaType CAudioDownMixer::MakeOutputCMediaTypeFromParams(const CMediaType *InputMediaType)
{
	ASSERT(InputMediaType!=NULL || m_pInput->IsConnected());

	CMediaType ret;

	WAVEFORMATEX	input_wfe = (InputMediaType!=NULL) 
										? (* (WAVEFORMATEX*) InputMediaType->Format())
										: (* (WAVEFORMATEX*) m_pInput->CurrentMediaType().Format())
										;

	//We will use the fact that the WAVEFORMATEXTENSIBLE begins with a WAVEFORMATEX struct
	WAVEFORMATEX wfex;
	wfex.cbSize				=	sizeof(WAVEFORMATEX);
	wfex.wFormatTag			=	WAVE_FORMAT_PCM;

	wfex.nChannels			=	m_Channels;
	wfex.wBitsPerSample		=	m_BitsPerSample;
	wfex.nSamplesPerSec		=	input_wfe.nSamplesPerSec;

	wfex.nBlockAlign			=	(wfex.wBitsPerSample / 8) * wfex.nChannels;
	wfex.nAvgBytesPerSec		=	wfex.nBlockAlign * wfex.nSamplesPerSec;
	
	//These three will be copied into the return media type only if bIsMultichannel is TRUE

	ret.majortype	=	MEDIATYPE_Audio;
	ret.subtype		=	MEDIASUBTYPE_PCM;
	ret.bFixedSizeSamples		=	TRUE;
	ret.bTemporalCompression	=	FALSE;

	//POSSIBLE ERROR - PLEASE CHECK : I don't know what is this lSampleSize. I will just copy the source.
	ret.lSampleSize	=	wfex.nBlockAlign;//(InputMediaType!=NULL) ? InputMediaType->lSampleSize : m_pInput->CurrentMediaType().lSampleSize;
	
	ret.formattype	=	FORMAT_WaveFormatEx;
	ret.pUnk		=	NULL;

	ret.SetFormat((BYTE*)&wfex, wfex.cbSize);

	return ret;
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