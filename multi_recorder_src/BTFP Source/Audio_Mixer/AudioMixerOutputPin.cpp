#include "AudioMixerOutputPin.h"

#include "logger.h"

CAudioMixerOutputPin::CAudioMixerOutputPin(LPCTSTR pObjName, CAudioMixer *pFilter, HRESULT *phr, LPCWSTR pName) :
					CBaseOutputPin((LPTSTR)pObjName, pFilter, &pFilter->m_csFilter, phr, pName) ,
					m_pFilter(pFilter), m_pOutputQueue(NULL)
{
	logger.Log(TEXT("CAudioMixerOutputPin::Constructor"));
	ASSERT(pFilter);
}

CAudioMixerOutputPin::~CAudioMixerOutputPin()
{
	logger.Log(TEXT("CAudioMixerOutputPin::Destructor"));
}

//
// DecideBufferSize
//
// This has to be present to override the PURE virtual class base function
//
HRESULT CAudioMixerOutputPin::DecideBufferSize(IMemAllocator *pAlloc,
										ALLOCATOR_PROPERTIES * pProperties)
{
	logger.Log(TEXT("CAudioMixerOutputPin::DecideBufferSize"));

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

#define NUMBER_OF_BUFFERS 3

	pProperties->cBuffers = NUMBER_OF_BUFFERS;

	//But we need input pin's allocator buffer size and the input type 
	//to determine the buffer size required for us

	int wBitsPerSample = 16;
	int sampleRate = 48000;
	int outputChannels = 2;

	// Set The value of output bitrate
	DWORD DestByteSizeOfSamples = wBitsPerSample / 8;
	DWORD DestNumberOfChannels =outputChannels;
	DWORD SourceByteSizeOfSamples = wBitsPerSample / 8;
	DWORD SourceNumberOfChannels = outputChannels;

	// Size of each buffer in bytes. /2 = Default to 500ms buffer duration.
	pProperties->cbBuffer = BUFFER_SIZE;//DestByteSizeOfSamples*DestNumberOfChannels*sampleRate/2;//25

	LOG((TEXT("Output buffer size: %d"), pProperties->cbBuffer));

	// Keep this number to the filter.
	//m_pTee->SetOutputBufferSize(pProperties->cbBuffer);
	// Create empty buffers for all of the pins
	if(m_pFilter)
		m_pFilter->CreateInputBuffers();

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

HRESULT	CAudioMixerOutputPin::GetMediaType(int iPosition, CMediaType *pMediaType)
{
	CAutoLock lock_it(m_pLock);

	logger.Log(TEXT("CAudioMixerOutputPin::GetMediaType"));

	
	return m_pFilter->GetMediaType(iPosition, pMediaType);
}

//
// CheckMediaType
//
HRESULT CAudioMixerOutputPin::CheckMediaType(const CMediaType *pmt)
{
	if(!AreWeConnected())
		return VFW_E_NOT_CONNECTED;

	CAutoLock lock_it(m_pLock);

	logger.Log(TEXT("CAudioMixerOutputPin::CheckInputType"));

	// This filter always has the output media type as input media type.
	return m_pFilter->CheckOutputType(pmt);
} // CheckMediaType
//
// AreWeConnected - is this filter connected
//
BOOL CAudioMixerOutputPin::AreWeConnected()
{
	if(!m_pFilter)
		return FALSE;

	// Loop through all of the input pins - first is the output pin
	for(int i=0; i < m_pFilter->m_iInPinsAmt; i++)
	{
		CAudioMixerInputPin *pPin = m_pFilter->GetInPin(i);
		// Make sure that we have at least one input pin connected
		if(pPin && pPin->IsConnected())
			return TRUE;
	}
	
	// No input pins are connected
	return FALSE;
}
//
// Active
//
// This is called when we start running or go paused. We create the
// output queue object to send data to our associated peer pin
//
HRESULT CAudioMixerOutputPin::Active()
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
HRESULT CAudioMixerOutputPin::Inactive()
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
HRESULT CAudioMixerOutputPin::Deliver(IMediaSample *pMediaSample)
{
	CheckPointer(pMediaSample,E_POINTER);
	CAutoLock lock_it(m_pLock);

	// Make sure that we have an output queue
	if(m_pOutputQueue == NULL)
		return NOERROR;

	LONGLONG rtStart, rtStop;
	pMediaSample->GetTime(&rtStart, &rtStop);
	LOG((TEXT("Deliver: rtStart: %d[ms], rtStop: %d[ms]"), long(rtStart/10000), long(rtStop/10000)) );

	HRESULT hr = m_pOutputQueue->Receive(pMediaSample);


	return hr;
} // Deliver

//
// DeliverEndOfStream
//
HRESULT CAudioMixerOutputPin::DeliverEndOfStream()
{
	// Signal EOS downstream, when all input pins are EOS
	if(!m_pFilter->AreAllInputsEOS())
		return NOERROR;

	// Make sure that we have an output queue
	if(m_pOutputQueue == NULL)
		return NOERROR;

	m_pOutputQueue->EOS();

	return NOERROR;
} // DeliverEndOfStream


//
// DeliverBeginFlush
//
HRESULT CAudioMixerOutputPin::DeliverBeginFlush()
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
HRESULT CAudioMixerOutputPin::DeliverEndFlush()
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
HRESULT CAudioMixerOutputPin::DeliverNewSegment(REFERENCE_TIME tStart, 
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
CAudioMixerOutputPin::GetCapabilities(DWORD * pCapabilities)
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
CAudioMixerOutputPin::CheckCapabilities(DWORD * pCapabilities)
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
CAudioMixerOutputPin::IsFormatSupported(const GUID * pFormat)
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
CAudioMixerOutputPin::QueryPreferredFormat(GUID * pFormat)
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
CAudioMixerOutputPin::GetTimeFormat(GUID *pFormat)
{
	return QueryPreferredFormat(pFormat);
}

STDMETHODIMP 
CAudioMixerOutputPin::IsUsingTimeFormat(const GUID * pFormat)
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
CAudioMixerOutputPin::ConvertTimeFormat(
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
CAudioMixerOutputPin::SetTimeFormat(const GUID * pFormat)
{
	if(*pFormat == TIME_FORMAT_MEDIA_TIME)
		return S_OK;

	return E_NOTIMPL;
}

STDMETHODIMP 
CAudioMixerOutputPin::GetDuration(LONGLONG *pDuration)
{
	// Get the longest duration from all of the pins. Actually, we're
	// looking for duration upstream of our input pins.
	REFERENCE_TIME rtDuration = 0;
	if(!m_pFilter)
		return S_FALSE;

	POSITION pos = m_pFilter->m_lstInPins.GetHeadPosition();
	while(pos)
	{
		CAudioMixerInputPin* pInput = m_pFilter->m_lstInPins.GetNext(pos);

		IMediaSeeking* pSeek = NULL;
		IPin* pPin = NULL;
		pPin = pInput->GetConnected();
		if(!pPin)
			continue;
		pPin->QueryInterface(IID_IMediaSeeking, (void**)&pSeek);
		if(pSeek)
		{
			REFERENCE_TIME rtThis;
			pSeek->GetDuration(&rtThis);
			if(rtThis > rtDuration)
				rtDuration = rtThis;

			pSeek->Release();
		}
	}
	*pDuration = rtDuration;
	return S_OK;
}

STDMETHODIMP 
CAudioMixerOutputPin::GetStopPosition(LONGLONG *pStop)
{
	REFERENCE_TIME tStop = 0;
	if(!m_pFilter)
		return S_FALSE;

	POSITION pos = m_pFilter->m_lstInPins.GetHeadPosition();
	while(pos)
	{
		CAudioMixerInputPin* pInput = m_pFilter->m_lstInPins.GetNext(pos);

		IMediaSeeking* pSeek = NULL;
		IPin* pPin = NULL;
		pPin = pInput->GetConnected();
		if(!pPin)
			continue;
		pPin->QueryInterface(IID_IMediaSeeking, (void**)&pSeek);
		if(pSeek)
		{
			REFERENCE_TIME rtThis;
			pSeek->GetStopPosition(&rtThis);
			if(rtThis > tStop)
				tStop = rtThis;

			pSeek->Release();
		}
	}

	*pStop = tStop;

	return S_OK;
}

STDMETHODIMP 
CAudioMixerOutputPin::GetCurrentPosition(LONGLONG *pCurrent)
{
	// this method is not supposed to report the previous start
	// position, but rather where we are now. This is normally
	// implemented by renderers, not parsers
	return E_NOTIMPL;
}

STDMETHODIMP 
CAudioMixerOutputPin::SetPositions(
										LONGLONG * pCurrent, 
										DWORD dwCurrentFlags, 
										LONGLONG * pStop, 
										DWORD dwStopFlags)
{
	// We don't support this
	return E_NOTIMPL;

	try
	{

		dwCurrentFlags &= AM_SEEKING_PositioningBitsMask;
		dwStopFlags &= AM_SEEKING_PositioningBitsMask;

		if(dwCurrentFlags)
		{
			POSITION pos = m_pFilter->m_lstInPins.GetHeadPosition();
			while(pos)
			{
				CAudioMixerInputPin* pInput = m_pFilter->m_lstInPins.GetNext(pos);

				IMediaSeeking* pSeek = NULL;
				IPin* pPin = NULL;
				pPin = pInput->GetConnected();
				if(!pPin)
					continue;
				pPin->QueryInterface(IID_IMediaSeeking, (void**)&pSeek);
				if(pSeek)
				{
					pSeek->SetPositions(pCurrent, dwCurrentFlags, pStop, dwStopFlags);
					pSeek->Release();
				}
			}
		}
		else
		{
			return S_FALSE;
		}
	}
	catch(...)
	{
		LOG((TEXT("SetPositions FAILED")));
	}



	return S_OK;
}

STDMETHODIMP 
CAudioMixerOutputPin::GetPositions(LONGLONG * pCurrent, LONGLONG * pStop)
{
	REFERENCE_TIME tStop=0;
	if(!m_pFilter)
		return S_FALSE;

	POSITION pos = m_pFilter->m_lstInPins.GetHeadPosition();
	while(pos)
	{
		CAudioMixerInputPin* pInput = m_pFilter->m_lstInPins.GetNext(pos);

		IMediaSeeking* pSeek = NULL;
		IPin* pPin = NULL;
		pPin = pInput->GetConnected();
		if(!pPin)
			continue;
		pPin->QueryInterface(IID_IMediaSeeking, (void**)&pSeek);
		if(pSeek)
		{
			REFERENCE_TIME tempStop;
			pSeek->GetPositions(pCurrent, &tempStop);
			if(tempStop > tStop)
				tStop = tempStop;

			pSeek->Release();
		}
	}

	*pStop = tStop;

	return S_OK;
}

STDMETHODIMP 
CAudioMixerOutputPin::GetAvailable(LONGLONG * pEarliest, LONGLONG * pLatest)
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
CAudioMixerOutputPin::SetRate(double dRate)
{
	if(!m_pFilter)
		return S_FALSE;

	POSITION pos = m_pFilter->m_lstInPins.GetHeadPosition();
	while(pos)
	{
		CAudioMixerInputPin* pInput = m_pFilter->m_lstInPins.GetNext(pos);

		IMediaSeeking* pSeek = NULL;
		IPin* pPin = NULL;
		pPin = pInput->GetConnected();
		if(!pPin)
			continue;
		pPin->QueryInterface(IID_IMediaSeeking, (void**)&pSeek);
		if(pSeek)
		{
			pSeek->SetRate(dRate);
			pSeek->Release();
		}
	}
	return S_OK;
}



STDMETHODIMP 
CAudioMixerOutputPin::GetRate(double * pdRate)
{
	if(!m_pFilter)
		return S_FALSE;

	POSITION pos = m_pFilter->m_lstInPins.GetHeadPosition();
	while(pos)
	{
		CAudioMixerInputPin* pInput = m_pFilter->m_lstInPins.GetNext(pos);

		IMediaSeeking* pSeek = NULL;
		IPin* pPin = NULL;
		pPin = pInput->GetConnected();
		if(!pPin)
			continue;
		pPin->QueryInterface(IID_IMediaSeeking, (void**)&pSeek);
		if(pSeek)
		{
			pSeek->GetRate(pdRate);
			pSeek->Release();
			break;
		}
	}


	return S_OK;
}

STDMETHODIMP 
CAudioMixerOutputPin::GetPreroll(LONGLONG * pllPreroll)
{
	// don't need to allow any preroll time for us
	*pllPreroll = 0;
	return S_OK;
}
