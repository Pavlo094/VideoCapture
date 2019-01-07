#include "AudioChannelSplitterInputPin.h"


//-----------------------------------------------------------------------------
// Construct and Deconstruct
//-----------------------------------------------------------------------------
CAudioChannelSplitterInputPin::CAudioChannelSplitterInputPin(LPCTSTR lpszObjName, CAudioChannelSplitter *pFilter, HRESULT *phr, LPCWSTR pName) :
	CBaseInputPin((LPTSTR)lpszObjName, pFilter, &pFilter->m_csFilter, phr, pName),
		m_pFilter(pFilter),
		m_cOurRef(0),
		IsEOS(false)
{
	
}

CAudioChannelSplitterInputPin::~CAudioChannelSplitterInputPin()
{
	
}
HRESULT CAudioChannelSplitterInputPin::CompleteConnect(IPin *pReceivePin)
{
	CAutoLock lock_it(m_pLock);
	ASSERT(m_Connected == pReceivePin);
	HRESULT hr = NOERROR;

	CBaseInputPin::CompleteConnect(pReceivePin);

	// Create the output pins.
	hr = m_pFilter->CreateOutputPins();

	return hr;
}

HRESULT CAudioChannelSplitterInputPin::BeginFlush(void)
{
	HRESULT hr = NOERROR;

	if(!m_pFilter)
		return hr;

	hr = CBaseInputPin::BeginFlush();
	
	m_pFilter->DeliverBeginFlushOnOutputPins();

	return hr;
}

HRESULT CAudioChannelSplitterInputPin::EndFlush(void)
{
	HRESULT hr = NOERROR;

	if(!m_pFilter)
		return hr;

	// Should this be here?
	m_pFilter->DeliverEndFlushOnOutputPins();
	hr = CBaseInputPin::EndFlush();

	return hr;
}
//
// EndOfStream
//
HRESULT CAudioChannelSplitterInputPin::EndOfStream()
{
	if(!IsConnected())
		return S_FALSE;

	if(!m_pFilter)
		return S_FALSE;

	IsEOS = true;
	// Signal EOS to output pins.
	return m_pFilter->DeliverEndOfStreamOnOutputPins();

} // EndOfStream

//
// NewSegment
//                   
HRESULT CAudioChannelSplitterInputPin::NewSegment(REFERENCE_TIME tStart,
											 REFERENCE_TIME tStop,
											 double dRate)
{
	CAutoLock lock_it(m_pLock);

	HRESULT hr = CBaseInputPin::NewSegment(tStart, tStop, dRate);

	IsEOS = false;
	return m_pFilter->DeliverNewSegmentOnOutputPins(tStart, tStop, dRate);
} // NewSegment


HRESULT CAudioChannelSplitterInputPin::Receive(IMediaSample *pSample)
{
	HRESULT hr = NOERROR;

	hr = CBaseInputPin::Receive(pSample);
	if(FAILED(hr))
		return hr;

	// Ping Filter.
	if(m_pFilter)
		m_pFilter->OnInputSample(pSample);

	return hr;
}
HRESULT CAudioChannelSplitterInputPin::CheckMediaType(const CMediaType *pmt)
{
	return m_pFilter->CheckInputType(pmt);
}