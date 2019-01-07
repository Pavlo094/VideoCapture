#ifndef _CAudioMixerOutputPin_H__INCLUDED_
#define _CAudioMixerOutputPin_H__INCLUDED_

#include "AudioMixer.h"

class CAudioMixerOutputPin : public CBaseOutputPin, public IMediaSeeking
{
	friend class CAudioMixer;
	CAudioMixer *m_pFilter;                  // Main filter object pointer
public:
	DECLARE_IUNKNOWN
	// Override to expose IMediaPosition
	STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, void **ppvoid);

	// Constructor and destructor
	CAudioMixerOutputPin(LPCTSTR pObjName, CAudioMixer *pFilter, HRESULT *phr, LPCWSTR pName);
	~CAudioMixerOutputPin();

	HRESULT CheckMediaType(const CMediaType *pmt);
	HRESULT	GetMediaType(int iPosition, CMediaType *pMediaType);
	HRESULT DecideBufferSize(IMemAllocator *pAlloc, ALLOCATOR_PROPERTIES * pProp);

	// Used to create output queue objects
	HRESULT Active();
	HRESULT Inactive();

	// Overriden to pass data to the output queues
	HRESULT Deliver(IMediaSample *pMediaSample);
	HRESULT DeliverEndOfStream();
	HRESULT DeliverBeginFlush();
	HRESULT DeliverEndFlush();
	HRESULT DeliverNewSegment(
		REFERENCE_TIME tStart,
		REFERENCE_TIME tStop,
		double dRate);

private:
	BOOL AreWeConnected();
	COutputQueue *m_pOutputQueue;  // Streams data to the peer pin

	
	// IMediaSeeking
public:
	STDMETHODIMP GetCapabilities(DWORD * pCapabilities );
	STDMETHODIMP CheckCapabilities(DWORD * pCapabilities );
	STDMETHODIMP IsFormatSupported(const GUID * pFormat);
	STDMETHODIMP QueryPreferredFormat(GUID * pFormat);
	STDMETHODIMP GetTimeFormat(GUID *pFormat);
	STDMETHODIMP IsUsingTimeFormat(const GUID * pFormat);
	STDMETHODIMP SetTimeFormat(const GUID * pFormat);
	STDMETHODIMP GetDuration(LONGLONG *pDuration);
	STDMETHODIMP GetStopPosition(LONGLONG *pStop);
	STDMETHODIMP GetCurrentPosition(LONGLONG *pCurrent);
	STDMETHODIMP ConvertTimeFormat(LONGLONG * pTarget, const GUID * pTargetFormat,
		LONGLONG    Source, const GUID * pSourceFormat );
	STDMETHODIMP SetPositions(LONGLONG * pCurrent, DWORD dwCurrentFlags
		, LONGLONG * pStop, DWORD dwStopFlags );
	STDMETHODIMP GetPositions(LONGLONG * pCurrent,
		LONGLONG * pStop );
	STDMETHODIMP GetAvailable(LONGLONG * pEarliest, LONGLONG * pLatest );
	STDMETHODIMP SetRate(double dRate);
	STDMETHODIMP GetRate(double * pdRate);
	STDMETHODIMP GetPreroll(LONGLONG * pllPreroll);
}; 

#endif // #ifndef _CAudioMixerOutputPin_H__INCLUDED_