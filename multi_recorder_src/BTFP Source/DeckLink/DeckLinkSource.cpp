#include "stdafx.h"

#include "DecklLinkSource.h"
#include "DeckLinkFilterConfigPage.h"

#include "log.h"
#include <vld.h>

AMOVIESETUP_FILTER sudDeckLinkHDStereoSource =
{
	&CLSID_DeckLinkHDStereoSource,
	L"DeckLinkHDStereoSource",
	MERIT_DO_NOT_USE,
	0,                  // # of pins
	NULL                // pin struct
};

CDeckLinkSource::CDeckLinkSource(IUnknown * pUnk, HRESULT * phr )
	: CBaseFilter(NAME("DeckLinkHDStereoSource"), pUnk, &m_cStateLock, CLSID_DeckLinkHDStereoSource)
	, m_pDeckLinkDevice(NULL)
	, m_pPropertyBag(NULL)
	, m_pVideoPin(new CDeckLinkVideoOutputPin(this, phr, true))
	, m_pVideoPinPreview(new CDeckLinkVideoOutputPin(this, phr, false, PIN_CATEGORY_PREVIEW))
	, m_pAudioPin(new CDeckLinkAudioOutputPin(this, phr))

{
	if (phr)
	{
		*phr = S_OK;
	}
	m_pPins.push_back(m_pVideoPin.get());
	m_pPins.push_back(m_pAudioPin.get());
	m_pPins.push_back(m_pVideoPinPreview.get());
}

CDeckLinkSource::~CDeckLinkSource()
{
	Save(m_pPropertyBag, FALSE, FALSE);
	m_pDeckLinkDevice = NULL;	
};

ULONG CDeckLinkSource::AddRef() {                             
    return GetOwner()->AddRef();                            
};                                                          
    
ULONG CDeckLinkSource::Release() {                            
    return GetOwner()->Release();                           
};

int CDeckLinkSource::GetPinCount()
{
	return m_pPins.size();
}

CBasePin* CDeckLinkSource::GetPin(int n)
{
	if (m_pPins.size() > static_cast<size_t>(n))
	{
		return m_pPins.at(n);
	}
	return nullptr;
}


CUnknown * WINAPI CDeckLinkSource::CreateInstance(LPUNKNOWN lpunk, HRESULT *phr)
{
	CDeckLinkSource *pNewFilter = new CDeckLinkSource(lpunk, phr );

	if (phr)
	{
		if (pNewFilter == NULL) 
			*phr = E_OUTOFMEMORY;
		else
			*phr = S_OK;
	}

	return pNewFilter;
}

STDMETHODIMP CDeckLinkSource::GetPages(CAUUID *pPages)
{
	CheckPointer(pPages,E_POINTER);

	pPages->cElems = 1;
	pPages->pElems = (GUID *) CoTaskMemAlloc(pPages->cElems*sizeof(GUID));

	if (pPages->pElems == NULL)
		return E_OUTOFMEMORY;

	pPages->pElems[0] = CLSID_CDeckLinkFilterConfigPage;

	return NOERROR;

}

STDMETHODIMP CDeckLinkSource::NonDelegatingQueryInterface(REFIID riid, void** ppv)
{
	if (riid == IID_ISpecifyPropertyPages) 
		return GetInterface((ISpecifyPropertyPages *) this, ppv);
	else if (riid == IID_IPersistPropertyBag) 
		return GetInterface((IPersistPropertyBag *) this, ppv);
	else if (riid == IID_IVideoDeviceHolder)
		return GetInterface((IVideoDeviceHolder *) this, ppv);
	else if (riid == IID_IDeckLinkSourceConfig)
		return GetInterface((IDeckLinkSourceConfig *) this, ppv);
	else if (riid == IID_IAMDroppedFrames)
		return GetInterface((IAMDroppedFrames *) this, ppv);

	return CBaseFilter::NonDelegatingQueryInterface(riid, ppv);
}

HRESULT CDeckLinkSource::InitNew( void)
{
	return E_NOTIMPL;
}

HRESULT CDeckLinkSource::Load( 

									   /* [in] */ __RPC__in_opt IPropertyBag *pPropBag,
									   /* [unique][in] */ __RPC__in_opt IErrorLog *pErrorLog)
{
	CheckPointer(pPropBag, E_POINTER);
	m_pPropertyBag = pPropBag;
	
	VARIANT var;
	var.vt = VT_BSTR;
	HRESULT hr = pPropBag->Read(DEVICENAME, &var, 0);
	if (SUCCEEDED(hr) && var.bstrVal)
	{
		CComBSTR bstrName = var.bstrVal;
		m_sDeviceName = bstrName; 
	}

	CAutoLock al(&m_csDevice);
	if FAILED(UpdateDevice())
	{
		return S_FALSE;
	}

	if (!m_pDeckLinkDevice)
	{
		return E_POINTER;
	}

	INT iWidth;
	BOOL bWidth = FALSE;
	INT iHeight;
	BOOL bHeight = FALSE;
	var.vt = VT_I4;
	hr = pPropBag->Read(SWIDTH, &var, 0);
	if (SUCCEEDED(hr))
	{
		iWidth = var.lVal;	
		bWidth = TRUE;
	}
	hr = pPropBag->Read(SHEIGHT, &var, 0);
	if (SUCCEEDED(hr))
	{
		iHeight = var.lVal;	
		bHeight = TRUE;
	}
	
	INT iBpp;
	BOOL bBpp = FALSE;
	var.vt = VT_I4;
	hr = pPropBag->Read(BPP, &var, 0);
	if (SUCCEEDED(hr))
	{
		iBpp = var.lVal;	
		bBpp = TRUE;
	}

	DWORD dwPixelFormat = 0;
	BOOL bPixelFormat = FALSE;
	var.vt = VT_I4;
	hr = pPropBag->Read(PIXELFORMAT, &var, 0);
	if (SUCCEEDED(hr))
	{
		dwPixelFormat = var.lVal;
		bPixelFormat = TRUE;
	}
	if (dwPixelFormat == pfV210 ||
		dwPixelFormat == pfR210)
	{
		dwPixelFormat = pfUYVY;
	}	

	double dFramerate;
	BOOL bFrameRate = FALSE;
	var.vt = VT_I4;
	hr = pPropBag->Read(FRAMERATE, &var, 0);
	if (SUCCEEDED(hr))
	{
		dFramerate = (double)var.lVal / 1000000.;
		bFrameRate = TRUE;
	}

	DWORD dwFieldOrder;
	BOOL bFieldOrder = FALSE;
	var.vt = VT_I4;
	hr = pPropBag->Read(FIELDORDER, &var, 0);
	if (SUCCEEDED(hr))
	{
		dwFieldOrder = var.lVal;
		bFieldOrder = TRUE;
	}

	DWORD dwStereoMode;
	BOOL bStereoMode = FALSE;
	var.vt = VT_I4;
	hr = pPropBag->Read(STEREOMODE, &var, 0);
	if (SUCCEEDED(hr))
	{
		dwStereoMode = var.lVal;
		bStereoMode = TRUE;
	}

	CComPtr<IVideoDeviceModeIterator> pModesIterator;
	m_pDeckLinkDevice->GetModesIterator(&pModesIterator);
	CComPtr<IVideoDeviceMode> pVideoMode;
	while (SUCCEEDED(pModesIterator->Next(&pVideoMode)) && pVideoMode)
	{
		if (bWidth)
		{
			if (pVideoMode->GetWidth() != iWidth)
			{
				pVideoMode = NULL;
				continue;
			}
		}
		if (bHeight)
		{
			if (pVideoMode->GetHeight() != iHeight)
			{
				pVideoMode = NULL;
				continue;
			}
		}
		if (bBpp)
		{
			if (pVideoMode->GetBpp() != iBpp)
			{
				pVideoMode = NULL;
				continue;
			}
		}
		if (bPixelFormat)
		{
			if (pVideoMode->GetPixelFormat() != dwPixelFormat)
			{
				pVideoMode = NULL;
				continue;
			}
		}
		if (bFrameRate)
		{
			if (fabs(pVideoMode->GetFramesPerSecond() - dFramerate) > 1e-4)
			{
				pVideoMode = NULL;
				continue;
			}
		}
		if (bFieldOrder)
		{			
			if (pVideoMode->GetFieldOrder() != dwFieldOrder)
			{
				pVideoMode = NULL;
				continue;
			}
		}
		if (bStereoMode)
		{
			if (pVideoMode->GetStereoMode() != dwStereoMode)
			{
				pVideoMode = NULL;
				continue;
			}
		}
		m_pDeckLinkDevice->SetVideoMode(pVideoMode);
		m_pVideoPin->UpdateFormat();
		m_pVideoPinPreview->UpdateFormat();
		pVideoMode = NULL;

		break;
	}
	
	return S_OK;
}

HRESULT CDeckLinkSource::Save( 
									   /* [in] */ __RPC__in_opt IPropertyBag *pPropBag,
									   /* [in] */ BOOL /*fClearDirty*/,
									   /* [in] */ BOOL /*fSaveAllProperties*/)
{	
	CheckPointer(pPropBag, E_POINTER);

	CAutoLock al(&m_csDevice);

	VARIANT var;
	var.vt = VT_BSTR;
	var.bstrVal = CComBSTR(m_sDeviceName);
	HRESULT hr = pPropBag->Write(DEVICENAME, &var);
	
	if (!m_pDeckLinkDevice)
	{
		return S_OK;
	}
	CComPtr<IVideoDeviceMode> pVideoMode;
	m_pDeckLinkDevice->GetCurrentVideoMode(&pVideoMode);
	if (!pVideoMode)
	{
		return S_OK;
	}
	var.vt = VT_I4;
	var.lVal = pVideoMode->GetWidth();
	hr = pPropBag->Write(SWIDTH, &var);

	var.vt = VT_I4;
	var.lVal = pVideoMode->GetHeight();
	hr = pPropBag->Write(SHEIGHT, &var);
	
	var.vt = VT_I4;
	var.lVal = pVideoMode->GetBpp();
	hr = pPropBag->Write(BPP, &var);

	var.vt = VT_I4;
	var.lVal = pVideoMode->GetPixelFormat();
	hr = pPropBag->Write(PIXELFORMAT, &var);

	var.vt = VT_I4;
	var.lVal = 1000000 * pVideoMode->GetFramesPerSecond();
	hr = pPropBag->Write(FRAMERATE, &var);

	var.vt = VT_I4;
	var.lVal = pVideoMode->GetFieldOrder();
	hr = pPropBag->Write(FIELDORDER, &var);

	var.vt = VT_I4;
	var.lVal = pVideoMode->GetStereoMode();
	hr = pPropBag->Write(STEREOMODE, &var);

	return S_OK;
}

HRESULT CDeckLinkSource::GetClassID( 
	/* [out] */ CLSID *pClassID) 
{
	CheckPointer(pClassID, E_POINTER);
	*pClassID = CLSID_DeckLinkHDStereoSource;
	return S_OK;
}

HRESULT CDeckLinkSource::UpdateDevice()
{
	FILTER_STATE state;
	HRESULT hr = GetState(1000, &state);
	if (FAILED(hr) || State_Stopped != state)
	{
		return E_FAIL;
	}

	CComPtr<IDeckLink> pDeckLink;
	int n = 0;
	hr = E_FAIL;

	CComPtr<IDeckLinkIterator> deckLinkIterator = NULL;
	if (CoCreateInstance(CLSID_CDeckLinkIterator, NULL, CLSCTX_ALL, IID_IDeckLinkIterator, (void**)&deckLinkIterator) != S_OK || 
		deckLinkIterator == NULL) 
	{
		return E_FAIL;
	}

	m_pDeckLinkDevice = nullptr;
	while (deckLinkIterator->Next(&pDeckLink) == S_OK) {
		CComBSTR cardNameBSTR;			
		hr = pDeckLink->GetDisplayName(&cardNameBSTR);

		CString sDeviceName = cardNameBSTR;
		if (m_sDeviceName == sDeviceName)
		{
			CAutoLock al(&m_csDevice);
			m_pDeckLinkDevice = new CDeckLinkVideoDevice(NULL, pDeckLink);
			BMDDisplayMode displayMode;
			CComQIPtr<IDeckLinkStatus> status = pDeckLink;
			LONGLONG value = 0;
			if SUCCEEDED(status->GetInt(bmdDeckLinkStatusDetectedVideoInputMode, &value))
			{
				displayMode = (BMDDisplayMode)value;

			}
			return S_OK;
		}
		pDeckLink = NULL;
	}	

	CAutoLock al(&m_csDevice);
	m_pDeckLinkDevice = NULL;	

	return E_FAIL;
}

HRESULT CDeckLinkSource::IsPageDirty( void) 
{
	return S_OK;
}

HRESULT CDeckLinkSource::Save( void)
{
	HRESULT hr = Save(m_pPropertyBag, FALSE, FALSE);	
	if (m_pVideoPin)
	{
		m_pVideoPin->UpdateFormat();
	}
	if (m_pVideoPinPreview)
	{
		m_pVideoPinPreview->UpdateFormat();
	}
	return S_OK;
}

STDMETHODIMP CDeckLinkSource::Stop() 
{
	if (m_pDeckLinkDevice) m_pDeckLinkDevice->Stop();
	return __super::Stop();
}

STDMETHODIMP CDeckLinkSource::Pause()
{
	return __super::Pause();
}

STDMETHODIMP CDeckLinkSource::Run(REFERENCE_TIME tStart)
{
	FILTER_STATE state;
	GetState(0, &state);
	if (state != State_Running)
	{
		if (!m_pDeckLinkDevice) return E_POINTER;
		m_pDeckLinkDevice->Start(this, this);
	}
	return __super::Run(tStart);
}

STDMETHODIMP CDeckLinkSource::GetState(DWORD dwMSecs, FILTER_STATE *State)
{
	HRESULT hr = __super::GetState(dwMSecs, State);
	if (SUCCEEDED(hr) && State && *State == State_Paused)
	{
		return VFW_S_CANT_CUE;
	}
	return hr;
}

HRESULT CDeckLinkSource::VideoInputFormatChanged( 
	/* [in] */ BMDVideoInputFormatChangedEvents notificationEvents,
	/* [in] */ IDeckLinkDisplayMode *newDisplayMode,
	/* [in] */ BMDDetectedVideoInputFormatFlags detectedSignalFlags)
{
	//std::cout << "asdasd" << detectedSignalFlags;
	newDisplayMode = nullptr;
	newDisplayMode->GetDisplayMode();
	return S_FALSE;
}

HRESULT CDeckLinkSource::VideoInputFrameArrived( 
	/* [in] */ IDeckLinkVideoInputFrame *videoFrame,
	/* [in] */ IDeckLinkAudioInputPacket *audioPacket)
{
	/*FILTER_STATE state;
	GetState(0, &state);
	if (state != State_Running)
	{
		return S_FALSE;
	}
	*/
	//P7_CRITICAL(TEXT("VideoInputFrameArrived"), (audioPacket != nullptr), (videoFrame != nullptr));
#if 0 //logging
	{
		BMDTimeValue frameTime;
		BMDTimeValue frameDuration;
		BMDTimeScale timeScale = UNITS;
		if (videoFrame) 
		{
			videoFrame->GetStreamTime(&frameTime, &frameDuration, timeScale);
		}
		else if (audioPacket)
		{
			audioPacket->GetPacketTime(&frameTime, timeScale);
		}

		P7_CRITICAL(TEXT("FrameArrived Time=%I64d audio=%d video=%d"), frameTime / 10000, (audioPacket != nullptr), (videoFrame != nullptr));
	}
#endif 

	bool canWait = false;
	SUCCEEDED(m_pDeckLinkDevice->ValidateBufferFullness());
	if (m_pVideoPin && m_pVideoPin->IsConnected())
	{
		m_pVideoPin->ProcessFrame(videoFrame, canWait);
	}
	//P7_CRITICAL(TEXT("Frame processed 1"));
	/*if (m_pVideoPinPreview && m_pVideoPinPreview->IsConnected())
	{
		if (canWait)
		{
			m_pVideoPinPreview->ProcessFrame(videoFrame, false);
		}
	}*/
	//P7_CRITICAL(TEXT("Frame processed 2"));
	if (m_pAudioPin && m_pAudioPin->IsConnected())
	{
		m_pAudioPin->ProcessFrame(audioPacket);
	}
	//P7_CRITICAL(TEXT("Frame processed"));

	
	return S_OK;
}

HRESULT CDeckLinkSource::DrawFrame( 
		/* [in] */ IDeckLinkVideoFrame *theFrame)
{
	//P7_CRITICAL(TEXT("DrawFrame"));
	static int cnt = 0;
	cnt++;
	//if (cnt % 5) return S_FALSE;
	if (!theFrame)
		return S_FALSE;
	CComQIPtr<IDeckLinkVideoInputFrame> inputFrame = theFrame;
	if (inputFrame && m_pVideoPinPreview && m_pVideoPinPreview->IsConnected())
	{
		//Sleep(10);
		m_pVideoPinPreview->ProcessFrame(inputFrame, false);
	}
	//P7_CRITICAL(TEXT("DrawFrame Processed"));
	return S_OK;
}


HRESULT CDeckLinkSource::LockDevice( 
	IVideoDevice **ppDevide)
{
	CheckPointer(ppDevide, E_POINTER);
	m_csDevice.Lock();
	if (!m_pDeckLinkDevice)
	{
		m_csDevice.Unlock();
		return E_FAIL;
	}

	*ppDevide = m_pDeckLinkDevice;
	if (*ppDevide)
	{
		(*ppDevide)->AddRef();
	}
	return S_OK;
}

HRESULT CDeckLinkSource::UnLockDevice(void)
{
	m_csDevice.Unlock();

	return S_OK;
}

HRESULT CDeckLinkSource::SetDeviceName( 
		/* [in] */ BSTR name)
{
	CComBSTR bstrName = name;
	m_sDeviceName = bstrName; 

	CAutoLock al(&m_csDevice);
	if FAILED(UpdateDevice())
	{
		return S_FALSE;
	}

	if (!m_pDeckLinkDevice)
	{
		return E_POINTER;
	}
	return S_OK;
}
        
HRESULT CDeckLinkSource::SetVideoMode( 
        /* [in] */ int width,
        /* [in] */ int height,
		/* [in] */ long long avgFrameDuration,
		/* [in] */ int fieldOrder)
{
	CComPtr<IVideoDeviceModeIterator> pModesIterator;
	m_pDeckLinkDevice->GetModesIterator(&pModesIterator);
	CComPtr<IVideoDeviceMode> pVideoMode;
	
	EFieldOrder _fieldOrder;
	switch (fieldOrder)
	{
	case bmdUnknownFieldDominance:
		{
			_fieldOrder = foDefault;
			break;
		}
	case bmdLowerFieldFirst:
		{
			_fieldOrder = foLowFieldFirst;
			break;
		}
	case bmdUpperFieldFirst:
		{
			_fieldOrder = foUpFieldFirst;
			break;
		}
	case bmdProgressiveFrame:
		{
			_fieldOrder = foProgressive;
			break;
		}
	case bmdProgressiveSegmentedFrame:
		{
			_fieldOrder = foProgressiveSegmented;
			break;
		}		
	}
	while (SUCCEEDED(pModesIterator->Next(&pVideoMode)) && pVideoMode)
	{
		if (pVideoMode->GetWidth() != width)
		{
			pVideoMode = NULL;
			continue;
		}
		if (pVideoMode->GetHeight() != height)
		{
			pVideoMode = NULL;
			continue;
		}
		
		if (llabs(pVideoMode->GetFrameDuration() - avgFrameDuration) > 1)
		{
			pVideoMode = NULL;
			continue;
		}
		if (pVideoMode->GetFieldOrder() != _fieldOrder)
		{
			pVideoMode = NULL;
			continue;
		}
		if (pVideoMode->GetPixelFormat() != pfUYVY)
		{
			pVideoMode = nullptr;
			continue;
		}
		m_pDeckLinkDevice->SetVideoMode(pVideoMode);
		m_pVideoPin->UpdateFormat();
		m_pVideoPinPreview->UpdateFormat();
		pVideoMode = NULL;

		break;
	}
	return S_OK;
}


HRESULT CDeckLinkSource::GetNumDropped( 
    /* [annotation][out] */ 
	_Out_  long *plDropped)
{
	CheckPointer(m_pVideoPin, E_POINTER);
	if (plDropped) *plDropped = m_pVideoPin->getDrops();
	return S_OK;
}
        
HRESULT CDeckLinkSource::GetNumNotDropped( 
    /* [annotation][out] */ 
    _Out_  long *plNotDropped)
{
	return E_NOTIMPL;
}
        
HRESULT CDeckLinkSource::GetDroppedInfo( 
    /* [in] */ long lSize,
    /* [annotation][out] */ 
    _Out_  long *plArray,
    /* [annotation][out] */ 
    _Out_  long *plNumCopied)
{
	return E_NOTIMPL;
}
        
HRESULT CDeckLinkSource::GetAverageFrameSize( 
    /* [annotation][out] */ 
    _Out_  long *plAverageSize)
{
	return E_NOTIMPL;
}