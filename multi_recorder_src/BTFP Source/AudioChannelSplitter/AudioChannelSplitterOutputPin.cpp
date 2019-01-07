#include <atlbase.h>
#include "AudioChannelSplitterOutputPin.h"


CAudioChannelSplitterOutputPin::CAudioChannelSplitterOutputPin(LPCTSTR pObjName, CAudioChannelSplitter *pFilter, HRESULT *phr, LPCWSTR pName) :
					CBaseOutputPin((LPTSTR)pObjName, pFilter, &pFilter->m_csFilter, phr, pName) ,
					m_pFilter(pFilter), m_pOutputQueue(NULL), m_BitCount(16), m_SampleRate(48000), m_Channels(1),
					m_cOurRef(0)
{
	ASSERT(pFilter);
}

CAudioChannelSplitterOutputPin::~CAudioChannelSplitterOutputPin()
{
}

//
// NonDelegatingQueryInterface
//
// This function is overridden to expose IMediaSeeking.
//
STDMETHODIMP
CAudioChannelSplitterOutputPin::NonDelegatingQueryInterface(REFIID riid, void **ppv)
{
	CheckPointer(ppv,E_POINTER);
	ASSERT(ppv);
	CAutoLock lock_it(m_pLock);
	if (riid == IID_IMediaSeeking)
	{
		return GetInterface((IMediaSeeking*)this, ppv);
	}
	else
	{
		return CBaseOutputPin::NonDelegatingQueryInterface(riid, ppv);
	}
} // NonDelegatingQueryInterface

//-----------------------------------------------------------------------------
// NonDelegatingAddRef
//
// We need override this method so that we can do proper reference counting
// on our pin. The base class CBasePin does not do any reference
// counting on the pin in RETAIL.
//
// Please refer to the comments for the NonDelegatingRelease method for more
// info on why we need to do this.
//-----------------------------------------------------------------------------
STDMETHODIMP_(ULONG) CAudioChannelSplitterOutputPin::NonDelegatingAddRef()
{
	CAutoLock lock_it(m_pLock);

#ifdef DEBUG
	// Update the debug only variable maintained by the base class
	m_cRef++;
	ASSERT(m_cRef > 0);
#endif

	// Now update our reference count
	m_cOurRef++;
	ASSERT(m_cOurRef > 0);
	return m_cOurRef;

} // NonDelegatingAddRef

//-----------------------------------------------------------------------------
// NonDelegatingRelease
//
// Overrides this class so that we can take the pin of our
// pins list and delete it when its reference count drops to 1 and there
// is atleast one free pins.
//
// Note that CreateNextPin holds a reference count on the pin so that
// when the count drops to 1, we know that no one else has the pin.
//
// Moreover, the pin that we are about to delete must be a free pin(or else
// the reference would not have dropped to 1, and we must have atleast one
// other free pin(as the filter always wants to have one more free pin)
//
// Also, since CBasePin::NonDelegatingAddRef passes the call to the owning
// filter, we will have to call Release on the owning filter as well.
//
// Also, note that we maintain our own reference count m_cOurRef as the m_cRef
// variable maintained by CBasePin is debug only.
//
STDMETHODIMP_(ULONG) CAudioChannelSplitterOutputPin::NonDelegatingRelease()
{
	CAutoLock lock_it(m_pLock);

#ifdef DEBUG
	// Update the debug only variable in CBasePin
	m_cRef--;
	ASSERT(m_cRef >= 0);
#endif

	// Now update our reference count
	m_cOurRef--;
	ASSERT(m_cOurRef >= 0);

	// if the reference count on the object has gone to one, remove
	// the pin from our output pins list and physically delete it
	// provided there are atealst two free pins in the list(including
	// this one)

	// Also, when the ref count drops to 0, it really means that our
	// filter that is holding one ref count has released it so we
	// should delete the pin as well.

	if(m_cOurRef <= 0)
	{
		// If there are two free pins, delete this one.
		// NOTE: normall
		//if(n >= 2)
		{
			m_cOurRef = 0;
#ifdef DEBUG
			m_cRef = 0;
#endif
			m_pFilter->DeleteOutPin(this);
			return(ULONG) 0;
		}
	}

	return(ULONG) m_cOurRef;

} // NonDelegatingRelease
//
// DecideBufferSize
//
// This has to be present to override the PURE virtual class base function
//
HRESULT CAudioChannelSplitterOutputPin::DecideBufferSize(IMemAllocator *pAlloc,
										ALLOCATOR_PROPERTIES * pProperties)
{
	// Are the input pins connected
	if (!AreWeConnected())
		return VFW_E_NOT_CONNECTED;

	ASSERT(pAlloc);
	ASSERT(pProperties);
	CAutoLock lock_it(m_pLock);
	HRESULT hr = NOERROR;

	//Let' say we want some buffers... just to make sure the downstream filters 
	//don't starve and can cache data

	//But beware: a too high number of buffers will cause a noticeable delay when adjusting volume

#define NUMBER_OF_BUFFERS 5

	pProperties->cBuffers = NUMBER_OF_BUFFERS;

	//But we need input pin's allocator buffer size and the input type 
	//to determine the buffer size required for us

	int wBitsPerSample = m_BitCount;
	int sampleRate = m_SampleRate;
	int outputChannels = m_Channels;

	// Set The value of output bitrate
	DWORD DestByteSizeOfSamples = wBitsPerSample / 8;
	DWORD DestNumberOfChannels =outputChannels;
	DWORD SourceByteSizeOfSamples = wBitsPerSample / 8;
	DWORD SourceNumberOfChannels = outputChannels;

	// Size of each buffer in bytes. /2 = Default to 500ms buffer duration.
	pProperties->cbBuffer = DestByteSizeOfSamples*DestNumberOfChannels*sampleRate/2;//BUFFER_SIZE

	// Keep this number to the filter.
	//m_pTee->SetOutputBufferSize(pProperties->cbBuffer);
	// Create empty buffers for all of the pins
	//if(m_pFilter)
	//	m_pFilter->CreateInputBuffers();

	if (FAILED(hr)) return hr;

	ASSERT(pProperties->cbBuffer);

	//As far as the alignament is concerned, we must align to nBlockAlign 
	//(I think... somebody please check this)

	//Since at this time the output is still not officially connected, I will
	//create the output media type again in order to retrieve the nBlockAlign.
	//pProperties->cbAlign=((WAVEFORMATEX*)MakeOutputCMediaTypeFromParams(m_StereoDownmixerParams).Format())->nBlockAlign;

	// nBlockAlign is the same as the input type.
	pProperties->cbAlign=(DestByteSizeOfSamples)*outputChannels;

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
} // DecideBufferSize

HRESULT	CAudioChannelSplitterOutputPin::GetMediaType(int iPosition, CMediaType *pMediaType)
{
	CAutoLock lock_it(m_pLock);
	
	return m_pFilter->GetMediaType(iPosition, pMediaType);
}

//
// CheckMediaType
//
HRESULT CAudioChannelSplitterOutputPin::CheckMediaType(const CMediaType *pmt)
{
	if(!AreWeConnected())
		return VFW_E_NOT_CONNECTED;

	CAutoLock lock_it(m_pLock);

	// This filter always has the output media type as input media type.
	return m_pFilter->CheckOutputType(pmt);
} // CheckMediaType
//
// AreWeConnected - is this filter connected
//
BOOL CAudioChannelSplitterOutputPin::AreWeConnected()
{
	if(!m_pFilter)
		return FALSE;

	return m_pFilter->m_pInputPin->IsConnected();
}
//
// Active
//
// This is called when we start running or go paused. We create the
// output queue object to send data to our associated peer pin
//
HRESULT CAudioChannelSplitterOutputPin::Active()
{

	CAutoLock lock_it(m_pLock);
	HRESULT hr = NOERROR;

	// Make sure that the pin is connected
	if(m_Connected == NULL)
		return NOERROR;

	// Create the output queue if we have to
	if(m_pOutputQueue == NULL)
	{
		m_pOutputQueue = new COutputQueue(m_Connected, &hr, TRUE, FALSE);
		if(m_pOutputQueue == NULL)
			return E_OUTOFMEMORY;

		// Make sure that the constructor did not return any error
		if(FAILED(hr))
		{
			delete m_pOutputQueue;
			m_pOutputQueue = NULL;
			return hr;
		}
	}

	// Pass the call on to the base class
	CBaseOutputPin::Active();


	return NOERROR;

} // Active


//
// Inactive
//
// This is called when we stop streaming
// We delete the output queue at this time
//
HRESULT CAudioChannelSplitterOutputPin::Inactive()
{
	CAutoLock lock_it(m_pLock);

	// Delete the output queues associated with the pin.
	if(m_pOutputQueue)
	{
		delete m_pOutputQueue;
		m_pOutputQueue = NULL;
	}

	CBaseOutputPin::Inactive();

	return NOERROR;

} // Inactive
//
// Deliver
//
HRESULT CAudioChannelSplitterOutputPin::Deliver(IMediaSample *pMediaSample)
{
	CheckPointer(pMediaSample,E_POINTER);
	CAutoLock lock_it(m_pLock);

	// Make sure that we have an output queue
	if(m_pOutputQueue == NULL)
		return NOERROR;

	LONGLONG rtStart, rtStop;
	pMediaSample->GetTime(&rtStart, &rtStop);

	pMediaSample->AddRef();
	HRESULT hr = m_pOutputQueue->Receive(pMediaSample);


	return hr;
} // Deliver

//
// DeliverEndOfStream
//
HRESULT CAudioChannelSplitterOutputPin::DeliverEndOfStream()
{
	// Make sure that we have an output queue
	if(m_pOutputQueue == NULL)
		return NOERROR;

	m_pOutputQueue->EOS();

	return NOERROR;
} // DeliverEndOfStream


//
// DeliverBeginFlush
//
HRESULT CAudioChannelSplitterOutputPin::DeliverBeginFlush()
{
	// Make sure that we have an output queue
	if(m_pOutputQueue == NULL)
		return NOERROR;

	m_pOutputQueue->BeginFlush();

	return NOERROR;
} // DeliverBeginFlush


//
// DeliverEndFlush
//
HRESULT CAudioChannelSplitterOutputPin::DeliverEndFlush()
{
	// Make sure that we have an output queue
	if(m_pOutputQueue == NULL)
		return NOERROR;

	m_pOutputQueue->EndFlush();

	return NOERROR;
} // DeliverEndFlish

//
// DeliverNewSegment
//
HRESULT CAudioChannelSplitterOutputPin::DeliverNewSegment(REFERENCE_TIME tStart, 
													 REFERENCE_TIME tStop,  
													 double dRate)
{
	// Make sure that we have an output queue
	if(m_pOutputQueue == NULL)
		return NOERROR;

	m_pOutputQueue->NewSegment(tStart, tStop, dRate);

	return NOERROR;
} // DeliverNewSegment


//////////////////////////////////////////////////////////////////////////
// -- output pin seeking methods -------------------------
//////////////////////////////////////////////////////////////////////////
STDMETHODIMP 
CAudioChannelSplitterOutputPin::GetCapabilities(DWORD * pCapabilities)
{

	// Some versions of DShow have an aggregation bug that
	// affects playback with Media Player. To work around this,
	// we need to report the capabilities and time format the
	// same on all pins, even though only one
	// can seek at once.
	*pCapabilities =        AM_SEEKING_CanSeekAbsolute |
		AM_SEEKING_CanSeekForwards |
		AM_SEEKING_CanSeekBackwards |
		AM_SEEKING_CanGetDuration |
		AM_SEEKING_CanGetCurrentPos |
		AM_SEEKING_CanGetStopPos;
	return S_OK;
}

STDMETHODIMP 
CAudioChannelSplitterOutputPin::CheckCapabilities(DWORD * pCapabilities)
{
	DWORD dwActual;
	GetCapabilities(&dwActual);
	if (*pCapabilities & (~dwActual))
	{
		return S_FALSE;
	}
	return S_OK;
}

STDMETHODIMP 
CAudioChannelSplitterOutputPin::IsFormatSupported(const GUID * pFormat)
{
	// Some versions of DShow have an aggregation bug that
	// affects playback with Media Player. To work around this,
	// we need to report the capabilities and time format the
	// same on all pins, even though only one
	// can seek at once.
	if (*pFormat == TIME_FORMAT_MEDIA_TIME)
	{
		return S_OK;
	}
	return S_FALSE;

}
STDMETHODIMP 
CAudioChannelSplitterOutputPin::QueryPreferredFormat(GUID * pFormat)
{
	// Some versions of DShow have an aggregation bug that
	// affects playback with Media Player. To work around this,
	// we need to report the capabilities and time format the
	// same on all pins, even though only one
	// can seek at once.
	*pFormat = TIME_FORMAT_MEDIA_TIME;
	return S_OK;
}

STDMETHODIMP 
CAudioChannelSplitterOutputPin::GetTimeFormat(GUID *pFormat)
{
	return QueryPreferredFormat(pFormat);
}

STDMETHODIMP 
CAudioChannelSplitterOutputPin::IsUsingTimeFormat(const GUID * pFormat)
{
	GUID guidActual;
	HRESULT hr = GetTimeFormat(&guidActual);

	if (SUCCEEDED(hr) && (guidActual == *pFormat))
	{
		return S_OK;
	} else
	{
		return S_FALSE;
	}
}

STDMETHODIMP 
CAudioChannelSplitterOutputPin::ConvertTimeFormat(
	LONGLONG* pTarget, 
	const GUID* pTargetFormat,
	LONGLONG Source, 
	const GUID* pSourceFormat)
{
	// format guids can be null to indicate current format

	// since we only support TIME_FORMAT_MEDIA_TIME, we don't really
	// offer any conversions.
	if (pTargetFormat == 0 || *pTargetFormat == TIME_FORMAT_MEDIA_TIME)
	{
		if (pSourceFormat == 0 || *pSourceFormat == TIME_FORMAT_MEDIA_TIME)
		{
			*pTarget = Source;
			return S_OK;
		}
	}

	return E_INVALIDARG;
}

STDMETHODIMP 
CAudioChannelSplitterOutputPin::SetTimeFormat(const GUID * pFormat)
{
	if(*pFormat == TIME_FORMAT_MEDIA_TIME)
		return S_OK;

	return E_NOTIMPL;
}

STDMETHODIMP 
CAudioChannelSplitterOutputPin::GetDuration(LONGLONG *pDuration)
{
	// Get the longest duration from all of the pins. Actually, we're
	// looking for duration upstream of our input pins.
	if(!m_pFilter)
		return S_FALSE;

	HRESULT hr = NO_ERROR;
	
	IPin* pPin = NULL;
	pPin = m_pFilter->m_pInputPin->GetConnected();
	if(!pPin)
		return E_FAIL;
	CComQIPtr<IMediaSeeking> pSeek = pPin;
	if(pSeek)
	{
		hr = pSeek->GetDuration(pDuration);
	}

	return hr;
}

STDMETHODIMP 
CAudioChannelSplitterOutputPin::GetStopPosition(LONGLONG *pStop)
{
	if(!m_pFilter)
		return S_FALSE;

	HRESULT hr = NO_ERROR;

	IPin* pPin = NULL;
	pPin = m_pFilter->m_pInputPin->GetConnected();
	if(!pPin)
		return E_FAIL;
	CComQIPtr<IMediaSeeking> pSeek = pPin;
	if(pSeek)
	{
		hr = pSeek->GetStopPosition(pStop);
	}

	return hr;
}

STDMETHODIMP 
CAudioChannelSplitterOutputPin::GetCurrentPosition(LONGLONG *pCurrent)
{
	// this method is not supposed to report the previous start
	// position, but rather where we are now. This is normally
	// implemented by renderers, not parsers
	return E_NOTIMPL;
}

STDMETHODIMP 
CAudioChannelSplitterOutputPin::SetPositions(
										LONGLONG * pCurrent, 
										DWORD dwCurrentFlags, 
										LONGLONG * pStop, 
										DWORD dwStopFlags)
{
	// We don't support this
	//return E_NOTIMPL;
	HRESULT hr = NO_ERROR;
	try
	{

		dwCurrentFlags &= AM_SEEKING_PositioningBitsMask;
		dwStopFlags &= AM_SEEKING_PositioningBitsMask;

		if(dwCurrentFlags)
		{
			IPin* pPin = NULL;
			pPin = m_pFilter->m_pInputPin->GetConnected();
			if(!pPin)
				return E_FAIL;
			CComQIPtr<IMediaSeeking> pSeek = pPin;
			if(pSeek)
			{
				hr = pSeek->SetPositions(pCurrent, dwCurrentFlags, pStop, dwStopFlags);
			}
		}
		else
		{
			return S_FALSE;
		}
	}
	catch(...)
	{
		//LOG((TEXT("SetPositions FAILED")));
	}



	return hr;
}

STDMETHODIMP 
CAudioChannelSplitterOutputPin::GetPositions(LONGLONG * pCurrent, LONGLONG * pStop)
{
	if(!m_pFilter)
		return S_FALSE;

	HRESULT hr = NO_ERROR;
	
	IPin* pPin = NULL;
	pPin = m_pFilter->m_pInputPin->GetConnected();
	if(!pPin)
		return E_FAIL;
	CComQIPtr<IMediaSeeking> pSeek = pPin;
	if(pSeek)
	{
		hr = pSeek->GetPositions(pCurrent, pStop);
	}

	return hr;
}

STDMETHODIMP 
CAudioChannelSplitterOutputPin::GetAvailable(LONGLONG * pEarliest, LONGLONG * pLatest)
{
	if (pEarliest != NULL)
	{
		*pEarliest = 0;
	}
	if (pLatest != NULL)
	{
		GetDuration(pLatest);
	}
	return S_OK;
}

STDMETHODIMP 
CAudioChannelSplitterOutputPin::SetRate(double dRate)
{
	if(!m_pFilter)
		return S_FALSE;

	HRESULT hr = NO_ERROR;
	IPin* pPin = NULL;
	pPin = m_pFilter->m_pInputPin->GetConnected();
	if(!pPin)
		return E_FAIL;
	CComQIPtr<IMediaSeeking> pSeek = pPin;
	if(pSeek)
	{
		hr = pSeek->SetRate(dRate);
	}
	return hr;
}



STDMETHODIMP 
CAudioChannelSplitterOutputPin::GetRate(double * pdRate)
{
	if(!m_pFilter)
		return S_FALSE;

	HRESULT hr = NO_ERROR;
	IPin* pPin = NULL;
	pPin = m_pFilter->m_pInputPin->GetConnected();
	if(!pPin)
		return E_FAIL;
	CComQIPtr<IMediaSeeking> pSeek = pPin;
	if(pSeek)
	{
		hr = pSeek->GetRate(pdRate);
	}
	return hr;
}

STDMETHODIMP 
CAudioChannelSplitterOutputPin::GetPreroll(LONGLONG * pllPreroll)
{
	// don't need to allow any preroll time for us
	*pllPreroll = 0;
	return S_OK;
}
