#pragma once
#include "DeckLinkAPI_h.h"
//#include "VideoProperty.h"
#include <vector>
#include <atlcomcli.h>

using namespace std;

class CDeckLinkDevice
{
public:
	CDeckLinkDevice(IDeckLink *pDevice);
	~CDeckLinkDevice(void);

public:
	HRESULT DoRefreshPropertiesList(void);
	HRESULT IsModeSupported(IDeckLinkDisplayMode *pDisplayMode, BMDPixelFormat bmdPixelFormat);
	HRESULT GetModesCount(LONG *);
	HRESULT	GetMode(CVideoProperty *pVideoProperty,LONG lIndex);
	HRESULT GetDevice(IDeckLink **ppDevice);
		
private:
	vector<CVideoProperty> m_vProperties;
	CComPtr<IDeckLink> m_pDevice;
	CComQIPtr<IDeckLinkInput> m_pDeviceInput;
};


class CDeckLinkDeviceCollection
{
public:
	CDeckLinkDeviceCollection();
	~CDeckLinkDeviceCollection();

public:
	HRESULT GetDeviceCount(LONG *pCount);
	HRESULT	GetDevice(CDeckLinkDevice **ppDecklinkDevice);

private:
	vector<CDeckLinkDevice> m_vDevices;
};
