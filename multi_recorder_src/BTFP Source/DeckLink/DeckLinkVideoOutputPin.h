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

class CDeckLinkVideoOutputPin :
	public CBaseOutputPin,
	public IKsPropertySet 
{

public:
	CDeckLinkVideoOutputPin(CDeckLinkSource *pFilter, HRESULT * phr, bool forceYV12 = false, GUID pinCategory = PIN_CATEGORY_CAPTURE);
	~CDeckLinkVideoOutputPin();

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

	//IKsPropertySet : public IUnknown
public:
    virtual /* [local] */ HRESULT STDMETHODCALLTYPE Set( 
        /* [in] */ REFGUID guidPropSet,
        /* [in] */ DWORD dwPropID,
        /* [annotation][size_is][in] */ 
        _In_reads_bytes_(cbInstanceData)  LPVOID pInstanceData,
        /* [in] */ DWORD cbInstanceData,
        /* [annotation][size_is][in] */ 
        _In_reads_bytes_(cbPropData)  LPVOID pPropData,
        /* [in] */ DWORD cbPropData);
        
    virtual /* [local] */ HRESULT STDMETHODCALLTYPE Get( 
        /* [in] */ REFGUID guidPropSet,
        /* [in] */ DWORD dwPropID,
        /* [annotation][size_is][in] */ 
        _In_reads_bytes_(cbInstanceData)  LPVOID pInstanceData,
        /* [in] */ DWORD cbInstanceData,
        /* [annotation][size_is][out] */ 
        _Out_writes_bytes_to_(cbPropData, *pcbReturned)  LPVOID pPropData,
        /* [in] */ DWORD cbPropData,
        /* [annotation][out] */ 
        _Out_  DWORD *pcbReturned);
        
    virtual HRESULT STDMETHODCALLTYPE QuerySupported( 
        /* [in] */ REFGUID guidPropSet,
        /* [in] */ DWORD dwPropID,
        /* [annotation][out] */ 
        _Out_  DWORD *pTypeSupport);

public:
	// Used to create output queue objects
	HRESULT Active() override;
	HRESULT Inactive() override;

	// Overriden to pass data to the output queues
	HRESULT Deliver(IMediaSample *pMediaSample) override;

public:
	HRESULT UpdateFormat();

	HRESULT ProcessFrame(IDeckLinkVideoInputFrame *videoFrame, bool canWait);

	long long getDrops() const;
private:
	virtual HRESULT FillBuffer(IMediaSample* pSample, IDeckLinkVideoInputFrame *videoFrame);
	virtual LONG getWidth();
	virtual LONG getHeight();

private:
	CMediaType	m_mediaType;

	CComPtr<KsPropertySetImpl>			m_pKeyProperty;	
	CDeckLinkSource*			m_pSource;
	long long drops;
	bool forceYV12;
	GUID pinCategory;
	BMDTimeValue prevFrameTime;
	std::unique_ptr<COutputQueue> m_pOutputQueue; 
};