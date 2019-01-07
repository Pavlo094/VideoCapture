#include "AudioMixerInputPin.h"
#include "logger.h"

//-----------------------------------------------------------------------------
// Construct and Deconstruct
//-----------------------------------------------------------------------------
CAudioMixerInputPin::CAudioMixerInputPin(LPCTSTR lpszObjName, CAudioMixer *pFilter, HRESULT *phr, LPCWSTR pName) :
	CBaseInputPin((LPTSTR)lpszObjName, pFilter, &pFilter->m_csFilter, phr, pName),
		m_pFilter(pFilter),
		m_cOurRef(0),
		m_pRingBuffer(NULL),
		IsEOS(false)
{
	logger.Log(TEXT("CAudioMixerInputPin::Constructor: %s"), pName);
	m_pinName = new wchar_t[100];
	wcscpy(m_pinName, pName);
}

CAudioMixerInputPin::~CAudioMixerInputPin()
{
	logger.Log(TEXT("CAudioMixerInputPin::Destructor"));

	if(m_pRingBuffer)
	{
		rb_clear(m_pRingBuffer);
		delete [] m_pRingBuffer->buffer;
		delete m_pRingBuffer;
		m_pRingBuffer = NULL;
	}
	if(m_pinName)
		delete [] m_pinName;
}

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
STDMETHODIMP_(ULONG) CAudioMixerInputPin::NonDelegatingAddRef()
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
STDMETHODIMP_(ULONG) CAudioMixerInputPin::NonDelegatingRelease()
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

	if(m_cOurRef <= 1)
	{
		int n = 2;                     // default forces pin deletion
		if(m_cOurRef == 1)
		{
			// Walk the list of pins, looking for count of free pins
			n = m_pFilter->GetFreeInPinsAmt();
		}

		// If there are two free pins, delete this one.
		// NOTE: normall
		if(n >= 2)
		{
			m_cOurRef = 0;
#ifdef DEBUG
			m_cRef = 0;
#endif
			m_pFilter->DeleteInPin(this);
			return(ULONG) 0;
		}
	}

	return(ULONG) m_cOurRef;

} // NonDelegatingRelease

HRESULT CAudioMixerInputPin::CompleteConnect(IPin *pReceivePin)
{
	logger.Log(TEXT("CAudioMixerInputPin::CompleteConnect"));

	CAutoLock lock_it(m_pLock);
	ASSERT(m_Connected == pReceivePin);
	HRESULT hr = NOERROR;

	CBaseInputPin::CompleteConnect(pReceivePin);

	if(!m_pRingBuffer)
		CreateBuffer();

	// If there are no free input pins, then creat one.
	// Creation condition is in function CAudioMixer::CreateNextInPin()
	if (m_pFilter->GetFreeInPinsAmt() == 0)
		hr = m_pFilter->CreateNextInPin();

	return hr;
}

HRESULT CAudioMixerInputPin::BeginFlush(void)
{
	HRESULT hr = NOERROR;
	logger.Log(TEXT("CAudioMixerInputPin::BeginFlush"));

	if(!m_pFilter)
		return hr;

	hr = CBaseInputPin::BeginFlush();
	logger.Log(TEXT("CAudioMixerInputPin::BeginFlush. Pin: %s"), m_pinName);
	
	m_pFilter->m_pOutPin->DeliverBeginFlush();

	return hr;
}

HRESULT CAudioMixerInputPin::EndFlush(void)
{
	HRESULT hr = NOERROR;

	logger.Log(TEXT("CAudioMixerInputPin::EndFlush"));

	if(!m_pFilter)
		return hr;

	logger.Log(TEXT("CAudioMixerInputPin::EndFlush. Pin: %s"), m_pinName);
	//m_pFilter->m_pOutPin->DeliverEndFlush();
	hr = CBaseInputPin::EndFlush();

	return hr;
}
//
// EndOfStream
//
HRESULT CAudioMixerInputPin::EndOfStream()
{
	if(!IsConnected())
		return S_FALSE;

	if(!m_pFilter)
		return S_FALSE;

	logger.Log(TEXT("CAudioMixerInputPin::EndOfStream. Pin: %s"), m_pinName);
	IsEOS = true;
	// Signal EOS to output pin. Output pin will wait for all input pins to signal EOS.
	return m_pFilter->m_pOutPin->DeliverEndOfStream();

} // EndOfStream

//
// NewSegment
//                   
HRESULT CAudioMixerInputPin::NewSegment(REFERENCE_TIME tStart,
											 REFERENCE_TIME tStop,
											 double dRate)
{
	CAutoLock lock_it(m_pLock);

	HRESULT hr = CBaseInputPin::NewSegment(tStart, tStop, dRate);

	logger.Log(TEXT("CAudioMixerInputPin::NewSegment. Pin: %s, tStart: %d[ms], tStop: %d[ms]"),
										m_pinName, long(tStart/10000), long(tStop/10000));
	IsEOS = false;
	return m_pFilter->m_pOutPin->DeliverNewSegment(tStart, tStop, dRate);
} // NewSegment


HRESULT CAudioMixerInputPin::Receive(IMediaSample *pSample)
{
	HRESULT hr = NOERROR;

	hr = CBaseInputPin::Receive(pSample);
	if(FAILED(hr))
		return hr;

	if(m_pRingBuffer)
	{
		// Get the sample data
		BYTE* pData = NULL;
		LONG cbSize = pSample->GetActualDataLength();
		pSample->GetPointer(&pData);

		// Check if we have available storage in the ring buffer
		// If not block this call
		LONG cbFree = (LONG) rb_free(m_pRingBuffer);
		if(cbFree < cbSize)
		{
			do
			{
				if(m_bFlushing || m_pFilter->m_State == State_Stopped)
					break;

				// Wait for other pins to deliver too
				Sleep(10);
				// Ping Filter. Eventually it will require buffer from us
				// and it will free some data so we can write - hopefully!
				if(m_pFilter)
					m_pFilter->OnInputSample();

				cbFree = (LONG) rb_free(m_pRingBuffer);
			}while(cbFree < cbSize);
		}

		if(m_bFlushing || m_pFilter->m_State == State_Stopped)
			return S_FALSE;

		// WE have storage now - write
		rb_write(m_pRingBuffer, pData, cbSize);
	}

	// Ping Filter.
	if(m_pFilter)
		m_pFilter->OnInputSample();

	return hr;
}
HRESULT CAudioMixerInputPin::CheckMediaType(const CMediaType *pmt)
{
	//logger.Log(TEXT("CAudioMixerInputPin::CheckMediaType"));

	return m_pFilter->CheckInputType(pmt);
}
void CAudioMixerInputPin::CreateBuffer()
{
	if(m_pRingBuffer)
	{
		rb_clear(m_pRingBuffer);
		delete [] m_pRingBuffer->buffer;
		delete m_pRingBuffer;
		m_pRingBuffer = NULL;
	}

	rb_init(&m_pRingBuffer, 144000);
}
void CAudioMixerInputPin::ClearBuffer()
{
	if(m_pRingBuffer)
		rb_clear(m_pRingBuffer);

	logger.Log(TEXT("CAudioMixerInputPin::ClearBuffer. Pin: %s, Free Buffer: %d"), m_pinName, rb_free(m_pRingBuffer));
}
bool CAudioMixerInputPin::IsSampleReady()
{
	if(!m_pRingBuffer)
		return false;

	if(IsEOS)
		return true;

	return rb_data_size(m_pRingBuffer) >= BUFFER_SIZE;
}
void CAudioMixerInputPin::GetBuffer(BYTE * pBuffer, LONG cbSize)
{
	if(!m_pRingBuffer)
		return;

	rb_read(m_pRingBuffer, pBuffer, cbSize);
}
LONG CAudioMixerInputPin::BufferSize()
{
	if(!m_pRingBuffer)
		return 0;

	
	return (LONG)rb_data_size(m_pRingBuffer);
}