#include "StdAfx.h"
#include "DeckLinkVideoDevice.h"

#include "log.h"

CDeckLinkVideoDevice::CDeckLinkVideoDevice(LPUNKNOWN pUnk, IDeckLink *pDevice)
: CBaseVideoDevice(pUnk)
, m_pCurrentVideoMode(NULL)
{
	m_pDevice = pDevice;
	m_pDeviceInput = m_pDevice;
	CComQIPtr<IDeckLinkConfiguration> pConfigurator = pDevice;
	if (pConfigurator)
	{
		LONGLONG value;
		pConfigurator->GetInt(bmdDeckLinkConfigVideoInputConversionMode, &value);
		pConfigurator->SetInt(bmdDeckLinkConfigVideoInputConversionMode, bmdVideoInputAnamorphicDownconversionFromHD1080);
		pConfigurator->WriteConfigurationToPreferences();
	}
}

CDeckLinkVideoDevice::~CDeckLinkVideoDevice(void)
{
	Stop();
	m_pDevice = NULL;
}

HRESULT CDeckLinkVideoDevice::GetModesIterator( 
	/* [out] */ IVideoDeviceModeIterator **ppIterator)
{
	CheckPointer(ppIterator, E_POINTER);

	CComPtr<IVideoDeviceModeIterator> pIterator = new CDeckLinkVideoModeIterator(this);
	return pIterator.CopyTo(ppIterator);
}

HRESULT CDeckLinkVideoDevice::IsVideoModeSupported( 
		/* [in] */ IVideoDeviceMode *videoMode)
{
	CComPtr<CDeckLinkVideoMode> pModeDL = NULL;
	CComPtr<IVideoDeviceModeIterator> pIterator = NULL;
	
	if (FAILED(GetModesIterator(&pIterator)))
	{
		return E_FAIL;
	}

	while (SUCCEEDED(pIterator->Next((IVideoDeviceMode**)&pModeDL)))
	{
		if (SUCCEEDED(pModeDL->Compare(videoMode)))
		{
			CComPtr<IDeckLinkDisplayMode> pDisplayMode = NULL;
			if (!pModeDL)
				continue;
			if (FAILED(pModeDL->GetDeckLinkDisplayMode(&pDisplayMode)))
				continue;

			BMDPixelFormat bmdPixelFormat;
			pModeDL->GetBMDPixelFormat(&bmdPixelFormat);

			BMDVideoInputFlags bmdInputFlags = bmdVideoInputFlagDefault;

			BMDDisplayModeSupport result = bmdDisplayModeNotSupported;
			m_pDeviceInput->DoesSupportVideoMode(pDisplayMode->GetDisplayMode(), bmdPixelFormat, bmdInputFlags, &result, &pDisplayMode);
			if (result == bmdDisplayModeSupported)
			{
				return S_OK;
			}
			else if (result == bmdDisplayModeSupportedWithConversion)
			{
				return S_OK;
			}
			else
			{
				return E_FAIL;
			}
		}
		pModeDL = NULL;
	}

	return E_FAIL;
}

HRESULT CDeckLinkVideoDevice::SetVideoMode( 
	/* [in] */ IVideoDeviceMode *videoMode)
{
	CheckPointer(videoMode, E_POINTER);
	CComPtr<IVideoDeviceMode> pMode = NULL;
	CComPtr<IVideoDeviceModeIterator> pIterator = NULL;
	
	if (FAILED(GetModesIterator(&pIterator)))
	{
		return E_FAIL;
	}

	BMDDisplayModeSupport result = bmdDisplayModeNotSupported;
	while (SUCCEEDED(pIterator->Next(&pMode)) && pMode &&
		result == bmdDisplayModeNotSupported)
	{
		if (SUCCEEDED(pMode->Compare(videoMode)))
		{
			CComPtr<IDeckLinkDisplayMode> pDisplayMode = NULL;
			CComQIPtr<IDeckLinkVideoMode> pModeDL = pMode;
			if (!pModeDL)
				continue;
			if (FAILED(pModeDL->GetDeckLinkDisplayMode(&pDisplayMode)))
				continue;

			BMDPixelFormat bmdPixelFormat;
			pModeDL->GetBMDPixelFormat(&bmdPixelFormat);

			BMDVideoInputFlags bmdInputFlags = bmdVideoInputFlagDefault;
			if (smSideBySide == videoMode->GetStereoMode())
			{
				bmdInputFlags = bmdVideoInputDualStream3D;
			}

			CComPtr<IDeckLinkDisplayMode> pRealDisplayMode;
			m_pDeviceInput->DoesSupportVideoMode(pDisplayMode->GetDisplayMode(), bmdPixelFormat, bmdInputFlags, &result, &pRealDisplayMode);
			if (result == bmdDisplayModeSupported)
			{
				m_pCurrentVideoMode = pMode;
				return S_OK;
			}
			else 
			{
				return E_FAIL;
			}
		}
		pMode = NULL;
	}

	return S_OK;
}

HRESULT CDeckLinkVideoDevice::GetCurrentVideoMode( 
	/* [out] */ IVideoDeviceMode **pVideoMode)
{
	CheckPointer(pVideoMode, E_POINTER);
	CheckPointer(m_pCurrentVideoMode, E_POINTER);
	
	return m_pCurrentVideoMode->QueryInterface(IID_PPV_ARGS(pVideoMode));
}

HRESULT CDeckLinkVideoDevice::GetDeviceName( 
	/* [out] */ BSTR *name)
{
	return m_pDevice->GetDisplayName(name);
}


HRESULT CDeckLinkVideoDevice::Start(IDeckLinkInputCallback* pCallback, IDeckLinkScreenPreviewCallback* previewCallBack)
{
	CheckPointer(m_pDeviceInput, E_POINTER);
	CheckPointer(m_pCurrentVideoMode, E_POINTER);
	HRESULT hr = Stop();
	if FAILED(hr)
	{
		return hr;
	}

	CComPtr<IDeckLinkDisplayMode> pDisplayMode = NULL;
	CComQIPtr<IDeckLinkVideoMode> pModeDL = m_pCurrentVideoMode;
	CheckPointer(pModeDL, E_POINTER);
	hr = pModeDL->GetDeckLinkDisplayMode(&pDisplayMode);
	CheckPointer(pDisplayMode, E_POINTER);
	if FAILED(hr)
	{
		return hr;
	}

	BMDPixelFormat bmdPixelFormat;
	hr = pModeDL->GetBMDPixelFormat(&bmdPixelFormat);
	if FAILED(hr)
	{
		return hr;
	}

	BMDVideoInputFlags flags = 0;	
	if (smSideBySide == m_pCurrentVideoMode->GetStereoMode())
	{
		flags = bmdVideoInputDualStream3D;
	}
	if (0 == flags)
	{
		flags = bmdVideoInputFlagDefault;
	}
	m_pDeviceInput->SetCallback(pCallback);
	m_pDeviceInput->SetScreenPreviewCallback(previewCallBack);
	hr = m_pDeviceInput->EnableVideoInput(pDisplayMode->GetDisplayMode(), bmdPixelFormat, flags/* & bmdVideoInputEnableFormatDetection*/);
	long channelCount = 8;
	hr = m_pDeviceInput->EnableAudioInput(bmdAudioSampleRate48kHz, bmdAudioSampleType16bitInteger, channelCount);	
	if SUCCEEDED(hr)
	{
		hr = m_pDeviceInput->StartStreams();
	}
	return hr;
}

HRESULT CDeckLinkVideoDevice::Stop()
{
	CheckPointer(m_pDeviceInput, E_POINTER);
	HRESULT hr = S_OK;
	m_pDeviceInput->StopStreams();
	m_pDeviceInput->DisableVideoInput();
	m_pDeviceInput->DisableAudioInput();
	m_pDeviceInput->SetCallback(NULL);
	m_pDeviceInput->SetScreenPreviewCallback(NULL);
	return hr;
}

HRESULT CDeckLinkVideoDevice::ValidateBufferFullness()
{
	CheckPointer(m_pDeviceInput, E_POINTER);
	UINT availableFrameCountV = 0;
	m_pDeviceInput->GetAvailableVideoFrameCount(&availableFrameCountV);
	UINT availableFrameCountA = 0;
	m_pDeviceInput->GetAvailableAudioSampleFrameCount(&availableFrameCountA);
	if (availableFrameCountV > 10)
	{
		//P7_CRITICAL(TEXT("Buffers V=%d A=%d Full video"), availableFrameCountV, availableFrameCountA);
		//m_pDeviceInput->FlushStreams();
		return E_FAIL;
	}	
	if (availableFrameCountA >= 48000)
	{		
		//P7_CRITICAL(TEXT("Buffers V=%d A=%d Full Audio"), availableFrameCountV, availableFrameCountA);
		//m_pDeviceInput->FlushStreams();
		return E_FAIL;
	}
	/*if (availableFrameCountA > 48000 / 2)
	{		
		P7_CRITICAL(TEXT("Buffers V=%d A=%d Full"), availableFrameCountV, availableFrameCountA);
		return E_FAIL;
	}*/
	//P7_CRITICAL(TEXT("Buffers V=%d A=%d"), availableFrameCountV, availableFrameCountA);
	return S_OK;
}


CDeckLinkVideoMode::CDeckLinkVideoMode(LPUNKNOWN pUnk, IDeckLinkDisplayMode *pMode, BMDPixelFormat bmdPixelFormat,
									   EStereoMode eStereoMode)
: CBaseVideoDeviceMode(pUnk)
{
	m_pDisplayMode = pMode;
	m_bmdPixelFormat = bmdPixelFormat;
	SetWidth(m_pDisplayMode->GetWidth());
	SetHeight(m_pDisplayMode->GetHeight());
	switch (bmdPixelFormat)
	{
	case bmdFormat8BitYUV:
		{
			SetBpp(2);
			SetPixelFormat(pfUYVY);
			break;
		}
	case bmdFormat10BitYUV:
		{
			SetBpp(2); //TODO: Correct Bpp
			SetPixelFormat(pfV210);
			break;
		}
	case bmdFormat8BitARGB:
		{
			SetBpp(4);
			SetPixelFormat(pfARGB);
			break;
		}
	case bmdFormat8BitBGRA:	
		{
			SetBpp(4);
			SetPixelFormat(pfBGRA);
			break;
		}
	case bmdFormat10BitRGB:
		{
			SetBpp(4);//TODO: Correct Bpp
			SetPixelFormat(pfR210);
			break;
		}
	}
	
	switch (pMode->GetFieldDominance())
	{
	case bmdUnknownFieldDominance:
		{
			SetFieldOrder(foDefault);
			break;
		}
	case bmdLowerFieldFirst:
		{
			SetFieldOrder(foLowFieldFirst);
			break;
		}
	case bmdUpperFieldFirst:
		{
			SetFieldOrder(foUpFieldFirst);
			break;
		}
	case bmdProgressiveFrame:
		{
			SetFieldOrder(foProgressive);
			break;
		}
	case bmdProgressiveSegmentedFrame:
		{
			SetFieldOrder(foProgressiveSegmented);
			break;
		}		
	}

	SetStereoMode(eStereoMode);
	SetConnectionType(vctDefault);

	BMDTimeValue timeValue = 0;
	BMDTimeScale timeScale = 0;	
	pMode->GetFrameRate(&timeValue, &timeScale);
	SetTimeUnitsPerSecond(timeScale);
	SetFrameDuration(timeValue);
}

CDeckLinkVideoMode::~CDeckLinkVideoMode()
{
}

STDMETHODIMP CDeckLinkVideoMode::NonDelegatingQueryInterface(REFIID riid, void **ppv)
{
	if(IsEqualGUID(riid, __uuidof(IDeckLinkVideoMode))) {
		return GetInterface(static_cast<IDeckLinkVideoMode*>(this), ppv);		
	}
	else 	
	{
		return CBaseVideoDeviceMode::NonDelegatingQueryInterface(riid, ppv);
	}
}

HRESULT CDeckLinkVideoMode::GetDeckLinkDisplayMode(IDeckLinkDisplayMode **ppMode)
{
	CheckPointer(ppMode, E_POINTER);
	return m_pDisplayMode.CopyTo(ppMode);
}

HRESULT CDeckLinkVideoMode::GetBMDPixelFormat(BMDPixelFormat *pbmdPixelFormat)
{
	CheckPointer(pbmdPixelFormat, E_POINTER);
	*pbmdPixelFormat = m_bmdPixelFormat;
	return S_OK;
}

CDeckLinkVideoModeIterator::CDeckLinkVideoModeIterator(CDeckLinkVideoDevice *pDevice)
: CUnknown("DeckLinkVideoModeIterator", NULL)
, m_uiPixelFormat(0)
, m_uiVideoInputFlag(0)
{
	m_pVideoDevice = pDevice;
	if (m_pVideoDevice)
	{
		m_pDeviceInput = m_pVideoDevice->m_pDevice;
		m_pDeviceInput->GetDisplayModeIterator(&m_pIterator);
	}
}

CDeckLinkVideoModeIterator::~CDeckLinkVideoModeIterator(void)
{
}

HRESULT CDeckLinkVideoModeIterator::Next( 
        /* [out] */ IVideoDeviceMode **ppVideoMode)
{
	CheckPointer(m_pIterator, E_POINTER);
	CheckPointer(ppVideoMode, E_POINTER);
	if (*ppVideoMode != NULL)
	{
		return E_FAIL;
	}

	if ((m_uiPixelFormat < uiPixelFormatsCount) &&
		(m_uiVideoInputFlag < uiVideoInputFlagsCount) &&
		m_pDisplayMode)
	{		
		BMDDisplayModeSupport result = bmdDisplayModeNotSupported;
		while (result == bmdDisplayModeNotSupported)
		{			
			CComPtr<IDeckLinkDisplayMode> pRealDisplayMode;
			m_pDeviceInput->DoesSupportVideoMode(m_pDisplayMode->GetDisplayMode(), ePixelFormats[m_uiPixelFormat], eVideoInputFlags[m_uiVideoInputFlag], &result, &pRealDisplayMode);			
			if (result == bmdDisplayModeSupported)
			{
				EStereoMode stereoMode = smDefault;
				if (eVideoInputFlags[m_uiVideoInputFlag] == bmdVideoInputDualStream3D)
					stereoMode = smSideBySide;
				*ppVideoMode = new CDeckLinkVideoMode(NULL, pRealDisplayMode, ePixelFormats[m_uiPixelFormat], stereoMode);
				if (*ppVideoMode)
				{
					(*ppVideoMode)->AddRef();					
				}
			}
			else
			{
				if (result == bmdDisplayModeSupportedWithConversion)
				{
					break;
				}
			}

			m_uiVideoInputFlag++;
			if (!(m_uiVideoInputFlag < uiVideoInputFlagsCount))
			{
				m_uiVideoInputFlag = 0;
				m_uiPixelFormat++;
				if (!(m_uiPixelFormat < uiPixelFormatsCount))
				{
					break;
				}
			}
		}		
	}
	
	if (*ppVideoMode)
	{
		return S_OK;
	}

	if (!(m_uiPixelFormat < uiPixelFormatsCount) || m_pDisplayMode == NULL)
	{
		m_uiPixelFormat = 0;
		m_uiVideoInputFlag = 0;
		m_pDisplayMode = NULL;		
		HRESULT hr = m_pIterator->Next(&m_pDisplayMode);
		if (SUCCEEDED(hr) && m_pDisplayMode != NULL)
		{
			return Next(ppVideoMode);			
		}
		else 
			return hr;
	}

	
	return E_FAIL;
}


const BMDPixelFormat CDeckLinkVideoModeIterator::ePixelFormats[] = 
{
	bmdFormat8BitYUV,
	bmdFormat10BitYUV,
	bmdFormat8BitARGB,
	bmdFormat8BitBGRA,
	bmdFormat10BitRGB
};

const BMDVideoInputFlags CDeckLinkVideoModeIterator::eVideoInputFlags[] =
{
	bmdVideoInputFlagDefault,
	bmdVideoInputDualStream3D
};

const UINT CDeckLinkVideoModeIterator::uiVideoInputFlagsCount = 
	sizeof(CDeckLinkVideoModeIterator::eVideoInputFlags) / sizeof(*CDeckLinkVideoModeIterator::eVideoInputFlags);

const UINT CDeckLinkVideoModeIterator::uiPixelFormatsCount = 
	sizeof(CDeckLinkVideoModeIterator::ePixelFormats) / sizeof(*CDeckLinkVideoModeIterator::ePixelFormats);