// StereoDownmixerProp.h: implementation of CStereoDownmixerPropP1 class
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
#include "StereoDownmixerPropP1.h"

#include <ks.h>
#include <ksmedia.h>

///////////////////////////////////////////////////////////////////////
// CreateInstance: Used by the DirectShow base classes to create 
// instances
///////////////////////////////////////////////////////////////////////
CUnknown *CStereoDownmixerPropP1::CreateInstance(LPUNKNOWN lpunk, HRESULT *phr)
{
    CUnknown *punk = new CStereoDownmixerPropP1(lpunk, phr);
    if (punk == NULL) {
    *phr = E_OUTOFMEMORY;
    }
    return punk;

}

///////////////////////////////////////////////////////////////////////
// Constructor
///////////////////////////////////////////////////////////////////////
CStereoDownmixerPropP1::CStereoDownmixerPropP1(LPUNKNOWN pUnk, HRESULT *phr) :
    CBasePropertyPage(NAME("StereoDownmixer Property Page"),
                      pUnk,IDD_PROPERTIES_PAGE1,IDS_TITLE_PROPERTY_PAGE1),
    m_pIStereoDownmixer(NULL),
    m_bIsInitialized(FALSE)
{
	m_MediaTypeEditBoxFont=CreateFont(14,0,0,0,FW_NORMAL,0,0,0,
		ANSI_CHARSET,OUT_TT_ONLY_PRECIS,CLIP_DEFAULT_PRECIS,PROOF_QUALITY,
		VARIABLE_PITCH|FF_SCRIPT,
		"Arial Narrow"
		);

    ASSERT(phr);
}

CStereoDownmixerPropP1::~CStereoDownmixerPropP1()
{
	DeleteObject(m_MediaTypeEditBoxFont);
	m_MediaTypeEditBoxFont=NULL;
}

///////////////////////////////////////////////////////////////////////
// OnReceiveMessage: Handles the messages for our property window
///////////////////////////////////////////////////////////////////////
BOOL CStereoDownmixerPropP1::OnReceiveMessage(HWND hwnd,
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
			//Handle all the rest like this:
			UpdateView();

			if (m_bIsInitialized) {
				// Set dirty to indicate the setting has changed
				m_bDirty = TRUE;
				if (m_pPageSite)
					m_pPageSite->OnStatusChange(PROPPAGESTATUS_DIRTY);
			}
			
			return 1;
        }
		
    }
    return CBasePropertyPage::OnReceiveMessage(hwnd,uMsg,wParam,lParam);
}

void CStereoDownmixerPropP1::OnInitDialog()
{
	//Setting the trackbar control range to 0-400
	::SendMessage(GetDlgItem(m_Dlg,IDC_SLIDER_VOLUME_GAIN_PERCENT),TBM_SETRANGE,(WPARAM)TRUE,(LPARAM) MAKELONG(0, 400));

	UpdateView();

	
	//Set up the input and output displays
	
	CMediaType mt;
	HRESULT hres;
	
	hres=m_pIStereoDownmixer->GetCurrentInputMediaType(mt);
	if (VFW_E_NOT_CONNECTED==hres)
	{
		SetWindowText(GetDlgItem(m_Dlg,IDC_EDIT_CURRENT_INPUT),"Not connected");
	}
	else 
	{
		char str[500];
		GetStringFromMediaType(str,500,mt);
		SetWindowText(GetDlgItem(m_Dlg,IDC_EDIT_CURRENT_INPUT),str);
	}

	hres=m_pIStereoDownmixer->GetCurrentOutputMediaType(mt);
	if (VFW_E_NOT_CONNECTED==hres)
	{
		SetWindowText(GetDlgItem(m_Dlg,IDC_EDIT_CURRENT_OUTPUT),"Not connected");
	}
	else 
	{
		char str[500];
		GetStringFromMediaType(str,500,mt);
		SetWindowText(GetDlgItem(m_Dlg,IDC_EDIT_CURRENT_OUTPUT),str);
	}

	//Setting the font for the media type editboxes
	SendMessage(GetDlgItem(m_Dlg,IDC_EDIT_CURRENT_INPUT),WM_SETFONT,(WPARAM)m_MediaTypeEditBoxFont,MAKELPARAM(TRUE,0));
	SendMessage(GetDlgItem(m_Dlg,IDC_EDIT_CURRENT_OUTPUT),WM_SETFONT,(WPARAM)m_MediaTypeEditBoxFont,MAKELPARAM(TRUE,0));

	//Setting the font for the bottom notices
	SendMessage(GetDlgItem(m_Dlg,IDC_STATIC_BOTTOM_NOTICE_1),WM_SETFONT,(WPARAM)m_MediaTypeEditBoxFont,MAKELPARAM(TRUE,0));
	SendMessage(GetDlgItem(m_Dlg,IDC_STATIC_BOTTOM_NOTICE_2),WM_SETFONT,(WPARAM)m_MediaTypeEditBoxFont,MAKELPARAM(TRUE,0));
}

///////////////////////////////////////////////////////////////////////
// OnConnect: Called when we connect to a transform filter
///////////////////////////////////////////////////////////////////////
HRESULT CStereoDownmixerPropP1::OnConnect(IUnknown *pUnknown)
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
HRESULT CStereoDownmixerPropP1::OnDisconnect()
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
HRESULT CStereoDownmixerPropP1::OnActivate()
{
    ASSERT(m_pIStereoDownmixer);
    // Get the current setup of the filter
    m_pIStereoDownmixer->GetParams(&m_StereoDownmixerParams,StereoDownmixerParams::SectionChoose::general);
    // Set the control to the current setup
    SetControlValues();
    m_bIsInitialized = TRUE;

    return NOERROR;
}

///////////////////////////////////////////////////////////////////////
// OnDeactivate: We are being deactivated
///////////////////////////////////////////////////////////////////////
HRESULT CStereoDownmixerPropP1::OnDeactivate(void)
{
    ASSERT(m_pIStereoDownmixer);
    m_bIsInitialized = FALSE;
    
    return NOERROR;
}

///////////////////////////////////////////////////////////////////////
// OnApplyChanges: Apply any changes so far made
///////////////////////////////////////////////////////////////////////
HRESULT CStereoDownmixerPropP1::OnApplyChanges()
{
    ASSERT(m_pIStereoDownmixer);
    // Update the setup from controls and set it into the filter
    GetControlValues();

	//We check if the type is acceptable to the downstream filter, and if it's not, we DISALLOW THE CHANGE!!
	//Should we allow the change in that case, the setting would be saved on the registry (or Win.ini file)
	//and a Windows Media Player user would never be able to use this filter again.
	//Also we should perform the same sort of check for the input requirements.

	//Ok, we will also allow to clear the settings by re-registering this filter

	BOOL bUpstreamOK=TRUE;
	BOOL bDownstreamOK=TRUE;

	if (S_OK==m_pIStereoDownmixer->IsInputPinConnected() && CurrentInputTypeCompatibleWithParams()==FALSE)
	{
		if (S_OK!=m_pIStereoDownmixer->CanThisFormatBeProvidedFromUpstream(&m_StereoDownmixerParams)) bUpstreamOK=FALSE;
	}

	if (S_OK==m_pIStereoDownmixer->IsOutputPinConnected() && CurrentOutputTypeCompatibleWithParams()==FALSE) 
	{
		if (S_OK!=m_pIStereoDownmixer->IsThisFormatAcceptedDownstream(&m_StereoDownmixerParams)) bDownstreamOK=FALSE;
	}

	if (bUpstreamOK==FALSE || bDownstreamOK==FALSE)
	{
		if (bUpstreamOK==FALSE) MessageBox(m_Dlg,"Upstream filter cannot provide the type you require! Changes NOT applied.","Changes NOT applied",MB_OK|MB_ICONEXCLAMATION);
		if (bDownstreamOK==FALSE) MessageBox(m_Dlg,"Downstream filter won't accept this change! Changes NOT applied.","Changes NOT applied",MB_OK|MB_ICONEXCLAMATION);

		m_bDirty = TRUE;
		if (m_pPageSite) m_pPageSite->OnStatusChange(PROPPAGESTATUS_DIRTY);
		return E_FAIL;
	}
	else
	{
		//Need to disconnect input/output pins?
		if (CurrentInputTypeCompatibleWithParams()==FALSE || CurrentOutputTypeCompatibleWithParams()==FALSE)
		{
			char *pins;
			if (CurrentInputTypeCompatibleWithParams()==FALSE && CurrentOutputTypeCompatibleWithParams()==FALSE) pins="input and output pins";
			else if (CurrentInputTypeCompatibleWithParams()==FALSE) pins="input pin";
			else pins="output pin";

			char msg[400];
			char caption[100];
			sprintf(msg,"You will need to disconnect and reconnect the %s in order for changes to take effect.\nIf this filter is running under Windows Media Player, this means reopening the media file.",pins);
			sprintf(caption,"Reconnect %s",pins);

			MessageBox(m_Dlg,msg,caption,MB_OK);
		}

		m_pIStereoDownmixer->SetParams(&m_StereoDownmixerParams,StereoDownmixerParams::SectionChoose::general);

		return NOERROR;
	}
}

void CStereoDownmixerPropP1::UpdateView()
{
	//Output format
	BOOL IsMultichannel=IsDlgButtonChecked(m_Dlg,IDC_RADIO_SPK_MULTICHANNEL);

	EnableWindow(GetDlgItem(m_Dlg,IDC_CHECK_SPK_FL_FR  ),IsMultichannel);
	EnableWindow(GetDlgItem(m_Dlg,IDC_CHECK_SPK_FC     ),IsMultichannel);
	EnableWindow(GetDlgItem(m_Dlg,IDC_CHECK_SPK_BL_BR  ),IsMultichannel);
	EnableWindow(GetDlgItem(m_Dlg,IDC_CHECK_SPK_FLC_FRC),IsMultichannel);
	EnableWindow(GetDlgItem(m_Dlg,IDC_CHECK_SPK_LF     ),IsMultichannel);

	EnableWindow(GetDlgItem(m_Dlg,IDC_CHECK_SPK_BC     ),IsMultichannel);
	EnableWindow(GetDlgItem(m_Dlg,IDC_CHECK_SPK_SL_SR  ),IsMultichannel);
	EnableWindow(GetDlgItem(m_Dlg,IDC_CHECK_SPK_TC     ),IsMultichannel);
	EnableWindow(GetDlgItem(m_Dlg,IDC_CHECK_SPK_TFC    ),IsMultichannel);
	EnableWindow(GetDlgItem(m_Dlg,IDC_CHECK_SPK_TFL_TFR),IsMultichannel);
	EnableWindow(GetDlgItem(m_Dlg,IDC_CHECK_SPK_TBC    ),IsMultichannel);
	EnableWindow(GetDlgItem(m_Dlg,IDC_CHECK_SPK_TBL_TBR),IsMultichannel);

	CheckDlgButton(m_Dlg,IDC_CHECK_SPK_FL_FR,BST_CHECKED); //So nobody will be able to disable it.

	//Input requirements
	EnableWindow(GetDlgItem(m_Dlg,IDC_EDIT_REQUIRE_NUMBER_OF_INPUT_CHANNELS),IsDlgButtonChecked(m_Dlg,IDC_RADIO_REQUIRE_NUMBER_OF_INPUT_CHANNELS));
	EnableWindow(GetDlgItem(m_Dlg,IDC_EDIT_REQUIRE_INPUT_SAMPLERATE),IsDlgButtonChecked(m_Dlg,IDC_RADIO_REQUIRE_INPUT_SAMPLERATE));

	//Volume modifications
	EnableWindow(GetDlgItem(m_Dlg,IDC_SLIDER_VOLUME_GAIN_PERCENT),IsDlgButtonChecked(m_Dlg,IDC_RADIO_ALTER_VOLUME));
	EnableWindow(GetDlgItem(m_Dlg,IDC_EDIT_VOLUME_GAIN_PERCENT),IsDlgButtonChecked(m_Dlg,IDC_RADIO_ALTER_VOLUME));
}

///////////////////////////////////////////////////////////////////////
// GetControlValues: read the setup from current controls
///////////////////////////////////////////////////////////////////////
void CStereoDownmixerPropP1::GetControlValues()
{
	// --- Output format

	//The wFormatTag is a bit tricky
    m_StereoDownmixerParams.p1.wFormatTag=WAVE_FORMAT_PCM;

	//Bits per sample
	if (BST_CHECKED == IsDlgButtonChecked(m_Dlg,IDC_RADIO_8BITS)) m_StereoDownmixerParams.p1.wBitsPerSample=8;
	else if (BST_CHECKED == IsDlgButtonChecked(m_Dlg,IDC_RADIO_24BITS)) m_StereoDownmixerParams.p1.wBitsPerSample=24;
	else if (BST_CHECKED == IsDlgButtonChecked(m_Dlg,IDC_RADIO_32BITS)) m_StereoDownmixerParams.p1.wBitsPerSample=32;
	else m_StereoDownmixerParams.p1.wBitsPerSample=16; //Default
	if (m_StereoDownmixerParams.p1.wBitsPerSample > 16) m_StereoDownmixerParams.p1.wFormatTag=WAVE_FORMAT_EXTENSIBLE;


	//Multichannel speaker configuration
	m_StereoDownmixerParams.p1.dwChannelMask=0;

	m_StereoDownmixerParams.p1.dwChannelMask |= SPEAKER_FRONT_LEFT|SPEAKER_FRONT_RIGHT; //ALWAYS enabled
	if (BST_CHECKED == IsDlgButtonChecked(m_Dlg,IDC_CHECK_SPK_FC)) m_StereoDownmixerParams.p1.dwChannelMask      |= SPEAKER_FRONT_CENTER                                      ;
	if (BST_CHECKED == IsDlgButtonChecked(m_Dlg,IDC_CHECK_SPK_BL_BR)) m_StereoDownmixerParams.p1.dwChannelMask   |= SPEAKER_BACK_LEFT|SPEAKER_BACK_RIGHT                      ;
	if (BST_CHECKED == IsDlgButtonChecked(m_Dlg,IDC_CHECK_SPK_FLC_FRC)) m_StereoDownmixerParams.p1.dwChannelMask |= SPEAKER_FRONT_LEFT_OF_CENTER|SPEAKER_FRONT_RIGHT_OF_CENTER;
	if (BST_CHECKED == IsDlgButtonChecked(m_Dlg,IDC_CHECK_SPK_LF)) m_StereoDownmixerParams.p1.dwChannelMask      |= SPEAKER_LOW_FREQUENCY                                     ;

	if (BST_CHECKED == IsDlgButtonChecked(m_Dlg,IDC_CHECK_SPK_BC     )) m_StereoDownmixerParams.p1.dwChannelMask |= SPEAKER_BACK_CENTER                                       ;
	if (BST_CHECKED == IsDlgButtonChecked(m_Dlg,IDC_CHECK_SPK_SL_SR  )) m_StereoDownmixerParams.p1.dwChannelMask |= SPEAKER_SIDE_LEFT|SPEAKER_SIDE_RIGHT                      ;
	if (BST_CHECKED == IsDlgButtonChecked(m_Dlg,IDC_CHECK_SPK_TC     )) m_StereoDownmixerParams.p1.dwChannelMask |= SPEAKER_TOP_CENTER                                        ;
	if (BST_CHECKED == IsDlgButtonChecked(m_Dlg,IDC_CHECK_SPK_TFC    )) m_StereoDownmixerParams.p1.dwChannelMask |= SPEAKER_TOP_FRONT_CENTER                                  ;
	if (BST_CHECKED == IsDlgButtonChecked(m_Dlg,IDC_CHECK_SPK_TFL_TFR)) m_StereoDownmixerParams.p1.dwChannelMask |= SPEAKER_TOP_FRONT_LEFT|SPEAKER_TOP_FRONT_RIGHT            ;
	if (BST_CHECKED == IsDlgButtonChecked(m_Dlg,IDC_CHECK_SPK_TBC    )) m_StereoDownmixerParams.p1.dwChannelMask |= SPEAKER_TOP_BACK_CENTER                                   ;
	if (BST_CHECKED == IsDlgButtonChecked(m_Dlg,IDC_CHECK_SPK_TBL_TBR)) m_StereoDownmixerParams.p1.dwChannelMask |= SPEAKER_TOP_BACK_LEFT|SPEAKER_TOP_BACK_RIGHT              ;


	//Speaker type
	if (BST_CHECKED == IsDlgButtonChecked(m_Dlg,IDC_RADIO_SPK_MONO))
	{
		m_StereoDownmixerParams.p1.nChannels=1;
		m_StereoDownmixerParams.p1.bIsMultichannel=FALSE;
	}
	else if (BST_CHECKED == IsDlgButtonChecked(m_Dlg,IDC_RADIO_SPK_STEREO))
	{
		m_StereoDownmixerParams.p1.nChannels=2;
		m_StereoDownmixerParams.p1.bIsMultichannel=FALSE;
	}
	else //multichannel
	{
		m_StereoDownmixerParams.p1.wFormatTag=WAVE_FORMAT_EXTENSIBLE;
		m_StereoDownmixerParams.p1.bIsMultichannel=TRUE;

		//Let's count how many speakers we have
		int counter=0; 
		DWORD spkmask=m_StereoDownmixerParams.p1.dwChannelMask;
		while (spkmask)
		{
			if (spkmask & 1) counter++;
			spkmask >>= 1;
		}
		
		m_StereoDownmixerParams.p1.nChannels=counter;
	}



	// --- Input requirements
	if (BST_CHECKED == IsDlgButtonChecked(m_Dlg,IDC_RADIO_REQUIRE_1_BYTE)) m_StereoDownmixerParams.p1.RequiredInputBitDepth=8;
	else if (BST_CHECKED == IsDlgButtonChecked(m_Dlg,IDC_RADIO_REQUIRE_2_BYTES)) m_StereoDownmixerParams.p1.RequiredInputBitDepth=16;
	else if (BST_CHECKED == IsDlgButtonChecked(m_Dlg,IDC_RADIO_REQUIRE_3_BYTES)) m_StereoDownmixerParams.p1.RequiredInputBitDepth=24;
	else if (BST_CHECKED == IsDlgButtonChecked(m_Dlg,IDC_RADIO_REQUIRE_4_BYTES)) m_StereoDownmixerParams.p1.RequiredInputBitDepth=32;
	else m_StereoDownmixerParams.p1.RequiredInputBitDepth=0;


	if (BST_CHECKED == IsDlgButtonChecked(m_Dlg,IDC_RADIO_REQUIRE_NUMBER_OF_INPUT_CHANNELS)) m_StereoDownmixerParams.p1.bRequireInputNumberOfChannels=TRUE;
	else m_StereoDownmixerParams.p1.bRequireInputNumberOfChannels=FALSE;
	m_StereoDownmixerParams.p1.RequiredInputNumberOfChannels=GetDlgItemInt(m_Dlg,IDC_EDIT_REQUIRE_NUMBER_OF_INPUT_CHANNELS,NULL,FALSE);

	if (BST_CHECKED == IsDlgButtonChecked(m_Dlg,IDC_RADIO_REQUIRE_INPUT_SAMPLERATE)) m_StereoDownmixerParams.p1.bRequireInputSamplerate=TRUE;
	else m_StereoDownmixerParams.p1.bRequireInputSamplerate=FALSE;
	m_StereoDownmixerParams.p1.RequiredInputSamplerate=GetDlgItemInt(m_Dlg,IDC_EDIT_REQUIRE_INPUT_SAMPLERATE,NULL,FALSE);


	//Raise Priority And Single Instance
	m_StereoDownmixerParams.p1.bRaiseMeritAndSingleInstance = (BST_CHECKED==IsDlgButtonChecked(m_Dlg,IDC_CHECK_RAISE_FILTER_MERIT));
}

///////////////////////////////////////////////////////////////////////
// SetControlValues: set the current setup to the controls
///////////////////////////////////////////////////////////////////////
void CStereoDownmixerPropP1::SetControlValues()
{
	// --- Output format

	//Bits per sample
	if (m_StereoDownmixerParams.p1.wBitsPerSample==8) CheckRadioButton(m_Dlg,IDC_RADIO_8BITS,IDC_RADIO_32BITS,IDC_RADIO_8BITS);
	else if (m_StereoDownmixerParams.p1.wBitsPerSample==24) CheckRadioButton(m_Dlg,IDC_RADIO_8BITS,IDC_RADIO_32BITS,IDC_RADIO_24BITS);
	else if (m_StereoDownmixerParams.p1.wBitsPerSample==32) CheckRadioButton(m_Dlg,IDC_RADIO_8BITS,IDC_RADIO_32BITS,IDC_RADIO_32BITS);
	else CheckRadioButton(m_Dlg,IDC_RADIO_8BITS,IDC_RADIO_32BITS,IDC_RADIO_16BITS); //Default = 16 bits.

    //Speaker type
	if (m_StereoDownmixerParams.p1.nChannels==1)					CheckRadioButton(m_Dlg,IDC_RADIO_SPK_MONO,IDC_RADIO_SPK_MULTICHANNEL,IDC_RADIO_SPK_MONO);
	else if (m_StereoDownmixerParams.p1.bIsMultichannel==FALSE)	CheckRadioButton(m_Dlg,IDC_RADIO_SPK_MONO,IDC_RADIO_SPK_MULTICHANNEL,IDC_RADIO_SPK_STEREO);
	else CheckRadioButton(m_Dlg,IDC_RADIO_SPK_MONO,IDC_RADIO_SPK_MULTICHANNEL,IDC_RADIO_SPK_MULTICHANNEL);

	//Multichannel speaker configuration
	CheckDlgButton(m_Dlg, IDC_CHECK_SPK_FL_FR, BST_CHECKED); //Can't uncheck this one
	CheckDlgButton(m_Dlg, IDC_CHECK_SPK_FC,     (m_StereoDownmixerParams.p1.dwChannelMask & (SPEAKER_FRONT_CENTER                                      )) ? BST_CHECKED : BST_UNCHECKED);
	CheckDlgButton(m_Dlg, IDC_CHECK_SPK_BL_BR,  (m_StereoDownmixerParams.p1.dwChannelMask & (SPEAKER_BACK_LEFT|SPEAKER_BACK_RIGHT                      )) ? BST_CHECKED : BST_UNCHECKED);
	CheckDlgButton(m_Dlg, IDC_CHECK_SPK_FLC_FRC,(m_StereoDownmixerParams.p1.dwChannelMask & (SPEAKER_FRONT_LEFT_OF_CENTER|SPEAKER_FRONT_RIGHT_OF_CENTER)) ? BST_CHECKED : BST_UNCHECKED);
	CheckDlgButton(m_Dlg, IDC_CHECK_SPK_LF,     (m_StereoDownmixerParams.p1.dwChannelMask & (SPEAKER_LOW_FREQUENCY                                     )) ? BST_CHECKED : BST_UNCHECKED);

	CheckDlgButton(m_Dlg, IDC_CHECK_SPK_BC     ,(m_StereoDownmixerParams.p1.dwChannelMask & (SPEAKER_BACK_CENTER                                       )) ? BST_CHECKED : BST_UNCHECKED);
	CheckDlgButton(m_Dlg, IDC_CHECK_SPK_SL_SR  ,(m_StereoDownmixerParams.p1.dwChannelMask & (SPEAKER_SIDE_LEFT|SPEAKER_SIDE_RIGHT                      )) ? BST_CHECKED : BST_UNCHECKED);
	CheckDlgButton(m_Dlg, IDC_CHECK_SPK_TC     ,(m_StereoDownmixerParams.p1.dwChannelMask & (SPEAKER_TOP_CENTER                                        )) ? BST_CHECKED : BST_UNCHECKED);
	CheckDlgButton(m_Dlg, IDC_CHECK_SPK_TFC    ,(m_StereoDownmixerParams.p1.dwChannelMask & (SPEAKER_TOP_FRONT_CENTER                                  )) ? BST_CHECKED : BST_UNCHECKED);
	CheckDlgButton(m_Dlg, IDC_CHECK_SPK_TFL_TFR,(m_StereoDownmixerParams.p1.dwChannelMask & (SPEAKER_TOP_FRONT_LEFT|SPEAKER_TOP_FRONT_RIGHT            )) ? BST_CHECKED : BST_UNCHECKED);
	CheckDlgButton(m_Dlg, IDC_CHECK_SPK_TBC    ,(m_StereoDownmixerParams.p1.dwChannelMask & (SPEAKER_TOP_BACK_CENTER                                   )) ? BST_CHECKED : BST_UNCHECKED);
	CheckDlgButton(m_Dlg, IDC_CHECK_SPK_TBL_TBR,(m_StereoDownmixerParams.p1.dwChannelMask & (SPEAKER_TOP_BACK_LEFT|SPEAKER_TOP_BACK_RIGHT              )) ? BST_CHECKED : BST_UNCHECKED);
	


	// --- Input requirements
	if (m_StereoDownmixerParams.p1.RequiredInputBitDepth==8) CheckRadioButton(m_Dlg,IDC_RADIO_ALLOW_ANY_INPUT_BIT_DEPTH,IDC_RADIO_REQUIRE_4_BYTES,IDC_RADIO_REQUIRE_1_BYTE);
	else if (m_StereoDownmixerParams.p1.RequiredInputBitDepth==16) CheckRadioButton(m_Dlg,IDC_RADIO_ALLOW_ANY_INPUT_BIT_DEPTH,IDC_RADIO_REQUIRE_4_BYTES,IDC_RADIO_REQUIRE_2_BYTES);
	else if (m_StereoDownmixerParams.p1.RequiredInputBitDepth==24) CheckRadioButton(m_Dlg,IDC_RADIO_ALLOW_ANY_INPUT_BIT_DEPTH,IDC_RADIO_REQUIRE_4_BYTES,IDC_RADIO_REQUIRE_3_BYTES);
	else if (m_StereoDownmixerParams.p1.RequiredInputBitDepth==32) CheckRadioButton(m_Dlg,IDC_RADIO_ALLOW_ANY_INPUT_BIT_DEPTH,IDC_RADIO_REQUIRE_4_BYTES,IDC_RADIO_REQUIRE_4_BYTES);
	else CheckRadioButton(m_Dlg,IDC_RADIO_ALLOW_ANY_INPUT_BIT_DEPTH,IDC_RADIO_REQUIRE_4_BYTES,IDC_RADIO_ALLOW_ANY_INPUT_BIT_DEPTH);

	if (m_StereoDownmixerParams.p1.bRequireInputNumberOfChannels==TRUE) 
		CheckRadioButton(m_Dlg,
			IDC_RADIO_ALLOW_ANY_INPUT_NUMBER_OF_CHANNELS,
			IDC_RADIO_REQUIRE_NUMBER_OF_INPUT_CHANNELS,
			IDC_RADIO_REQUIRE_NUMBER_OF_INPUT_CHANNELS
			);
	else CheckRadioButton(m_Dlg,
			IDC_RADIO_ALLOW_ANY_INPUT_NUMBER_OF_CHANNELS,
			IDC_RADIO_REQUIRE_NUMBER_OF_INPUT_CHANNELS,
			IDC_RADIO_ALLOW_ANY_INPUT_NUMBER_OF_CHANNELS
			);
	SetDlgItemInt(m_Dlg,IDC_EDIT_REQUIRE_NUMBER_OF_INPUT_CHANNELS,m_StereoDownmixerParams.p1.RequiredInputNumberOfChannels,FALSE);

	if (m_StereoDownmixerParams.p1.bRequireInputSamplerate==TRUE) 
		CheckRadioButton(m_Dlg,
			IDC_RADIO_ALLOW_ANY_INPUT_SAMPLERATE,
			IDC_RADIO_REQUIRE_INPUT_SAMPLERATE,
			IDC_RADIO_REQUIRE_INPUT_SAMPLERATE
			);
	else CheckRadioButton(m_Dlg,
			IDC_RADIO_ALLOW_ANY_INPUT_SAMPLERATE,
			IDC_RADIO_REQUIRE_INPUT_SAMPLERATE,
			IDC_RADIO_ALLOW_ANY_INPUT_SAMPLERATE
			);
	SetDlgItemInt(m_Dlg,IDC_EDIT_REQUIRE_INPUT_SAMPLERATE,m_StereoDownmixerParams.p1.RequiredInputSamplerate,FALSE);
	
	//Raise Priority And Single Instance
	CheckDlgButton(
		m_Dlg,
		IDC_CHECK_RAISE_FILTER_MERIT,
		(m_StereoDownmixerParams.p1.bRaiseMeritAndSingleInstance == TRUE) ? BST_CHECKED : BST_UNCHECKED
		);

	UpdateView(); //So we enable / disable in the right way all the controls which depend on the radio buttons we have just set

	
}

BOOL CStereoDownmixerPropP1::CurrentInputTypeCompatibleWithParams()
{
	CMediaType cmt;
	if (FAILED(m_pIStereoDownmixer->GetCurrentInputMediaType(cmt))) return TRUE; //If input not connected, everything is good.

	if (m_pIStereoDownmixer->IsThisInputMediaTypeAcceptableAndMatchesParams(cmt,m_StereoDownmixerParams)) return TRUE;
	else return FALSE;

	return TRUE;
}

BOOL CStereoDownmixerPropP1::CurrentOutputTypeCompatibleWithParams()
{
	CMediaType cmt;
	if (FAILED(m_pIStereoDownmixer->GetCurrentOutputMediaType(cmt))) return TRUE; //If input not connected, everything is good.

	WAVEFORMATEXTENSIBLE out_wfex; memset(&out_wfex,0,sizeof(out_wfex));
	memcpy(&out_wfex,cmt.Format(),cmt.FormatLength());

	if (m_StereoDownmixerParams.p1.nChannels != out_wfex.Format.nChannels) return FALSE;

	if (m_StereoDownmixerParams.p1.nChannels > 2)
	{
		if (out_wfex.Format.wFormatTag!=WAVE_FORMAT_EXTENSIBLE) return FALSE;
		if (m_StereoDownmixerParams.p1.dwChannelMask != out_wfex.dwChannelMask) return FALSE;
	}

	if (m_StereoDownmixerParams.p1.wBitsPerSample != out_wfex.Format.wBitsPerSample) return FALSE;
	
	return TRUE;
}

void CStereoDownmixerPropP1::GetStringFromMediaType(char *str, DWORD buffersize, CMediaType &mt)
{
	// We want to create a string like this:
	//
	// 44100Hz 16bits 6chan [FL FR SUB + 3 unspecified]

	WAVEFORMATEXTENSIBLE wfex; memset(&wfex,0,sizeof(wfex));
	memcpy(&wfex,mt.Format(),mt.FormatLength());

	/*
	If it's a waveformatex I don't put any channelmask_str
	if it's a waveformatextensible I put [FL FR ... FC + 2 unspecified]
	*/

	char channelmask_str[500]="";	//No brackets ( ... ) if it's WAVE_FORMAT_PCM
	if (wfex.Format.wFormatTag==WAVE_FORMAT_EXTENSIBLE)
	{
		DWORD count_channels_present=0;
		DWORD channelmask_clone=wfex.dwChannelMask;
		while (channelmask_clone) {channelmask_clone >>=1; count_channels_present++;}
		
		char channels_present_str[50]="";
		sprintf(channels_present_str,"%s%s%s%s%s%s%s%s",
			wfex.dwChannelMask & 0x01 ? "FL " : "",
			wfex.dwChannelMask & 0x02 ? "FR " : "",
			wfex.dwChannelMask & 0x04 ? "FC " : "",
			wfex.dwChannelMask & 0x08 ? "SUB ": "",
			wfex.dwChannelMask & 0x10 ? "RL " : "",
			wfex.dwChannelMask & 0x20 ? "RR " : "",
			wfex.dwChannelMask & 0x40 ? "FLC ": "",
			wfex.dwChannelMask & 0x80 ? "FRC ": ""
			);

		char channels_remaining_str[50]="";
		if (count_channels_present<wfex.Format.nChannels) sprintf(channels_remaining_str,"%d unspecified ",
			wfex.Format.nChannels - count_channels_present
			);

		sprintf(channelmask_str,"[%s%s%s",
			channels_present_str,
			(channels_present_str[0]!=0 && channels_remaining_str[0]!=0) ? "+ " : "",
			channels_remaining_str
			);

		//Substitute last char (a whitespace) with a ]
		char *ptr=channelmask_str;
		while (*ptr) ptr++;
		ptr--; *ptr=']';
	}
	

	_snprintf(str,buffersize,"%uHz %ubits %uchan %s",
		wfex.Format.nSamplesPerSec,
		wfex.Format.wBitsPerSample,
		wfex.Format.nChannels,
		channelmask_str
		);


	//Debug:
	//strcpy(str,"44100Hz 16bits 6chan [FL FR SUB + 3 unspecified]");
}

