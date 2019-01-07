#ifndef _AudioChannelSplitterInputPin_H__INCLUDED_
#define _AudioChannelSplitterInputPin_H__INCLUDED_

#include "AudioChannelSplitter.h"

class CAudioChannelSplitter;

// Class CStereoSplitterInputPin
class CAudioChannelSplitterInputPin : public CBaseInputPin
{
public:
	DECLARE_IUNKNOWN;
	CAudioChannelSplitterInputPin(LPCTSTR lpszObjName, CAudioChannelSplitter *pFilter, HRESULT *phr, LPCWSTR pName);
	~CAudioChannelSplitterInputPin();

	BOOL			IsConnected() const {return (m_Connected!=NULL);}
	IMemAllocator*	PeekAllocator() const {return m_pAllocator; }

	// overide
	HRESULT			CompleteConnect(IPin *pReceivePin);

	// Pass through calls downstream
	STDMETHODIMP BeginFlush(void);
	STDMETHODIMP EndFlush(void);
	STDMETHODIMP EndOfStream();
	STDMETHODIMP NewSegment(
		REFERENCE_TIME tStart,
		REFERENCE_TIME tStop,
		double dRate);
	STDMETHODIMP Receive(IMediaSample *pSample);
	STDMETHODIMP ReceiveCanBlock(){return S_OK;}
	//HRESULT GetMediaType(int iPosition,	CMediaType *pMediaType);
	HRESULT CheckMediaType(const CMediaType *pmt);
protected:
	CAudioChannelSplitter *m_pFilter;
	LONG m_cOurRef;					// reference count
	
public:
	bool IsEOS;									// Is end of stream reached for this input pin.
};

#endif // #ifndef _AudioChannelSplitterInputPin_H__INCLUDED_
