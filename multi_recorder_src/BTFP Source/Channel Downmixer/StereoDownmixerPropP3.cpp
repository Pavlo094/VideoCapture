// StereoDownmixerProp.h: implementation of CStereoDownmixerPropP3 class
// 
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"

#include <streams.h>
#include <commctrl.h>
#include <olectl.h>
#include <memory.h>
#include <stdlib.h>
#include <stdio.h>
#include <tchar.h>

#include "resource.h"
#include "iStereoDownmixer.h"
#include "StereoDownmixer.h"
#include "StereoDownmixerPropP3.h"

#include <ks.h>
#include <ksmedia.h>

///////////////////////////////////////////////////////////////////////
// CreateInstance: Used by the DirectShow base classes to create 
// instances
///////////////////////////////////////////////////////////////////////
CUnknown *CStereoDownmixerPropP3::CreateInstance(LPUNKNOWN lpunk, HRESULT *phr)
{
    CUnknown *punk = new CStereoDownmixerPropP3(lpunk, phr);
    if (punk == NULL) {
    *phr = E_OUTOFMEMORY;
    }
    return punk;

}

///////////////////////////////////////////////////////////////////////
// Constructor
///////////////////////////////////////////////////////////////////////
CStereoDownmixerPropP3::CStereoDownmixerPropP3(LPUNKNOWN pUnk, HRESULT *phr) :
    CBasePropertyPage(NAME("StereoDownmixer Property Page"),
                      pUnk,IDD_PROPERTIES_PAGE3,IDS_TITLE_PROPERTY_PAGE3),
    m_pIStereoDownmixer(NULL),
    m_bIsInitialized(FALSE)
{

    ASSERT(phr);
}

CStereoDownmixerPropP3::~CStereoDownmixerPropP3()
{

}

///////////////////////////////////////////////////////////////////////
// OnReceiveMessage: Handles the messages for our property window
///////////////////////////////////////////////////////////////////////
BOOL CStereoDownmixerPropP3::OnReceiveMessage(HWND hwnd,
                                          UINT uMsg,
                                          WPARAM wParam,
                                          LPARAM lParam)
{
    switch (uMsg)
    {
        case WM_INITDIALOG:  // init property page
			OnInitDialog();
            break;

        case WM_COMMAND:
        {
			if (!m_bIsInitialized) return 1;

			if (HIWORD(wParam)==BN_CLICKED && LOWORD(wParam)==IDC_BUTTON_RELOAD_MATRIX_NOW)
			{	
				OnClickedReloadMatrixNow();
				return 1;
			}
			else if (HIWORD(wParam)==BN_CLICKED && LOWORD(wParam)==IDC_BUTTON_BROWSE)
			{	
				OnClickedBrowse();
				return 1;
			}
			else if (
				   HIWORD(wParam)==EN_CHANGE
				|| (HIWORD(wParam)==BN_CLICKED && LOWORD(wParam)==IDC_RADIO_USE_INTERNAL_MATRIX)
				|| (HIWORD(wParam)==BN_CLICKED && LOWORD(wParam)==IDC_RADIO_USE_CUSTOM_MATRIX)
				)
			{			
				// Set dirty to indicate the setting has changed
				UpdateView();
				m_bDirty = TRUE;
				if (m_pPageSite)
					m_pPageSite->OnStatusChange(PROPPAGESTATUS_DIRTY);
			}
			
			return 1;
        }
		
    }
    return CBasePropertyPage::OnReceiveMessage(hwnd,uMsg,wParam,lParam);
}

void CStereoDownmixerPropP3::OnInitDialog()
{
    // Get the current setup of the filter
    m_pIStereoDownmixer->GetParams(&m_StereoDownmixerParams,StereoDownmixerParams::SectionChoose::mixmatrix);
    
	// Set the control to the current setup
	SetControlValues();
	UpdateView();
	m_bIsInitialized = TRUE;
}

///////////////////////////////////////////////////////////////////////
// OnConnect: Called when we connect to a transform filter
///////////////////////////////////////////////////////////////////////
HRESULT CStereoDownmixerPropP3::OnConnect(IUnknown *pUnknown)
{
    ASSERT(m_pIStereoDownmixer == NULL);

    HRESULT hr = pUnknown->QueryInterface(IID_IStereoDownmixer, (void **) &m_pIStereoDownmixer);
    if (FAILED(hr)) {
        return E_NOINTERFACE;
    }

    ASSERT(m_pIStereoDownmixer);
    m_bIsInitialized = FALSE ;

    return NOERROR;
}

///////////////////////////////////////////////////////////////////////
// OnDisconnect: Likewise called when we disconnect from a filter
///////////////////////////////////////////////////////////////////////
HRESULT CStereoDownmixerPropP3::OnDisconnect()
{
    if (m_pIStereoDownmixer == NULL) {
        return E_UNEXPECTED;
    }

    m_pIStereoDownmixer->Release();
    m_pIStereoDownmixer = NULL;
    return NOERROR;
}

///////////////////////////////////////////////////////////////////////
// OnActivate: We are being activated
///////////////////////////////////////////////////////////////////////
HRESULT CStereoDownmixerPropP3::OnActivate()
{
    ASSERT(m_pIStereoDownmixer);

    return NOERROR;
}

///////////////////////////////////////////////////////////////////////
// OnDeactivate: We are being deactivated
///////////////////////////////////////////////////////////////////////
HRESULT CStereoDownmixerPropP3::OnDeactivate(void)
{
    ASSERT(m_pIStereoDownmixer);
    m_bIsInitialized = FALSE;
    
    return NOERROR;
}

///////////////////////////////////////////////////////////////////////
// OnApplyChanges: Apply any changes so far made
///////////////////////////////////////////////////////////////////////
HRESULT CStereoDownmixerPropP3::OnApplyChanges()
{
    ASSERT(m_pIStereoDownmixer);

    // Update the setup from controls and set it into the filter
	
	GetControlValues();
	
	HRESULT hres = m_pIStereoDownmixer->SetParams(&m_StereoDownmixerParams,StereoDownmixerParams::SectionChoose::mixmatrix);
	if (FAILED(hres))
	{
		MessageBox(m_Dlg,"Could not open specified file for loading the custom matrix.\nPlease check that the filename and path are correct.\nMatrix not loaded/changed.","Error in opening matrix file",MB_OK|MB_ICONEXCLAMATION);
		return hres;
	}

	return NOERROR;
}

void CStereoDownmixerPropP3::UpdateView()
{
	BOOL bUseCustomMixMatrix = BST_CHECKED==IsDlgButtonChecked(m_Dlg,IDC_RADIO_USE_CUSTOM_MATRIX);

	EnableWindow(GetDlgItem(m_Dlg,IDC_EDIT_CUSTOM_MATRIX_FULL_FILENAME),bUseCustomMixMatrix);
	EnableWindow(GetDlgItem(m_Dlg,IDC_BUTTON_BROWSE),bUseCustomMixMatrix);
	EnableWindow(GetDlgItem(m_Dlg,IDC_BUTTON_RELOAD_MATRIX_NOW),bUseCustomMixMatrix);
}

///////////////////////////////////////////////////////////////////////
// GetControlValues: read the setup from current controls
///////////////////////////////////////////////////////////////////////
void CStereoDownmixerPropP3::GetControlValues()
{
	m_StereoDownmixerParams.p3.bCustomMixMatrix = BST_CHECKED==IsDlgButtonChecked(m_Dlg,IDC_RADIO_USE_CUSTOM_MATRIX);
	GetWindowText(GetDlgItem(m_Dlg,IDC_EDIT_CUSTOM_MATRIX_FULL_FILENAME),m_StereoDownmixerParams.p3.CustomMixMatrixFilename,MAX_PATH);
}

///////////////////////////////////////////////////////////////////////
// SetControlValues: set the current setup to the controls
///////////////////////////////////////////////////////////////////////
void CStereoDownmixerPropP3::SetControlValues()
{
	CheckRadioButton(m_Dlg,
		IDC_RADIO_USE_INTERNAL_MATRIX,
		IDC_RADIO_USE_CUSTOM_MATRIX,
		m_StereoDownmixerParams.p3.bCustomMixMatrix ? IDC_RADIO_USE_CUSTOM_MATRIX : IDC_RADIO_USE_INTERNAL_MATRIX
		);
	SetWindowText(GetDlgItem(m_Dlg,IDC_EDIT_CUSTOM_MATRIX_FULL_FILENAME),m_StereoDownmixerParams.p3.CustomMixMatrixFilename);

	UpdateView(); //So we enable / disable in the right way all the controls which depend on the radio buttons we have just set
}

void CStereoDownmixerPropP3::OnClickedReloadMatrixNow()
{
	OnApplyChanges();
}

void CStereoDownmixerPropP3::OnClickedBrowse()
{
	char startfilename[MAX_PATH];
	char startpath[MAX_PATH];

	GetWindowText(GetDlgItem(m_Dlg,IDC_EDIT_CUSTOM_MATRIX_FULL_FILENAME),startfilename,MAX_PATH);
	strcpy(startpath,startfilename);
	

	char *ptr=startpath;

	//Cut the part after the last slash to get the path
	while (*ptr) ptr++; //Get to the end
	if (*(ptr-1)!='/' || *(ptr-1)!='\\') //We already have a path written!
	{
		while (ptr > startpath && *ptr!='\\' && *ptr!='/') ptr--; //Go to the last slash in the string and cut it.
		*ptr='\0'; 
	}

	OPENFILENAME ofn;
	memset(&ofn,0,sizeof(ofn));

	ofn.lStructSize=sizeof(ofn);
	ofn.hwndOwner=m_Dlg;
    ofn.hInstance=NULL;
    ofn.lpstrFilter="Mix-matrix text files (*.mixmatrix)\0*.mixmatrix\0All files (*.*)\0*.*\0\0"; 
    ofn.lpstrCustomFilter=NULL;
    ofn.nMaxCustFilter=0;
    ofn.nFilterIndex=0;
    ofn.lpstrFile=startfilename;
    ofn.nMaxFile=MAX_PATH;
    ofn.lpstrFileTitle=NULL; //I only want the full-path version
    ofn.nMaxFileTitle=0;
    ofn.lpstrInitialDir=startpath;
    ofn.lpstrTitle="Choose your Mix-matrix file";
    ofn.Flags=OFN_ENABLESIZING|OFN_FILEMUSTEXIST|OFN_HIDEREADONLY|OFN_LONGNAMES|OFN_PATHMUSTEXIST|OFN_READONLY;
    ofn.nFileOffset=0; //Not an input value
    ofn.nFileExtension=0; //Idem
    ofn.lpstrDefExt=NULL;
    ofn.lCustData=0;
    ofn.lpfnHook=NULL;
    ofn.lpTemplateName=NULL;

	GetOpenFileName(&ofn);

	SetWindowText(GetDlgItem(m_Dlg,IDC_EDIT_CUSTOM_MATRIX_FULL_FILENAME),ofn.lpstrFile);
}





