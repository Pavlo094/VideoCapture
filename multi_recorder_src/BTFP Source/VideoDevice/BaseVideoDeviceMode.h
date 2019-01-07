#pragma once

#include "combase.h"
#include "VideoDevice_h.h"

class CBaseVideoDeviceMode :	
	public IVideoDeviceMode,
	public CUnknown
{
public:
	CBaseVideoDeviceMode(LPUNKNOWN pUnk);
	virtual ~CBaseVideoDeviceMode(void);
	CBaseVideoDeviceMode(IVideoDeviceMode *pMode);

public: //CUnknown
	DECLARE_IUNKNOWN;

	STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, void **ppv);
public: //IVideoDeviceMode
	virtual const int STDMETHODCALLTYPE GetWidth( void) ;

	virtual const int STDMETHODCALLTYPE GetHeight( void) ;

	virtual HRESULT STDMETHODCALLTYPE SetWidth( 
		/* [in] */ const int width);

	virtual HRESULT STDMETHODCALLTYPE SetHeight( 
		/* [in] */ const int height);

	virtual const int STDMETHODCALLTYPE GetBpp( void) ;

	virtual HRESULT STDMETHODCALLTYPE SetBpp( 
		/* [in] */ const int bpp);

	virtual EPixelFormat STDMETHODCALLTYPE GetPixelFormat( void) ;

	virtual HRESULT STDMETHODCALLTYPE SetPixelFormat( 
		/* [in] */ const EPixelFormat pixelFormat);

	virtual EFieldOrder STDMETHODCALLTYPE GetFieldOrder( void) ;

	virtual HRESULT STDMETHODCALLTYPE SetFieldOrder( 
		/* [in] */ const EFieldOrder fieldOrder);

	virtual HRESULT STDMETHODCALLTYPE SetTimeUnitsPerSecond( 
		/* [in] */ const DWORD unitsCount);

	virtual const DWORD STDMETHODCALLTYPE GetFrameDuration( void) ;

	virtual const double STDMETHODCALLTYPE GetFramesPerSecond( void) ;

	virtual HRESULT STDMETHODCALLTYPE SetFrameDuration( 
		/* [in] */ const DWORD frameDuration);

	virtual HRESULT STDMETHODCALLTYPE SetFramesPerSecond( 
		/* [in] */ const DWORD framesCount);

	virtual EStereoMode STDMETHODCALLTYPE GetStereoMode( void) ;

	virtual HRESULT STDMETHODCALLTYPE SetStereoMode( 
		/* [in] */ const EStereoMode stereoMode);

	virtual EVideoConnectionType STDMETHODCALLTYPE GetConnectionType( void) ;

	virtual HRESULT STDMETHODCALLTYPE SetConnectionType( 
		/* [in] */ const EVideoConnectionType connectionType);

	virtual HRESULT STDMETHODCALLTYPE Compare( 
            /* [in] */ IVideoDeviceMode *pMode);

private:
	int			m_iWidth;
	int			m_iHeight;
	int			m_iBpp;

	EPixelFormat			m_ePixelFormat;
	EFieldOrder				m_eFieldOrder;
	EStereoMode				m_eStereoMode;
	EVideoConnectionType	m_eVideoConnectionType;

	DWORD		m_dwUnitsCount;
	DWORD		m_dwFrameDuration;
	double		m_dFramesPerSecond;
	bool		m_bFramesCountMode;


};