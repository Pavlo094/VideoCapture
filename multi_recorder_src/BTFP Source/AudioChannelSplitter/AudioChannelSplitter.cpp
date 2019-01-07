#include <atlbase.h>
#include <initguid.h>
#include "AudioChannelSplitter.h"

// This struct is 24 bit data struct used for 24bit audio processing
struct Uint24
{
    unsigned char bits[3]; // assuming char is 8 bits

    Uint24()
        : bits()
    {}

    Uint24(unsigned val) {
        *this = val;
    }

    Uint24& operator=(unsigned val) {
        // store as little-endian
        bits[2] = val >> 16 & 0xff;
        bits[1] = val >> 8 & 0xff;
        bits[0] = val & 0xff;
        return *this;
    }

    unsigned as_unsigned() const {
        return bits[0] | bits[1] << 8 | bits[2] << 16;
    }
};




static const WCHAR g_wszFilterName[] = L"Audio Channel Splitter";
static const TCHAR g_szFilterName[] = TEXT("Audio Channel Splitter");

AMOVIESETUP_FILTER FilterInfo =
{
	&CLSID_AudioChannelSplitter,	// CLSID
	g_wszFilterName,				// Name
	MERIT_DO_NOT_USE,				// Merit
	0,								// Number of AMOVIESETUP_PIN structs
	NULL							// Pin registration information.
};

CFactoryTemplate g_Templates[] = 
{
    { 
		g_wszFilterName,							// Name
		&CLSID_AudioChannelSplitter,				// CLSID
		CAudioChannelSplitter::CreateInstance,		// Method to create an instance of MyComponent
		NULL,										// Initialization function
		&FilterInfo									// Set-up information (for filters)
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

CUnknown* WINAPI CAudioChannelSplitter::CreateInstance(LPUNKNOWN pUnk, HRESULT *pHr)
{
    ASSERT(pHr);

	CAudioChannelSplitter *pFilter = new CAudioChannelSplitter(g_szFilterName, pUnk, CLSID_AudioChannelSplitter, pHr);
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
CAudioChannelSplitter::CAudioChannelSplitter(LPCTSTR lpszName, LPUNKNOWN pUnk, REFCLSID clsid, HRESULT *phr, int iMaxInPinAmt) : 
			CBaseFilter((LPTSTR)lpszName, pUnk,  &m_csFilter, clsid, phr),
			m_pInputPin(NULL),
			m_lstOutPins(NAME("Audio Channel Splitter Output Pins")),
			m_pCurMediaType(NULL),
			m_curTime(0),
			m_bitsPerSample(16)
{
    ASSERT(phr);

	HRESULT hr = NOERROR;
	// Create input Pin.
	m_pInputPin = new CAudioChannelSplitterInputPin(NAME("Input"), this, &hr, L"Input");
	ASSERT(NULL != m_pInputPin);
	// Clear Output Pin list.
	ClearOutPinsList();
	
	// Initialize params
	ZeroMemory(&m_params, sizeof(AudioChannelSplitterParameters));
	m_params.MixMatrix[0][0] = 1;
	m_params.MixMatrix[1][1] = 1;
	m_params.MixMatrix[2][2] = 1;
	m_params.MixMatrix[3][3] = 1;
	m_params.MixMatrix[4][4] = 1;
	m_params.MixMatrix[5][5] = 1;

	*phr = hr;
}

//-----------------------------------------------------------------------------
// Deonstructor
//-----------------------------------------------------------------------------
CAudioChannelSplitter::~CAudioChannelSplitter()
{
	ClearOutPinsList();
	SafeDelete(m_pInputPin);
	if(m_pCurMediaType)
		delete m_pCurMediaType;
}

//-----------------------------------------------------------------------------
// CheckInputType()
// Check input connection media type. The first connection can use any audio type,
// and the others will use the first connection mediatype.
//-----------------------------------------------------------------------------
HRESULT CAudioChannelSplitter::CheckInputType(const CMediaType* mtIn)
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
			if( wvex->nChannels % 2 == 0 && //Only even pair of channels
				(wvex->wBitsPerSample == 16 || wvex->wBitsPerSample == 24) // Only 16 and 24 bit
				)
			{
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
			hr = S_OK;
		}
	}


	return hr;
}
HRESULT CAudioChannelSplitter::CheckOutputType(const CMediaType * mtOut)
{
	HRESULT hr = E_FAIL;

	if(NULL == m_pCurMediaType) return hr;

	if(mtOut->majortype == m_pCurMediaType->majortype &&
		mtOut->subtype == m_pCurMediaType->subtype &&
		mtOut->formattype == m_pCurMediaType->formattype)
	{
		WAVEFORMATEX * wvex = (WAVEFORMATEX*)mtOut->pbFormat;
		WAVEFORMATEX * wvExIn = (WAVEFORMATEX*)m_pCurMediaType->pbFormat;
		if(wvex->nChannels == 1 && wvex->wBitsPerSample == wvExIn->wBitsPerSample &&
			wvex->nSamplesPerSec == wvex->nSamplesPerSec)
		{
			hr = S_OK;
		}
	}

	return hr;
}

//-----------------------------------------------------------------------------
// GetMediaType()
// Retrieves a preferred media type.
// In the filter, it is determined by the first connection.
//-----------------------------------------------------------------------------
HRESULT CAudioChannelSplitter::GetMediaType(int iPosition, CMediaType *pMediaType)
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

	pMediaType->InitMediaType();
	pMediaType->SetTemporalCompression(m_pCurMediaType->bTemporalCompression);
	pMediaType->SetType(&m_pCurMediaType->majortype);
	pMediaType->SetSampleSize(m_pCurMediaType->lSampleSize);
	pMediaType->SetFormatType(&m_pCurMediaType->formattype);
	pMediaType->SetSubtype(&m_pCurMediaType->subtype);

	WAVEFORMATEX * pwfexOut = (WAVEFORMATEX*)pMediaType->AllocFormatBuffer(sizeof(WAVEFORMATEX));
	ZeroMemory(pwfexOut, sizeof(WAVEFORMATEX));

	WAVEFORMATEX * pwfexIn = (WAVEFORMATEX*)m_pCurMediaType->pbFormat;
	pwfexOut->nChannels = 1;
	pwfexOut->nSamplesPerSec = pwfexIn->nSamplesPerSec;
	pwfexOut->wBitsPerSample = pwfexIn->wBitsPerSample;
	pwfexOut->wFormatTag = pwfexIn->wFormatTag;
	pwfexOut->nBlockAlign = pwfexOut->nChannels * (pwfexOut->wBitsPerSample/8);
	pwfexOut->nAvgBytesPerSec = pwfexOut->nSamplesPerSec * pwfexOut->nBlockAlign;
	pwfexOut->cbSize = 0;
	


	return NOERROR;
}


STDMETHODIMP CAudioChannelSplitter::Stop()
{
	CAutoLock lock(&m_csReceive);

	HRESULT hr = CBaseFilter::Stop();
	m_curTime = 0;

	return hr;
}
//-----------------------------------------------------------------------------
// GetPinCount()
// Retrieves pins count
//-----------------------------------------------------------------------------
int CAudioChannelSplitter::GetPinCount()
{
	return m_lstOutPins.GetCount() + 1;// Outpins + 1 input pin
}

//-----------------------------------------------------------------------------
// GetPin()
// First return Input pin, then return OutPins.
//-----------------------------------------------------------------------------
CBasePin *CAudioChannelSplitter::GetPin(int n)
{
	if (n < 0)
		return NULL ;

	if(n == 0)
	{
		ASSERT(NULL!=m_pInputPin);
		return m_pInputPin;
	}

	return GetOutPin(n - 1);	
}

//-----------------------------------------------------------------------------
// ClearInPinsList()
// Clear the ouytput pins list. Release all pins and remove nodes
// Invoke by construct and deconstruct
//-----------------------------------------------------------------------------
void CAudioChannelSplitter::ClearOutPinsList()
{

	POSITION pos = m_lstOutPins.GetHeadPosition();

	while(pos)
	{
		CAudioChannelSplitterOutputPin *pPin = m_lstOutPins.GetNext(pos);
		pPin->Release();
	}
	m_lstOutPins.RemoveAll();

}

//-----------------------------------------------------------------------------
// GetInPin()
// Retrieves a output pin from list.
//-----------------------------------------------------------------------------
CAudioChannelSplitterOutputPin *CAudioChannelSplitter::GetOutPin(int n)
{
	if (n >= m_lstOutPins.GetCount())
		return NULL;

	POSITION pos = m_lstOutPins.GetHeadPosition();

	CAudioChannelSplitterOutputPin *pPin = NULL;

	while( n >= 0 ) 
	{
		pPin = m_lstOutPins.GetNext(pos);
		n--;
	}

	return pPin;
}
//-----------------------------------------------------------------------------
// OnInputSample - this is called from input pin when new sample
// arrives.
//-----------------------------------------------------------------------------
void CAudioChannelSplitter::OnInputSample(IMediaSample * pSample)
{
	CAutoLock lock_it(&m_csReceive);

	// Get source buffer
	BYTE * pSrc = NULL;
	pSample->GetPointer(&pSrc);
	LONG cbSrcSize = pSample->GetActualDataLength();
	REFERENCE_TIME rtStart, rtStop;
	bool timeValid = false;
	if SUCCEEDED(pSample->GetTime(&rtStart, &rtStop))
	{
		timeValid = true;
	}
	REFERENCE_TIME rtStartMedia, rtStopMedia;
	bool timeValidMedia = false;
	if SUCCEEDED(pSample->GetMediaTime(&rtStartMedia, &rtStopMedia))
	{
		timeValidMedia = true;
	}

	// Get the start of the output pins.
	POSITION pos = m_lstOutPins.GetHeadPosition();
	int myCount = m_lstOutPins.GetCount();
	for (int outChcounter = 0; (pos); outChcounter++)
	{
		// Get the next output pin.
		CAudioChannelSplitterOutputPin * pOutPin = m_lstOutPins.GetNext(pos);
		// If it is not connected - ignore it
		if(!pOutPin || !pOutPin->IsConnected())
			continue;

		// Get output sample
		CComPtr<IMediaSample> pOutputSample = NULL;
		//IMediaSample* pOutputSample = NULL;
		pOutPin->GetDeliveryBuffer(&pOutputSample, (REFERENCE_TIME*)NULL, (REFERENCE_TIME*)NULL, 0);
		if(!pOutputSample)
		{
			break;
		}
		// Get output buffer
		BYTE* pDest = NULL;
		pOutputSample->GetPointer(&pDest);
		LONG cbDestSize = pOutputSample->GetActualDataLength();

		long cbActualDestSize = CopyChannelData(pSrc, cbSrcSize, m_bitsPerSample / 8, m_numberOfInputChannels ,pDest, outChcounter);

		if (timeValid)
		{
			pOutputSample->SetTime(&rtStart, &rtStop);
			HRESULT hr = pOutputSample->GetTime(&rtStart, &rtStop);
		}
		if (timeValidMedia)
		{
			pOutputSample->SetMediaTime(&rtStartMedia, &rtStopMedia);
		}
		pOutputSample->SetActualDataLength(cbActualDestSize);

		// Deliver to the output pin.
		pOutPin->Deliver(pOutputSample);
	}
}

long CAudioChannelSplitter::CopyChannelData(BYTE * pSrc, long cbSrcSize, long bps, long channels, BYTE * pDest, int outCh)
{
	const long samplesCount = cbSrcSize / (bps * channels);
	pSrc += outCh * bps;
	for (long sample = 0; sample < samplesCount; sample++)
	{
		memcpy(pDest, pSrc, bps);
		pDest += bps;
		pSrc += channels * bps;
	}
	return bps * samplesCount;
}

HRESULT CAudioChannelSplitter::CreateOutputPins()
{
	HRESULT hr = NOERROR;

	if(!(m_pCurMediaType->formattype == FORMAT_WaveFormatEx))return E_INVALIDARG;

	WAVEFORMATEX * pwfex = (WAVEFORMATEX*)m_pCurMediaType->pbFormat;

	m_numberOfInputChannels = pwfex->nChannels;
	m_bitsPerSample = pwfex->wBitsPerSample;

	int nrPins = pwfex->nChannels;
	for(int i=0; i< nrPins; i++)
	{
		WCHAR wszbuf[20];
		ZeroMemory(wszbuf, sizeof(WCHAR)*20);
		swprintf(wszbuf, 20, L"Output %d\0", i+1);

		CAudioChannelSplitterOutputPin * pOutPin = new CAudioChannelSplitterOutputPin(NAME("Output"), this, &hr, (LPCWSTR)wszbuf);

		if (FAILED(hr) || pOutPin == NULL) 
		{
			SafeDelete(pOutPin);
			return hr;
		}

		pOutPin->AddRef();

		pOutPin->m_BitCount = pwfex->wBitsPerSample;
		pOutPin->m_SampleRate = pwfex->nSamplesPerSec;

		m_lstOutPins.AddTail(pOutPin);
	}

	return hr;
}
void CAudioChannelSplitter::DeliverBeginFlushOnOutputPins()
{
	POSITION pos = m_lstOutPins.GetHeadPosition();
	while(pos)
	{
		CAudioChannelSplitterOutputPin *pPin = m_lstOutPins.Get(pos);
		if(pPin)
			pPin->DeliverBeginFlush();

		pos = m_lstOutPins.Next(pos);
	}
}
void CAudioChannelSplitter::DeliverEndFlushOnOutputPins()
{
	POSITION pos = m_lstOutPins.GetHeadPosition();
	while(pos)
	{
		CAudioChannelSplitterOutputPin *pPin = m_lstOutPins.Get(pos);
		if(pPin)
			pPin->DeliverEndFlush();

		pos = m_lstOutPins.Next(pos);
	}
}
HRESULT CAudioChannelSplitter::DeliverEndOfStreamOnOutputPins()
{
	HRESULT hr = NO_ERROR;
	POSITION pos = m_lstOutPins.GetHeadPosition();
	while(pos)
	{
		CAudioChannelSplitterOutputPin *pPin = m_lstOutPins.Get(pos);
		if(pPin)
			pPin->DeliverEndOfStream();

		pos = m_lstOutPins.Next(pos);
	}

	return hr;
}
HRESULT CAudioChannelSplitter::DeliverNewSegmentOnOutputPins(REFERENCE_TIME tStart, REFERENCE_TIME tStop, double dRate)
{
	HRESULT hr = NO_ERROR;

	POSITION pos = m_lstOutPins.GetHeadPosition();
	while(pos)
	{
		CAudioChannelSplitterOutputPin *pPin = m_lstOutPins.Get(pos);
		if(pPin)
			pPin->DeliverNewSegment(tStart, tStop, dRate);

		pos = m_lstOutPins.Next(pos);
	}

	return hr;
}
STDMETHODIMP CAudioChannelSplitter::NonDelegatingQueryInterface(REFIID riid, void **ppv)
{
	CheckPointer(ppv, E_POINTER);

	CAutoLock lock_it(m_pLock);

	if(riid == IID_IAudioChannelSplitter)
	{
		return GetInterface((IAudioChannelSplitter*)this, ppv);
	}
	else
	{
		return CBaseFilter::NonDelegatingQueryInterface(riid, ppv);
	}
}
void CAudioChannelSplitter::DeleteOutPin(CAudioChannelSplitterOutputPin *pOutPin)
{
	ASSERT(NULL !=pOutPin);
	if (NULL == pOutPin)
		return;

	POSITION pos = m_lstOutPins.GetHeadPosition();

	while(pos) 
	{
		POSITION posold = pos;         // Remember this position
		CAudioChannelSplitterOutputPin *pPin = m_lstOutPins.GetNext(pos);

		if (pOutPin == pPin) 
		{
			m_lstOutPins.Remove(posold);
			SafeDelete(pOutPin);

			break;
		}
	}
}
/******************************************************************************************************
/ IStereoSplitter methods
/*****************************************************************************************************/
STDMETHODIMP CAudioChannelSplitter::get_Params(AudioChannelSplitterParameters *irp)
{
	CheckPointer(irp, E_POINTER);

	*irp = m_params;

	return S_OK;
}
STDMETHODIMP CAudioChannelSplitter::put_Params(AudioChannelSplitterParameters irp)
{
	m_params = irp;

	return S_OK;
}
int CAudioChannelSplitter::GetOutputStereoChannel(int inputCh)
{
	for(int i=0; i<MAX_CHANNELS; i++)
	{
		if(m_params.MixMatrix[inputCh][i])return i;
	}
	return -1;
}
