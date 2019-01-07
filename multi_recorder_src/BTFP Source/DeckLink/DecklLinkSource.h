#pragma once
#include <amfilter.h>
#include <source.h>
#include <dvdmedia.h>

#include "Filter_h.h"
#include "PropertySet.h"
#include "DeckLinkAPI_h.h"
#include "DeckLinkVideoDevice.h"
#include <winnt.h>
#include <vector>
#include <memory>
#include "DeckLinkVideoOutputPin.h"
#include "DeckLinkAudioOutputPin.h"

extern AMOVIESETUP_FILTER sudDeckLinkHDStereoSource;

class CDeckLinkSource :
	public CBaseFilter,
	public ISpecifyPropertyPages,
	public IPersistPropertyBag,
	public IDeckLinkInputCallback,
	public IDeckLinkScreenPreviewCallback,
	public IVideoDeviceHolder,
	public IDeckLinkSourceConfig,
	public IAMDroppedFrames
{

friend CDeckLinkVideoOutputPin;
friend CDeckLinkAudioOutputPin;

public:
	CDeckLinkSource(IUnknown * pUnk, HRESULT * phr );
	~CDeckLinkSource();

public:
	//DECLARE_IUNKNOWN;

	STDMETHODIMP QueryInterface(REFIID riid, void **ppv) {      \
        return GetOwner()->QueryInterface(riid,ppv);            \
    };                                                          

	STDMETHODIMP_(ULONG) AddRef();                                                          
    STDMETHODIMP_(ULONG) Release();

	static CUnknown * WINAPI CreateInstance(LPUNKNOWN lpunk, HRESULT *phr);

	STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, void** ppv);

public:
	int GetPinCount() override;
	CBasePin *GetPin(int n) override;

	STDMETHODIMP Stop() override;
	STDMETHODIMP Pause() override;
	
	STDMETHODIMP Run(REFERENCE_TIME tStart) override;

	STDMETHODIMP GetState(DWORD dwMSecs, FILTER_STATE *State) override;

public: // ISpecifyPropertyPages
	STDMETHODIMP GetPages(CAUUID *pPages);

public: //IPersistPropertyBag
  virtual HRESULT STDMETHODCALLTYPE InitNew( void);

  virtual HRESULT STDMETHODCALLTYPE Load( 
	  /* [in] */ __RPC__in_opt IPropertyBag *pPropBag,
	  /* [unique][in] */ __RPC__in_opt IErrorLog *pErrorLog);

  virtual HRESULT STDMETHODCALLTYPE Save( 
	  /* [in] */ __RPC__in_opt IPropertyBag *pPropBag,
	  /* [in] */ BOOL fClearDirty,
	  /* [in] */ BOOL fSaveAllProperties);
  virtual HRESULT STDMETHODCALLTYPE GetClassID( 
	  /* [out] */ CLSID *pClassID);

public: // IVideoDeviceHolder
	virtual HRESULT STDMETHODCALLTYPE LockDevice( 
		IVideoDevice **ppDevide);

	virtual HRESULT STDMETHODCALLTYPE UnLockDevice( void);

public:
  virtual HRESULT STDMETHODCALLTYPE Save( void);

//IDeckLinkInputCallback : public IUnknown
public:
	virtual HRESULT STDMETHODCALLTYPE VideoInputFormatChanged( 
		/* [in] */ BMDVideoInputFormatChangedEvents notificationEvents,
		/* [in] */ IDeckLinkDisplayMode *newDisplayMode,
		/* [in] */ BMDDetectedVideoInputFormatFlags detectedSignalFlags) override;

	virtual HRESULT STDMETHODCALLTYPE VideoInputFrameArrived( 
		/* [in] */ IDeckLinkVideoInputFrame *videoFrame,
		/* [in] */ IDeckLinkAudioInputPacket *audioPacket) override;

//IDeckLinkScreenPreviewCallback : public IUnknown
public:
    virtual HRESULT STDMETHODCALLTYPE DrawFrame( 
		/* [in] */ IDeckLinkVideoFrame *theFrame) override;

//IDeckLinkSourceConfig : public IUnknown
public:
    virtual HRESULT STDMETHODCALLTYPE SetDeviceName( 
		/* [in] */ BSTR name) override;
        
    virtual HRESULT STDMETHODCALLTYPE SetVideoMode( 
        /* [in] */ int width,
        /* [in] */ int height,
		/* [in] */ long long avgFrameDuration,
		/* [in] */ int fieldOrder) override;

//IAMDroppedFrames : public IUnknown
public:
    virtual HRESULT STDMETHODCALLTYPE GetNumDropped( 
        /* [annotation][out] */ 
		_Out_  long *plDropped) override;
        
    virtual HRESULT STDMETHODCALLTYPE GetNumNotDropped( 
        /* [annotation][out] */ 
        _Out_  long *plNotDropped) override;
        
    virtual HRESULT STDMETHODCALLTYPE GetDroppedInfo( 
        /* [in] */ long lSize,
        /* [annotation][out] */ 
        _Out_  long *plArray,
        /* [annotation][out] */ 
        _Out_  long *plNumCopied) override;
        
    virtual HRESULT STDMETHODCALLTYPE GetAverageFrameSize( 
        /* [annotation][out] */ 
        _Out_  long *plAverageSize) override;
public:
   virtual HRESULT STDMETHODCALLTYPE IsPageDirty( void);

protected:
	HRESULT UpdateDevice();

protected:
	CComPtr<CDeckLinkVideoDevice>	m_pDeckLinkDevice;
	CCritSec						m_csDevice;
	CCritSec						m_cStateLock;	// Lock this to serialize function accesses to the filter state

private:	
	std::vector<CBasePin*>			m_pPins;
	std::unique_ptr<CDeckLinkVideoOutputPin>	m_pVideoPin;
	std::unique_ptr<CDeckLinkAudioOutputPin>	m_pAudioPin;
	std::unique_ptr<CDeckLinkVideoOutputPin>	m_pVideoPinPreview;

	CString m_sDeviceName;	

	CComPtr<IPropertyBag> m_pPropertyBag;
};

static const LPCTSTR DEVICENAME		= TEXT("Device Name");
static const LPCTSTR SWIDTH			= TEXT("width");
static const LPCTSTR SHEIGHT		= TEXT("height");
static const LPCTSTR BPP			= TEXT("bpp");
static const LPCTSTR PIXELFORMAT	= TEXT("pixelformat");
static const LPCTSTR FRAMERATE		= TEXT("framerate");
static const LPCTSTR FIELDORDER		= TEXT("fieldorder");
static const LPCTSTR STEREOMODE		= TEXT("stereomode");