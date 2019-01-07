// StereoDownmixerProp.h: implementation of CStereoDownmixerPropP2 class
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
#include "StereoDownmixerPropP2.h"

#include <ks.h>
#include <ksmedia.h>

///////////////////////////////////////////////////////////////////////
// CreateInstance: Used by the DirectShow base classes to create 
// instances
///////////////////////////////////////////////////////////////////////
CUnknown *CStereoDownmixerPropP2::CreateInstance(LPUNKNOWN lpunk, HRESULT *phr)
{
    CUnknown *punk = new CStereoDownmixerPropP2(lpunk, phr);
    if (punk == NULL) {
    *phr = E_OUTOFMEMORY;
    }
    return punk;

}

///////////////////////////////////////////////////////////////////////
// Constructor
///////////////////////////////////////////////////////////////////////
CStereoDownmixerPropP2::CStereoDownmixerPropP2(LPUNKNOWN pUnk, HRESULT *phr) :
    CBasePropertyPage(NAME("StereoDownmixer Property Page"),
                      pUnk,IDD_PROPERTIES_PAGE2,IDS_TITLE_PROPERTY_PAGE2),
    m_pIStereoDownmixer(NULL),
    m_bIsBeingInitialized(TRUE),
	m_bIsInputConnected(FALSE),
	m_bIsOutputConnected(FALSE)
{
    ASSERT(phr);
}

CStereoDownmixerPropP2::~CStereoDownmixerPropP2()
{
}

///////////////////////////////////////////////////////////////////////
// OnReceiveMessage: Handles the messages for our property window
///////////////////////////////////////////////////////////////////////
BOOL CStereoDownmixerPropP2::OnReceiveMessage(HWND hwnd,
                                          UINT uMsg,
                                          WPARAM wParam,
                                          LPARAM lParam)
{
    switch (uMsg)
    {
        case WM_INITDIALOG:  // init property page
			OnInitDialog();
            break;

		case WM_HSCROLL:

			if (m_bIsBeingInitialized) return 1;

			OnSomeVolumeScrollbarMoved(hwnd,uMsg,wParam,lParam);
			GetControlValues();
			SaveDataToFilter();
			return 1;

        case WM_COMMAND:
        {
			if (m_bIsBeingInitialized) return 1;

			if (HIWORD(wParam)==EN_CHANGE)	//Volume changes via editbox
			{
				OnSomeVolumeEditBoxChanged(hwnd,uMsg,wParam,lParam);
				GetControlValues();
				SaveDataToFilter();
				return 1;
			}
			else if (HIWORD(wParam)==BN_CLICKED)	//Checkbox clicked
			{
				UpdateView();
				GetControlValues();
				SaveDataToFilter();
				return 1;
			}

			//Handle all the rest like this:
			UpdateView();

			// We never set this page dirty
			//
			// Set dirty to indicate the setting has changed
			//m_bDirty = TRUE;
			//if (m_pPageSite)
			//	m_pPageSite->OnStatusChange(PROPPAGESTATUS_DIRTY);
			//
			return 1;
        }
		
    }
    return CBasePropertyPage::OnReceiveMessage(hwnd,uMsg,wParam,lParam);
}

void CStereoDownmixerPropP2::OnInitDialog()
{

#define MINVAL	0
#define MAXVAL	400

	//Setting the trackbar control range to 0-400
	::SendMessage(GetDlgItem(m_Dlg,IDC_SLIDER_I_FL ),TBM_SETRANGE,(WPARAM)TRUE,(LPARAM) MAKELONG(MINVAL, MAXVAL));
	::SendMessage(GetDlgItem(m_Dlg,IDC_SLIDER_I_FR ),TBM_SETRANGE,(WPARAM)TRUE,(LPARAM) MAKELONG(MINVAL, MAXVAL));
	::SendMessage(GetDlgItem(m_Dlg,IDC_SLIDER_I_FC ),TBM_SETRANGE,(WPARAM)TRUE,(LPARAM) MAKELONG(MINVAL, MAXVAL));
	::SendMessage(GetDlgItem(m_Dlg,IDC_SLIDER_I_LF ),TBM_SETRANGE,(WPARAM)TRUE,(LPARAM) MAKELONG(MINVAL, MAXVAL));
	::SendMessage(GetDlgItem(m_Dlg,IDC_SLIDER_I_BL ),TBM_SETRANGE,(WPARAM)TRUE,(LPARAM) MAKELONG(MINVAL, MAXVAL));
	::SendMessage(GetDlgItem(m_Dlg,IDC_SLIDER_I_BR ),TBM_SETRANGE,(WPARAM)TRUE,(LPARAM) MAKELONG(MINVAL, MAXVAL));
	::SendMessage(GetDlgItem(m_Dlg,IDC_SLIDER_I_FLC),TBM_SETRANGE,(WPARAM)TRUE,(LPARAM) MAKELONG(MINVAL, MAXVAL));
	::SendMessage(GetDlgItem(m_Dlg,IDC_SLIDER_I_FRC),TBM_SETRANGE,(WPARAM)TRUE,(LPARAM) MAKELONG(MINVAL, MAXVAL));
	::SendMessage(GetDlgItem(m_Dlg,IDC_SLIDER_I_BC ),TBM_SETRANGE,(WPARAM)TRUE,(LPARAM) MAKELONG(MINVAL, MAXVAL));
	::SendMessage(GetDlgItem(m_Dlg,IDC_SLIDER_I_SL ),TBM_SETRANGE,(WPARAM)TRUE,(LPARAM) MAKELONG(MINVAL, MAXVAL));
	::SendMessage(GetDlgItem(m_Dlg,IDC_SLIDER_I_SR ),TBM_SETRANGE,(WPARAM)TRUE,(LPARAM) MAKELONG(MINVAL, MAXVAL));
	::SendMessage(GetDlgItem(m_Dlg,IDC_SLIDER_I_TC ),TBM_SETRANGE,(WPARAM)TRUE,(LPARAM) MAKELONG(MINVAL, MAXVAL));
	::SendMessage(GetDlgItem(m_Dlg,IDC_SLIDER_I_TFL),TBM_SETRANGE,(WPARAM)TRUE,(LPARAM) MAKELONG(MINVAL, MAXVAL));
	::SendMessage(GetDlgItem(m_Dlg,IDC_SLIDER_I_TFC),TBM_SETRANGE,(WPARAM)TRUE,(LPARAM) MAKELONG(MINVAL, MAXVAL));
	::SendMessage(GetDlgItem(m_Dlg,IDC_SLIDER_I_TFR),TBM_SETRANGE,(WPARAM)TRUE,(LPARAM) MAKELONG(MINVAL, MAXVAL));
	::SendMessage(GetDlgItem(m_Dlg,IDC_SLIDER_I_TBL),TBM_SETRANGE,(WPARAM)TRUE,(LPARAM) MAKELONG(MINVAL, MAXVAL));
	::SendMessage(GetDlgItem(m_Dlg,IDC_SLIDER_I_TBC),TBM_SETRANGE,(WPARAM)TRUE,(LPARAM) MAKELONG(MINVAL, MAXVAL));
	::SendMessage(GetDlgItem(m_Dlg,IDC_SLIDER_I_TBR),TBM_SETRANGE,(WPARAM)TRUE,(LPARAM) MAKELONG(MINVAL, MAXVAL));

	::SendMessage(GetDlgItem(m_Dlg,IDC_SLIDER_O_FL ),TBM_SETRANGE,(WPARAM)TRUE,(LPARAM) MAKELONG(MINVAL, MAXVAL));
	::SendMessage(GetDlgItem(m_Dlg,IDC_SLIDER_O_FR ),TBM_SETRANGE,(WPARAM)TRUE,(LPARAM) MAKELONG(MINVAL, MAXVAL));
	::SendMessage(GetDlgItem(m_Dlg,IDC_SLIDER_O_FC ),TBM_SETRANGE,(WPARAM)TRUE,(LPARAM) MAKELONG(MINVAL, MAXVAL));
	::SendMessage(GetDlgItem(m_Dlg,IDC_SLIDER_O_LF ),TBM_SETRANGE,(WPARAM)TRUE,(LPARAM) MAKELONG(MINVAL, MAXVAL));
	::SendMessage(GetDlgItem(m_Dlg,IDC_SLIDER_O_BL ),TBM_SETRANGE,(WPARAM)TRUE,(LPARAM) MAKELONG(MINVAL, MAXVAL));
	::SendMessage(GetDlgItem(m_Dlg,IDC_SLIDER_O_BR ),TBM_SETRANGE,(WPARAM)TRUE,(LPARAM) MAKELONG(MINVAL, MAXVAL));
	::SendMessage(GetDlgItem(m_Dlg,IDC_SLIDER_O_FLC),TBM_SETRANGE,(WPARAM)TRUE,(LPARAM) MAKELONG(MINVAL, MAXVAL));
	::SendMessage(GetDlgItem(m_Dlg,IDC_SLIDER_O_FRC),TBM_SETRANGE,(WPARAM)TRUE,(LPARAM) MAKELONG(MINVAL, MAXVAL));
	::SendMessage(GetDlgItem(m_Dlg,IDC_SLIDER_O_BC ),TBM_SETRANGE,(WPARAM)TRUE,(LPARAM) MAKELONG(MINVAL, MAXVAL));
	::SendMessage(GetDlgItem(m_Dlg,IDC_SLIDER_O_SL ),TBM_SETRANGE,(WPARAM)TRUE,(LPARAM) MAKELONG(MINVAL, MAXVAL));
	::SendMessage(GetDlgItem(m_Dlg,IDC_SLIDER_O_SR ),TBM_SETRANGE,(WPARAM)TRUE,(LPARAM) MAKELONG(MINVAL, MAXVAL));
	::SendMessage(GetDlgItem(m_Dlg,IDC_SLIDER_O_TC ),TBM_SETRANGE,(WPARAM)TRUE,(LPARAM) MAKELONG(MINVAL, MAXVAL));
	::SendMessage(GetDlgItem(m_Dlg,IDC_SLIDER_O_TFL),TBM_SETRANGE,(WPARAM)TRUE,(LPARAM) MAKELONG(MINVAL, MAXVAL));
	::SendMessage(GetDlgItem(m_Dlg,IDC_SLIDER_O_TFC),TBM_SETRANGE,(WPARAM)TRUE,(LPARAM) MAKELONG(MINVAL, MAXVAL));
	::SendMessage(GetDlgItem(m_Dlg,IDC_SLIDER_O_TFR),TBM_SETRANGE,(WPARAM)TRUE,(LPARAM) MAKELONG(MINVAL, MAXVAL));
	::SendMessage(GetDlgItem(m_Dlg,IDC_SLIDER_O_TBL),TBM_SETRANGE,(WPARAM)TRUE,(LPARAM) MAKELONG(MINVAL, MAXVAL));
	::SendMessage(GetDlgItem(m_Dlg,IDC_SLIDER_O_TBC),TBM_SETRANGE,(WPARAM)TRUE,(LPARAM) MAKELONG(MINVAL, MAXVAL));
	::SendMessage(GetDlgItem(m_Dlg,IDC_SLIDER_O_TBR),TBM_SETRANGE,(WPARAM)TRUE,(LPARAM) MAKELONG(MINVAL, MAXVAL));

	::SendMessage(GetDlgItem(m_Dlg,IDC_SLIDER_MASTER_VOLUME),TBM_SETRANGE,(WPARAM)TRUE,(LPARAM) MAKELONG(MINVAL, MAXVAL));

	SetControlValues();

	UpdateView();

	m_bIsBeingInitialized = FALSE;
}

///////////////////////////////////////////////////////////////////////
// OnConnect: Called when we connect to a transform filter
///////////////////////////////////////////////////////////////////////
HRESULT CStereoDownmixerPropP2::OnConnect(IUnknown *pUnknown)
{
    ASSERT(m_pIStereoDownmixer == NULL);

    HRESULT hr = pUnknown->QueryInterface(IID_IStereoDownmixer, (void **) &m_pIStereoDownmixer);
    if (FAILED(hr)) {
        return E_NOINTERFACE;
    }

    ASSERT(m_pIStereoDownmixer);

	if (VFW_E_NOT_CONNECTED == m_pIStereoDownmixer->GetCurrentInputMediaType(m_InputMediaType)) m_bIsInputConnected=FALSE;
	else m_bIsInputConnected=TRUE;

	if (VFW_E_NOT_CONNECTED == m_pIStereoDownmixer->GetCurrentOutputMediaType(m_OutputMediaType)) m_bIsOutputConnected=FALSE;
	else m_bIsOutputConnected=TRUE;

	//Get the params!
	m_pIStereoDownmixer->GetParams(&m_StereoDownmixerParams, StereoDownmixerParams::SectionChoose::gains);

    m_bIsBeingInitialized = TRUE ;

    return NOERROR;
}

///////////////////////////////////////////////////////////////////////
// OnDisconnect: Likewise called when we disconnect from a filter
///////////////////////////////////////////////////////////////////////
HRESULT CStereoDownmixerPropP2::OnDisconnect()
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
HRESULT CStereoDownmixerPropP2::OnActivate()
{
    ASSERT(m_pIStereoDownmixer);

    return NOERROR;
}

///////////////////////////////////////////////////////////////////////
// OnDeactivate: We are being deactivated
///////////////////////////////////////////////////////////////////////
HRESULT CStereoDownmixerPropP2::OnDeactivate(void)
{
    ASSERT(m_pIStereoDownmixer);
    m_bIsBeingInitialized = TRUE;
    
    return NOERROR;
}

///////////////////////////////////////////////////////////////////////
// OnApplyChanges: Apply any changes so far made
///////////////////////////////////////////////////////////////////////
HRESULT CStereoDownmixerPropP2::OnApplyChanges()
{
    ASSERT(m_pIStereoDownmixer);
    // Update the setup from controls and set it into the filter
    GetControlValues();

	SaveDataToFilter();

	return NOERROR;
}

void CStereoDownmixerPropP2::UpdateView()
{
	//sample code: Volume modifications
//	EnableWindow(GetDlgItem(m_Dlg,IDC_SLIDER_VOLUME_GAIN_PERCENT),IsDlgButtonChecked(m_Dlg,IDC_RADIO_ALTER_VOLUME));
//	EnableWindow(GetDlgItem(m_Dlg,IDC_EDIT_VOLUME_GAIN_PERCENT),IsDlgButtonChecked(m_Dlg,IDC_RADIO_ALTER_VOLUME));


	//Enable/disable controls

	WAVEFORMATEXTENSIBLE in_wfe={0};
	WAVEFORMATEXTENSIBLE out_wfe={0};
	DWORD in_channelmask=0;
	DWORD out_channelmask=0;

	if (m_bIsInputConnected)
	{
		memcpy(&in_wfe,m_InputMediaType.Format(),m_InputMediaType.FormatLength());
		in_channelmask = (in_wfe.Format.wFormatTag==WAVE_FORMAT_EXTENSIBLE) 
			? in_wfe.dwChannelMask
			: (
				in_wfe.Format.nChannels==2 ? SPEAKER_FRONT_LEFT|SPEAKER_FRONT_RIGHT
				: SPEAKER_FRONT_CENTER
				)
			;
	}

	if (m_bIsOutputConnected)
	{
		memcpy(&out_wfe,m_OutputMediaType.Format(),m_OutputMediaType.FormatLength());
		out_channelmask = (out_wfe.Format.wFormatTag==WAVE_FORMAT_EXTENSIBLE) 
			? out_wfe.dwChannelMask
			: (
				out_wfe.Format.nChannels==2 ? SPEAKER_FRONT_LEFT|SPEAKER_FRONT_RIGHT
				: SPEAKER_FRONT_CENTER
				);
	}

	//Code equal to the one in StereoDownmixer::Transform(...);
	if (m_bIsInputConnected)
	{
		//in_channelmask

		DWORD SetBitCounter=0;
		DWORD ChannelMaskClone=in_channelmask;
		while (ChannelMaskClone)
		{
			if (ChannelMaskClone & 0x1) SetBitCounter++;
			ChannelMaskClone >>= 1;
		}

		int AddBits=((signed)in_wfe.Format.nChannels) - ((signed)SetBitCounter);

		//Adding bits from the lowest significant one
		DWORD BitAdder=0x1;
		while (AddBits > 0)
		{
			while (in_channelmask & BitAdder) BitAdder <<= 1; //This bit is at 1 already
			in_channelmask |= BitAdder;
			AddBits--;
		}

		//Clearing bits from the higher value one
		DWORD BitClearer=0x80000000;
		while (AddBits < 0)
		{
			while ((in_channelmask & BitClearer)==0) BitClearer >>= 1; //This bit is at 0 already
			in_channelmask ^= BitClearer;
			AddBits++;
		}
	}
	else in_channelmask=0xffffffff;  //If the filter is not connected we enable everything


	if (m_bIsOutputConnected)
	{
		//out_channelmask

		DWORD SetBitCounter=0;
		DWORD ChannelMaskClone=out_channelmask;
		while (ChannelMaskClone)
		{
			if (ChannelMaskClone & 0x1) SetBitCounter++;
			ChannelMaskClone >>= 1;
		}

		int AddBits=((signed)out_wfe.Format.nChannels) - ((signed)SetBitCounter);

		//Adding bits from the lowest significant one
		DWORD BitAdder=0x1;
		while (AddBits > 0)
		{
			while (out_channelmask & BitAdder) BitAdder <<= 1; //This bit is at 1 already
			out_channelmask |= BitAdder;
			AddBits--;
		}

		//Clearing bits from the higher value one
		DWORD BitClearer=0x80000000;
		while (AddBits < 0)
		{
			while ((out_channelmask & BitClearer)==0) BitClearer >>= 1; //This bit is at 0 already
			out_channelmask ^= BitClearer;
			AddBits++;
		}
	}
	else out_channelmask=0xffffffff;


	//Now we have a correct channel mask

#define EW(ID,expression) EnableWindow(GetDlgItem(m_Dlg,ID),(0!=(expression)));

	//Input enable
	EW(IDC_BUTTON_I_EN_FL ,in_channelmask & SPEAKER_FRONT_LEFT           );
	EW(IDC_BUTTON_I_EN_FR ,in_channelmask & SPEAKER_FRONT_RIGHT          );
	EW(IDC_BUTTON_I_EN_FC ,in_channelmask & SPEAKER_FRONT_CENTER         );
	EW(IDC_BUTTON_I_EN_LF ,in_channelmask & SPEAKER_LOW_FREQUENCY        );
	EW(IDC_BUTTON_I_EN_BL ,in_channelmask & SPEAKER_BACK_LEFT            );
	EW(IDC_BUTTON_I_EN_BR ,in_channelmask & SPEAKER_BACK_RIGHT           );
	EW(IDC_BUTTON_I_EN_FLC,in_channelmask & SPEAKER_FRONT_LEFT_OF_CENTER );
	EW(IDC_BUTTON_I_EN_FRC,in_channelmask & SPEAKER_FRONT_RIGHT_OF_CENTER);
	EW(IDC_BUTTON_I_EN_BC ,in_channelmask & SPEAKER_BACK_CENTER          );
	EW(IDC_BUTTON_I_EN_SL ,in_channelmask & SPEAKER_SIDE_LEFT            );
	EW(IDC_BUTTON_I_EN_SR ,in_channelmask & SPEAKER_SIDE_RIGHT           );
	EW(IDC_BUTTON_I_EN_TC ,in_channelmask & SPEAKER_TOP_CENTER           );
	EW(IDC_BUTTON_I_EN_TFL,in_channelmask & SPEAKER_TOP_FRONT_LEFT       );
	EW(IDC_BUTTON_I_EN_TFC,in_channelmask & SPEAKER_TOP_FRONT_CENTER     );
	EW(IDC_BUTTON_I_EN_TFR,in_channelmask & SPEAKER_TOP_FRONT_RIGHT      );
	EW(IDC_BUTTON_I_EN_TBL,in_channelmask & SPEAKER_TOP_BACK_LEFT        );
	EW(IDC_BUTTON_I_EN_TBC,in_channelmask & SPEAKER_TOP_BACK_CENTER      );
	EW(IDC_BUTTON_I_EN_TBR,in_channelmask & SPEAKER_TOP_BACK_RIGHT       );

	//Output enable
	EW(IDC_BUTTON_O_EN_FL, out_channelmask & SPEAKER_FRONT_LEFT           );
	EW(IDC_BUTTON_O_EN_FR, out_channelmask & SPEAKER_FRONT_RIGHT          );
	EW(IDC_BUTTON_O_EN_FC, out_channelmask & SPEAKER_FRONT_CENTER         );
	EW(IDC_BUTTON_O_EN_LF, out_channelmask & SPEAKER_LOW_FREQUENCY        );
	EW(IDC_BUTTON_O_EN_BL, out_channelmask & SPEAKER_BACK_LEFT            );
	EW(IDC_BUTTON_O_EN_BR, out_channelmask & SPEAKER_BACK_RIGHT           );
	EW(IDC_BUTTON_O_EN_FLC,out_channelmask & SPEAKER_FRONT_LEFT_OF_CENTER );
	EW(IDC_BUTTON_O_EN_FRC,out_channelmask & SPEAKER_FRONT_RIGHT_OF_CENTER);
	EW(IDC_BUTTON_O_EN_BC, out_channelmask & SPEAKER_BACK_CENTER          );
	EW(IDC_BUTTON_O_EN_SL, out_channelmask & SPEAKER_SIDE_LEFT            );
	EW(IDC_BUTTON_O_EN_SR, out_channelmask & SPEAKER_SIDE_RIGHT           );
	EW(IDC_BUTTON_O_EN_TC, out_channelmask & SPEAKER_TOP_CENTER           );
	EW(IDC_BUTTON_O_EN_TFL,out_channelmask & SPEAKER_TOP_FRONT_LEFT       );
	EW(IDC_BUTTON_O_EN_TFC,out_channelmask & SPEAKER_TOP_FRONT_CENTER     );
	EW(IDC_BUTTON_O_EN_TFR,out_channelmask & SPEAKER_TOP_FRONT_RIGHT      );
	EW(IDC_BUTTON_O_EN_TBL,out_channelmask & SPEAKER_TOP_BACK_LEFT        );
	EW(IDC_BUTTON_O_EN_TBC,out_channelmask & SPEAKER_TOP_BACK_CENTER      );
	EW(IDC_BUTTON_O_EN_TBR,out_channelmask & SPEAKER_TOP_BACK_RIGHT       );

	UINT bInputGainsEnable = (BST_CHECKED==IsDlgButtonChecked(m_Dlg,IDC_CHECK_ENABLE_INPUT_GAINS));
	UINT bOutputGainsEnable = (BST_CHECKED==IsDlgButtonChecked(m_Dlg,IDC_CHECK_ENABLE_OUTPUT_GAINS));

	EW(IDC_STATIC_FRAME_INPUT_GAINS,bInputGainsEnable);
	EW(IDC_STATIC_FRAME_OUTPUT_GAINS,bOutputGainsEnable);

	//Static channel names
	EW(IDC_STATIC_I_FL ,bInputGainsEnable && (in_channelmask & SPEAKER_FRONT_LEFT           ));
	EW(IDC_STATIC_I_FR ,bInputGainsEnable && (in_channelmask & SPEAKER_FRONT_RIGHT          ));
	EW(IDC_STATIC_I_FC ,bInputGainsEnable && (in_channelmask & SPEAKER_FRONT_CENTER         ));
	EW(IDC_STATIC_I_LF ,bInputGainsEnable && (in_channelmask & SPEAKER_LOW_FREQUENCY        ));
	EW(IDC_STATIC_I_BL ,bInputGainsEnable && (in_channelmask & SPEAKER_BACK_LEFT            ));
	EW(IDC_STATIC_I_BR ,bInputGainsEnable && (in_channelmask & SPEAKER_BACK_RIGHT           ));
	EW(IDC_STATIC_I_FLC,bInputGainsEnable && (in_channelmask & SPEAKER_FRONT_LEFT_OF_CENTER ));
	EW(IDC_STATIC_I_FRC,bInputGainsEnable && (in_channelmask & SPEAKER_FRONT_RIGHT_OF_CENTER));
	EW(IDC_STATIC_I_BC ,bInputGainsEnable && (in_channelmask & SPEAKER_BACK_CENTER          ));
	EW(IDC_STATIC_I_SL ,bInputGainsEnable && (in_channelmask & SPEAKER_SIDE_LEFT            ));
	EW(IDC_STATIC_I_SR ,bInputGainsEnable && (in_channelmask & SPEAKER_SIDE_RIGHT           ));
	EW(IDC_STATIC_I_TC ,bInputGainsEnable && (in_channelmask & SPEAKER_TOP_CENTER           ));
	EW(IDC_STATIC_I_TFL,bInputGainsEnable && (in_channelmask & SPEAKER_TOP_FRONT_LEFT       ));
	EW(IDC_STATIC_I_TFC,bInputGainsEnable && (in_channelmask & SPEAKER_TOP_FRONT_CENTER     ));
	EW(IDC_STATIC_I_TFR,bInputGainsEnable && (in_channelmask & SPEAKER_TOP_FRONT_RIGHT      ));
	EW(IDC_STATIC_I_TBL,bInputGainsEnable && (in_channelmask & SPEAKER_TOP_BACK_LEFT        ));
	EW(IDC_STATIC_I_TBC,bInputGainsEnable && (in_channelmask & SPEAKER_TOP_BACK_CENTER      ));
	EW(IDC_STATIC_I_TBR,bInputGainsEnable && (in_channelmask & SPEAKER_TOP_BACK_RIGHT       ));

	EW(IDC_STATIC_O_FL ,bOutputGainsEnable && (out_channelmask & SPEAKER_FRONT_LEFT           ));
	EW(IDC_STATIC_O_FR ,bOutputGainsEnable && (out_channelmask & SPEAKER_FRONT_RIGHT          ));
	EW(IDC_STATIC_O_FC ,bOutputGainsEnable && (out_channelmask & SPEAKER_FRONT_CENTER         ));
	EW(IDC_STATIC_O_LF ,bOutputGainsEnable && (out_channelmask & SPEAKER_LOW_FREQUENCY        ));
	EW(IDC_STATIC_O_BL ,bOutputGainsEnable && (out_channelmask & SPEAKER_BACK_LEFT            ));
	EW(IDC_STATIC_O_BR ,bOutputGainsEnable && (out_channelmask & SPEAKER_BACK_RIGHT           ));
	EW(IDC_STATIC_O_FLC,bOutputGainsEnable && (out_channelmask & SPEAKER_FRONT_LEFT_OF_CENTER ));
	EW(IDC_STATIC_O_FRC,bOutputGainsEnable && (out_channelmask & SPEAKER_FRONT_RIGHT_OF_CENTER));
	EW(IDC_STATIC_O_BC ,bOutputGainsEnable && (out_channelmask & SPEAKER_BACK_CENTER          ));
	EW(IDC_STATIC_O_SL ,bOutputGainsEnable && (out_channelmask & SPEAKER_SIDE_LEFT            ));
	EW(IDC_STATIC_O_SR ,bOutputGainsEnable && (out_channelmask & SPEAKER_SIDE_RIGHT           ));
	EW(IDC_STATIC_O_TC ,bOutputGainsEnable && (out_channelmask & SPEAKER_TOP_CENTER           ));
	EW(IDC_STATIC_O_TFL,bOutputGainsEnable && (out_channelmask & SPEAKER_TOP_FRONT_LEFT       ));
	EW(IDC_STATIC_O_TFC,bOutputGainsEnable && (out_channelmask & SPEAKER_TOP_FRONT_CENTER     ));
	EW(IDC_STATIC_O_TFR,bOutputGainsEnable && (out_channelmask & SPEAKER_TOP_FRONT_RIGHT      ));
	EW(IDC_STATIC_O_TBL,bOutputGainsEnable && (out_channelmask & SPEAKER_TOP_BACK_LEFT        ));
	EW(IDC_STATIC_O_TBC,bOutputGainsEnable && (out_channelmask & SPEAKER_TOP_BACK_CENTER      ));
	EW(IDC_STATIC_O_TBR,bOutputGainsEnable && (out_channelmask & SPEAKER_TOP_BACK_RIGHT       ));

	//Slider bars
	EW(IDC_SLIDER_I_FL ,bInputGainsEnable && (in_channelmask & SPEAKER_FRONT_LEFT           ));
	EW(IDC_SLIDER_I_FR ,bInputGainsEnable && (in_channelmask & SPEAKER_FRONT_RIGHT          ));
	EW(IDC_SLIDER_I_FC ,bInputGainsEnable && (in_channelmask & SPEAKER_FRONT_CENTER         ));
	EW(IDC_SLIDER_I_LF ,bInputGainsEnable && (in_channelmask & SPEAKER_LOW_FREQUENCY        ));
	EW(IDC_SLIDER_I_BL ,bInputGainsEnable && (in_channelmask & SPEAKER_BACK_LEFT            ));
	EW(IDC_SLIDER_I_BR ,bInputGainsEnable && (in_channelmask & SPEAKER_BACK_RIGHT           ));
	EW(IDC_SLIDER_I_FLC,bInputGainsEnable && (in_channelmask & SPEAKER_FRONT_LEFT_OF_CENTER ));
	EW(IDC_SLIDER_I_FRC,bInputGainsEnable && (in_channelmask & SPEAKER_FRONT_RIGHT_OF_CENTER));
	EW(IDC_SLIDER_I_BC ,bInputGainsEnable && (in_channelmask & SPEAKER_BACK_CENTER          ));
	EW(IDC_SLIDER_I_SL ,bInputGainsEnable && (in_channelmask & SPEAKER_SIDE_LEFT            ));
	EW(IDC_SLIDER_I_SR ,bInputGainsEnable && (in_channelmask & SPEAKER_SIDE_RIGHT           ));
	EW(IDC_SLIDER_I_TC ,bInputGainsEnable && (in_channelmask & SPEAKER_TOP_CENTER           ));
	EW(IDC_SLIDER_I_TFL,bInputGainsEnable && (in_channelmask & SPEAKER_TOP_FRONT_LEFT       ));
	EW(IDC_SLIDER_I_TFC,bInputGainsEnable && (in_channelmask & SPEAKER_TOP_FRONT_CENTER     ));
	EW(IDC_SLIDER_I_TFR,bInputGainsEnable && (in_channelmask & SPEAKER_TOP_FRONT_RIGHT      ));
	EW(IDC_SLIDER_I_TBL,bInputGainsEnable && (in_channelmask & SPEAKER_TOP_BACK_LEFT        ));
	EW(IDC_SLIDER_I_TBC,bInputGainsEnable && (in_channelmask & SPEAKER_TOP_BACK_CENTER      ));
	EW(IDC_SLIDER_I_TBR,bInputGainsEnable && (in_channelmask & SPEAKER_TOP_BACK_RIGHT       ));

	EW(IDC_SLIDER_O_FL ,bOutputGainsEnable && (out_channelmask & SPEAKER_FRONT_LEFT           ));
	EW(IDC_SLIDER_O_FR ,bOutputGainsEnable && (out_channelmask & SPEAKER_FRONT_RIGHT          ));
	EW(IDC_SLIDER_O_FC ,bOutputGainsEnable && (out_channelmask & SPEAKER_FRONT_CENTER         ));
	EW(IDC_SLIDER_O_LF ,bOutputGainsEnable && (out_channelmask & SPEAKER_LOW_FREQUENCY        ));
	EW(IDC_SLIDER_O_BL ,bOutputGainsEnable && (out_channelmask & SPEAKER_BACK_LEFT            ));
	EW(IDC_SLIDER_O_BR ,bOutputGainsEnable && (out_channelmask & SPEAKER_BACK_RIGHT           ));
	EW(IDC_SLIDER_O_FLC,bOutputGainsEnable && (out_channelmask & SPEAKER_FRONT_LEFT_OF_CENTER ));
	EW(IDC_SLIDER_O_FRC,bOutputGainsEnable && (out_channelmask & SPEAKER_FRONT_RIGHT_OF_CENTER));
	EW(IDC_SLIDER_O_BC ,bOutputGainsEnable && (out_channelmask & SPEAKER_BACK_CENTER          ));
	EW(IDC_SLIDER_O_SL ,bOutputGainsEnable && (out_channelmask & SPEAKER_SIDE_LEFT            ));
	EW(IDC_SLIDER_O_SR ,bOutputGainsEnable && (out_channelmask & SPEAKER_SIDE_RIGHT           ));
	EW(IDC_SLIDER_O_TC ,bOutputGainsEnable && (out_channelmask & SPEAKER_TOP_CENTER           ));
	EW(IDC_SLIDER_O_TFL,bOutputGainsEnable && (out_channelmask & SPEAKER_TOP_FRONT_LEFT       ));
	EW(IDC_SLIDER_O_TFC,bOutputGainsEnable && (out_channelmask & SPEAKER_TOP_FRONT_CENTER     ));
	EW(IDC_SLIDER_O_TFR,bOutputGainsEnable && (out_channelmask & SPEAKER_TOP_FRONT_RIGHT      ));
	EW(IDC_SLIDER_O_TBL,bOutputGainsEnable && (out_channelmask & SPEAKER_TOP_BACK_LEFT        ));
	EW(IDC_SLIDER_O_TBC,bOutputGainsEnable && (out_channelmask & SPEAKER_TOP_BACK_CENTER      ));
	EW(IDC_SLIDER_O_TBR,bOutputGainsEnable && (out_channelmask & SPEAKER_TOP_BACK_RIGHT       ));

	//Edit controls
	EW(IDC_EDIT_I_FL ,bInputGainsEnable && (in_channelmask & SPEAKER_FRONT_LEFT           ));
	EW(IDC_EDIT_I_FR ,bInputGainsEnable && (in_channelmask & SPEAKER_FRONT_RIGHT          ));
	EW(IDC_EDIT_I_FC ,bInputGainsEnable && (in_channelmask & SPEAKER_FRONT_CENTER         ));
	EW(IDC_EDIT_I_LF ,bInputGainsEnable && (in_channelmask & SPEAKER_LOW_FREQUENCY        ));
	EW(IDC_EDIT_I_BL ,bInputGainsEnable && (in_channelmask & SPEAKER_BACK_LEFT            ));
	EW(IDC_EDIT_I_BR ,bInputGainsEnable && (in_channelmask & SPEAKER_BACK_RIGHT           ));
	EW(IDC_EDIT_I_FLC,bInputGainsEnable && (in_channelmask & SPEAKER_FRONT_LEFT_OF_CENTER ));
	EW(IDC_EDIT_I_FRC,bInputGainsEnable && (in_channelmask & SPEAKER_FRONT_RIGHT_OF_CENTER));
	EW(IDC_EDIT_I_BC ,bInputGainsEnable && (in_channelmask & SPEAKER_BACK_CENTER          ));
	EW(IDC_EDIT_I_SL ,bInputGainsEnable && (in_channelmask & SPEAKER_SIDE_LEFT            ));
	EW(IDC_EDIT_I_SR ,bInputGainsEnable && (in_channelmask & SPEAKER_SIDE_RIGHT           ));
	EW(IDC_EDIT_I_TC ,bInputGainsEnable && (in_channelmask & SPEAKER_TOP_CENTER           ));
	EW(IDC_EDIT_I_TFL,bInputGainsEnable && (in_channelmask & SPEAKER_TOP_FRONT_LEFT       ));
	EW(IDC_EDIT_I_TFC,bInputGainsEnable && (in_channelmask & SPEAKER_TOP_FRONT_CENTER     ));
	EW(IDC_EDIT_I_TFR,bInputGainsEnable && (in_channelmask & SPEAKER_TOP_FRONT_RIGHT      ));
	EW(IDC_EDIT_I_TBL,bInputGainsEnable && (in_channelmask & SPEAKER_TOP_BACK_LEFT        ));
	EW(IDC_EDIT_I_TBC,bInputGainsEnable && (in_channelmask & SPEAKER_TOP_BACK_CENTER      ));
	EW(IDC_EDIT_I_TBR,bInputGainsEnable && (in_channelmask & SPEAKER_TOP_BACK_RIGHT       ));

	EW(IDC_EDIT_O_FL ,bOutputGainsEnable && (out_channelmask & SPEAKER_FRONT_LEFT           ));
	EW(IDC_EDIT_O_FR ,bOutputGainsEnable && (out_channelmask & SPEAKER_FRONT_RIGHT          ));
	EW(IDC_EDIT_O_FC ,bOutputGainsEnable && (out_channelmask & SPEAKER_FRONT_CENTER         ));
	EW(IDC_EDIT_O_LF ,bOutputGainsEnable && (out_channelmask & SPEAKER_LOW_FREQUENCY        ));
	EW(IDC_EDIT_O_BL ,bOutputGainsEnable && (out_channelmask & SPEAKER_BACK_LEFT            ));
	EW(IDC_EDIT_O_BR ,bOutputGainsEnable && (out_channelmask & SPEAKER_BACK_RIGHT           ));
	EW(IDC_EDIT_O_FLC,bOutputGainsEnable && (out_channelmask & SPEAKER_FRONT_LEFT_OF_CENTER ));
	EW(IDC_EDIT_O_FRC,bOutputGainsEnable && (out_channelmask & SPEAKER_FRONT_RIGHT_OF_CENTER));
	EW(IDC_EDIT_O_BC ,bOutputGainsEnable && (out_channelmask & SPEAKER_BACK_CENTER          ));
	EW(IDC_EDIT_O_SL ,bOutputGainsEnable && (out_channelmask & SPEAKER_SIDE_LEFT            ));
	EW(IDC_EDIT_O_SR ,bOutputGainsEnable && (out_channelmask & SPEAKER_SIDE_RIGHT           ));
	EW(IDC_EDIT_O_TC ,bOutputGainsEnable && (out_channelmask & SPEAKER_TOP_CENTER           ));
	EW(IDC_EDIT_O_TFL,bOutputGainsEnable && (out_channelmask & SPEAKER_TOP_FRONT_LEFT       ));
	EW(IDC_EDIT_O_TFC,bOutputGainsEnable && (out_channelmask & SPEAKER_TOP_FRONT_CENTER     ));
	EW(IDC_EDIT_O_TFR,bOutputGainsEnable && (out_channelmask & SPEAKER_TOP_FRONT_RIGHT      ));
	EW(IDC_EDIT_O_TBL,bOutputGainsEnable && (out_channelmask & SPEAKER_TOP_BACK_LEFT        ));
	EW(IDC_EDIT_O_TBC,bOutputGainsEnable && (out_channelmask & SPEAKER_TOP_BACK_CENTER      ));
	EW(IDC_EDIT_O_TBR,bOutputGainsEnable && (out_channelmask & SPEAKER_TOP_BACK_RIGHT       ));

	//Join checkboxes

	//Macro to check if two controls are both enabled
#define IWE2(CONTROLID_1,CONTROLID_2) (IsWindowEnabled(GetDlgItem(m_Dlg,CONTROLID_1)) && IsWindowEnabled(GetDlgItem(m_Dlg,CONTROLID_2)))
	
	EW(IDC_CHECK_JOIN_I_FLFR,	IWE2(IDC_SLIDER_I_FL,IDC_SLIDER_I_FR  ));
	EW(IDC_CHECK_JOIN_I_BLBR,	IWE2(IDC_SLIDER_I_BL,IDC_SLIDER_I_BR  ));
	EW(IDC_CHECK_JOIN_I_FLCFRC,	IWE2(IDC_SLIDER_I_FLC,IDC_SLIDER_I_FRC));
	EW(IDC_CHECK_JOIN_I_SLSR,	IWE2(IDC_SLIDER_I_SL,IDC_SLIDER_I_SR  ));
	EW(IDC_CHECK_JOIN_I_TFLTFR,	IWE2(IDC_SLIDER_I_TFL,IDC_SLIDER_I_TFR));
	EW(IDC_CHECK_JOIN_I_TBLTBR,	IWE2(IDC_SLIDER_I_TBL,IDC_SLIDER_I_TBR));

	EW(IDC_CHECK_JOIN_O_FLFR,	IWE2(IDC_SLIDER_O_FL,IDC_SLIDER_O_FR  ));
	EW(IDC_CHECK_JOIN_O_BLBR,	IWE2(IDC_SLIDER_O_BL,IDC_SLIDER_O_BR  ));
	EW(IDC_CHECK_JOIN_O_FLCFRC,	IWE2(IDC_SLIDER_O_FLC,IDC_SLIDER_O_FRC));
	EW(IDC_CHECK_JOIN_O_SLSR,	IWE2(IDC_SLIDER_O_SL,IDC_SLIDER_O_SR  ));
	EW(IDC_CHECK_JOIN_O_TFLTFR,	IWE2(IDC_SLIDER_O_TFL,IDC_SLIDER_O_TFR));
	EW(IDC_CHECK_JOIN_O_TBLTBR,	IWE2(IDC_SLIDER_O_TBL,IDC_SLIDER_O_TBR));

	//Master volume
	EW(IDC_SLIDER_MASTER_VOLUME, BST_CHECKED==IsDlgButtonChecked(m_Dlg,IDC_CHECK_ENABLE_MASTER_VOLUME));
	EW(IDC_EDIT_MASTER_VOLUME, BST_CHECKED==IsDlgButtonChecked(m_Dlg,IDC_CHECK_ENABLE_MASTER_VOLUME));

}

///////////////////////////////////////////////////////////////////////
// GetControlValues: read the setup from current controls
///////////////////////////////////////////////////////////////////////
void CStereoDownmixerPropP2::GetControlValues()
{
	//Checkboxes
	m_StereoDownmixerParams.p2.bEnableInputGains  =FALSE;
	m_StereoDownmixerParams.p2.bEnableOutputGains =FALSE;
	m_StereoDownmixerParams.p2.bEnableMasterVolume=FALSE;
	if (BST_CHECKED==IsDlgButtonChecked(m_Dlg,IDC_CHECK_ENABLE_INPUT_GAINS  )) m_StereoDownmixerParams.p2.bEnableInputGains  =TRUE;
	if (BST_CHECKED==IsDlgButtonChecked(m_Dlg,IDC_CHECK_ENABLE_OUTPUT_GAINS )) m_StereoDownmixerParams.p2.bEnableOutputGains =TRUE;
	if (BST_CHECKED==IsDlgButtonChecked(m_Dlg,IDC_CHECK_ENABLE_MASTER_VOLUME)) m_StereoDownmixerParams.p2.bEnableMasterVolume=TRUE;

	//Join checkboxes
	m_StereoDownmixerParams.p2.I.bJoinFLFR  = FALSE;
	m_StereoDownmixerParams.p2.I.bJoinBLBR  = FALSE;
	m_StereoDownmixerParams.p2.I.bJoinFLCFRC= FALSE;
	m_StereoDownmixerParams.p2.I.bJoinSLSR  = FALSE;
	m_StereoDownmixerParams.p2.I.bJoinTFLTFR= FALSE;
	m_StereoDownmixerParams.p2.I.bJoinTBLTBR= FALSE;

	m_StereoDownmixerParams.p2.O.bJoinFLFR  = FALSE;
	m_StereoDownmixerParams.p2.O.bJoinBLBR  = FALSE;
	m_StereoDownmixerParams.p2.O.bJoinFLCFRC= FALSE;
	m_StereoDownmixerParams.p2.O.bJoinSLSR  = FALSE;
	m_StereoDownmixerParams.p2.O.bJoinTFLTFR= FALSE;
	m_StereoDownmixerParams.p2.O.bJoinTBLTBR= FALSE;

	if (BST_CHECKED==IsDlgButtonChecked(m_Dlg,IDC_CHECK_JOIN_I_FLFR  )) m_StereoDownmixerParams.p2.I.bJoinFLFR  =TRUE;
	if (BST_CHECKED==IsDlgButtonChecked(m_Dlg,IDC_CHECK_JOIN_I_BLBR  )) m_StereoDownmixerParams.p2.I.bJoinBLBR  =TRUE;
	if (BST_CHECKED==IsDlgButtonChecked(m_Dlg,IDC_CHECK_JOIN_I_FLCFRC)) m_StereoDownmixerParams.p2.I.bJoinFLCFRC=TRUE;
	if (BST_CHECKED==IsDlgButtonChecked(m_Dlg,IDC_CHECK_JOIN_I_SLSR  )) m_StereoDownmixerParams.p2.I.bJoinSLSR  =TRUE;
	if (BST_CHECKED==IsDlgButtonChecked(m_Dlg,IDC_CHECK_JOIN_I_TFLTFR)) m_StereoDownmixerParams.p2.I.bJoinTFLTFR=TRUE;
	if (BST_CHECKED==IsDlgButtonChecked(m_Dlg,IDC_CHECK_JOIN_I_TBLTBR)) m_StereoDownmixerParams.p2.I.bJoinTBLTBR=TRUE;

	if (BST_CHECKED==IsDlgButtonChecked(m_Dlg,IDC_CHECK_JOIN_O_FLFR  )) m_StereoDownmixerParams.p2.O.bJoinFLFR  =TRUE;
	if (BST_CHECKED==IsDlgButtonChecked(m_Dlg,IDC_CHECK_JOIN_O_BLBR  )) m_StereoDownmixerParams.p2.O.bJoinBLBR  =TRUE;
	if (BST_CHECKED==IsDlgButtonChecked(m_Dlg,IDC_CHECK_JOIN_O_FLCFRC)) m_StereoDownmixerParams.p2.O.bJoinFLCFRC=TRUE;
	if (BST_CHECKED==IsDlgButtonChecked(m_Dlg,IDC_CHECK_JOIN_O_SLSR  )) m_StereoDownmixerParams.p2.O.bJoinSLSR  =TRUE;
	if (BST_CHECKED==IsDlgButtonChecked(m_Dlg,IDC_CHECK_JOIN_O_TFLTFR)) m_StereoDownmixerParams.p2.O.bJoinTFLTFR=TRUE;
	if (BST_CHECKED==IsDlgButtonChecked(m_Dlg,IDC_CHECK_JOIN_O_TBLTBR)) m_StereoDownmixerParams.p2.O.bJoinTBLTBR=TRUE;

	//Input enables and output enables
	m_StereoDownmixerParams.p2.InputEnableBitmask=0;
	if (BST_CHECKED==IsDlgButtonChecked(m_Dlg,IDC_BUTTON_I_EN_FL )) m_StereoDownmixerParams.p2.InputEnableBitmask |= SPEAKER_FRONT_LEFT           ;
	if (BST_CHECKED==IsDlgButtonChecked(m_Dlg,IDC_BUTTON_I_EN_FR )) m_StereoDownmixerParams.p2.InputEnableBitmask |= SPEAKER_FRONT_RIGHT          ;
	if (BST_CHECKED==IsDlgButtonChecked(m_Dlg,IDC_BUTTON_I_EN_FC )) m_StereoDownmixerParams.p2.InputEnableBitmask |= SPEAKER_FRONT_CENTER         ;
	if (BST_CHECKED==IsDlgButtonChecked(m_Dlg,IDC_BUTTON_I_EN_LF )) m_StereoDownmixerParams.p2.InputEnableBitmask |= SPEAKER_LOW_FREQUENCY        ;
	if (BST_CHECKED==IsDlgButtonChecked(m_Dlg,IDC_BUTTON_I_EN_BL )) m_StereoDownmixerParams.p2.InputEnableBitmask |= SPEAKER_BACK_LEFT            ;
	if (BST_CHECKED==IsDlgButtonChecked(m_Dlg,IDC_BUTTON_I_EN_BR )) m_StereoDownmixerParams.p2.InputEnableBitmask |= SPEAKER_BACK_RIGHT           ;
	if (BST_CHECKED==IsDlgButtonChecked(m_Dlg,IDC_BUTTON_I_EN_FLC)) m_StereoDownmixerParams.p2.InputEnableBitmask |= SPEAKER_FRONT_LEFT_OF_CENTER ;
	if (BST_CHECKED==IsDlgButtonChecked(m_Dlg,IDC_BUTTON_I_EN_FRC)) m_StereoDownmixerParams.p2.InputEnableBitmask |= SPEAKER_FRONT_RIGHT_OF_CENTER;
	if (BST_CHECKED==IsDlgButtonChecked(m_Dlg,IDC_BUTTON_I_EN_BC )) m_StereoDownmixerParams.p2.InputEnableBitmask |= SPEAKER_BACK_CENTER          ;
	if (BST_CHECKED==IsDlgButtonChecked(m_Dlg,IDC_BUTTON_I_EN_SL )) m_StereoDownmixerParams.p2.InputEnableBitmask |= SPEAKER_SIDE_LEFT            ;
	if (BST_CHECKED==IsDlgButtonChecked(m_Dlg,IDC_BUTTON_I_EN_SR )) m_StereoDownmixerParams.p2.InputEnableBitmask |= SPEAKER_SIDE_RIGHT           ;
	if (BST_CHECKED==IsDlgButtonChecked(m_Dlg,IDC_BUTTON_I_EN_TC )) m_StereoDownmixerParams.p2.InputEnableBitmask |= SPEAKER_TOP_CENTER           ;
	if (BST_CHECKED==IsDlgButtonChecked(m_Dlg,IDC_BUTTON_I_EN_TFL)) m_StereoDownmixerParams.p2.InputEnableBitmask |= SPEAKER_TOP_FRONT_LEFT       ;
	if (BST_CHECKED==IsDlgButtonChecked(m_Dlg,IDC_BUTTON_I_EN_TFC)) m_StereoDownmixerParams.p2.InputEnableBitmask |= SPEAKER_TOP_FRONT_CENTER     ;
	if (BST_CHECKED==IsDlgButtonChecked(m_Dlg,IDC_BUTTON_I_EN_TFR)) m_StereoDownmixerParams.p2.InputEnableBitmask |= SPEAKER_TOP_FRONT_RIGHT      ;
	if (BST_CHECKED==IsDlgButtonChecked(m_Dlg,IDC_BUTTON_I_EN_TBL)) m_StereoDownmixerParams.p2.InputEnableBitmask |= SPEAKER_TOP_BACK_LEFT        ;
	if (BST_CHECKED==IsDlgButtonChecked(m_Dlg,IDC_BUTTON_I_EN_TBC)) m_StereoDownmixerParams.p2.InputEnableBitmask |= SPEAKER_TOP_BACK_CENTER      ;
	if (BST_CHECKED==IsDlgButtonChecked(m_Dlg,IDC_BUTTON_I_EN_TBR)) m_StereoDownmixerParams.p2.InputEnableBitmask |= SPEAKER_TOP_BACK_RIGHT       ;
       
	m_StereoDownmixerParams.p2.OutputEnableBitmask=0;
	if (BST_CHECKED==IsDlgButtonChecked(m_Dlg,IDC_BUTTON_O_EN_FL )) m_StereoDownmixerParams.p2.OutputEnableBitmask |= SPEAKER_FRONT_LEFT           ;
	if (BST_CHECKED==IsDlgButtonChecked(m_Dlg,IDC_BUTTON_O_EN_FR )) m_StereoDownmixerParams.p2.OutputEnableBitmask |= SPEAKER_FRONT_RIGHT          ;
	if (BST_CHECKED==IsDlgButtonChecked(m_Dlg,IDC_BUTTON_O_EN_FC )) m_StereoDownmixerParams.p2.OutputEnableBitmask |= SPEAKER_FRONT_CENTER         ;
	if (BST_CHECKED==IsDlgButtonChecked(m_Dlg,IDC_BUTTON_O_EN_LF )) m_StereoDownmixerParams.p2.OutputEnableBitmask |= SPEAKER_LOW_FREQUENCY        ;
	if (BST_CHECKED==IsDlgButtonChecked(m_Dlg,IDC_BUTTON_O_EN_BL )) m_StereoDownmixerParams.p2.OutputEnableBitmask |= SPEAKER_BACK_LEFT            ;
	if (BST_CHECKED==IsDlgButtonChecked(m_Dlg,IDC_BUTTON_O_EN_BR )) m_StereoDownmixerParams.p2.OutputEnableBitmask |= SPEAKER_BACK_RIGHT           ;
	if (BST_CHECKED==IsDlgButtonChecked(m_Dlg,IDC_BUTTON_O_EN_FLC)) m_StereoDownmixerParams.p2.OutputEnableBitmask |= SPEAKER_FRONT_LEFT_OF_CENTER ;
	if (BST_CHECKED==IsDlgButtonChecked(m_Dlg,IDC_BUTTON_O_EN_FRC)) m_StereoDownmixerParams.p2.OutputEnableBitmask |= SPEAKER_FRONT_RIGHT_OF_CENTER;
	if (BST_CHECKED==IsDlgButtonChecked(m_Dlg,IDC_BUTTON_O_EN_BC )) m_StereoDownmixerParams.p2.OutputEnableBitmask |= SPEAKER_BACK_CENTER          ;
	if (BST_CHECKED==IsDlgButtonChecked(m_Dlg,IDC_BUTTON_O_EN_SL )) m_StereoDownmixerParams.p2.OutputEnableBitmask |= SPEAKER_SIDE_LEFT            ;
	if (BST_CHECKED==IsDlgButtonChecked(m_Dlg,IDC_BUTTON_O_EN_SR )) m_StereoDownmixerParams.p2.OutputEnableBitmask |= SPEAKER_SIDE_RIGHT           ;
	if (BST_CHECKED==IsDlgButtonChecked(m_Dlg,IDC_BUTTON_O_EN_TC )) m_StereoDownmixerParams.p2.OutputEnableBitmask |= SPEAKER_TOP_CENTER           ;
	if (BST_CHECKED==IsDlgButtonChecked(m_Dlg,IDC_BUTTON_O_EN_TFL)) m_StereoDownmixerParams.p2.OutputEnableBitmask |= SPEAKER_TOP_FRONT_LEFT       ;
	if (BST_CHECKED==IsDlgButtonChecked(m_Dlg,IDC_BUTTON_O_EN_TFC)) m_StereoDownmixerParams.p2.OutputEnableBitmask |= SPEAKER_TOP_FRONT_CENTER     ;
	if (BST_CHECKED==IsDlgButtonChecked(m_Dlg,IDC_BUTTON_O_EN_TFR)) m_StereoDownmixerParams.p2.OutputEnableBitmask |= SPEAKER_TOP_FRONT_RIGHT      ;
	if (BST_CHECKED==IsDlgButtonChecked(m_Dlg,IDC_BUTTON_O_EN_TBL)) m_StereoDownmixerParams.p2.OutputEnableBitmask |= SPEAKER_TOP_BACK_LEFT        ;
	if (BST_CHECKED==IsDlgButtonChecked(m_Dlg,IDC_BUTTON_O_EN_TBC)) m_StereoDownmixerParams.p2.OutputEnableBitmask |= SPEAKER_TOP_BACK_CENTER      ;
	if (BST_CHECKED==IsDlgButtonChecked(m_Dlg,IDC_BUTTON_O_EN_TBR)) m_StereoDownmixerParams.p2.OutputEnableBitmask |= SPEAKER_TOP_BACK_RIGHT       ;


	//Gains
	//We rely upon the edit controls
	m_StereoDownmixerParams.p2.I.FL  =	GetDlgItemInt(m_Dlg,IDC_EDIT_I_FL ,NULL,FALSE);
	m_StereoDownmixerParams.p2.I.FR  =	GetDlgItemInt(m_Dlg,IDC_EDIT_I_FR ,NULL,FALSE);
	m_StereoDownmixerParams.p2.I.FC  =	GetDlgItemInt(m_Dlg,IDC_EDIT_I_FC ,NULL,FALSE);
	m_StereoDownmixerParams.p2.I.LF  =	GetDlgItemInt(m_Dlg,IDC_EDIT_I_LF ,NULL,FALSE);
	m_StereoDownmixerParams.p2.I.BL  =	GetDlgItemInt(m_Dlg,IDC_EDIT_I_BL ,NULL,FALSE);
	m_StereoDownmixerParams.p2.I.BR  =	GetDlgItemInt(m_Dlg,IDC_EDIT_I_BR ,NULL,FALSE);
	m_StereoDownmixerParams.p2.I.FLC =	GetDlgItemInt(m_Dlg,IDC_EDIT_I_FLC,NULL,FALSE);
	m_StereoDownmixerParams.p2.I.FRC =	GetDlgItemInt(m_Dlg,IDC_EDIT_I_FRC,NULL,FALSE);
	m_StereoDownmixerParams.p2.I.BC  =	GetDlgItemInt(m_Dlg,IDC_EDIT_I_BC ,NULL,FALSE);
	m_StereoDownmixerParams.p2.I.SL  =	GetDlgItemInt(m_Dlg,IDC_EDIT_I_SL ,NULL,FALSE);
	m_StereoDownmixerParams.p2.I.SR  =	GetDlgItemInt(m_Dlg,IDC_EDIT_I_SR ,NULL,FALSE);
	m_StereoDownmixerParams.p2.I.TC  =	GetDlgItemInt(m_Dlg,IDC_EDIT_I_TC ,NULL,FALSE);
	m_StereoDownmixerParams.p2.I.TFL =	GetDlgItemInt(m_Dlg,IDC_EDIT_I_TFL,NULL,FALSE);
	m_StereoDownmixerParams.p2.I.TFC =	GetDlgItemInt(m_Dlg,IDC_EDIT_I_TFC,NULL,FALSE);
	m_StereoDownmixerParams.p2.I.TFR =	GetDlgItemInt(m_Dlg,IDC_EDIT_I_TFR,NULL,FALSE);
	m_StereoDownmixerParams.p2.I.TBL =	GetDlgItemInt(m_Dlg,IDC_EDIT_I_TBL,NULL,FALSE);
	m_StereoDownmixerParams.p2.I.TBC =	GetDlgItemInt(m_Dlg,IDC_EDIT_I_TBC,NULL,FALSE);
	m_StereoDownmixerParams.p2.I.TFR =	GetDlgItemInt(m_Dlg,IDC_EDIT_I_TFR,NULL,FALSE);

	m_StereoDownmixerParams.p2.O.FL  =	GetDlgItemInt(m_Dlg,IDC_EDIT_O_FL ,NULL,FALSE);
	m_StereoDownmixerParams.p2.O.FR  =	GetDlgItemInt(m_Dlg,IDC_EDIT_O_FR ,NULL,FALSE);
	m_StereoDownmixerParams.p2.O.FC  =	GetDlgItemInt(m_Dlg,IDC_EDIT_O_FC ,NULL,FALSE);
	m_StereoDownmixerParams.p2.O.LF  =	GetDlgItemInt(m_Dlg,IDC_EDIT_O_LF ,NULL,FALSE);
	m_StereoDownmixerParams.p2.O.BL  =	GetDlgItemInt(m_Dlg,IDC_EDIT_O_BL ,NULL,FALSE);
	m_StereoDownmixerParams.p2.O.BR  =	GetDlgItemInt(m_Dlg,IDC_EDIT_O_BR ,NULL,FALSE);
	m_StereoDownmixerParams.p2.O.FLC =	GetDlgItemInt(m_Dlg,IDC_EDIT_O_FLC,NULL,FALSE);
	m_StereoDownmixerParams.p2.O.FRC =	GetDlgItemInt(m_Dlg,IDC_EDIT_O_FRC,NULL,FALSE);
	m_StereoDownmixerParams.p2.O.BC  =	GetDlgItemInt(m_Dlg,IDC_EDIT_O_BC ,NULL,FALSE);
	m_StereoDownmixerParams.p2.O.SL  =	GetDlgItemInt(m_Dlg,IDC_EDIT_O_SL ,NULL,FALSE);
	m_StereoDownmixerParams.p2.O.SR  =	GetDlgItemInt(m_Dlg,IDC_EDIT_O_SR ,NULL,FALSE);
	m_StereoDownmixerParams.p2.O.TC	 =	GetDlgItemInt(m_Dlg,IDC_EDIT_O_TC ,NULL,FALSE);
	m_StereoDownmixerParams.p2.O.TFL =	GetDlgItemInt(m_Dlg,IDC_EDIT_O_TFL,NULL,FALSE);
	m_StereoDownmixerParams.p2.O.TFC =	GetDlgItemInt(m_Dlg,IDC_EDIT_O_TFC,NULL,FALSE);
	m_StereoDownmixerParams.p2.O.TFR =	GetDlgItemInt(m_Dlg,IDC_EDIT_O_TFR,NULL,FALSE);
	m_StereoDownmixerParams.p2.O.TBL =	GetDlgItemInt(m_Dlg,IDC_EDIT_O_TBL,NULL,FALSE);
	m_StereoDownmixerParams.p2.O.TBC =	GetDlgItemInt(m_Dlg,IDC_EDIT_O_TBC,NULL,FALSE);
	m_StereoDownmixerParams.p2.O.TBR =	GetDlgItemInt(m_Dlg,IDC_EDIT_O_TBR,NULL,FALSE);

	//Master volume
	m_StereoDownmixerParams.p2.MasterVolumeGain	= GetDlgItemInt(m_Dlg,IDC_EDIT_MASTER_VOLUME,NULL,FALSE);
}

///////////////////////////////////////////////////////////////////////
// SetControlValues: set the current setup to the controls
///////////////////////////////////////////////////////////////////////
void CStereoDownmixerPropP2::SetControlValues()
{
	//Checkboxes
	CheckDlgButton(m_Dlg,IDC_CHECK_ENABLE_INPUT_GAINS  ,(m_StereoDownmixerParams.p2.bEnableInputGains  )? BST_CHECKED : BST_UNCHECKED);
	CheckDlgButton(m_Dlg,IDC_CHECK_ENABLE_OUTPUT_GAINS ,(m_StereoDownmixerParams.p2.bEnableOutputGains )? BST_CHECKED : BST_UNCHECKED);
	CheckDlgButton(m_Dlg,IDC_CHECK_ENABLE_MASTER_VOLUME,(m_StereoDownmixerParams.p2.bEnableMasterVolume)? BST_CHECKED : BST_UNCHECKED);

	//Join checkboxes
	CheckDlgButton(m_Dlg,IDC_CHECK_JOIN_I_FLFR  ,(m_StereoDownmixerParams.p2.I.bJoinFLFR  )? BST_CHECKED : BST_UNCHECKED);
	CheckDlgButton(m_Dlg,IDC_CHECK_JOIN_I_BLBR  ,(m_StereoDownmixerParams.p2.I.bJoinBLBR  )? BST_CHECKED : BST_UNCHECKED);
	CheckDlgButton(m_Dlg,IDC_CHECK_JOIN_I_FLCFRC,(m_StereoDownmixerParams.p2.I.bJoinFLCFRC)? BST_CHECKED : BST_UNCHECKED);
	CheckDlgButton(m_Dlg,IDC_CHECK_JOIN_I_SLSR  ,(m_StereoDownmixerParams.p2.I.bJoinSLSR  )? BST_CHECKED : BST_UNCHECKED);
	CheckDlgButton(m_Dlg,IDC_CHECK_JOIN_I_TFLTFR,(m_StereoDownmixerParams.p2.I.bJoinTFLTFR)? BST_CHECKED : BST_UNCHECKED);
	CheckDlgButton(m_Dlg,IDC_CHECK_JOIN_I_TBLTBR,(m_StereoDownmixerParams.p2.I.bJoinTBLTBR)? BST_CHECKED : BST_UNCHECKED);

	CheckDlgButton(m_Dlg,IDC_CHECK_JOIN_O_FLFR  ,(m_StereoDownmixerParams.p2.O.bJoinFLFR  )? BST_CHECKED : BST_UNCHECKED);
	CheckDlgButton(m_Dlg,IDC_CHECK_JOIN_O_BLBR  ,(m_StereoDownmixerParams.p2.O.bJoinBLBR  )? BST_CHECKED : BST_UNCHECKED);
	CheckDlgButton(m_Dlg,IDC_CHECK_JOIN_O_FLCFRC,(m_StereoDownmixerParams.p2.O.bJoinFLCFRC)? BST_CHECKED : BST_UNCHECKED);
	CheckDlgButton(m_Dlg,IDC_CHECK_JOIN_O_SLSR  ,(m_StereoDownmixerParams.p2.O.bJoinSLSR  )? BST_CHECKED : BST_UNCHECKED);
	CheckDlgButton(m_Dlg,IDC_CHECK_JOIN_O_TFLTFR,(m_StereoDownmixerParams.p2.O.bJoinTFLTFR)? BST_CHECKED : BST_UNCHECKED);
	CheckDlgButton(m_Dlg,IDC_CHECK_JOIN_O_TBLTBR,(m_StereoDownmixerParams.p2.O.bJoinTBLTBR)? BST_CHECKED : BST_UNCHECKED);

	//Input/Output enables
	CheckDlgButton(m_Dlg,IDC_BUTTON_I_EN_FL ,(m_StereoDownmixerParams.p2.InputEnableBitmask & SPEAKER_FRONT_LEFT           )? BST_CHECKED : BST_UNCHECKED);
	CheckDlgButton(m_Dlg,IDC_BUTTON_I_EN_FR ,(m_StereoDownmixerParams.p2.InputEnableBitmask & SPEAKER_FRONT_RIGHT          )? BST_CHECKED : BST_UNCHECKED);
	CheckDlgButton(m_Dlg,IDC_BUTTON_I_EN_FC ,(m_StereoDownmixerParams.p2.InputEnableBitmask & SPEAKER_FRONT_CENTER         )? BST_CHECKED : BST_UNCHECKED);
	CheckDlgButton(m_Dlg,IDC_BUTTON_I_EN_LF ,(m_StereoDownmixerParams.p2.InputEnableBitmask & SPEAKER_LOW_FREQUENCY        )? BST_CHECKED : BST_UNCHECKED);
	CheckDlgButton(m_Dlg,IDC_BUTTON_I_EN_BL ,(m_StereoDownmixerParams.p2.InputEnableBitmask & SPEAKER_BACK_LEFT            )? BST_CHECKED : BST_UNCHECKED);
	CheckDlgButton(m_Dlg,IDC_BUTTON_I_EN_BR ,(m_StereoDownmixerParams.p2.InputEnableBitmask & SPEAKER_BACK_RIGHT           )? BST_CHECKED : BST_UNCHECKED);
	CheckDlgButton(m_Dlg,IDC_BUTTON_I_EN_FLC,(m_StereoDownmixerParams.p2.InputEnableBitmask & SPEAKER_FRONT_LEFT_OF_CENTER )? BST_CHECKED : BST_UNCHECKED);
	CheckDlgButton(m_Dlg,IDC_BUTTON_I_EN_FRC,(m_StereoDownmixerParams.p2.InputEnableBitmask & SPEAKER_FRONT_RIGHT_OF_CENTER)? BST_CHECKED : BST_UNCHECKED);
	CheckDlgButton(m_Dlg,IDC_BUTTON_I_EN_BC ,(m_StereoDownmixerParams.p2.InputEnableBitmask & SPEAKER_BACK_CENTER          )? BST_CHECKED : BST_UNCHECKED);
	CheckDlgButton(m_Dlg,IDC_BUTTON_I_EN_SL ,(m_StereoDownmixerParams.p2.InputEnableBitmask & SPEAKER_SIDE_LEFT            )? BST_CHECKED : BST_UNCHECKED);
	CheckDlgButton(m_Dlg,IDC_BUTTON_I_EN_SR ,(m_StereoDownmixerParams.p2.InputEnableBitmask & SPEAKER_SIDE_RIGHT           )? BST_CHECKED : BST_UNCHECKED);
	CheckDlgButton(m_Dlg,IDC_BUTTON_I_EN_TC ,(m_StereoDownmixerParams.p2.InputEnableBitmask & SPEAKER_TOP_CENTER           )? BST_CHECKED : BST_UNCHECKED);
	CheckDlgButton(m_Dlg,IDC_BUTTON_I_EN_TFL,(m_StereoDownmixerParams.p2.InputEnableBitmask & SPEAKER_TOP_FRONT_LEFT       )? BST_CHECKED : BST_UNCHECKED);
	CheckDlgButton(m_Dlg,IDC_BUTTON_I_EN_TFC,(m_StereoDownmixerParams.p2.InputEnableBitmask & SPEAKER_TOP_FRONT_CENTER     )? BST_CHECKED : BST_UNCHECKED);
	CheckDlgButton(m_Dlg,IDC_BUTTON_I_EN_TFR,(m_StereoDownmixerParams.p2.InputEnableBitmask & SPEAKER_TOP_FRONT_RIGHT      )? BST_CHECKED : BST_UNCHECKED);
	CheckDlgButton(m_Dlg,IDC_BUTTON_I_EN_TBL,(m_StereoDownmixerParams.p2.InputEnableBitmask & SPEAKER_TOP_BACK_LEFT        )? BST_CHECKED : BST_UNCHECKED);
	CheckDlgButton(m_Dlg,IDC_BUTTON_I_EN_TBC,(m_StereoDownmixerParams.p2.InputEnableBitmask & SPEAKER_TOP_BACK_CENTER      )? BST_CHECKED : BST_UNCHECKED);
	CheckDlgButton(m_Dlg,IDC_BUTTON_I_EN_TBR,(m_StereoDownmixerParams.p2.InputEnableBitmask & SPEAKER_TOP_BACK_RIGHT       )? BST_CHECKED : BST_UNCHECKED);

	CheckDlgButton(m_Dlg,IDC_BUTTON_O_EN_FL ,(m_StereoDownmixerParams.p2.OutputEnableBitmask & SPEAKER_FRONT_LEFT           )? BST_CHECKED : BST_UNCHECKED);
	CheckDlgButton(m_Dlg,IDC_BUTTON_O_EN_FR ,(m_StereoDownmixerParams.p2.OutputEnableBitmask & SPEAKER_FRONT_RIGHT          )? BST_CHECKED : BST_UNCHECKED);
	CheckDlgButton(m_Dlg,IDC_BUTTON_O_EN_FC ,(m_StereoDownmixerParams.p2.OutputEnableBitmask & SPEAKER_FRONT_CENTER         )? BST_CHECKED : BST_UNCHECKED);
	CheckDlgButton(m_Dlg,IDC_BUTTON_O_EN_LF ,(m_StereoDownmixerParams.p2.OutputEnableBitmask & SPEAKER_LOW_FREQUENCY        )? BST_CHECKED : BST_UNCHECKED);
	CheckDlgButton(m_Dlg,IDC_BUTTON_O_EN_BL ,(m_StereoDownmixerParams.p2.OutputEnableBitmask & SPEAKER_BACK_LEFT            )? BST_CHECKED : BST_UNCHECKED);
	CheckDlgButton(m_Dlg,IDC_BUTTON_O_EN_BR ,(m_StereoDownmixerParams.p2.OutputEnableBitmask & SPEAKER_BACK_RIGHT           )? BST_CHECKED : BST_UNCHECKED);
	CheckDlgButton(m_Dlg,IDC_BUTTON_O_EN_FLC,(m_StereoDownmixerParams.p2.OutputEnableBitmask & SPEAKER_FRONT_LEFT_OF_CENTER )? BST_CHECKED : BST_UNCHECKED);
	CheckDlgButton(m_Dlg,IDC_BUTTON_O_EN_FRC,(m_StereoDownmixerParams.p2.OutputEnableBitmask & SPEAKER_FRONT_RIGHT_OF_CENTER)? BST_CHECKED : BST_UNCHECKED);
	CheckDlgButton(m_Dlg,IDC_BUTTON_O_EN_BC ,(m_StereoDownmixerParams.p2.OutputEnableBitmask & SPEAKER_BACK_CENTER          )? BST_CHECKED : BST_UNCHECKED);
	CheckDlgButton(m_Dlg,IDC_BUTTON_O_EN_SL ,(m_StereoDownmixerParams.p2.OutputEnableBitmask & SPEAKER_SIDE_LEFT            )? BST_CHECKED : BST_UNCHECKED);
	CheckDlgButton(m_Dlg,IDC_BUTTON_O_EN_SR ,(m_StereoDownmixerParams.p2.OutputEnableBitmask & SPEAKER_SIDE_RIGHT           )? BST_CHECKED : BST_UNCHECKED);
	CheckDlgButton(m_Dlg,IDC_BUTTON_O_EN_TC ,(m_StereoDownmixerParams.p2.OutputEnableBitmask & SPEAKER_TOP_CENTER           )? BST_CHECKED : BST_UNCHECKED);
	CheckDlgButton(m_Dlg,IDC_BUTTON_O_EN_TFL,(m_StereoDownmixerParams.p2.OutputEnableBitmask & SPEAKER_TOP_FRONT_LEFT       )? BST_CHECKED : BST_UNCHECKED);
	CheckDlgButton(m_Dlg,IDC_BUTTON_O_EN_TFC,(m_StereoDownmixerParams.p2.OutputEnableBitmask & SPEAKER_TOP_FRONT_CENTER     )? BST_CHECKED : BST_UNCHECKED);
	CheckDlgButton(m_Dlg,IDC_BUTTON_O_EN_TFR,(m_StereoDownmixerParams.p2.OutputEnableBitmask & SPEAKER_TOP_FRONT_RIGHT      )? BST_CHECKED : BST_UNCHECKED);
	CheckDlgButton(m_Dlg,IDC_BUTTON_O_EN_TBL,(m_StereoDownmixerParams.p2.OutputEnableBitmask & SPEAKER_TOP_BACK_LEFT        )? BST_CHECKED : BST_UNCHECKED);
	CheckDlgButton(m_Dlg,IDC_BUTTON_O_EN_TBC,(m_StereoDownmixerParams.p2.OutputEnableBitmask & SPEAKER_TOP_BACK_CENTER      )? BST_CHECKED : BST_UNCHECKED);
	CheckDlgButton(m_Dlg,IDC_BUTTON_O_EN_TBR,(m_StereoDownmixerParams.p2.OutputEnableBitmask & SPEAKER_TOP_BACK_RIGHT       )? BST_CHECKED : BST_UNCHECKED);

	//Gains

	//Editboxes:
	if (m_StereoDownmixerParams.p2.I.FL  != ::GetDlgItemInt(m_Dlg,IDC_EDIT_I_FL ,NULL,FALSE)) ::SetDlgItemInt(m_Dlg,IDC_EDIT_I_FL ,m_StereoDownmixerParams.p2.I.FL ,FALSE);
	if (m_StereoDownmixerParams.p2.I.FR  != ::GetDlgItemInt(m_Dlg,IDC_EDIT_I_FR ,NULL,FALSE)) ::SetDlgItemInt(m_Dlg,IDC_EDIT_I_FR ,m_StereoDownmixerParams.p2.I.FR ,FALSE);
	if (m_StereoDownmixerParams.p2.I.FC  != ::GetDlgItemInt(m_Dlg,IDC_EDIT_I_FC ,NULL,FALSE)) ::SetDlgItemInt(m_Dlg,IDC_EDIT_I_FC ,m_StereoDownmixerParams.p2.I.FC ,FALSE);
	if (m_StereoDownmixerParams.p2.I.LF  != ::GetDlgItemInt(m_Dlg,IDC_EDIT_I_LF ,NULL,FALSE)) ::SetDlgItemInt(m_Dlg,IDC_EDIT_I_LF ,m_StereoDownmixerParams.p2.I.LF ,FALSE);
	if (m_StereoDownmixerParams.p2.I.BL  != ::GetDlgItemInt(m_Dlg,IDC_EDIT_I_BL ,NULL,FALSE)) ::SetDlgItemInt(m_Dlg,IDC_EDIT_I_BL ,m_StereoDownmixerParams.p2.I.BL ,FALSE);
	if (m_StereoDownmixerParams.p2.I.BR  != ::GetDlgItemInt(m_Dlg,IDC_EDIT_I_BR ,NULL,FALSE)) ::SetDlgItemInt(m_Dlg,IDC_EDIT_I_BR ,m_StereoDownmixerParams.p2.I.BR ,FALSE);
	if (m_StereoDownmixerParams.p2.I.FLC != ::GetDlgItemInt(m_Dlg,IDC_EDIT_I_FLC,NULL,FALSE)) ::SetDlgItemInt(m_Dlg,IDC_EDIT_I_FLC,m_StereoDownmixerParams.p2.I.FLC,FALSE);
	if (m_StereoDownmixerParams.p2.I.FRC != ::GetDlgItemInt(m_Dlg,IDC_EDIT_I_FRC,NULL,FALSE)) ::SetDlgItemInt(m_Dlg,IDC_EDIT_I_FRC,m_StereoDownmixerParams.p2.I.FRC,FALSE);
	if (m_StereoDownmixerParams.p2.I.BC  != ::GetDlgItemInt(m_Dlg,IDC_EDIT_I_BC ,NULL,FALSE)) ::SetDlgItemInt(m_Dlg,IDC_EDIT_I_BC ,m_StereoDownmixerParams.p2.I.BC ,FALSE);
	if (m_StereoDownmixerParams.p2.I.SL  != ::GetDlgItemInt(m_Dlg,IDC_EDIT_I_SL ,NULL,FALSE)) ::SetDlgItemInt(m_Dlg,IDC_EDIT_I_SL ,m_StereoDownmixerParams.p2.I.SL ,FALSE);
	if (m_StereoDownmixerParams.p2.I.SR  != ::GetDlgItemInt(m_Dlg,IDC_EDIT_I_SR ,NULL,FALSE)) ::SetDlgItemInt(m_Dlg,IDC_EDIT_I_SR ,m_StereoDownmixerParams.p2.I.SR ,FALSE);
	if (m_StereoDownmixerParams.p2.I.TC  != ::GetDlgItemInt(m_Dlg,IDC_EDIT_I_TC ,NULL,FALSE)) ::SetDlgItemInt(m_Dlg,IDC_EDIT_I_TC ,m_StereoDownmixerParams.p2.I.TC ,FALSE);
	if (m_StereoDownmixerParams.p2.I.TFL != ::GetDlgItemInt(m_Dlg,IDC_EDIT_I_TFL,NULL,FALSE)) ::SetDlgItemInt(m_Dlg,IDC_EDIT_I_TFL,m_StereoDownmixerParams.p2.I.TFL,FALSE);
	if (m_StereoDownmixerParams.p2.I.TFC != ::GetDlgItemInt(m_Dlg,IDC_EDIT_I_TFC,NULL,FALSE)) ::SetDlgItemInt(m_Dlg,IDC_EDIT_I_TFC,m_StereoDownmixerParams.p2.I.TFC,FALSE);
	if (m_StereoDownmixerParams.p2.I.TFR != ::GetDlgItemInt(m_Dlg,IDC_EDIT_I_TFR,NULL,FALSE)) ::SetDlgItemInt(m_Dlg,IDC_EDIT_I_TFR,m_StereoDownmixerParams.p2.I.TFR,FALSE);
	if (m_StereoDownmixerParams.p2.I.TBL != ::GetDlgItemInt(m_Dlg,IDC_EDIT_I_TBL,NULL,FALSE)) ::SetDlgItemInt(m_Dlg,IDC_EDIT_I_TBL,m_StereoDownmixerParams.p2.I.TBL,FALSE);
	if (m_StereoDownmixerParams.p2.I.TBC != ::GetDlgItemInt(m_Dlg,IDC_EDIT_I_TBC,NULL,FALSE)) ::SetDlgItemInt(m_Dlg,IDC_EDIT_I_TBC,m_StereoDownmixerParams.p2.I.TBC,FALSE);
	if (m_StereoDownmixerParams.p2.I.TBR != ::GetDlgItemInt(m_Dlg,IDC_EDIT_I_TBR,NULL,FALSE)) ::SetDlgItemInt(m_Dlg,IDC_EDIT_I_TBR,m_StereoDownmixerParams.p2.I.TBR,FALSE);

	if (m_StereoDownmixerParams.p2.O.FL  != ::GetDlgItemInt(m_Dlg,IDC_EDIT_O_FL ,NULL,FALSE)) ::SetDlgItemInt(m_Dlg,IDC_EDIT_O_FL ,m_StereoDownmixerParams.p2.O.FL ,FALSE);
	if (m_StereoDownmixerParams.p2.O.FR  != ::GetDlgItemInt(m_Dlg,IDC_EDIT_O_FR ,NULL,FALSE)) ::SetDlgItemInt(m_Dlg,IDC_EDIT_O_FR ,m_StereoDownmixerParams.p2.O.FR ,FALSE);
	if (m_StereoDownmixerParams.p2.O.FC  != ::GetDlgItemInt(m_Dlg,IDC_EDIT_O_FC ,NULL,FALSE)) ::SetDlgItemInt(m_Dlg,IDC_EDIT_O_FC ,m_StereoDownmixerParams.p2.O.FC ,FALSE);
	if (m_StereoDownmixerParams.p2.O.LF  != ::GetDlgItemInt(m_Dlg,IDC_EDIT_O_LF ,NULL,FALSE)) ::SetDlgItemInt(m_Dlg,IDC_EDIT_O_LF ,m_StereoDownmixerParams.p2.O.LF ,FALSE);
	if (m_StereoDownmixerParams.p2.O.BL  != ::GetDlgItemInt(m_Dlg,IDC_EDIT_O_BL ,NULL,FALSE)) ::SetDlgItemInt(m_Dlg,IDC_EDIT_O_BL ,m_StereoDownmixerParams.p2.O.BL ,FALSE);
	if (m_StereoDownmixerParams.p2.O.BR  != ::GetDlgItemInt(m_Dlg,IDC_EDIT_O_BR ,NULL,FALSE)) ::SetDlgItemInt(m_Dlg,IDC_EDIT_O_BR ,m_StereoDownmixerParams.p2.O.BR ,FALSE);
	if (m_StereoDownmixerParams.p2.O.FLC != ::GetDlgItemInt(m_Dlg,IDC_EDIT_O_FLC,NULL,FALSE)) ::SetDlgItemInt(m_Dlg,IDC_EDIT_O_FLC,m_StereoDownmixerParams.p2.O.FLC,FALSE);
	if (m_StereoDownmixerParams.p2.O.FRC != ::GetDlgItemInt(m_Dlg,IDC_EDIT_O_FRC,NULL,FALSE)) ::SetDlgItemInt(m_Dlg,IDC_EDIT_O_FRC,m_StereoDownmixerParams.p2.O.FRC,FALSE);
	if (m_StereoDownmixerParams.p2.O.BC  != ::GetDlgItemInt(m_Dlg,IDC_EDIT_O_BC ,NULL,FALSE)) ::SetDlgItemInt(m_Dlg,IDC_EDIT_O_BC ,m_StereoDownmixerParams.p2.O.BC ,FALSE);
	if (m_StereoDownmixerParams.p2.O.SL  != ::GetDlgItemInt(m_Dlg,IDC_EDIT_O_SL ,NULL,FALSE)) ::SetDlgItemInt(m_Dlg,IDC_EDIT_O_SL ,m_StereoDownmixerParams.p2.O.SL ,FALSE);
	if (m_StereoDownmixerParams.p2.O.SR  != ::GetDlgItemInt(m_Dlg,IDC_EDIT_O_SR ,NULL,FALSE)) ::SetDlgItemInt(m_Dlg,IDC_EDIT_O_SR ,m_StereoDownmixerParams.p2.O.SR ,FALSE);
	if (m_StereoDownmixerParams.p2.O.TC  != ::GetDlgItemInt(m_Dlg,IDC_EDIT_O_TC ,NULL,FALSE)) ::SetDlgItemInt(m_Dlg,IDC_EDIT_O_TC ,m_StereoDownmixerParams.p2.O.TC ,FALSE);
	if (m_StereoDownmixerParams.p2.O.TFL != ::GetDlgItemInt(m_Dlg,IDC_EDIT_O_TFL,NULL,FALSE)) ::SetDlgItemInt(m_Dlg,IDC_EDIT_O_TFL,m_StereoDownmixerParams.p2.O.TFL,FALSE);
	if (m_StereoDownmixerParams.p2.O.TFC != ::GetDlgItemInt(m_Dlg,IDC_EDIT_O_TFC,NULL,FALSE)) ::SetDlgItemInt(m_Dlg,IDC_EDIT_O_TFC,m_StereoDownmixerParams.p2.O.TFC,FALSE);
	if (m_StereoDownmixerParams.p2.O.TFR != ::GetDlgItemInt(m_Dlg,IDC_EDIT_O_TFR,NULL,FALSE)) ::SetDlgItemInt(m_Dlg,IDC_EDIT_O_TFR,m_StereoDownmixerParams.p2.O.TFR,FALSE);
	if (m_StereoDownmixerParams.p2.O.TBL != ::GetDlgItemInt(m_Dlg,IDC_EDIT_O_TBL,NULL,FALSE)) ::SetDlgItemInt(m_Dlg,IDC_EDIT_O_TBL,m_StereoDownmixerParams.p2.O.TBL,FALSE);
	if (m_StereoDownmixerParams.p2.O.TBC != ::GetDlgItemInt(m_Dlg,IDC_EDIT_O_TBC,NULL,FALSE)) ::SetDlgItemInt(m_Dlg,IDC_EDIT_O_TBC,m_StereoDownmixerParams.p2.O.TBC,FALSE);
	if (m_StereoDownmixerParams.p2.O.TBR != ::GetDlgItemInt(m_Dlg,IDC_EDIT_O_TBR,NULL,FALSE)) ::SetDlgItemInt(m_Dlg,IDC_EDIT_O_TBR,m_StereoDownmixerParams.p2.O.TBR,FALSE);

	//Sliders
	if (m_StereoDownmixerParams.p2.I.FL  != (unsigned)::SendMessage(GetDlgItem(m_Dlg,IDC_SLIDER_I_FL ),TBM_GETPOS,0,0)) ::SendMessage(GetDlgItem(m_Dlg,IDC_SLIDER_I_FL ),TBM_SETPOS,TRUE,m_StereoDownmixerParams.p2.I.FL );
	if (m_StereoDownmixerParams.p2.I.FR  != (unsigned)::SendMessage(GetDlgItem(m_Dlg,IDC_SLIDER_I_FR ),TBM_GETPOS,0,0)) ::SendMessage(GetDlgItem(m_Dlg,IDC_SLIDER_I_FR ),TBM_SETPOS,TRUE,m_StereoDownmixerParams.p2.I.FR );
	if (m_StereoDownmixerParams.p2.I.FC  != (unsigned)::SendMessage(GetDlgItem(m_Dlg,IDC_SLIDER_I_FC ),TBM_GETPOS,0,0)) ::SendMessage(GetDlgItem(m_Dlg,IDC_SLIDER_I_FC ),TBM_SETPOS,TRUE,m_StereoDownmixerParams.p2.I.FC );
	if (m_StereoDownmixerParams.p2.I.LF  != (unsigned)::SendMessage(GetDlgItem(m_Dlg,IDC_SLIDER_I_LF ),TBM_GETPOS,0,0)) ::SendMessage(GetDlgItem(m_Dlg,IDC_SLIDER_I_LF ),TBM_SETPOS,TRUE,m_StereoDownmixerParams.p2.I.LF );
	if (m_StereoDownmixerParams.p2.I.BL  != (unsigned)::SendMessage(GetDlgItem(m_Dlg,IDC_SLIDER_I_BL ),TBM_GETPOS,0,0)) ::SendMessage(GetDlgItem(m_Dlg,IDC_SLIDER_I_BL ),TBM_SETPOS,TRUE,m_StereoDownmixerParams.p2.I.BL );
	if (m_StereoDownmixerParams.p2.I.BR  != (unsigned)::SendMessage(GetDlgItem(m_Dlg,IDC_SLIDER_I_BR ),TBM_GETPOS,0,0)) ::SendMessage(GetDlgItem(m_Dlg,IDC_SLIDER_I_BR ),TBM_SETPOS,TRUE,m_StereoDownmixerParams.p2.I.BR );
	if (m_StereoDownmixerParams.p2.I.FLC != (unsigned)::SendMessage(GetDlgItem(m_Dlg,IDC_SLIDER_I_FLC),TBM_GETPOS,0,0)) ::SendMessage(GetDlgItem(m_Dlg,IDC_SLIDER_I_FLC),TBM_SETPOS,TRUE,m_StereoDownmixerParams.p2.I.FLC);
	if (m_StereoDownmixerParams.p2.I.FRC != (unsigned)::SendMessage(GetDlgItem(m_Dlg,IDC_SLIDER_I_FRC),TBM_GETPOS,0,0)) ::SendMessage(GetDlgItem(m_Dlg,IDC_SLIDER_I_FRC),TBM_SETPOS,TRUE,m_StereoDownmixerParams.p2.I.FRC);
	if (m_StereoDownmixerParams.p2.I.BC  != (unsigned)::SendMessage(GetDlgItem(m_Dlg,IDC_SLIDER_I_BC ),TBM_GETPOS,0,0)) ::SendMessage(GetDlgItem(m_Dlg,IDC_SLIDER_I_BC ),TBM_SETPOS,TRUE,m_StereoDownmixerParams.p2.I.BC );
	if (m_StereoDownmixerParams.p2.I.SL  != (unsigned)::SendMessage(GetDlgItem(m_Dlg,IDC_SLIDER_I_SL ),TBM_GETPOS,0,0)) ::SendMessage(GetDlgItem(m_Dlg,IDC_SLIDER_I_SL ),TBM_SETPOS,TRUE,m_StereoDownmixerParams.p2.I.SL );
	if (m_StereoDownmixerParams.p2.I.SR  != (unsigned)::SendMessage(GetDlgItem(m_Dlg,IDC_SLIDER_I_SR ),TBM_GETPOS,0,0)) ::SendMessage(GetDlgItem(m_Dlg,IDC_SLIDER_I_SR ),TBM_SETPOS,TRUE,m_StereoDownmixerParams.p2.I.SR );
	if (m_StereoDownmixerParams.p2.I.TC  != (unsigned)::SendMessage(GetDlgItem(m_Dlg,IDC_SLIDER_I_TC ),TBM_GETPOS,0,0)) ::SendMessage(GetDlgItem(m_Dlg,IDC_SLIDER_I_TC ),TBM_SETPOS,TRUE,m_StereoDownmixerParams.p2.I.TC );
	if (m_StereoDownmixerParams.p2.I.TFL != (unsigned)::SendMessage(GetDlgItem(m_Dlg,IDC_SLIDER_I_TFL),TBM_GETPOS,0,0)) ::SendMessage(GetDlgItem(m_Dlg,IDC_SLIDER_I_TFL),TBM_SETPOS,TRUE,m_StereoDownmixerParams.p2.I.TFL);
	if (m_StereoDownmixerParams.p2.I.TFC != (unsigned)::SendMessage(GetDlgItem(m_Dlg,IDC_SLIDER_I_TFC),TBM_GETPOS,0,0)) ::SendMessage(GetDlgItem(m_Dlg,IDC_SLIDER_I_TFC),TBM_SETPOS,TRUE,m_StereoDownmixerParams.p2.I.TFC);
	if (m_StereoDownmixerParams.p2.I.TFR != (unsigned)::SendMessage(GetDlgItem(m_Dlg,IDC_SLIDER_I_TFR),TBM_GETPOS,0,0)) ::SendMessage(GetDlgItem(m_Dlg,IDC_SLIDER_I_TFR),TBM_SETPOS,TRUE,m_StereoDownmixerParams.p2.I.TFR);
	if (m_StereoDownmixerParams.p2.I.TBL != (unsigned)::SendMessage(GetDlgItem(m_Dlg,IDC_SLIDER_I_TBL),TBM_GETPOS,0,0)) ::SendMessage(GetDlgItem(m_Dlg,IDC_SLIDER_I_TBL),TBM_SETPOS,TRUE,m_StereoDownmixerParams.p2.I.TBL);
	if (m_StereoDownmixerParams.p2.I.TBC != (unsigned)::SendMessage(GetDlgItem(m_Dlg,IDC_SLIDER_I_TBC),TBM_GETPOS,0,0)) ::SendMessage(GetDlgItem(m_Dlg,IDC_SLIDER_I_TBC),TBM_SETPOS,TRUE,m_StereoDownmixerParams.p2.I.TBC);
	if (m_StereoDownmixerParams.p2.I.TBR != (unsigned)::SendMessage(GetDlgItem(m_Dlg,IDC_SLIDER_I_TBR),TBM_GETPOS,0,0)) ::SendMessage(GetDlgItem(m_Dlg,IDC_SLIDER_I_TBR),TBM_SETPOS,TRUE,m_StereoDownmixerParams.p2.I.TBR);

	if (m_StereoDownmixerParams.p2.O.FL  != (unsigned)::SendMessage(GetDlgItem(m_Dlg,IDC_SLIDER_O_FL ),TBM_GETPOS,0,0)) ::SendMessage(GetDlgItem(m_Dlg,IDC_SLIDER_O_FL ),TBM_SETPOS,TRUE,m_StereoDownmixerParams.p2.O.FL );
	if (m_StereoDownmixerParams.p2.O.FR  != (unsigned)::SendMessage(GetDlgItem(m_Dlg,IDC_SLIDER_O_FR ),TBM_GETPOS,0,0)) ::SendMessage(GetDlgItem(m_Dlg,IDC_SLIDER_O_FR ),TBM_SETPOS,TRUE,m_StereoDownmixerParams.p2.O.FR );
	if (m_StereoDownmixerParams.p2.O.FC  != (unsigned)::SendMessage(GetDlgItem(m_Dlg,IDC_SLIDER_O_FC ),TBM_GETPOS,0,0)) ::SendMessage(GetDlgItem(m_Dlg,IDC_SLIDER_O_FC ),TBM_SETPOS,TRUE,m_StereoDownmixerParams.p2.O.FC );
	if (m_StereoDownmixerParams.p2.O.LF  != (unsigned)::SendMessage(GetDlgItem(m_Dlg,IDC_SLIDER_O_LF ),TBM_GETPOS,0,0)) ::SendMessage(GetDlgItem(m_Dlg,IDC_SLIDER_O_LF ),TBM_SETPOS,TRUE,m_StereoDownmixerParams.p2.O.LF );
	if (m_StereoDownmixerParams.p2.O.BL  != (unsigned)::SendMessage(GetDlgItem(m_Dlg,IDC_SLIDER_O_BL ),TBM_GETPOS,0,0)) ::SendMessage(GetDlgItem(m_Dlg,IDC_SLIDER_O_BL ),TBM_SETPOS,TRUE,m_StereoDownmixerParams.p2.O.BL );
	if (m_StereoDownmixerParams.p2.O.BR  != (unsigned)::SendMessage(GetDlgItem(m_Dlg,IDC_SLIDER_O_BR ),TBM_GETPOS,0,0)) ::SendMessage(GetDlgItem(m_Dlg,IDC_SLIDER_O_BR ),TBM_SETPOS,TRUE,m_StereoDownmixerParams.p2.O.BR );
	if (m_StereoDownmixerParams.p2.O.FLC != (unsigned)::SendMessage(GetDlgItem(m_Dlg,IDC_SLIDER_O_FLC),TBM_GETPOS,0,0)) ::SendMessage(GetDlgItem(m_Dlg,IDC_SLIDER_O_FLC),TBM_SETPOS,TRUE,m_StereoDownmixerParams.p2.O.FLC);
	if (m_StereoDownmixerParams.p2.O.FRC != (unsigned)::SendMessage(GetDlgItem(m_Dlg,IDC_SLIDER_O_FRC),TBM_GETPOS,0,0)) ::SendMessage(GetDlgItem(m_Dlg,IDC_SLIDER_O_FRC),TBM_SETPOS,TRUE,m_StereoDownmixerParams.p2.O.FRC);
	if (m_StereoDownmixerParams.p2.O.BC  != (unsigned)::SendMessage(GetDlgItem(m_Dlg,IDC_SLIDER_O_BC ),TBM_GETPOS,0,0)) ::SendMessage(GetDlgItem(m_Dlg,IDC_SLIDER_O_BC ),TBM_SETPOS,TRUE,m_StereoDownmixerParams.p2.O.BC );
	if (m_StereoDownmixerParams.p2.O.SL  != (unsigned)::SendMessage(GetDlgItem(m_Dlg,IDC_SLIDER_O_SL ),TBM_GETPOS,0,0)) ::SendMessage(GetDlgItem(m_Dlg,IDC_SLIDER_O_SL ),TBM_SETPOS,TRUE,m_StereoDownmixerParams.p2.O.SL );
	if (m_StereoDownmixerParams.p2.O.SR  != (unsigned)::SendMessage(GetDlgItem(m_Dlg,IDC_SLIDER_O_SR ),TBM_GETPOS,0,0)) ::SendMessage(GetDlgItem(m_Dlg,IDC_SLIDER_O_SR ),TBM_SETPOS,TRUE,m_StereoDownmixerParams.p2.O.SR );
	if (m_StereoDownmixerParams.p2.O.TC  != (unsigned)::SendMessage(GetDlgItem(m_Dlg,IDC_SLIDER_O_TC ),TBM_GETPOS,0,0)) ::SendMessage(GetDlgItem(m_Dlg,IDC_SLIDER_O_TC ),TBM_SETPOS,TRUE,m_StereoDownmixerParams.p2.O.TC );
	if (m_StereoDownmixerParams.p2.O.TFL != (unsigned)::SendMessage(GetDlgItem(m_Dlg,IDC_SLIDER_O_TFL),TBM_GETPOS,0,0)) ::SendMessage(GetDlgItem(m_Dlg,IDC_SLIDER_O_TFL),TBM_SETPOS,TRUE,m_StereoDownmixerParams.p2.O.TFL);
	if (m_StereoDownmixerParams.p2.O.TFC != (unsigned)::SendMessage(GetDlgItem(m_Dlg,IDC_SLIDER_O_TFC),TBM_GETPOS,0,0)) ::SendMessage(GetDlgItem(m_Dlg,IDC_SLIDER_O_TFC),TBM_SETPOS,TRUE,m_StereoDownmixerParams.p2.O.TFC);
	if (m_StereoDownmixerParams.p2.O.TFR != (unsigned)::SendMessage(GetDlgItem(m_Dlg,IDC_SLIDER_O_TFR),TBM_GETPOS,0,0)) ::SendMessage(GetDlgItem(m_Dlg,IDC_SLIDER_O_TFR),TBM_SETPOS,TRUE,m_StereoDownmixerParams.p2.O.TFR);
	if (m_StereoDownmixerParams.p2.O.TBL != (unsigned)::SendMessage(GetDlgItem(m_Dlg,IDC_SLIDER_O_TBL),TBM_GETPOS,0,0)) ::SendMessage(GetDlgItem(m_Dlg,IDC_SLIDER_O_TBL),TBM_SETPOS,TRUE,m_StereoDownmixerParams.p2.O.TBL);
	if (m_StereoDownmixerParams.p2.O.TBC != (unsigned)::SendMessage(GetDlgItem(m_Dlg,IDC_SLIDER_O_TBC),TBM_GETPOS,0,0)) ::SendMessage(GetDlgItem(m_Dlg,IDC_SLIDER_O_TBC),TBM_SETPOS,TRUE,m_StereoDownmixerParams.p2.O.TBC);
	if (m_StereoDownmixerParams.p2.O.TBR != (unsigned)::SendMessage(GetDlgItem(m_Dlg,IDC_SLIDER_O_TBR),TBM_GETPOS,0,0)) ::SendMessage(GetDlgItem(m_Dlg,IDC_SLIDER_O_TBR),TBM_SETPOS,TRUE,m_StereoDownmixerParams.p2.O.TBR);

	//Master volume
	if (m_StereoDownmixerParams.p2.O.TBC != ::GetDlgItemInt(m_Dlg,IDC_EDIT_MASTER_VOLUME,NULL,FALSE)) ::SetDlgItemInt(m_Dlg,IDC_EDIT_MASTER_VOLUME,m_StereoDownmixerParams.p2.O.TBC,FALSE);
	if (m_StereoDownmixerParams.p2.O.TBC != (unsigned)::SendMessage(GetDlgItem(m_Dlg,IDC_SLIDER_MASTER_VOLUME),TBM_GETPOS,0,0)) ::SendMessage(GetDlgItem(m_Dlg,IDC_SLIDER_MASTER_VOLUME),TBM_SETPOS,TRUE,m_StereoDownmixerParams.p2.O.TBC);

	UpdateView(); //So we enable / disable in the right way all the controls which depend on the radio buttons we have just set
}

void CStereoDownmixerPropP2::SaveDataToFilter()
{
	ASSERT(m_pIStereoDownmixer);

	if (m_bIsBeingInitialized) return;
	
	m_pIStereoDownmixer->SetParams(&m_StereoDownmixerParams, StereoDownmixerParams::SectionChoose::gains);
}

void CStereoDownmixerPropP2::OnSomeVolumeScrollbarMoved(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	int ScrollBarID=GetDlgCtrlID((HWND)lParam);
	int EditBoxID=NULL;
	int CompanionScrollBar=NULL;
	int CompanionEditBox=NULL;
	int JoinCheckBox=NULL;

	//Input ones
	if		(ScrollBarID==IDC_SLIDER_I_FL )	{EditBoxID=IDC_EDIT_I_FL ;	CompanionScrollBar=IDC_SLIDER_I_FR;		CompanionEditBox=IDC_EDIT_I_FR ;JoinCheckBox=IDC_CHECK_JOIN_I_FLFR  ;}
	else if (ScrollBarID==IDC_SLIDER_I_FR )	{EditBoxID=IDC_EDIT_I_FR ;	CompanionScrollBar=IDC_SLIDER_I_FL;		CompanionEditBox=IDC_EDIT_I_FL ;JoinCheckBox=IDC_CHECK_JOIN_I_FLFR  ;}
	else if (ScrollBarID==IDC_SLIDER_I_FC )	{EditBoxID=IDC_EDIT_I_FC ;	CompanionScrollBar=NULL;				CompanionEditBox=NULL          ;JoinCheckBox=NULL                   ;}
	else if (ScrollBarID==IDC_SLIDER_I_LF )	{EditBoxID=IDC_EDIT_I_LF ;	CompanionScrollBar=NULL;				CompanionEditBox=NULL          ;JoinCheckBox=NULL                   ;}
	else if (ScrollBarID==IDC_SLIDER_I_BL )	{EditBoxID=IDC_EDIT_I_BL ;	CompanionScrollBar=IDC_SLIDER_I_BR;		CompanionEditBox=IDC_EDIT_I_BR ;JoinCheckBox=IDC_CHECK_JOIN_I_BLBR  ;}
	else if (ScrollBarID==IDC_SLIDER_I_BR )	{EditBoxID=IDC_EDIT_I_BR ;	CompanionScrollBar=IDC_SLIDER_I_BL;		CompanionEditBox=IDC_EDIT_I_BL ;JoinCheckBox=IDC_CHECK_JOIN_I_BLBR  ;}
	else if (ScrollBarID==IDC_SLIDER_I_FLC)	{EditBoxID=IDC_EDIT_I_FLC;	CompanionScrollBar=IDC_SLIDER_I_FRC;	CompanionEditBox=IDC_EDIT_I_FRC;JoinCheckBox=IDC_CHECK_JOIN_I_FLCFRC;}
	else if (ScrollBarID==IDC_SLIDER_I_FRC)	{EditBoxID=IDC_EDIT_I_FRC;	CompanionScrollBar=IDC_SLIDER_I_FLC;	CompanionEditBox=IDC_EDIT_I_FLC;JoinCheckBox=IDC_CHECK_JOIN_I_FLCFRC;}
	else if (ScrollBarID==IDC_SLIDER_I_BC )	{EditBoxID=IDC_EDIT_I_BC ;	CompanionScrollBar=NULL;				CompanionEditBox=NULL          ;JoinCheckBox=NULL                   ;}
	else if (ScrollBarID==IDC_SLIDER_I_SL )	{EditBoxID=IDC_EDIT_I_SL ;	CompanionScrollBar=IDC_SLIDER_I_SR;		CompanionEditBox=IDC_EDIT_I_SR ;JoinCheckBox=IDC_CHECK_JOIN_I_SLSR  ;}
	else if (ScrollBarID==IDC_SLIDER_I_SR )	{EditBoxID=IDC_EDIT_I_SR ;	CompanionScrollBar=IDC_SLIDER_I_SL;		CompanionEditBox=IDC_EDIT_I_SL ;JoinCheckBox=IDC_CHECK_JOIN_I_SLSR  ;}
	else if (ScrollBarID==IDC_SLIDER_I_TC )	{EditBoxID=IDC_EDIT_I_TC ;	CompanionScrollBar=NULL;				CompanionEditBox=NULL          ;JoinCheckBox=NULL                   ;}
	else if (ScrollBarID==IDC_SLIDER_I_TFL)	{EditBoxID=IDC_EDIT_I_TFL;	CompanionScrollBar=IDC_SLIDER_I_TFR;	CompanionEditBox=IDC_EDIT_I_TFR;JoinCheckBox=IDC_CHECK_JOIN_I_TFLTFR;}
	else if (ScrollBarID==IDC_SLIDER_I_TFC)	{EditBoxID=IDC_EDIT_I_TFC;	CompanionScrollBar=NULL;				CompanionEditBox=NULL          ;JoinCheckBox=NULL                   ;}
	else if (ScrollBarID==IDC_SLIDER_I_TFR)	{EditBoxID=IDC_EDIT_I_TFR;	CompanionScrollBar=IDC_SLIDER_I_TFL;	CompanionEditBox=IDC_EDIT_I_TFL;JoinCheckBox=IDC_CHECK_JOIN_I_TFLTFR;}
	else if (ScrollBarID==IDC_SLIDER_I_TBL)	{EditBoxID=IDC_EDIT_I_TBL;	CompanionScrollBar=IDC_SLIDER_I_TBR;	CompanionEditBox=IDC_EDIT_I_TBR;JoinCheckBox=IDC_CHECK_JOIN_I_TBLTBR;}
	else if (ScrollBarID==IDC_SLIDER_I_TBC)	{EditBoxID=IDC_EDIT_I_TBC;	CompanionScrollBar=NULL;				CompanionEditBox=NULL          ;JoinCheckBox=NULL                   ;}
	else if (ScrollBarID==IDC_SLIDER_I_TBR)	{EditBoxID=IDC_EDIT_I_TBR;	CompanionScrollBar=IDC_SLIDER_I_TBL;	CompanionEditBox=IDC_EDIT_I_TBL;JoinCheckBox=IDC_CHECK_JOIN_I_TBLTBR;}

	//Output ones
	else if	(ScrollBarID==IDC_SLIDER_O_FL )	{EditBoxID=IDC_EDIT_O_FL ;	CompanionScrollBar=IDC_SLIDER_O_FR;		CompanionEditBox=IDC_EDIT_O_FR;	JoinCheckBox=IDC_CHECK_JOIN_O_FLFR  ;}
	else if (ScrollBarID==IDC_SLIDER_O_FR )	{EditBoxID=IDC_EDIT_O_FR ;	CompanionScrollBar=IDC_SLIDER_O_FL;		CompanionEditBox=IDC_EDIT_O_FL;	JoinCheckBox=IDC_CHECK_JOIN_O_FLFR  ;}
	else if (ScrollBarID==IDC_SLIDER_O_FC )	{EditBoxID=IDC_EDIT_O_FC ;	CompanionScrollBar=NULL;				CompanionEditBox=NULL;			JoinCheckBox=NULL                   ;}
	else if (ScrollBarID==IDC_SLIDER_O_LF )	{EditBoxID=IDC_EDIT_O_LF ;	CompanionScrollBar=NULL;				CompanionEditBox=NULL;			JoinCheckBox=NULL                   ;}
	else if (ScrollBarID==IDC_SLIDER_O_BL )	{EditBoxID=IDC_EDIT_O_BL ;	CompanionScrollBar=IDC_SLIDER_O_BR;		CompanionEditBox=IDC_EDIT_O_BR;	JoinCheckBox=IDC_CHECK_JOIN_O_BLBR  ;}
	else if (ScrollBarID==IDC_SLIDER_O_BR )	{EditBoxID=IDC_EDIT_O_BR ;	CompanionScrollBar=IDC_SLIDER_O_BL;		CompanionEditBox=IDC_EDIT_O_BL;	JoinCheckBox=IDC_CHECK_JOIN_O_BLBR  ;}
	else if (ScrollBarID==IDC_SLIDER_O_FLC)	{EditBoxID=IDC_EDIT_O_FLC;	CompanionScrollBar=IDC_SLIDER_O_FRC;	CompanionEditBox=IDC_EDIT_O_FRC;JoinCheckBox=IDC_CHECK_JOIN_O_FLCFRC;}
	else if (ScrollBarID==IDC_SLIDER_O_FRC)	{EditBoxID=IDC_EDIT_O_FRC;	CompanionScrollBar=IDC_SLIDER_O_FLC;	CompanionEditBox=IDC_EDIT_O_FLC;JoinCheckBox=IDC_CHECK_JOIN_O_FLCFRC;}
	else if (ScrollBarID==IDC_SLIDER_O_BC )	{EditBoxID=IDC_EDIT_O_BC ;	CompanionScrollBar=NULL;				CompanionEditBox=NULL;			JoinCheckBox=NULL                   ;}
	else if (ScrollBarID==IDC_SLIDER_O_SL )	{EditBoxID=IDC_EDIT_O_SL ;	CompanionScrollBar=IDC_SLIDER_O_SR;		CompanionEditBox=IDC_EDIT_O_SR;	JoinCheckBox=IDC_CHECK_JOIN_O_SLSR; ;}
	else if (ScrollBarID==IDC_SLIDER_O_SR )	{EditBoxID=IDC_EDIT_O_SR ;	CompanionScrollBar=IDC_SLIDER_O_SL;		CompanionEditBox=IDC_EDIT_O_SL;	JoinCheckBox=IDC_CHECK_JOIN_O_SLSR; ;}
	else if (ScrollBarID==IDC_SLIDER_O_TC )	{EditBoxID=IDC_EDIT_O_TC ;	CompanionScrollBar=NULL;				CompanionEditBox=NULL;			JoinCheckBox=NULL                   ;}
	else if (ScrollBarID==IDC_SLIDER_O_TFL)	{EditBoxID=IDC_EDIT_O_TFL;	CompanionScrollBar=IDC_SLIDER_O_TFR;	CompanionEditBox=IDC_EDIT_O_TFR;JoinCheckBox=IDC_CHECK_JOIN_O_TFLTFR;}
	else if (ScrollBarID==IDC_SLIDER_O_TFC)	{EditBoxID=IDC_EDIT_O_TFC;	CompanionScrollBar=NULL;				CompanionEditBox=NULL;			JoinCheckBox=NULL                   ;}
	else if (ScrollBarID==IDC_SLIDER_O_TFR)	{EditBoxID=IDC_EDIT_O_TFR;	CompanionScrollBar=IDC_SLIDER_O_TFL;	CompanionEditBox=IDC_EDIT_O_TFL;JoinCheckBox=IDC_CHECK_JOIN_O_TFLTFR;}
	else if (ScrollBarID==IDC_SLIDER_O_TBL)	{EditBoxID=IDC_EDIT_O_TBL;	CompanionScrollBar=IDC_SLIDER_O_TBR;	CompanionEditBox=IDC_EDIT_O_TBR;JoinCheckBox=IDC_CHECK_JOIN_O_TBLTBR;}
	else if (ScrollBarID==IDC_SLIDER_O_TBC)	{EditBoxID=IDC_EDIT_O_TBC;	CompanionScrollBar=NULL;				CompanionEditBox=NULL;			JoinCheckBox=NULL                   ;}
	else if (ScrollBarID==IDC_SLIDER_O_TBR)	{EditBoxID=IDC_EDIT_O_TBR;	CompanionScrollBar=IDC_SLIDER_O_TBL;	CompanionEditBox=IDC_EDIT_O_TBL;JoinCheckBox=IDC_CHECK_JOIN_O_TBLTBR;}
	
	//Master volume
	else if (ScrollBarID==IDC_SLIDER_MASTER_VOLUME) {EditBoxID=IDC_EDIT_MASTER_VOLUME; CompanionScrollBar=NULL; CompanionEditBox=NULL; JoinCheckBox=NULL; }

	//Fetch the new value
	DWORD newval=(unsigned)::SendMessage(GetDlgItem(m_Dlg,ScrollBarID),TBM_GETPOS,0,0);
	if (newval > 400) newval=400; if (newval < 0) newval=0;
	
	//Update to both (only if different from current, to avoid retriggering) so we trim too high / too low values
	if (newval!=(unsigned)::SendMessage(GetDlgItem(m_Dlg,ScrollBarID),TBM_GETPOS,0,0))	::SendMessage(GetDlgItem(m_Dlg,ScrollBarID),TBM_SETPOS,TRUE,newval);
	if (newval!=::GetDlgItemInt(m_Dlg,EditBoxID,NULL,FALSE))					::SetDlgItemInt(m_Dlg,EditBoxID,newval,FALSE);

	//Check if we have a joint control
	if (JoinCheckBox!=NULL && BST_CHECKED==::IsDlgButtonChecked(m_Dlg,JoinCheckBox)) 
	{
		if (newval!=(unsigned)::SendMessage(GetDlgItem(m_Dlg,CompanionScrollBar),TBM_GETPOS,0,0))	::SendMessage(GetDlgItem(m_Dlg,CompanionScrollBar),TBM_SETPOS,TRUE,newval);
		if (newval!=::GetDlgItemInt(m_Dlg,CompanionEditBox,NULL,FALSE))				::SetDlgItemInt(m_Dlg,CompanionEditBox,newval,FALSE);
	}
}


void CStereoDownmixerPropP2::OnSomeVolumeEditBoxChanged(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{

	int EditBoxID=LOWORD(wParam);
	int ScrollBarID=NULL;	
	int CompanionScrollBar=NULL;
	int CompanionEditBox=NULL;
	int JoinCheckBox=NULL;

	//Input ones
	if		(EditBoxID==IDC_EDIT_I_FL )	{ScrollBarID=IDC_SLIDER_I_FL ;	CompanionScrollBar=IDC_SLIDER_I_FR;		CompanionEditBox=IDC_EDIT_I_FR;	JoinCheckBox=IDC_CHECK_JOIN_I_FLFR  ;}
	else if (EditBoxID==IDC_EDIT_I_FR )	{ScrollBarID=IDC_SLIDER_I_FR ;	CompanionScrollBar=IDC_SLIDER_I_FL;		CompanionEditBox=IDC_EDIT_I_FL;	JoinCheckBox=IDC_CHECK_JOIN_I_FLFR  ;}
	else if (EditBoxID==IDC_EDIT_I_FC )	{ScrollBarID=IDC_SLIDER_I_FC ;	CompanionScrollBar=NULL;				CompanionEditBox=NULL;			JoinCheckBox=NULL                   ;}
	else if (EditBoxID==IDC_EDIT_I_LF )	{ScrollBarID=IDC_SLIDER_I_LF ;	CompanionScrollBar=NULL;				CompanionEditBox=NULL;			JoinCheckBox=NULL                   ;}
	else if (EditBoxID==IDC_EDIT_I_BL )	{ScrollBarID=IDC_SLIDER_I_BL ;	CompanionScrollBar=IDC_SLIDER_I_BR;		CompanionEditBox=IDC_EDIT_I_BR;	JoinCheckBox=IDC_CHECK_JOIN_I_BLBR  ;}
	else if (EditBoxID==IDC_EDIT_I_BR )	{ScrollBarID=IDC_SLIDER_I_BR ;	CompanionScrollBar=IDC_SLIDER_I_BL;		CompanionEditBox=IDC_EDIT_I_BL;	JoinCheckBox=IDC_CHECK_JOIN_I_BLBR  ;}
	else if (EditBoxID==IDC_EDIT_I_FLC)	{ScrollBarID=IDC_SLIDER_I_FLC;	CompanionScrollBar=IDC_SLIDER_I_FRC;	CompanionEditBox=IDC_EDIT_I_FRC;JoinCheckBox=IDC_CHECK_JOIN_I_FLCFRC;}
	else if (EditBoxID==IDC_EDIT_I_FRC)	{ScrollBarID=IDC_SLIDER_I_FRC;	CompanionScrollBar=IDC_SLIDER_I_FLC;	CompanionEditBox=IDC_EDIT_I_FLC;JoinCheckBox=IDC_CHECK_JOIN_I_FLCFRC;}
	else if (EditBoxID==IDC_EDIT_I_BC )	{ScrollBarID=IDC_SLIDER_I_BC ;	CompanionScrollBar=NULL;				CompanionEditBox=NULL;			JoinCheckBox=NULL                   ;}
	else if (EditBoxID==IDC_EDIT_I_SL )	{ScrollBarID=IDC_SLIDER_I_SL ;	CompanionScrollBar=IDC_SLIDER_I_SR;		CompanionEditBox=IDC_EDIT_I_SR;	JoinCheckBox=IDC_CHECK_JOIN_I_SLSR  ;}
	else if (EditBoxID==IDC_EDIT_I_SR )	{ScrollBarID=IDC_SLIDER_I_SR ;	CompanionScrollBar=IDC_SLIDER_I_SL;		CompanionEditBox=IDC_EDIT_I_SL;	JoinCheckBox=IDC_CHECK_JOIN_I_SLSR  ;}
	else if (EditBoxID==IDC_EDIT_I_TC )	{ScrollBarID=IDC_SLIDER_I_TC ;	CompanionScrollBar=NULL;				CompanionEditBox=NULL;			JoinCheckBox=NULL                   ;}
	else if (EditBoxID==IDC_EDIT_I_TFL)	{ScrollBarID=IDC_SLIDER_I_TFL;	CompanionScrollBar=IDC_SLIDER_I_TFR;	CompanionEditBox=IDC_EDIT_I_TFR;JoinCheckBox=IDC_CHECK_JOIN_I_TFLTFR;}
	else if (EditBoxID==IDC_EDIT_I_TFC)	{ScrollBarID=IDC_SLIDER_I_TFC;	CompanionScrollBar=NULL;				CompanionEditBox=NULL;			JoinCheckBox=NULL                   ;}
	else if (EditBoxID==IDC_EDIT_I_TFR)	{ScrollBarID=IDC_SLIDER_I_TFR;	CompanionScrollBar=IDC_SLIDER_I_TFL;	CompanionEditBox=IDC_EDIT_I_TFL;JoinCheckBox=IDC_CHECK_JOIN_I_TFLTFR;}
	else if (EditBoxID==IDC_EDIT_I_TBL)	{ScrollBarID=IDC_SLIDER_I_TBL;	CompanionScrollBar=IDC_SLIDER_I_TBR;	CompanionEditBox=IDC_EDIT_I_TBR;JoinCheckBox=IDC_CHECK_JOIN_I_TBLTBR;}
	else if (EditBoxID==IDC_EDIT_I_TBC)	{ScrollBarID=IDC_SLIDER_I_TBC;	CompanionScrollBar=NULL;				CompanionEditBox=NULL;			JoinCheckBox=NULL                   ;}
	else if (EditBoxID==IDC_EDIT_I_TBR)	{ScrollBarID=IDC_SLIDER_I_TBR;	CompanionScrollBar=IDC_SLIDER_I_TBL;	CompanionEditBox=IDC_EDIT_I_TBL;JoinCheckBox=IDC_CHECK_JOIN_I_TBLTBR;}

	//Output ones
	else if	(EditBoxID==IDC_EDIT_O_FL )	{ScrollBarID=IDC_SLIDER_O_FL ;	CompanionScrollBar=IDC_SLIDER_O_FR;		CompanionEditBox=IDC_EDIT_O_FR;	JoinCheckBox=IDC_CHECK_JOIN_O_FLFR  ;}
	else if (EditBoxID==IDC_EDIT_O_FR )	{ScrollBarID=IDC_SLIDER_O_FR ;	CompanionScrollBar=IDC_SLIDER_O_FL;		CompanionEditBox=IDC_EDIT_O_FL;	JoinCheckBox=IDC_CHECK_JOIN_O_FLFR  ;}
	else if (EditBoxID==IDC_EDIT_O_FC )	{ScrollBarID=IDC_SLIDER_O_FC ;	CompanionScrollBar=NULL;				CompanionEditBox=NULL;			JoinCheckBox=NULL                   ;}
	else if (EditBoxID==IDC_EDIT_O_LF )	{ScrollBarID=IDC_SLIDER_O_LF ;	CompanionScrollBar=NULL;				CompanionEditBox=NULL;			JoinCheckBox=NULL                   ;}
	else if (EditBoxID==IDC_EDIT_O_BL )	{ScrollBarID=IDC_SLIDER_O_BL ;	CompanionScrollBar=IDC_SLIDER_O_BR;		CompanionEditBox=IDC_EDIT_O_BR;	JoinCheckBox=IDC_CHECK_JOIN_O_BLBR  ;}
	else if (EditBoxID==IDC_EDIT_O_BR )	{ScrollBarID=IDC_SLIDER_O_BR ;	CompanionScrollBar=IDC_SLIDER_O_BL;		CompanionEditBox=IDC_EDIT_O_BL;	JoinCheckBox=IDC_CHECK_JOIN_O_BLBR  ;}
	else if (EditBoxID==IDC_EDIT_O_FLC)	{ScrollBarID=IDC_SLIDER_O_FLC;	CompanionScrollBar=IDC_SLIDER_O_FRC;	CompanionEditBox=IDC_EDIT_O_FRC;JoinCheckBox=IDC_CHECK_JOIN_O_FLCFRC;}
	else if (EditBoxID==IDC_EDIT_O_FRC)	{ScrollBarID=IDC_SLIDER_O_FRC;	CompanionScrollBar=IDC_SLIDER_O_FLC;	CompanionEditBox=IDC_EDIT_O_FLC;JoinCheckBox=IDC_CHECK_JOIN_O_FLCFRC;}
	else if (EditBoxID==IDC_EDIT_O_BC )	{ScrollBarID=IDC_SLIDER_O_BC ;	CompanionScrollBar=NULL;				CompanionEditBox=NULL;			JoinCheckBox=NULL                   ;}
	else if (EditBoxID==IDC_EDIT_O_SL )	{ScrollBarID=IDC_SLIDER_O_SL ;	CompanionScrollBar=IDC_SLIDER_O_SR;		CompanionEditBox=IDC_EDIT_O_SR;	JoinCheckBox=IDC_CHECK_JOIN_O_SLSR; ;}
	else if (EditBoxID==IDC_EDIT_O_SR )	{ScrollBarID=IDC_SLIDER_O_SR ;	CompanionScrollBar=IDC_SLIDER_O_SL;		CompanionEditBox=IDC_EDIT_O_SL;	JoinCheckBox=IDC_CHECK_JOIN_O_SLSR; ;}
	else if (EditBoxID==IDC_EDIT_O_TC )	{ScrollBarID=IDC_SLIDER_O_TC ;	CompanionScrollBar=NULL;				CompanionEditBox=NULL;			JoinCheckBox=NULL                   ;}
	else if (EditBoxID==IDC_EDIT_O_TFL)	{ScrollBarID=IDC_SLIDER_O_TFL;	CompanionScrollBar=IDC_SLIDER_O_TFR;	CompanionEditBox=IDC_EDIT_O_TFR;JoinCheckBox=IDC_CHECK_JOIN_O_TFLTFR;}
	else if (EditBoxID==IDC_EDIT_O_TFC)	{ScrollBarID=IDC_SLIDER_O_TFC;	CompanionScrollBar=NULL;				CompanionEditBox=NULL;			JoinCheckBox=NULL                   ;}
	else if (EditBoxID==IDC_EDIT_O_TFR)	{ScrollBarID=IDC_SLIDER_O_TFR;	CompanionScrollBar=IDC_SLIDER_O_TFL;	CompanionEditBox=IDC_EDIT_O_TFL;JoinCheckBox=IDC_CHECK_JOIN_O_TFLTFR;}
	else if (EditBoxID==IDC_EDIT_O_TBL)	{ScrollBarID=IDC_SLIDER_O_TBL;	CompanionScrollBar=IDC_SLIDER_O_TBR;	CompanionEditBox=IDC_EDIT_O_TBR;JoinCheckBox=IDC_CHECK_JOIN_O_TBLTBR;}
	else if (EditBoxID==IDC_EDIT_O_TBC)	{ScrollBarID=IDC_SLIDER_O_TBC;	CompanionScrollBar=NULL;				CompanionEditBox=NULL;			JoinCheckBox=NULL                   ;}
	else if (EditBoxID==IDC_EDIT_O_TBR)	{ScrollBarID=IDC_SLIDER_O_TBR;	CompanionScrollBar=IDC_SLIDER_O_TBL;	CompanionEditBox=IDC_EDIT_O_TBL;JoinCheckBox=IDC_CHECK_JOIN_O_TBLTBR;}
	
	//Master volume
	else if (EditBoxID==IDC_EDIT_MASTER_VOLUME) {ScrollBarID=IDC_SLIDER_MASTER_VOLUME; CompanionScrollBar=NULL; CompanionEditBox=NULL; JoinCheckBox=NULL;}

	//Fetch the new value
	DWORD newval=::GetDlgItemInt(m_Dlg,EditBoxID,NULL,FALSE);
	if (newval > 400) newval=400; if (newval < 0) newval=0;
	
	//Update to both
	if (newval!=(unsigned)::SendMessage(GetDlgItem(m_Dlg,ScrollBarID),TBM_GETPOS,0,0))	::SendMessage(GetDlgItem(m_Dlg,ScrollBarID),TBM_SETPOS,TRUE,newval);
	if (newval!=::GetDlgItemInt(m_Dlg,EditBoxID,NULL,FALSE))					::SetDlgItemInt(m_Dlg,EditBoxID,newval,FALSE);

	//Check if we have a joint control
	if (JoinCheckBox!=NULL && BST_CHECKED==::IsDlgButtonChecked(m_Dlg,JoinCheckBox)) 
	{
		if (newval!=(unsigned)::SendMessage(GetDlgItem(m_Dlg,CompanionScrollBar),TBM_GETPOS,0,0))	::SendMessage(GetDlgItem(m_Dlg,CompanionScrollBar),TBM_SETPOS,TRUE,newval);
		if (newval!=::GetDlgItemInt(m_Dlg,CompanionEditBox,NULL,FALSE))				::SetDlgItemInt(m_Dlg,CompanionEditBox,newval,FALSE);
	}




	/*
	DWORD newval=GetDlgItemInt(m_Dlg,IDC_EDIT_VOLUME_GAIN_PERCENT,NULL,FALSE);
	if (newval > 400) newval=400;

	//Update to both
	if (newval!=(unsigned)::SendMessage(GetDlgItem(m_Dlg,IDC_SLIDER_VOLUME_GAIN_PERCENT),TBM_GETPOS,0,0)) ::SendMessage(GetDlgItem(m_Dlg,IDC_SLIDER_VOLUME_GAIN_PERCENT),TBM_SETPOS,TRUE,newval);
	if (newval!=GetDlgItemInt(m_Dlg,IDC_EDIT_VOLUME_GAIN_PERCENT,NULL,FALSE)) SetDlgItemInt(m_Dlg,IDC_EDIT_VOLUME_GAIN_PERCENT,newval,FALSE);

	//Update volume to filter immediately
	StereoDownmixerParams temp_params;
	m_pIStereoDownmixer->GetParams(&temp_params,StereoDownmixerParams::SectionChoose::gains);
	temp_params.p1.VolumeGainPercent=newval;
	m_pIStereoDownmixer->SetParams(&temp_params,StereoDownmixerParams::SectionChoose::gains);
	*/
}
