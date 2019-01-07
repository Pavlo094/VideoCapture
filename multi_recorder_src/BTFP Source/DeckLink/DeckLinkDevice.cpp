#include "stdafx.h"
#include "DeckLinkDevice.h"

CDeckLinkDevice::CDeckLinkDevice(IDeckLink *pDevice)
{
	m_pDevice = pDevice;
	m_pDeviceInput = m_pDevice;
}

CDeckLinkDevice::~CDeckLinkDevice(void)
{
	m_pDevice = NULL;	
}


HRESULT CDeckLinkDevice::DoRefreshPropertiesList(void)
{
	if (!m_pDeviceInput)
	{
		return E_POINTER;
	}

	CComPtr<IDeckLinkDisplayModeIterator> pDisplayModeIterator;
	m_pDeviceInput->GetDisplayModeIterator(&pDisplayModeIterator);
	
	if (!pDisplayModeIterator)
	{
		return E_POINTER;
	}
	CComPtr<IDeckLinkDisplayMode> pDisplayMode;
	while (pDisplayModeIterator->Next(&pDisplayMode) == S_OK)
	{
		CVideoProperty videoProperty;
		videoProperty.Width(pDisplayMode->GetWidth());
		videoProperty.Height(pDisplayMode->GetHeight());		
		switch (pDisplayMode->GetFieldDominance())
		{
			case bmdLowerFieldFirst:
				videoProperty.FieldOrder(foLowFieldFirst);
				break;
			case bmdUpperFieldFirst:
				videoProperty.FieldOrder(foUpFieldFirst);
				break;
			case bmdProgressiveFrame:
				videoProperty.FieldOrder(foProgressive);
				break;
			case bmdProgressiveSegmentedFrame:
				videoProperty.FieldOrder(foProgressiveSegmented);
				break;
		}		

		if (bmdFrameFlagFlipVertical == pDisplayMode->GetFlags())
		{
			videoProperty.Height(-videoProperty.Height());
		}

		BMDTimeValue frameDuration;
		BMDTimeScale timeScale;
		pDisplayMode->GetFrameRate(&frameDuration, &timeScale);
		videoProperty.FrameTime(frameDuration * timeScale / 100);


		if (S_OK == IsModeSupported(pDisplayMode, BMDPixelFormat::bmdFormat8BitYUV))
			videoProperty.AddPixelFormat(BMDPixelFormat::bmdFormat8BitYUV);
		if (S_OK == IsModeSupported(pDisplayMode, BMDPixelFormat::bmdFormat8BitARGB))
			videoProperty.AddPixelFormat(BMDPixelFormat::bmdFormat8BitARGB);
		if (S_OK == IsModeSupported(pDisplayMode, BMDPixelFormat::bmdFormat8BitBGRA))
			videoProperty.AddPixelFormat(BMDPixelFormat::bmdFormat8BitBGRA);

		if (S_OK == IsModeSupported(pDisplayMode, BMDPixelFormat::bmdFormat10BitYUV))
			videoProperty.AddPixelFormat(BMDPixelFormat::bmdFormat10BitYUV);
		if (S_OK == IsModeSupported(pDisplayMode, BMDPixelFormat::bmdFormat10BitRGB))
			videoProperty.AddPixelFormat(BMDPixelFormat::bmdFormat10BitRGB);
		

		m_vProperties.push_back(videoProperty);
		pDisplayMode = NULL;
	}

	return S_OK;
}

HRESULT CDeckLinkDevice::IsModeSupported(IDeckLinkDisplayMode *pDisplayMode, BMDPixelFormat bmdPixelFormat)
{
	BMDDisplayModeSupport resultMode;
	CComPtr<IDeckLinkDisplayMode> pResultDisplayMode = NULL;		

	if (S_OK ==m_pDeviceInput->DoesSupportVideoMode(pDisplayMode->GetDisplayMode(), bmdPixelFormat, 
		bmdVideoInputFlagDefault, &resultMode, &pResultDisplayMode))
	{
		if (resultMode != bmdDisplayModeNotSupported)
		{
			pResultDisplayMode = NULL;
			return S_OK;
		}
	}
	pResultDisplayMode = NULL;
	return S_FALSE;
}

HRESULT CDeckLinkDevice::GetModesCount(LONG *plIndex)
{
	CheckPointer(plIndex, E_POINTER);

	*plIndex = m_vProperties.size();

	return S_OK;
}
HRESULT	CDeckLinkDevice::GetMode(CVideoProperty *pVideoProperty,LONG lIndex)
{
	CheckPointer(pVideoProperty, E_POINTER);

	if (lIndex >= m_vProperties.size())
		return E_FAIL;
	
	*pVideoProperty = m_vProperties[lIndex];
}

HRESULT CDeckLinkDevice::GetDevice(IDeckLink **ppDevice)
{
	CheckPointer(ppDevice, E_POINTER);
	CheckPointer(m_pDevice, E_POINTER);

	*ppDevice = m_pDevice;
	return E_POINTER;
}


CDeckLinkDeviceCollection::CDeckLinkDeviceCollection()
{
	IDeckLink* pDeckLink;
	int n = 0;
	HRESULT result;

	CComPtr<IDeckLinkIterator> deckLinkIterator = NULL;
	if (CoCreateInstance(CLSID_CDeckLinkIterator, NULL, CLSCTX_ALL, IID_IDeckLinkIterator, (void**)&deckLinkIterator) != S_OK || 
		deckLinkIterator == NULL) 
	{
	}

	while (deckLinkIterator->Next(&pDeckLink) == S_OK) {	
		m_vDevices.push_back(CDeckLinkDevice(pDeckLink));
	}
}

CDeckLinkDeviceCollection::~CDeckLinkDeviceCollection()
{

}

HRESULT CDeckLinkDeviceCollection::GetDeviceCount(LONG *pCount)
{
	CheckPointer(pCount, E_POINTER);
	*pCount = m_vDevices.size();
	return S_OK;
}

HRESULT	CDeckLinkDeviceCollection::GetDevice(CDeckLinkDevice **ppDecklinkDevice)
{
	return S_OK;
}