
#include <initguid.h>
#include "AudioMixer.h"
#include "logger.h"

CLogger logger(TEXT("c:\\AudioMixer.txt"));

// {77CED088-610B-4e3f-9705-035365649A41}
DEFINE_GUID(CLSID_AudioMixer, 
0x77ced088, 0x610b, 0x4e3f, 0x97, 0x5, 0x3, 0x53, 0x65, 0x64, 0x9a, 0x41);

static const WCHAR g_wszFilterName[] = L"Audio Mixer";
static const TCHAR g_szFilterName[] = TEXT("Audio Mixer");

AMOVIESETUP_FILTER FilterInfo =
{
	&CLSID_AudioMixer,	// CLSID
	g_wszFilterName,	// Name
	MERIT_DO_NOT_USE,	// Merit
	0,					// Number of AMOVIESETUP_PIN structs
	NULL				// Pin registration information.
};

CFactoryTemplate g_Templates[] = 
{
    { 
		g_wszFilterName,				// Name
		&CLSID_AudioMixer,				// CLSID
		CAudioMixer::CreateInstance, // Method to create an instance of MyComponent
		NULL,							// Initialization function
		&FilterInfo						// Set-up information (for filters)
    }
};

int g_cTemplates = sizeof(g_Templates) / sizeof(g_Templates[0]);    

// Functions needed by the DLL, for registration.
STDAPI DllRegisterServer(void)
{
    return AMovieDllRegisterServer2(TRUE);
}

STDAPI DllUnregisterServer()
{
    return AMovieDllRegisterServer2(FALSE);
}

EXTERN_C BOOL WINAPI DllEntryPoint(HINSTANCE, ULONG, LPVOID);
BOOL APIENTRY DllMain(HANDLE hModule,DWORD dwReason,LPVOID lpReserved)
{
    return DllEntryPoint((HINSTANCE)hModule,(ULONG)dwReason,lpReserved);
}
// }}}-----------Filter Info -------------
///////////////////////////////////////////////////////////////////////////////

CUnknown* WINAPI CAudioMixer::CreateInstance(LPUNKNOWN pUnk, HRESULT *pHr)
{
    ASSERT(pHr);

	CAudioMixer *pFilter = new CAudioMixer(g_szFilterName, pUnk, CLSID_AudioMixer, pHr);
    if (pFilter == NULL) 
    {
		if(NULL != pHr)
		{
			*pHr = E_OUTOFMEMORY;
		}
    }
    return pFilter;
}

//-----------------------------------------------------------------------------
// Constructor
//-----------------------------------------------------------------------------
CAudioMixer::CAudioMixer(LPCTSTR lpszName, LPUNKNOWN pUnk, REFCLSID clsid, HRESULT *phr, int iMaxInPinAmt) : 
			CBaseFilter((LPTSTR)lpszName, pUnk,  &m_csFilter, clsid, phr),
			m_pOutPin(NULL),
			m_lstInPins(NAME("Audio Mixer Output Pins list")),
			m_iMaxInPinsAmt(iMaxInPinAmt),
			m_iNextInPinNum(0),
			m_iInPinsAmt(0),
			m_pCurMediaType(NULL),
			m_curTime(0),
			m_Volume(1.0)
{
	logger.Log(TEXT("CAudioMixer::Constructor"));

    ASSERT(phr);

	HRESULT hr = NOERROR;
	// Create output Pin.
	m_pOutPin = new CAudioMixerOutputPin(NAME("Output"), this, &hr, L"Output");
	ASSERT(NULL != m_pOutPin);
	// Clear Input Pin list.
	ClearInPinsList();
	// Create the first input pin
	CreateNextInPin();
	*phr = hr;
}

//-----------------------------------------------------------------------------
// Deonstructor
//-----------------------------------------------------------------------------
CAudioMixer::~CAudioMixer()
{
	logger.Log(TEXT("CAudioMixer::Destructor"));
	ClearInPinsList();
	SafeDelete(m_pOutPin);
	if(m_pCurMediaType)
		delete m_pCurMediaType;
}


//
// NonDelegatingQueryInterface
//
// This function is overridden to expose IMediaSeeking.
//
STDMETHODIMP
CAudioMixerOutputPin::NonDelegatingQueryInterface(REFIID riid, void **ppv)
{
	CheckPointer(ppv,E_POINTER);
	ASSERT(ppv);
	CAutoLock lock_it(m_pLock);
	if (riid == IID_IMediaSeeking)
	{
		return GetInterface((IMediaSeeking*)this, ppv);
	} else
	{
		return CBaseOutputPin::NonDelegatingQueryInterface(riid, ppv);
	}
} // NonDelegatingQueryInterface
//-----------------------------------------------------------------------------
// CheckInputType()
// Check input connection media type. The first connection can use any audio type,
// and the others will use the first connection mediatype.
//-----------------------------------------------------------------------------
HRESULT CAudioMixer::CheckInputType(const CMediaType* mtIn)
{
	HRESULT hr = E_INVALIDARG;
	if(NULL == m_pCurMediaType) // The first success input connection
	{
		if(mtIn->majortype == MEDIATYPE_Audio &&
			(mtIn->subtype == MEDIASUBTYPE_WAVE ||
			mtIn->subtype == MEDIASUBTYPE_PCM ) &&
			mtIn->formattype == FORMAT_WaveFormatEx)
		{
			WAVEFORMATEX * wvex = (WAVEFORMATEX*)mtIn->pbFormat;
			if(wvex->nChannels == 1 &&
				wvex->nSamplesPerSec == 48000 &&
				wvex->wBitsPerSample == 16)
			{
				
				logger.Log(TEXT("CAudioMixer::CheckInputType first connection S_OK"));
				hr = S_OK;
			}
		}
		if(SUCCEEDED(hr))
		{
			m_pCurMediaType = new CMediaType(*mtIn, &hr);
			ASSERT(NULL != m_pCurMediaType);
		}
	}
	else	// Exist a valid input connection
	{
		if(*mtIn == *m_pCurMediaType)
		{
			logger.Log(TEXT("CAudioMixer::CheckInputType Exist a valid input connection S_OK"));
			hr = S_OK;
		}
	}


	return hr;
}
HRESULT CAudioMixer::CheckOutputType(const CMediaType* mtOut)
{
	// The output type must come out from MakeOutputCMediaTypeFromParams for our params
	// I can transform only to THAT sample type
	CMediaType myoutput= MakeOutputCMediaTypeFromParams(mtOut);
	
	if (*mtOut==myoutput) return S_OK;
	else return VFW_E_TYPE_NOT_ACCEPTED;
}
//HRESULT CCatAuMix::CheckTransform(const CMediaType* mtIn, const CMediaType* mtOut)
//{
//	CheckPointer(mtIn, E_POINTER);
//	CheckPointer(mtOut, E_POINTER);
//	
//	// formats must be big enough 
//	if(mtIn->FormatLength() < sizeof(WAVEFORMATEX) ||
//		mtOut->FormatLength() < sizeof(WAVEFORMATEX))
//		return E_INVALIDARG;
//
//	WAVEFORMATEX *pFmtIn = (WAVEFORMATEX *)mtIn->Format();
//	WAVEFORMATEX *pFmtOut = (WAVEFORMATEX *)mtOut->Format();
//
//	int iCmpResult = memcmp(pFmtIn, pFmtOut, sizeof(WAVEFORMATEX));
//	if(0 == iCmpResult)
//		return NOERROR;
//
//	return E_INVALIDARG;
//}
//
//HRESULT CCatAuMix::DecideBufferSize(IMemAllocator * pAlloc, ALLOCATOR_PROPERTIES *pProp)
//{
//	CheckPointer(pAlloc,E_POINTER);
//	CheckPointer(pProp,E_POINTER);
//
//	// Is the input pin connected
//	if(!m_pInput->IsConnected())
//	{
//		return E_UNEXPECTED;
//	}
//
//	HRESULT hr = NOERROR;
//	ALLOCATOR_PROPERTIES InProp;
//	hr = ((CCatAuMixInputPin *)m_pInput)->PeekAllocator()->GetProperties(&InProp);
//	if(FAILED(hr))
//	{
//		return hr;
//	}
//	hr = pAlloc->SetProperties(&InProp, pProp);
//	if(FAILED(hr))
//	{
//		return hr;
//	}
//
//	int iCmpResult = memcmp(&InProp, pProp, sizeof(ALLOCATOR_PROPERTIES));
//	if(0 != iCmpResult)
//	{
//		return E_UNEXPECTED;
//	}
//
//	return NOERROR;
//}
//

//-----------------------------------------------------------------------------
// GetMediaType()
// Retrieves a preferred media type.
// In the filter, it is determined by the first connection.
//-----------------------------------------------------------------------------
HRESULT CAudioMixer::GetMediaType(int iPosition, CMediaType *pMediaType)
{
	CheckPointer(pMediaType,E_POINTER);

	// Is the input pin connected
	if(NULL == m_pCurMediaType)
	{
		return E_UNEXPECTED;
	}

	// This should never happen
	if(iPosition < 0)
	{
		return E_INVALIDARG;
	}

	// Do we have more items to offer
	if(iPosition > 0)
	{
		return VFW_S_NO_MORE_ITEMS;
	}

	//*pMediaType = *m_pCurMediaType;
	*pMediaType = MakeOutputCMediaTypeFromParams();
	return NOERROR;
}


STDMETHODIMP CAudioMixer::Stop()
{
	CAutoLock lock(&m_csReceive);

	HRESULT hr = CBaseFilter::Stop();
	LOG((TEXT("CAudioMixer::Stop")));
	m_curTime = 0;
	ClearInputBuffers();

	return hr;
}
//-----------------------------------------------------------------------------
// GetPinCount()
// Retrieves pins count
//-----------------------------------------------------------------------------
int CAudioMixer::GetPinCount()
{
	return m_iInPinsAmt + 1; // InPins and one output pin.
}

//-----------------------------------------------------------------------------
// GetPin()
// First return Output pin, then return InPins.
//-----------------------------------------------------------------------------
CBasePin *CAudioMixer::GetPin(int n)
{
	if (n < 0)
		return NULL ;

	if(n == 0)
	{
		ASSERT(NULL!=m_pOutPin);
		return m_pOutPin;
	}

	return GetInPin(n-1);	
}

//-----------------------------------------------------------------------------
// ClearInPinsList()
// Clear the input pins list. Release all pins and remove nodes
// Invoke by construct and deconstruct
//-----------------------------------------------------------------------------
void CAudioMixer::ClearInPinsList()
{
	logger.Log(TEXT("CAudioMixer::ClearInPinsList"));

	POSITION pos = m_lstInPins.GetHeadPosition();

	while(pos)
	{
		CAudioMixerInputPin *pPin = m_lstInPins.GetNext(pos);
		pPin->Release();
	}

	m_iInPinsAmt = 0;
	m_lstInPins.RemoveAll();

}

//-----------------------------------------------------------------------------
// GetInPin()
// Retrieves a input pin from list.
//-----------------------------------------------------------------------------
CAudioMixerInputPin *CAudioMixer::GetInPin(int n)
{
	if (n >= m_iInPinsAmt)
		return NULL;

	POSITION pos = m_lstInPins.GetHeadPosition();

	CAudioMixerInputPin *pPin = NULL;

	while( n >= 0 ) 
	{
		pPin = m_lstInPins.GetNext(pos);
		n--;
	}

	return pPin;
}

//-----------------------------------------------------------------------------
// CreateNextInPin()
// Create a new input pin.
// Invoke by CCatAuMixInputPin::CompleteConnecte()
//-----------------------------------------------------------------------------
HRESULT CAudioMixer::CreateNextInPin()
{
	logger.Log(TEXT("CAudioMixer::CreateNextInPin"));

	HRESULT hr = NOERROR;

	int n = GetFreeInPinsAmt();
	ASSERT(n <= 1);		// there just one free input pin or none.
	if(n == 1 || m_iInPinsAmt == m_iMaxInPinsAmt)
		return NOERROR;

	WCHAR wszbuf[20];
	ZeroMemory(wszbuf, sizeof(WCHAR)*20);
	swprintf(wszbuf, L"Input%d\0", m_iNextInPinNum);
	CAudioMixerInputPin *pPin = 
		new CAudioMixerInputPin(NAME("Input"), this, &hr, (LPCWSTR)wszbuf);

	if (FAILED(hr) || pPin == NULL) 
	{
		SafeDelete(pPin);
		return hr;
	}

	pPin->AddRef();
	m_iInPinsAmt++;
	m_iNextInPinNum++;
	m_lstInPins.AddTail(pPin);
	IncrementPinVersion();

	return hr;
}

//-----------------------------------------------------------------------------
// DeleteInPin()
//-----------------------------------------------------------------------------
void CAudioMixer::DeleteInPin(CAudioMixerInputPin *pInPin)
{
	logger.Log(TEXT("CAudioMixer::DeleteInPin"));

	ASSERT(NULL !=pInPin);
	if (NULL == pInPin)
		return;

	POSITION pos = m_lstInPins.GetHeadPosition();

	while(pos) 
	{
		POSITION posold = pos;         // Remember this position
		CAudioMixerInputPin *pPin = m_lstInPins.GetNext(pos);

		if (pInPin == pPin) 
		{
			m_lstInPins.Remove(posold);
			//delete pPin;
			SafeDelete(pInPin);

			m_iInPinsAmt--;
			IncrementPinVersion();
			break;
		}
	}
}

//-----------------------------------------------------------------------------
// GetFreeInPinsAmt()
//-----------------------------------------------------------------------------
int CAudioMixer::GetFreeInPinsAmt()
{
	int n = 0;
	POSITION pos = m_lstInPins.GetHeadPosition();

	while(pos) 
	{
		CAudioMixerInputPin *pPin = m_lstInPins.GetNext(pos);

		if (pPin && !pPin->IsConnected())
			n++;
	}

	return n;
}
//-----------------------------------------------------------------------------
// OnInputSample - this is called from any of the input pins when new sample
// arrives.
//-----------------------------------------------------------------------------
void CAudioMixer::OnInputSample()
{
	CAutoLock lock_it(&m_csReceive);

	// Check if all input pins have samples ready
	if(AreAllInputPinsFilled())
	{
		// Get output sample
		IMediaSample* pOutputSample = NULL;
		HRESULT hr = m_pOutPin->GetDeliveryBuffer(&pOutputSample, NULL, NULL, 0);
		if(hr != S_OK)
		{
			LOG((TEXT("CAudioMixer::OnInputSample -- GetDeliveryBuffer FAILED")));
			return;
		}

		// Get output buffer
		BYTE* pDest = NULL;
		pOutputSample->GetPointer(&pDest);
		LONG cbDestSize = pOutputSample->GetActualDataLength();
		LONG actualSize = 0;
		// Mix the input samples into output
		MixInput(pDest, cbDestSize, &actualSize);

		// Calculate start /top times
		REFERENCE_TIME rtStart, rtStop;
		rtStart = m_curTime;
		rtStop = m_curTime + 1000 * 10000;

		m_curTime = rtStop;

		// Set start and stop time
		pOutputSample->SetTime(&rtStart, &rtStop);
		pOutputSample->SetMediaTime(&rtStart, &rtStop);
		pOutputSample->SetActualDataLength(actualSize);

		// Deliver the sample
		m_pOutPin->Deliver(pOutputSample);
	}
		
}
//-----------------------------------------------------------------------------
// AreAllInputPinsFilled - checks if all of the input pins have IMedaSample ready.
//-----------------------------------------------------------------------------
bool CAudioMixer::AreAllInputPinsFilled()
{
	POSITION pos = m_lstInPins.GetHeadPosition();
	while(pos)
	{
		CAudioMixerInputPin *pPin = m_lstInPins.Get(pos);
		if(pPin->IsConnected() && !pPin->IsSampleReady())
			return false;

		pos = m_lstInPins.Next(pos);
	}
	
	return true;
}
//-----------------------------------------------------------------------------
// AreAllInputPinsFilled - checks if all of the input pins have IMedaSample ready.
//-----------------------------------------------------------------------------
bool CAudioMixer::AreAllInputsEOS()
{
	POSITION pos = m_lstInPins.GetHeadPosition();
	while(pos)
	{
		CAudioMixerInputPin *pPin = m_lstInPins.Get(pos);
		if(pPin->IsConnected() && !pPin->IsEOS)
			return false;

		pos = m_lstInPins.Next(pos);
	}
	
	return true;
}
//-----------------------------------------------------------------------------
// CreateInputBuffers - creates buffers on all of the input pins.
//-----------------------------------------------------------------------------
void CAudioMixer::CreateInputBuffers()
{
	POSITION pos = m_lstInPins.GetHeadPosition();
	while(pos)
	{
		CAudioMixerInputPin *pPin = m_lstInPins.Get(pos);
		if(pPin->IsConnected())
		{
			pPin->CreateBuffer();
		}

		pos = m_lstInPins.Next(pos);
	}
}
void CAudioMixer::ClearInputBuffers()
{
	POSITION pos = m_lstInPins.GetHeadPosition();
	while(pos)
	{
		CAudioMixerInputPin *pPin = m_lstInPins.Get(pos);
		if(pPin->IsConnected())
		{
			pPin->ClearBuffer();
		}

		pos = m_lstInPins.Next(pos);
	}
}
//-----------------------------------------------------------------------------
// MixInput - mixes samples of all input pins.
//-----------------------------------------------------------------------------
void CAudioMixer::MixInput(BYTE* pDest, LONG cbSize, LONG* actSize)
{
	
	//int counter = 0;
	BYTE* pSrc = NULL;
	POSITION pos = m_lstInPins.GetHeadPosition();
	memset(pDest, 0, cbSize);
	LONG cbSourceAllocated;
	int pinCount = 0;
	*actSize = 0;
	while(pos)
	{
		CAudioMixerInputPin *pPin = m_lstInPins.Get(pos);
		if(!pPin->IsConnected())
			goto NEXT;

		// Get available size
		LONG cbSource = pPin->BufferSize();
		//cbSource = min(cbSource, cbSize);
		if(cbSource > cbSize/2)
		{
			cbSource = cbSize/2;
		}

		if(*actSize < 2*cbSource)
		{
			*actSize = 2*cbSource;
		}

		if(!cbSource)
			goto NEXT;

		// Check if we have Source Buffer allocated.
		if(!pSrc)
		{
			pSrc = (BYTE*) malloc(cbSource);
			cbSourceAllocated = cbSource;
		}

		// The source allocated in previous run is different
		// then the buffer size needed now. reallocate.
		if(cbSource != cbSourceAllocated)
		{
			if(pSrc)
			{
				delete pSrc;
				pSrc = NULL;
			}

			pSrc = (BYTE*) malloc(cbSource);
			cbSourceAllocated = cbSource;
		}
		
		// Read the source buffer from the pin
		pPin->GetBuffer(pSrc, cbSource);

		// Now, mix the dest buffer with the source buffer
		//
		// Get the number of samples(only 16 bit audio)
		long samples = cbSize/2;// cbSource/2;
		short* pSourceBuffer = (short*)pSrc;
		short* pDestBuffer = (short*)pDest;
		int sampleCounter = 0;
		while(samples--)
		{
			if((sampleCounter%2 == 0 && pinCount%2 == 0) ||
				(sampleCounter%2 != 0 && pinCount%2 != 0))
			{
				short srcValue = (*pSourceBuffer);
				short dstValue = (*pDestBuffer);

				long res = (srcValue * AUDIO_VOLUME + dstValue);

				// Apply volume
				res = (long)(res * m_Volume);

				if(res > 32767)
					res = 32767;
				else if(res < -32767)
					res = 32767;

				*pDestBuffer = (short)res;

				pSourceBuffer++;
			}

			pDestBuffer++;
			sampleCounter++;
		}

NEXT:
		// Advance to the next pin
		pos = m_lstInPins.Next(pos);
		pinCount++;
	}
	
	if(pSrc)
	{
		delete pSrc;
		pSrc = NULL;
	}
}
//
// NonDelegatingQueryInterface
//
// We expose IAudioMixer interface here
STDMETHODIMP CAudioMixer::NonDelegatingQueryInterface(REFIID riid, void **ppv)
{
	CheckPointer(ppv, E_POINTER);

	// Check if it is looking for our interface
	if(riid == IID_IAudioMixer)
	{
		// Get the interface
		return GetInterface((IAudioMixer*)this, ppv);
	}
	else
	{
		// Forward it to our base class
		return CBaseFilter::NonDelegatingQueryInterface(riid, ppv);
	}
}
//
// Set volume for each channel
STDMETHODIMP CAudioMixer::SetVolume(double volume)
{
	m_Volume = volume;

	return S_OK;
}

//Only call this if our input pin is connected (we need this for samplerate). 
//Or provide input type as second param
CMediaType CAudioMixer::MakeOutputCMediaTypeFromParams(const CMediaType *InputMediaType)
{
	CMediaType ret;

	//WAVEFORMATEX	input_wfe = (InputMediaType!=NULL) 
	//									? (* (WAVEFORMATEX*) InputMediaType->Format())
	//									: (* (WAVEFORMATEX*) m_pInput->CurrentMediaType().Format())
	//									;

	//We will use the fact that the WAVEFORMATEXTENSIBLE begins with a WAVEFORMATEX struct
	WAVEFORMATEX wfex;
	wfex.cbSize				=	0;// Extra Information
	wfex.wFormatTag			=	WAVE_FORMAT_PCM;

	wfex.nChannels			=	2;
	wfex.wBitsPerSample		=	16;
	wfex.nSamplesPerSec		=	48000;

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

	ret.SetFormat((BYTE*)&wfex, sizeof(WAVEFORMATEX));

	return ret;
}