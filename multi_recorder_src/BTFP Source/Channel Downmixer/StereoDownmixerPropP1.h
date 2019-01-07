// StereoDownmixerProp.h: interface for the CStereoDownmixerPropP1 class
// 
//////////////////////////////////////////////////////////////////////
#pragma once

///////////////////////////////////////////////////////////////////////
//  CStereoDownmixerPropP1: property page of CStereoDownmixer
//  It provides a dialogbox to setup the CStereoDownmixer.
///////////////////////////////////////////////////////////////////////
class CStereoDownmixerPropP1 : public CBasePropertyPage
{
public:
    static CUnknown * WINAPI CreateInstance(LPUNKNOWN lpunk, HRESULT *phr);

private:
    // Construction/destruction
    CStereoDownmixerPropP1(LPUNKNOWN lpunk, HRESULT *phr);
	~CStereoDownmixerPropP1();
    
	//First time initialization
	void OnInitDialog();
    // Handle the messages in the dialogbox
    BOOL OnReceiveMessage(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam);

    // Connect the property page to filter (pUnknown) or Disconnect
    HRESULT OnConnect(IUnknown *pUnknown);
    HRESULT OnDisconnect();
	

    // When activate or deactivate or apply changes the property page
    HRESULT OnActivate();
    HRESULT OnDeactivate();
    HRESULT OnApplyChanges();
	void UpdateView(); //Update controls influenced by other controls.

    // Helper functions
    void GetControlValues(); // Read parameters from controls
    void SetControlValues(); // Set controls according to parameters
	BOOL CurrentInputTypeCompatibleWithParams(); //Is current input type compatible with the params set on the property page?
	BOOL CurrentOutputTypeCompatibleWithParams();
	void GetStringFromMediaType(char *str, DWORD buffersize, CMediaType &mt);

    BOOL m_bIsInitialized; // Used to ignore startup messages
    IStereoDownmixer *m_pIStereoDownmixer;  // interface of the filter
    StereoDownmixerParams m_StereoDownmixerParams; // Filter parameters
	HFONT m_MediaTypeEditBoxFont;
};

