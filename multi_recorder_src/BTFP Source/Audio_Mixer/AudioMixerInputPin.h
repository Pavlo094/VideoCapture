#ifndef _CAudioMixerInputPin_H__INCLUDED_
#define _CAudioMixerInputPin_H__INCLUDED_

#include "AudioMixer.h"
#include "ringbuffer.h"

class CAudioMixer;

// Class CCatAuMixInputPin
class CAudioMixerInputPin : public CBaseInputPin
{
	//typedef CGenericList <IMediaSample> InSamplesList;
public:
	DECLARE_IUNKNOWN;
	CAudioMixerInputPin(LPCTSTR lpszObjName, CAudioMixer *pFilter, HRESULT *phr, LPCWSTR pName);
	~CAudioMixerInputPin();

	BOOL			IsConnected() const {return (m_Connected!=NULL);}
	IMemAllocator*	PeekAllocator() const {return m_pAllocator; }

	// overide
	HRESULT			CompleteConnect(IPin *pReceivePin);
	// Override since the life time of pins and filters are not the same
	STDMETHODIMP_(ULONG) NonDelegatingAddRef();
	STDMETHODIMP_(ULONG) NonDelegatingRelease();

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
	CAudioMixer *m_pFilter;
	LONG m_cOurRef;					// reference count
	
	OutRingBuffer *m_pRingBuffer;
	wchar_t* m_pinName;
	
public:
	//InSamplesList m_lstSamples;				// samples list
	bool IsSampleReady();
	void GetBuffer(BYTE* pBuffer, LONG cbSize);
	void CreateBuffer();						// Create Input Pin Buffer
	void ClearBuffer();							// Clear buffer
	LONG BufferSize();							// Number of bytes written to the input pin buffer
	bool IsEOS;									// Is end of stream reached for this input pin.
};

#endif // #ifndef _CAudioMixerInputPin_H__INCLUDED_
