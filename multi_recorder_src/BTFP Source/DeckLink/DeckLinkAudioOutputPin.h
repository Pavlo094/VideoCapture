#pragma once

#include <amfilter.h>
#include <source.h>
#include <dvdmedia.h>

#include "Filter_h.h"
#include "PropertySet.h"
#include "DeckLinkAPI_h.h"
#include "DeckLinkVideoDevice.h"
#include <winnt.h>
#include <list>
#include <memory>
#include <mtype.h>

class CDeckLinkSource;

class CDeckLinkAudioOutputPin :
	public CBaseOutputPin
{

public:
	CDeckLinkAudioOutputPin(CDeckLinkSource *pFilter, HRESULT * phr);
	~CDeckLinkAudioOutputPin();

	DECLARE_IUNKNOWN_WOQ;

public:
	virtual HRESULT CheckMediaType(const CMediaType *pMediaType) override;
	virtual HRESULT GetMediaType(int iPosition, CMediaType *pMediaType) override;
	virtual HRESULT DecideBufferSize(IMemAllocator *pAlloc, ALLOCATOR_PROPERTIES *pProp) override;


	STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, void **ppv);
	
	//================================================================================
	// IQualityControl methods
	//================================================================================

	STDMETHODIMP Notify(IBaseFilter * pSender, Quality q);

public:
	HRESULT UpdateFormat();

	HRESULT ProcessFrame(IDeckLinkAudioInputPacket *audioPacket);

private:
	virtual HRESULT FillBuffer(IMediaSample* pSample, IDeckLinkAudioInputPacket *videoFrame);

private:
	CMediaType	m_mediaType;

	int m_iFrameNumber;					// number of frames delivered
	REFERENCE_TIME m_rtFrameLength;		// frame duration in 100ns units
	REFERENCE_TIME m_rtNextValidStart;

	CComPtr<KsPropertySetImpl>			m_pKeyProperty;	
	CDeckLinkSource*			m_pSource;

	static const long channelsCount = 8;
	long long drops;
};