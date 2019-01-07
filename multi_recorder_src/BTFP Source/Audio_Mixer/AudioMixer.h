#ifndef _CAudioMixer_H__INCLUDED_
#define _CAudioMixer_H__INCLUDED_


#include <stdio.h>
#include <streams.h>


#include "AudioMixerOutputPin.h"
#include "AudioMixerInputPin.h"

#include "AudioMixer_i.h"

#define BUFFER_SIZE		192000//96000
#define AUDIO_VOLUME	0.6

#define SafeRelese(p) { if (p) p->Release(); p = NULL; }
#define SafeDelete(p) { if(p) delete p; p= NULL; }
#define SafeDeleteArray(p) { if(p) { delete[] (p); (p)=NULL; } }
#define SafeCloseHandle(p) { if(p) CloseHandle(p); p = 0; }

EXTERN_C const GUID CLSID_AudioMixer;

class CAudioMixer : public CBaseFilter, public IAudioMixer
{
	friend class CAudioMixerInputPin;
	friend class CAudioMixerOutputPin;
	typedef CGenericList <CAudioMixerInputPin> InPinsList;
public:
    static CUnknown* WINAPI CreateInstance(LPUNKNOWN pUnk, HRESULT *pHr);
	// Query interface
	STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, void ** ppv);
	DECLARE_IUNKNOWN;
	
	CAudioMixer(LPCTSTR lpszName, LPUNKNOWN pUnk, REFCLSID clsid, 
		HRESULT* phr, int iMaxInPinAmt = 16);
	~CAudioMixer();

	HRESULT		CheckInputType(const CMediaType* mtIn);
	HRESULT		GetMediaType(int iPosition, CMediaType *pMediaType);
	int			GetPinCount(void);
	CBasePin*	GetPin(int n);

	STDMETHODIMP Stop();

	STDMETHODIMP SetVolume(double volume);

protected:
	// Pins list manage
	void				ClearInPinsList();
	CAudioMixerInputPin*	GetInPin(int n);
	HRESULT				CreateNextInPin();
	void				DeleteInPin(CAudioMixerInputPin *pInPin);
	int					GetFreeInPinsAmt();

protected:
	CCritSec m_csFilter;
	CCritSec m_csReceive;

	CAudioMixerOutputPin* m_pOutPin;	// Output pin;
	// Pins list manage 
	InPinsList	m_lstInPins;		// List of the pins
	int			m_iMaxInPinsAmt;	// Max pins amount, default to 4
	int			m_iInPinsAmt;		// Current input pin count
	int			m_iNextInPinNum;	// Increases monotonically.

	CMediaType* m_pCurMediaType;	// Connection media type. Determined by first connected Input pin.

	REFERENCE_TIME m_curTime;

	double m_Volume;

protected:
	void OnInputSample();
	bool AreAllInputPinsFilled();
	bool AreAllInputsEOS();
	void CreateInputBuffers();
	void ClearInputBuffers();
	void MixInput(BYTE* pDest, LONG cbSize, LONG* actSize);
	CMediaType MakeOutputCMediaTypeFromParams(const CMediaType *InputMediaType = NULL);
	HRESULT		CheckOutputType(const CMediaType* mtOut);
};


#endif // _CAudioMixer_H__INCLUDED_