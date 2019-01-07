// StereoDownmixerProp.h: interface for the CStereoDownmixerPropP3 class
// 
//////////////////////////////////////////////////////////////////////
#pragma once

///////////////////////////////////////////////////////////////////////
//  CStereoDownmixerPropP3: property page of CStereoDownmixer
//  It provides a dialogbox to setup the CStereoDownmixer.
///////////////////////////////////////////////////////////////////////
class CStereoDownmixerPropP3 : public CBasePropertyPage
{
public:
    static CUnknown * WINAPI CreateInstance(LPUNKNOWN lpunk, HRESULT *phr);

private:
    // Construction/destruction
    CStereoDownmixerPropP3(LPUNKNOWN lpunk, HRESULT *phr);
	~CStereoDownmixerPropP3();
    
	//First time initialization
	void OnInitDialog();
    // Handle the messages in the dialogbox
    BOOL OnReceiveMessage(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam);
	void OnClickedReloadMatrixNow();
	void OnClickedBrowse();

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

    BOOL m_bIsInitialized; // Used to ignore startup messages
    IStereoDownmixer *m_pIStereoDownmixer;  // interface of the filter
    StereoDownmixerParams m_StereoDownmixerParams; // Filter parameters
};

