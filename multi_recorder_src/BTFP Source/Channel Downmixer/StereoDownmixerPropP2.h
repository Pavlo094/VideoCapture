// StereoDownmixerProp.h: interface for the CStereoDownmixerPropP2 class
// 
//////////////////////////////////////////////////////////////////////
#pragma once

///////////////////////////////////////////////////////////////////////
//  CStereoDownmixerPropP2: property page of CStereoDownmixer
//  It provides a dialogbox to setup the CStereoDownmixer.
///////////////////////////////////////////////////////////////////////
class CStereoDownmixerPropP2 : public CBasePropertyPage
{
public:
    static CUnknown * WINAPI CreateInstance(LPUNKNOWN lpunk, HRESULT *phr);

private:
    // Construction/destruction
    CStereoDownmixerPropP2(LPUNKNOWN lpunk, HRESULT *phr);
	~CStereoDownmixerPropP2();
    
	//First time initialization
	void OnInitDialog();
    // Handle the messages in the dialogbox
    BOOL OnReceiveMessage(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam);
	void OnSomeVolumeScrollbarMoved(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam);
	void OnSomeVolumeEditBoxChanged(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam);

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
	void SaveDataToFilter();

    BOOL m_bIsBeingInitialized; // Used to ignore startup messages and avoid saving data back to filter until the initializazion is complete
    IStereoDownmixer *m_pIStereoDownmixer;  // interface of the filter
    StereoDownmixerParams m_StereoDownmixerParams; // Filter parameters

	BOOL m_bIsInputConnected;
	BOOL m_bIsOutputConnected;
	CMediaType m_InputMediaType;
	CMediaType m_OutputMediaType;
};