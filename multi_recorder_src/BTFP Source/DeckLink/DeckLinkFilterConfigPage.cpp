#include "stdafx.h"
#include "DeckLinkFilterConfigPage.h"
#include <map>
#include "DeckLinkVideoDevice.h"

CDeckLinkFilterConfigPage::~CDeckLinkFilterConfigPage(void)
{
}

HRESULT CDeckLinkFilterConfigPage::OnConnect(IUnknown *pUnknown)
{		
	CheckPointer(pUnknown,E_POINTER);
	// Ask the renderer for it's IQualProp interface

	m_pDevideHolder = NULL;
	HRESULT hr = pUnknown->QueryInterface(IID_IVideoDeviceHolder, (void **)&m_pDevideHolder);
	if (FAILED(hr)) return E_NOINTERFACE;

	CheckPointer(m_pDevideHolder, E_POINTER);

	m_pVideDevice = NULL;
	m_pDevideHolder->LockDevice(&m_pVideDevice);
	if (SUCCEEDED(hr) && !m_pVideDevice)
	{		
		m_pDevideHolder->UnLockDevice();
		return E_POINTER;
	}
	m_pVideDeviceMode = NULL;
	m_pVideDevice->GetCurrentVideoMode(&m_pVideDeviceMode);
	return NOERROR;

}  // OnConnect

HRESULT CDeckLinkFilterConfigPage::OnDisconnect()
{			
	if (m_pDevideHolder && m_pVideDevice)
	{
		m_pVideDevice = NULL;
		m_pDevideHolder->UnLockDevice();
	}
	m_pVideDevice = NULL;
	m_pDevideHolder = NULL;
	return NOERROR;
}  // OnDisconnect


HRESULT CDeckLinkFilterConfigPage::OnActivate()
{
	HRESULT hr = S_OK;

	m_comboVideoMode.Attach(::GetDlgItem(m_Dlg, IDC_RESOLUTION));
	m_comboVideoMode.SetDroppedWidth(10);

	CheckPointer(m_pVideDevice, E_POINTER);

	CComPtr<IVideoDeviceModeIterator> pIterator;
	hr = E_POINTER;
	if FAILED(hr = m_pVideDevice->GetModesIterator(&pIterator) || !pIterator)
	{
		return hr;
	}

	return UpdateFormat();

}//OnActivate

HRESULT CDeckLinkFilterConfigPage::OnDeactivate()
{

	m_comboVideoMode.Detach();

	return NOERROR;
}

INT_PTR CDeckLinkFilterConfigPage::OnReceiveMessage(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_INITDIALOG:
		{			
			LRESULT res = CBasePropertyPage::OnReceiveMessage(hwnd,uMsg,wParam,lParam);
			return res;
		}

	case WM_COMMAND:
		{
			if ((LOWORD(wParam) == IDC_RESOLUTION))
			{
				if ((HIWORD(wParam) == CBN_SELCHANGE))
				{
					UpdateCurrentMode();
				}
			}
			if ((LOWORD(wParam) == IDC_FRAMERATE) ||
				(LOWORD(wParam) == IDC_FORMAT) ||
				(LOWORD(wParam) == IDC_FIELDORDER) ||
				(LOWORD(wParam) == IDC_STEREOMODE))
			{
				if ((HIWORD(wParam) == CBN_SELCHANGE))
				{
					UpdateFormat();
				}
			}

			return (LRESULT) 1;
		}

	}
	return CBasePropertyPage::OnReceiveMessage(hwnd,uMsg,wParam,lParam);
}// OnReceiveMessage

HRESULT CDeckLinkFilterConfigPage::UpdateFormat()
{
	m_comboVideoMode.ResetContent();
	m_pModes.clear();

	CheckPointer(m_pVideDevice, E_POINTER);
	
	CComPtr<IVideoDeviceModeIterator> pIterator;
	HRESULT hr = E_POINTER;
	if FAILED(hr = m_pVideDevice->GetModesIterator(&pIterator) || !pIterator)
	{
		return hr;
	}

	int currentModeIndex = 0;
		
	CComPtr<IVideoDeviceMode> pVideoMode = NULL;
	int count = 0;
	while (SUCCEEDED(pIterator->Next(&pVideoMode)) && pVideoMode)
	{
		EStereoMode dwStereo = pVideoMode->GetStereoMode();
		if (dwStereo != smLeft)
		{
			pVideoMode = NULL;
			continue;
		}

		EPixelFormat dwFormat = pVideoMode->GetPixelFormat();
		switch (dwFormat)
		{		
		case pfBGRA:
		case pfV210:
		case pfR210:
			pVideoMode = NULL;
			continue;
		}
		
		m_comboVideoMode.InsertString(count, composeVideoModeName(pVideoMode).c_str());
		m_comboVideoMode.SetItemData(count, count);

		m_pModes.push_back(pVideoMode);
		if SUCCEEDED(pVideoMode->Compare(m_pVideDeviceMode))
		{
			currentModeIndex = count;
		}
		pVideoMode = NULL;
		count++;
	}

	if (count == 0)
	{
		m_comboVideoMode.InsertString(0, TEXT("none"));
		m_comboVideoMode.SetItemData(0, -1);
	}

	m_comboVideoMode.SetCurSel(currentModeIndex);
	return S_OK;
}

HRESULT CDeckLinkFilterConfigPage::UpdateCurrentMode()
{
	int index = m_comboVideoMode.GetItemData(m_comboVideoMode.GetCurSel());
	if (index > (int)m_pModes.size() ||
		index < 0)
	{
		return E_FAIL;
	}

	m_pVideDeviceMode = m_pModes.at(index);

	SetDirty();

	return S_OK;
}

HRESULT CDeckLinkFilterConfigPage::OnApplyChanges()
{
	CheckPointer(m_pVideDevice, E_POINTER);
	CheckPointer(m_pDevideHolder, E_POINTER);
	m_pVideDevice->SetVideoMode(m_pVideDeviceMode);
	m_pDevideHolder->Save();
	return NOERROR;
}// OnApplyChanges

std::wstring composeVideoModeName(IVideoDeviceMode* pVideoMode)
{
	CComQIPtr<IDeckLinkVideoMode> pModeDL = pVideoMode;
	CheckPointer(pModeDL, TEXT(""));
	CComPtr<IDeckLinkDisplayMode> pDisplayMode;
	pModeDL->GetDeckLinkDisplayMode(&pDisplayMode);
	_bstr_t modeName;
	pDisplayMode->GetName(modeName.GetAddress());	

	EPixelFormat dwFormat = pVideoMode->GetPixelFormat();
	std::wstring modeNameFull = modeName + " - ";
	switch (dwFormat)
	{							
	case pfYUV2:
		modeNameFull += TEXT("YUV");
		break;
	case pfYUYV:
		modeNameFull += TEXT("YUV");
		break;
	case pfUYVY:
		modeNameFull += TEXT("YUV");
		break;
	case pfV210:
		modeNameFull += TEXT("YUV");
		break;
	case pfARGB:
		modeNameFull += TEXT("ARGB");
		break;
	case pfBGRA:
		modeNameFull += TEXT("BGRA");
		break;
	case pfR210:
		modeNameFull += TEXT("R210");
		break;
	}
	
	return modeNameFull;
}