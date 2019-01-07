#pragma once
#include "BaseVideoDevice.h"
#include "DeckLinkAPI_h.h"

class CDeckLinkVideoModeIterator;
class CDeckLinkVideoDevice;
class CDeckLinkVideoMode;
class CDecklinkCallback;

interface 
	__declspec(uuid("e00a89ea-719b-4582-9bfb-1add3152e9ee")) 
	__declspec(novtable)
IDeckLinkVideoMode: public IUnknown
{
	virtual HRESULT GetDeckLinkDisplayMode(IDeckLinkDisplayMode **ppMode) = 0;
	virtual HRESULT GetBMDPixelFormat(BMDPixelFormat *bmdPixelFormat) = 0;
};

class CDeckLinkVideoDevice :
	public CBaseVideoDevice
{
friend CDeckLinkVideoModeIterator;

public:
	CDeckLinkVideoDevice(LPUNKNOWN pUnk, IDeckLink *pDevice);
	virtual ~CDeckLinkVideoDevice(void);

public: //CUnknown
	DECLARE_IUNKNOWN;

public:
	virtual	HRESULT STDMETHODCALLTYPE IsVideoModeSupported( 
			/* [in] */ IVideoDeviceMode *videoMode) override;
	virtual HRESULT STDMETHODCALLTYPE GetModesIterator( 
		/* [out] */ IVideoDeviceModeIterator **pIterator) override;
	virtual HRESULT STDMETHODCALLTYPE SetVideoMode( 
		/* [in] */ IVideoDeviceMode *videoMode) override;

	virtual HRESULT STDMETHODCALLTYPE GetCurrentVideoMode( 
		/* [out] */ IVideoDeviceMode **pVideoMode) override;

	virtual HRESULT STDMETHODCALLTYPE GetDeviceName( 
		/* [out] */ BSTR *name) override;

public:
	virtual HRESULT STDMETHODCALLTYPE Start(IDeckLinkInputCallback* pCallback, IDeckLinkScreenPreviewCallback* previewCallBack = nullptr);

	virtual HRESULT STDMETHODCALLTYPE Stop();

	virtual HRESULT STDMETHODCALLTYPE ValidateBufferFullness();

protected:
	CComPtr<IDeckLink>			m_pDevice;
	CComQIPtr<IDeckLinkInput>	m_pDeviceInput;

private:
	CComPtr<IVideoDeviceMode>	m_pCurrentVideoMode;
};

class CDeckLinkVideoMode:
	public IDeckLinkVideoMode,
	public CBaseVideoDeviceMode
{
public:
	CDeckLinkVideoMode(LPUNKNOWN pUnk, IDeckLinkDisplayMode *pMode, BMDPixelFormat bmdPixelFormat,
		EStereoMode eStereoMode = smDefault);
	virtual ~CDeckLinkVideoMode();

public: //CUnknown
	DECLARE_IUNKNOWN;

	STDMETHODIMP NonDelegatingQueryInterface(REFIID, void **);

public:
	HRESULT GetDeckLinkDisplayMode(IDeckLinkDisplayMode **ppMode) override;
	HRESULT GetBMDPixelFormat(BMDPixelFormat *bmdPixelFormat) override;

private: 
	CComPtr<IDeckLinkDisplayMode>	m_pDisplayMode;
	BMDPixelFormat					m_bmdPixelFormat;

};


class CDeckLinkVideoModeIterator :	
	public IVideoDeviceModeIterator,
	public CUnknown
{
public:
	CDeckLinkVideoModeIterator(CDeckLinkVideoDevice *pDevice);
	virtual ~CDeckLinkVideoModeIterator(void);

public: //CUnknown
	DECLARE_IUNKNOWN;

public:
    virtual HRESULT STDMETHODCALLTYPE Next( 
        /* [out] */ IVideoDeviceMode **pVideoMode);

private:
	CComPtr<CDeckLinkVideoDevice>			m_pVideoDevice;
	CComQIPtr<IDeckLinkInput>				m_pDeviceInput;
	CComPtr<IDeckLinkDisplayModeIterator>	m_pIterator;
	CComPtr<IDeckLinkDisplayMode>			m_pDisplayMode;

	UINT	m_uiVideoInputFlag;
	UINT	m_uiPixelFormat;

	static const BMDPixelFormat ePixelFormats[];
	static const BMDVideoInputFlags eVideoInputFlags[];
	static const UINT uiVideoInputFlagsCount;
	static const UINT uiPixelFormatsCount;

};