#include "StdAfx.h"
#include "BaseVideoDeviceMode.h"

const LONGLONG MILLISECONDS = (1000);            // 10 ^ 3
const LONGLONG NANOSECONDS = (1000000000);       // 10 ^ 9
const LONGLONG UNITS = (NANOSECONDS / 100);      // 10 ^ 7

CBaseVideoDeviceMode::CBaseVideoDeviceMode(LPUNKNOWN pUnk)
: CUnknown("BaseVideoDeviceMode", pUnk)
, m_iWidth(0)
, m_iHeight(0)
, m_iBpp(0)
, m_ePixelFormat(pfDefault)
, m_eFieldOrder(foDefault)
, m_eStereoMode(smDefault)
, m_eVideoConnectionType(vctDefault)
, m_dwUnitsCount(10000000)
, m_dwFrameDuration(0)
, m_dFramesPerSecond(0)
, m_bFramesCountMode(true)
{
}

CBaseVideoDeviceMode::CBaseVideoDeviceMode(IVideoDeviceMode *pMode)
: CUnknown("BaseVideoDeviceMode", NULL)
{
	m_iWidth = pMode->GetWidth();
	m_iHeight = pMode->GetHeight();
	m_iBpp = pMode->GetBpp();
	m_ePixelFormat = pMode->GetPixelFormat();
	m_eFieldOrder = pMode->GetFieldOrder();
	m_eStereoMode = pMode->GetStereoMode();
	m_eVideoConnectionType = pMode->GetConnectionType();	
	m_dwFrameDuration = pMode->GetFrameDuration();
	m_dFramesPerSecond = pMode->GetFramesPerSecond();
	SetFrameDuration(pMode->GetFrameDuration());
}

CBaseVideoDeviceMode::~CBaseVideoDeviceMode(void)
{
}

STDMETHODIMP CBaseVideoDeviceMode::NonDelegatingQueryInterface(REFIID riid, void **ppv)
{
	if(IsEqualGUID(riid, __uuidof(IVideoDeviceMode))) {
		return GetInterface(static_cast<IVideoDeviceMode*>(this), ppv);		
	}
	else 	
	{
		return CUnknown::NonDelegatingQueryInterface(riid, ppv);
	}
}



const int CBaseVideoDeviceMode::GetWidth(void) 
{
	return m_iWidth;
}

const int CBaseVideoDeviceMode::GetHeight(void) 
{
	return m_iHeight;
}

HRESULT CBaseVideoDeviceMode::SetWidth(const int width)
{
	m_iWidth = width;
	return S_OK;
}

HRESULT CBaseVideoDeviceMode::SetHeight(const int height)
{
	m_iHeight = height;
	return S_OK;
}

const int CBaseVideoDeviceMode::GetBpp(void) 
{
	return m_iBpp;
}

HRESULT	CBaseVideoDeviceMode::SetBpp(const int bpp)
{
	m_iBpp = bpp;
	return S_OK;
}

EPixelFormat CBaseVideoDeviceMode::GetPixelFormat(void) 
{
	return m_ePixelFormat;
}

HRESULT	CBaseVideoDeviceMode::SetPixelFormat(const EPixelFormat pixelFormat)
{
	m_ePixelFormat = pixelFormat;
	return S_OK;
}

EFieldOrder	CBaseVideoDeviceMode::GetFieldOrder(void) 
{
	return m_eFieldOrder;
}

HRESULT	CBaseVideoDeviceMode::SetFieldOrder(const EFieldOrder fieldOrder)
{
	m_eFieldOrder = fieldOrder;
	return S_OK;
}

HRESULT	CBaseVideoDeviceMode::SetTimeUnitsPerSecond(const DWORD unitsCount)
{
	m_dwUnitsCount = unitsCount;
	return S_OK;
}

const DWORD	CBaseVideoDeviceMode::GetFrameDuration(void) 
{
	if (m_bFramesCountMode)
	{
		if (m_dFramesPerSecond)
			return static_cast<DWORD>((m_dwUnitsCount) / (m_dFramesPerSecond));
		else
			return 0;
	}
	else
	{
		if (m_dwUnitsCount)
		{
			return ( UNITS * m_dwFrameDuration) / (m_dwUnitsCount);
		}
		else
		{
			return 0;
		}
	}	
}

const double	CBaseVideoDeviceMode::GetFramesPerSecond(void) 
{
	if (m_bFramesCountMode)
	{
		return m_dFramesPerSecond;
	}
	else
	{
		if (m_dwFrameDuration)
			return (double)m_dwUnitsCount / m_dwFrameDuration;
		else
			return 0;
	}	
}

HRESULT	CBaseVideoDeviceMode::SetFrameDuration(const DWORD frameDuration)
{
	m_dwFrameDuration = frameDuration;
	m_bFramesCountMode = false;
	return S_OK;
}

HRESULT	CBaseVideoDeviceMode::SetFramesPerSecond(const DWORD framesCount)
{
	m_dFramesPerSecond = framesCount;
	m_bFramesCountMode = true;
	return S_OK;
}

EStereoMode	CBaseVideoDeviceMode::GetStereoMode(void) 
{
	return m_eStereoMode;
}

HRESULT	CBaseVideoDeviceMode::SetStereoMode(const EStereoMode stereoMode)
{
	m_eStereoMode = stereoMode;
	return S_OK;
}

EVideoConnectionType CBaseVideoDeviceMode::GetConnectionType(void) 
{
	return m_eVideoConnectionType;
}

HRESULT	CBaseVideoDeviceMode::SetConnectionType(const EVideoConnectionType connectionType)
{
	m_eVideoConnectionType = connectionType;
	return S_OK;
}

HRESULT CBaseVideoDeviceMode::Compare( 
            /* [in] */ IVideoDeviceMode *pMode)
{
	CheckPointer(pMode, E_POINTER);
	if (
		m_iWidth == pMode->GetWidth() &&
		m_iHeight == pMode->GetHeight() &&
		m_iBpp == pMode->GetBpp() &&
		m_ePixelFormat == pMode->GetPixelFormat() &&
		m_eFieldOrder == pMode->GetFieldOrder() &&
		m_eStereoMode == pMode->GetStereoMode() &&
		m_eVideoConnectionType == pMode->GetConnectionType() &&	
			(
			GetFrameDuration() == pMode->GetFrameDuration() ||
			GetFramesPerSecond() == pMode->GetFramesPerSecond()
			)
		)
	{
		return S_OK;
	}
	return E_FAIL;
}