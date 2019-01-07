#pragma once

#include <streams.h>
#include <initguid.h>
#include <dvdmedia.h>
#include <comutil.h>
#include <combase.h>
#include "resource.h"
#include "VideoDevice_h.h"


#include <atlbase.h>
#include <atlcom.h>
#include <atlapp.h>
#include <atlctrls.h>
#include <atlstr.h>

#include <utility>
#include <vector>

// {B92911C9-1DF0-4a8c-BD30-CA7B3A2953E9}
DEFINE_GUID(CLSID_CDeckLinkFilterConfigPage, 
			0xb92911c9, 0x1df0, 0x4a8c, 0xbd, 0x30, 0xca, 0x7b, 0x3a, 0x29, 0x53, 0xe9);


class CDeckLinkFilterConfigPage : public CBasePropertyPage
{
public:
	CDeckLinkFilterConfigPage(LPUNKNOWN lpUnk, HRESULT *phr)
		:CBasePropertyPage(NAME("DeckLink Property Page\0"),lpUnk, IDD_VIDEOCONFIG, IDS_VIDEOCONFIG_TITLE)
	{
		ASSERT(phr);		
	};
	~CDeckLinkFilterConfigPage(void);

public:

	static CUnknown * WINAPI CreateInstance(LPUNKNOWN pUnk, HRESULT *phr)
	{
		ASSERT(phr);
		return new CDeckLinkFilterConfigPage(pUnk, phr);
	}

	virtual HRESULT OnConnect(IUnknown *pUnknown);
	
	virtual HRESULT OnDisconnect();

	virtual HRESULT OnActivate();	

	virtual HRESULT OnDeactivate();

	virtual INT_PTR OnReceiveMessage(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam);

	HRESULT UpdateCurrentMode();

	virtual HRESULT OnApplyChanges();

	void SetDirty()
	{
		m_bDirty = TRUE;

		if (m_pPageSite)
			m_pPageSite->OnStatusChange(PROPPAGESTATUS_DIRTY);

	} // SetDirty

	HRESULT UpdateFormat();

private:
	HWND		m_hwndDeviceCombo;


	CComPtr<IVideoDeviceHolder>	m_pDevideHolder;
	CComPtr<IVideoDevice>		m_pVideDevice;
	CComPtr<IVideoDeviceMode>	m_pVideDeviceMode;

	CComboBox	m_comboVideoMode;	// IDC_RESOLUTION

	std::vector<CComPtr<IVideoDeviceMode>> m_pModes;
};

std::wstring composeVideoModeName(IVideoDeviceMode* pVideoMode);