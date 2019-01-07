#pragma once

#include "combase.h"
#include "VideoDevice_h.h"
#include "BaseVideoDeviceMode.h"

#include <vector>

class CBaseVideoDeviceModeIterator;

class CBaseVideoDevice :	
	public IVideoDevice,
	public CUnknown
{
	
friend class CBaseVideoDeviceModeIterator;

public:
	CBaseVideoDevice(LPUNKNOWN pUnk);
	virtual ~CBaseVideoDevice(void);

public: //CUnknown
	DECLARE_IUNKNOWN;

public: // IVideoDevice
	virtual HRESULT STDMETHODCALLTYPE IsVideoModeSupported( 
		/* [in] */ IVideoDeviceMode *videoMode) override;

	virtual HRESULT STDMETHODCALLTYPE UpdateVideoModes( void) override;

	virtual HRESULT STDMETHODCALLTYPE GetModesIterator( 
		/* [out] */ IVideoDeviceModeIterator **pIterator) override;

	virtual HRESULT STDMETHODCALLTYPE SetVideoMode( 
		/* [in] */ IVideoDeviceMode *videoMode) override;

	virtual HRESULT STDMETHODCALLTYPE GetCurrentVideoMode( 
		/* [out] */ IVideoDeviceMode **pVideoMode) override;

	virtual HRESULT STDMETHODCALLTYPE GetDeviceName( 
		/* [out] */ BSTR *name) override;

protected:
	HRESULT		GetVideoModeAt(const UINT index, IVideoDeviceMode **ppMode);
	const UINT	GetVideoModeCount(void) const;

	HRESULT		AddVideoMode(IVideoDeviceMode *pMode);

private:
	std::vector<CAdapt<CComPtr<IVideoDeviceMode>>>	m_vModes; 
	CComPtr<CBaseVideoDeviceMode>	m_pVideoMode;
};


class CBaseVideoDeviceModeIterator :	
	public IVideoDeviceModeIterator,
	public CUnknown
{
public:
	CBaseVideoDeviceModeIterator(LPUNKNOWN pUnk, CBaseVideoDevice *pOwner);
	virtual ~CBaseVideoDeviceModeIterator(void);

public: //CUnknown
	DECLARE_IUNKNOWN;

public:
    virtual HRESULT STDMETHODCALLTYPE Next( 
        /* [out] */ IVideoDeviceMode **pVideoMode);

private:
	CComPtr<CBaseVideoDevice>	m_pVideoDevice;
	ULONG						m_ulCurrentPos;
};