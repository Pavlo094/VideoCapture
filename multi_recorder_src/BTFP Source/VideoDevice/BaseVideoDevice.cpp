#include "StdAfx.h"
#include "BaseVideoDevice.h"

CBaseVideoDevice::CBaseVideoDevice(LPUNKNOWN pUnk)
: CUnknown("BaseVideoDevice", pUnk)
, m_pVideoMode(NULL)
{
}

CBaseVideoDevice::~CBaseVideoDevice(void)
{
	m_pVideoMode = NULL;
}

HRESULT CBaseVideoDevice::IsVideoModeSupported( 
		/* [in] */ IVideoDeviceMode *videoMode)
{
	auto iter = m_vModes.begin();
	for (; iter != m_vModes.end(); iter++)
	{
		if (S_OK == (*iter)->Compare(videoMode))
		{
			return S_OK;
		}
	}
	return E_NOINTERFACE;
}

HRESULT CBaseVideoDevice::GetModesIterator( 
		/* [out] */ IVideoDeviceModeIterator **pIterator)
{
	CheckPointer(pIterator, E_POINTER);

	*pIterator = new CBaseVideoDeviceModeIterator(0, this);

	if (*pIterator)
	{
		(*pIterator)->AddRef();
		return S_OK;
	}
	else
		return E_POINTER;
}

HRESULT CBaseVideoDevice::SetVideoMode( 
		/* [in] */ IVideoDeviceMode *videoMode)
{
	if (!IsVideoModeSupported(videoMode))
	{
		return E_FAIL;
	}

	CBaseVideoDeviceMode *ptr = new CBaseVideoDeviceMode(videoMode);
	m_pVideoMode = ptr;
	return S_OK;
}

HRESULT CBaseVideoDevice::GetCurrentVideoMode( 
		/* [out] */ IVideoDeviceMode **ppVideoMode)
{
	CheckPointer(ppVideoMode, E_POINTER);
	CheckPointer(m_pVideoMode, E_POINTER);
	
	return m_pVideoMode->QueryInterface(IID_PPV_ARGS(ppVideoMode));
}

HRESULT CBaseVideoDevice::GetDeviceName( 
		/* [out] */ BSTR *pName)
{
	CheckPointer(pName, E_POINTER);
	return S_OK;
}


HRESULT		CBaseVideoDevice::GetVideoModeAt(const UINT index, IVideoDeviceMode **ppMode)
{
	if (index >= GetVideoModeCount())
	{
		return E_ACCESSDENIED;
	}

	CheckPointer(ppMode, E_POINTER);

	return m_vModes.at(index).m_T.CopyTo(ppMode);
}

const UINT	CBaseVideoDevice::GetVideoModeCount(void) const
{	
	return (UINT)m_vModes.size();
}

HRESULT		CBaseVideoDevice::AddVideoMode(IVideoDeviceMode *pMode)
{
	CheckPointer(pMode, E_POINTER);

	m_vModes.push_back(CComPtr<IVideoDeviceMode>(pMode));

	return S_OK;
}

HRESULT CBaseVideoDevice::UpdateVideoModes( void)
{
	return S_OK;
}

CBaseVideoDeviceModeIterator::CBaseVideoDeviceModeIterator(LPUNKNOWN pUnk, CBaseVideoDevice *pOwner)
: CUnknown("BaseVideoDeviceModeIterator", pUnk)
, m_pVideoDevice(pOwner)
, m_ulCurrentPos(0)
{
}

CBaseVideoDeviceModeIterator::~CBaseVideoDeviceModeIterator()
{
	m_pVideoDevice = NULL;
}

HRESULT CBaseVideoDeviceModeIterator::Next(IVideoDeviceMode **pVideoMode)
{
	CheckPointer(pVideoMode, E_POINTER);
	if (*pVideoMode != NULL)
	{
		return E_FAIL;
	}


	if (m_pVideoDevice)
	{
		UINT uiModesCount = m_pVideoDevice->GetVideoModeCount();
		if (m_ulCurrentPos >= uiModesCount)
		{
			return E_FAIL;
		}
		return m_pVideoDevice->GetVideoModeAt(m_ulCurrentPos++, pVideoMode);
		
	}
	
	return E_FAIL;
}
