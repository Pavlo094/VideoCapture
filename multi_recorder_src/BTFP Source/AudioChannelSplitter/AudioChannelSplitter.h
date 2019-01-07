#ifndef _AUDIOCHANNELSPLITTER_H__INCLUDED_
#define _AUDIOCHANNELSPLITTER_H__INCLUDED_


#include <stdio.h>
#include <streams.h>


#include "AudioChannelSplitterOutputPin.h"
#include "AudioChannelSplitterInputPin.h"
#include "iAudioChannelSplitter.h"

#define BUFFER_SIZE		96000
#define AUDIO_VOLUME	0.5

#define SafeRelese(p) { if (p) p->Release(); p = NULL; }
#define SafeDelete(p) { if(p) delete p; p= NULL; }
#define SafeDeleteArray(p) { if(p) { delete[] (p); (p)=NULL; } }
#define SafeCloseHandle(p) { if(p) CloseHandle(p); p = 0; }

EXTERN_C const GUID CLSID_AudioChannelSplitter;

class CAudioChannelSplitter : public CBaseFilter, public IAudioChannelSplitter
{
	friend class CAudioChannelSplitterInputPin;
	friend class CAudioChannelSplitterOutputPin;
	typedef CGenericList <CAudioChannelSplitterOutputPin> OutPinsList;
public:
    static CUnknown* WINAPI CreateInstance(LPUNKNOWN pUnk, HRESULT *pHr);
	DECLARE_IUNKNOWN;

	// Reveals IStereoSplitter
    STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, void ** ppv);
	
	CAudioChannelSplitter(LPCTSTR lpszName, LPUNKNOWN pUnk, REFCLSID clsid, 
		HRESULT* phr, int iMaxInPinAmt = 16);
	~CAudioChannelSplitter();

	HRESULT		CheckInputType(const CMediaType* mtIn);
	HRESULT		CheckOutputType(const CMediaType * mtOut);
	HRESULT		GetMediaType(int iPosition, CMediaType *pMediaType);
	int			GetPinCount(void);
	CBasePin*	GetPin(int n);

	STDMETHODIMP Stop();

protected:
	CCritSec m_csFilter;
	CCritSec m_csReceive;

	CAudioChannelSplitterInputPin* m_pInputPin;	// Input pin;
	// Pins list manage 
	OutPinsList	m_lstOutPins;		// List of the pins
	int			m_iMaxOutPinsAmt;	// Max pins amount, default to 16

	CMediaType* m_pCurMediaType;	// Connection media type. Determined by first connected Input pin.

	REFERENCE_TIME m_curTime;

	AudioChannelSplitterParameters m_params;

	int m_numberOfInputChannels;
	int m_bitsPerSample;

	int GetOutputStereoChannel(int inputCh);

protected:
	// Pins list manage
	CAudioChannelSplitterOutputPin*	GetOutPin(int n);
	void ClearOutPinsList();
	HRESULT CreateOutputPins();

	void OnInputSample(IMediaSample * pSample);
	void DeliverBeginFlushOnOutputPins();
	void DeliverEndFlushOnOutputPins();
	HRESULT DeliverEndOfStreamOnOutputPins();
	HRESULT DeliverNewSegmentOnOutputPins(REFERENCE_TIME tStart, REFERENCE_TIME tStop, double dRate);

	void DeleteOutPin(CAudioChannelSplitterOutputPin *pOutPin);

	long CopyChannelData(BYTE * pSrc, long cbSrcSize, long bps, long channels, BYTE * pDest, int outChcounter);

public:
	// These implement the custom IStereoSplitter interface
	STDMETHODIMP get_Params(AudioChannelSplitterParameters *irp);
	STDMETHODIMP put_Params(AudioChannelSplitterParameters irp);
};


#endif // _AUDIOCHANNELSPLITTER_H__INCLUDED_