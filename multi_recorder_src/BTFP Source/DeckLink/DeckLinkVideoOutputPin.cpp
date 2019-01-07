#include "stdafx.h"

#include "DeckLinkVideoOutputPin.h"
#include "DecklLinkSource.h"

#include "log.h"

CDeckLinkVideoOutputPin::CDeckLinkVideoOutputPin(CDeckLinkSource *pFilter, HRESULT * phr, bool forceYV12, GUID pinCategory)
	: CBaseOutputPin(TEXT(""), pFilter, &pFilter->m_cStateLock, phr, L"Video Output")
	, m_pSource(pFilter)
	, drops(0)
	, forceYV12(forceYV12)
	, pinCategory(pinCategory)
	, prevFrameTime(0)
{
	//m_pKeyProperty = new KsPropertySetImpl(this);
	UpdateFormat();
}

CDeckLinkVideoOutputPin::~CDeckLinkVideoOutputPin()
{
}

STDMETHODIMP CDeckLinkVideoOutputPin::NonDelegatingQueryInterface(REFIID riid, void **ppv)
{
	if(IsEqualGUID(riid,IID_IKsPropertySet)) {
		return GetInterface(static_cast<IKsPropertySet*>(this), ppv);		
	}
	else 	
	{
		return CBaseOutputPin::NonDelegatingQueryInterface(riid, ppv);
	}
}

HRESULT CDeckLinkVideoOutputPin::UpdateFormat()
{
	CheckPointer(m_pSource, E_POINTER);
	CAutoLock lock(&m_pSource->m_csDevice);
	CheckPointer(m_pSource->m_pDeckLinkDevice, E_POINTER);	
	HRESULT hr = S_OK;
	CComPtr<IVideoDeviceMode> pVideoMode = NULL;
	if (FAILED(hr = m_pSource->m_pDeckLinkDevice->GetCurrentVideoMode(&pVideoMode)) )
	{
		return hr;
	}
	if (pVideoMode == NULL)
	{
		return hr;
	}
	bool bStereo = (smSideBySide == pVideoMode->GetStereoMode());
	m_mediaType.SetSampleSize(pVideoMode->GetWidth() * pVideoMode->GetHeight() *
		pVideoMode->GetBpp() * (bStereo ? 2 : 1));
	m_mediaType.SetType(&MEDIATYPE_Video);
	switch (pVideoMode->GetPixelFormat())
	{
	case pfYUV2	:
		m_mediaType.SetSubtype(&MEDIASUBTYPE_YUY2);
		break;
	case pfYUYV	:
	case pfUYVY	:
		m_mediaType.SetSubtype(forceYV12 ? &MEDIASUBTYPE_YV12 : &MEDIASUBTYPE_UYVY);
		break;
	case pfV210	:
		m_mediaType.SetSubtype(&__uuidof(MEDIASUBTYPE_v210a));
		break;
	case pfARGB :
		m_mediaType.SetSubtype(&MEDIASUBTYPE_RGB32);
		break;
	case pfBGRA	:
		m_mediaType.SetSubtype(&__uuidof(MEDIASUBTYPE_BGRA32));
		break;
	case pfR210	:
		m_mediaType.SetSubtype(&__uuidof(MEDIASUBTYPE_r210));
		break;
	}
#ifdef FORMAT_TYPE_VIDEOINFOHEADER2
	m_mediaType.SetFormatType(&FORMAT_VideoInfo2);
	m_mediaType.AllocFormatBuffer(sizeof(VIDEOINFOHEADER2));	
	if (*m_mediaType.FormatType() == FORMAT_VideoInfo2) {
		VIDEOINFOHEADER2 *pVIH = (VIDEOINFOHEADER2*)m_mediaType.Format(); 			
		ZeroMemory(pVIH, sizeof(VIDEOINFOHEADER2));		
		pVIH->AvgTimePerFrame = pVideoMode->GetFrameDuration(); 
		if (foProgressive != pVideoMode->GetFieldOrder())
		{
			pVIH->dwInterlaceFlags = AMINTERLACE_IsInterlaced;
		}
		if (foUpFieldFirst == pVideoMode->GetFieldOrder())
		{
			pVIH->dwInterlaceFlags |= AMINTERLACE_Field1First;
		}
		pVIH->bmiHeader.biSize = sizeof(pVIH->bmiHeader);
		pVIH->bmiHeader.biWidth = pVideoMode->GetWidth() * (bStereo ? 2 : 1);
		pVIH->bmiHeader.biHeight = pVideoMode->GetHeight();
		pVIH->bmiHeader.biPlanes = 1;
		pVIH->bmiHeader.biBitCount = 12;


		switch (pVideoMode->GetPixelFormat())
		{
		case pfYUV2	:
			pVIH->bmiHeader.biCompression = '2YUY';
			break;
		case pfYUYV	:
			pVIH->bmiHeader.biCompression = 'VYUY';
			break;		
		case pfUYVY	:
			pVIH->bmiHeader.biCompression = '21VY';
			break;				
		case pfARGB :
			pVIH->bmiHeader.biCompression = BI_RGB;
			break;
		case pfV210	:
			pVIH->bmiHeader.biCompression = '012V';
		case pfBGRA	:
			pVIH->bmiHeader.biCompression = 'ARGB';
		case pfR210	:	
			pVIH->bmiHeader.biCompression = '012R';
			break;
		}

		pVIH->bmiHeader.biSizeImage = pVideoMode->GetWidth() * pVideoMode->GetHeight() * pVIH->bmiHeader.biBitCount / 8;
		pVIH->dwBitRate = pVIH->bmiHeader.biSizeImage * 8 * (int)pVideoMode->GetFramesPerSecond();
	}
#else
	m_mediaType.SetFormatType(&FORMAT_VideoInfo);
	m_mediaType.AllocFormatBuffer(sizeof(VIDEOINFOHEADER));
	if (*m_mediaType.FormatType() == FORMAT_VideoInfo) {
		VIDEOINFOHEADER *pVIH = (VIDEOINFOHEADER*)m_mediaType.Format();
		ZeroMemory(pVIH, sizeof(VIDEOINFOHEADER));		
		pVIH->AvgTimePerFrame = pVideoMode->GetFrameDuration();
		
		pVIH->bmiHeader.biSize = sizeof(pVIH->bmiHeader);
		pVIH->bmiHeader.biWidth = pVideoMode->GetWidth() * (bStereo ? 2 : 1);
		pVIH->bmiHeader.biHeight = pVideoMode->GetHeight();
		pVIH->bmiHeader.biPlanes = 1;
		pVIH->bmiHeader.biBitCount = forceYV12 ? 12 : 16;


		switch (pVideoMode->GetPixelFormat())
		{
		case pfYUV2:
			pVIH->bmiHeader.biCompression = '2YUY';
			break;
		case pfYUYV:
			pVIH->bmiHeader.biCompression = 'VYUY';
			break;
		case pfUYVY:
			pVIH->bmiHeader.biCompression = forceYV12 ? '21VY' : 'YVYU';
			break;
		case pfARGB:
			pVIH->bmiHeader.biCompression = BI_RGB;
			break;
		case pfV210:
			pVIH->bmiHeader.biCompression = '012V';
		case pfBGRA:
			pVIH->bmiHeader.biCompression = 'ARGB';
		case pfR210:
			pVIH->bmiHeader.biCompression = '012R';
			break;
		}

		pVIH->bmiHeader.biSizeImage = pVideoMode->GetWidth() * pVideoMode->GetHeight() * pVIH->bmiHeader.biBitCount / 8;
		pVIH->dwBitRate = pVIH->bmiHeader.biSizeImage * 8 * (int)pVideoMode->GetFramesPerSecond();
	}
#endif
	return S_OK;
}

HRESULT CDeckLinkVideoOutputPin::CheckMediaType(const CMediaType *pMediaType)
{
	CheckPointer(pMediaType,E_POINTER);
	if (*pMediaType->Type() != MEDIATYPE_Video) 
		return VFW_E_TYPE_NOT_ACCEPTED;
	
	if(!(pMediaType->subtype == m_mediaType.subtype))
	{
		return VFW_E_TYPE_NOT_ACCEPTED;
	}


	BITMAPINFOHEADER *pBitmapInfo = NULL;
	if (*pMediaType->FormatType() == FORMAT_VideoInfo) {
		VIDEOINFOHEADER *pVIH = (VIDEOINFOHEADER*)m_mediaType.Format(); 
		pBitmapInfo = &pVIH->bmiHeader;
	}

	BITMAPINFOHEADER *pBitmapInfo2 = NULL;
	if (*m_mediaType.FormatType() == FORMAT_VideoInfo) {
		VIDEOINFOHEADER *pVIH = (VIDEOINFOHEADER*)m_mediaType.Format(); 
		pBitmapInfo2 = &pVIH->bmiHeader;
	}

	if (pBitmapInfo && pBitmapInfo2 &&
		(
		pBitmapInfo->biWidth != pBitmapInfo->biWidth ||
		pBitmapInfo->biHeight != pBitmapInfo->biHeight ||
		pBitmapInfo->biSizeImage != pBitmapInfo->biSizeImage))
	{
		return VFW_E_TYPE_NOT_ACCEPTED;
	}

	return S_OK;
}

HRESULT CDeckLinkVideoOutputPin::GetMediaType(int iPosition, CMediaType *pMediaType)
{

	CheckPointer(pMediaType,E_POINTER);

	if (iPosition < 0) 
	{
		return E_INVALIDARG;
	}

	// Have we run off the end of types

	if( iPosition > 0 ) 
	{
		return VFW_S_NO_MORE_ITEMS;
	}

	*pMediaType = m_mediaType;	
	return S_OK;
}

HRESULT CDeckLinkVideoOutputPin::DecideBufferSize(IMemAllocator *pAllocator, ALLOCATOR_PROPERTIES *pRequest)
{
	HRESULT hr;

	CheckPointer(pAllocator, E_POINTER);
	CheckPointer(pRequest, E_POINTER);

	//P7_CRITICAL(TEXT("BufferSize request cBuffers=%d cbBuffer=%d"), pRequest->cBuffers, pRequest->cbBuffer);

	if (pRequest->cBuffers == 0) {
		if (forceYV12)
		{
			pRequest->cBuffers = 20;
		}
		else
		{
			pRequest->cBuffers = 5;
		}
	}

	pRequest->cbBuffer = m_mediaType.GetSampleSize();

	ALLOCATOR_PROPERTIES Actual;
	hr = pAllocator->SetProperties(pRequest, &Actual);

	if (FAILED(hr)) 
	{
		return hr;
	}

	if (Actual.cbBuffer < pRequest->cbBuffer) 
	{
		return E_FAIL;
	}


	/*std::vector<CComPtr<IMediaSample>> buffers;
	buffers.reserve(50);
	for (int i = 0; i < 50; i++)
	{
		CComPtr<IMediaSample> buffer;
		pAllocator->GetBuffer(&buffer, nullptr, nullptr, 0);
		buffers.push_back(buffer);
	}*/

	return S_OK;
}
//
// Active
//
// This is called when we start running or go paused. We create the
// output queue object to send data to our associated peer pin
//
HRESULT CDeckLinkVideoOutputPin::Active()
{

	CAutoLock lock_it(m_pLock);
	HRESULT hr = NOERROR;

	// Make sure that the pin is connected
	if(m_Connected == NULL)
		return NOERROR;

	// Create the output queue if we have to
	if(m_pOutputQueue == NULL)
	{
		m_pOutputQueue.reset(new COutputQueue(m_Connected, &hr, TRUE, FALSE));
		if(m_pOutputQueue == NULL)
			return E_OUTOFMEMORY;

		// Make sure that the constructor did not return any error
		if(FAILED(hr))
		{
			m_pOutputQueue.reset(nullptr);
			return hr;
		}
	}

	// Pass the call on to the base class
	CBaseOutputPin::Active();


	return NOERROR;

} // Active


//
// Inactive
//
// This is called when we stop streaming
// We delete the output queue at this time
//
HRESULT CDeckLinkVideoOutputPin::Inactive()
{
	CAutoLock lock_it(m_pLock);

	// Delete the output queues associated with the pin.
	if(m_pOutputQueue)
	{
		m_pOutputQueue.reset(nullptr);
	}

	CBaseOutputPin::Inactive();

	return NOERROR;

} // Inactive
// Deliver
//
HRESULT CDeckLinkVideoOutputPin::Deliver(IMediaSample *pMediaSample)
{
	CheckPointer(pMediaSample,E_POINTER);	

	CAutoLock lock_it(m_pLock);
	// Make sure that we have an output queue
	if(m_pOutputQueue == NULL)
		return NOERROR;

	LONGLONG rtStart, rtStop;
	pMediaSample->GetTime(&rtStart, &rtStop);

	pMediaSample->AddRef();
	HRESULT hr = m_pOutputQueue->Receive(pMediaSample);


	return hr;
} // Deliver

HRESULT CDeckLinkVideoOutputPin::ProcessFrame(IDeckLinkVideoInputFrame *videoFrame, bool canWait)
{	
	CheckPointer(videoFrame, E_POINTER);
	CComPtr<IMediaSample> pSample;
	HRESULT hr = GetDeliveryBuffer(&pSample, NULL, NULL, canWait ? 0 : AM_GBF_NOWAIT);
	if (FAILED(hr) || !pSample)
	{		
		drops++;
		if (forceYV12)
		{
			BMDTimeValue frameTime;
			BMDTimeValue frameDuration;
			BMDTimeScale timeScale = UNITS;
			videoFrame->GetStreamTime(&frameTime, &frameDuration, timeScale);
			//P7_CRITICAL(TEXT("Drop video frame Time=%I64d count=%I64d No free output buffer"), frameTime / 10000, drops);
		}
	}
	BMDTimeValue frameTime;
	BMDTimeValue frameDuration;
	BMDTimeScale timeScale = UNITS;
	videoFrame->GetStreamTime(&frameTime, &frameDuration, timeScale);
	if (llabs(prevFrameTime - frameTime) > frameDuration * 0.8)
	{
		drops += (llabs(prevFrameTime - frameTime) + frameDuration / 2) / frameDuration;
		if (forceYV12)
		{
			//P7_CRITICAL(TEXT("Drop video frame Time=%I64d count=%I64d"), frameTime / 10000, drops);
		}
	}
	prevFrameTime = frameTime + frameDuration;
	CheckPointer(pSample, E_POINTER);
	
	FillBuffer(pSample, videoFrame);

	Deliver(pSample);
	return S_OK;
}

HRESULT CDeckLinkVideoOutputPin::FillBuffer(IMediaSample* pSample, IDeckLinkVideoInputFrame *videoFrame)
{
	CheckPointer(videoFrame, E_POINTER);
	CheckPointer(pSample, E_POINTER);

	LONG actualDataSize = 0;
	PBYTE bufferDst;
	ULONG bufferSizeDst;
	ULONG strideDst = 0;
	pSample->GetPointer(&bufferDst);
	bufferSizeDst = pSample->GetSize();
	LONG width = getWidth();
	LONG height = getHeight();
	if (height == 0)
	{
		return E_FAIL;
	}
	if (forceYV12)
	{
		strideDst = width;
	}
	else
	{
		strideDst = bufferSizeDst / height;
	}

	PBYTE bufferSrc;
	ULONG bufferSizeSrc;
	ULONG strideSrc;
	videoFrame->GetBytes(reinterpret_cast<void**>(&bufferSrc));	
	strideSrc = videoFrame->GetRowBytes();
	bufferSizeSrc = strideSrc * videoFrame->GetHeight();

	if (forceYV12)
	{
		LONG heightSrc = videoFrame->GetHeight();
		LONG heightDst = getHeight();
		LONG widthSrc = videoFrame->GetWidth();
		LONG widthDst = getWidth();
		LONG height = __min(heightSrc, heightDst);
		LONG stride = __min(strideSrc, strideDst);
		LONG width = __min(widthSrc, widthDst);
		
		PBYTE scanLineSrc = bufferSrc;
		PBYTE scanLineDst = bufferDst;
		for (int line = 0; line < height; line++)
		{
			PBYTE srcPtr = scanLineSrc + 1;
			PBYTE dstPtr = scanLineDst;
			for (int x = 0; x < width; x++)
			{
				*dstPtr = *srcPtr;
				dstPtr++;
				srcPtr += 2;
			}				
			actualDataSize += strideDst;
			scanLineSrc += strideSrc;
			scanLineDst += strideDst;
		}
		strideDst /= 2;
		for (int i = 1; i >= 0; i--)
		{
			scanLineSrc = bufferSrc;
			for (int line = 0; line < height / 2; line++)
			{
				PBYTE srcPtr = scanLineSrc + (i * 2);
				PBYTE dstPtr = scanLineDst;
				for (int x = 0; x < width; x++)
				{
					*dstPtr = *srcPtr;
					dstPtr++;
					srcPtr += 4;
				}				
				actualDataSize += strideDst;
				scanLineSrc += strideSrc * 2;
				scanLineDst += strideDst;
			}
		}
	}
	else
	{
		if (strideDst == strideSrc && bufferSizeDst >= bufferSizeSrc && videoFrame->GetPixelFormat() != bmdFormat8BitARGB)
		{
			memcpy(bufferDst, bufferSrc, bufferSizeSrc);
			actualDataSize = bufferSizeSrc;
		}
		else
		{
			LONG heightSrc = videoFrame->GetHeight();
			LONG heightDst = getHeight();
			LONG widthSrc = videoFrame->GetWidth();
			LONG widthDst = getWidth();
			LONG height = __min(heightSrc, heightDst);
			LONG stride = __min(strideSrc, strideDst);
			LONG width = __min(widthSrc, widthDst);
			PBYTE scanLineSrc = bufferSrc;
			PBYTE scanLineDst = bufferDst;
			if (bmdFormat8BitARGB != videoFrame->GetPixelFormat())
			{
				for (int line = 0; line < height; line++)
				{
					memcpy(scanLineDst, scanLineSrc, stride);
					actualDataSize += strideDst;
					scanLineSrc += strideSrc;
					scanLineDst += strideDst;
				}
			}
			else
			{
				scanLineSrc += (height - 1)* strideSrc;
				for (int line = 0; line < height; line++)
				{
					ULONG* src = (ULONG*)scanLineSrc;
					ULONG* dst = (ULONG*)scanLineDst;
					for (int x = 0; x < width; x++)
					{
						*dst = _byteswap_ulong(*src);
						dst++;
						src++;					
					}				
					actualDataSize += strideDst;
					scanLineSrc -= strideSrc;
					scanLineDst += strideDst;
				}
			}
		}
	}

	pSample->SetActualDataLength(actualDataSize);
	pSample->SetDiscontinuity(FALSE);

	BMDTimeValue frameTime;
	BMDTimeValue frameDuration;
	BMDTimeScale timeScale = UNITS;
	videoFrame->GetStreamTime(&frameTime, &frameDuration, timeScale);

	REFERENCE_TIME startTime = frameTime;
	REFERENCE_TIME endTime = startTime + frameDuration;
	pSample->SetTime(&startTime, &endTime);
	REFERENCE_TIME startTimeME = frameTime / frameDuration;
	REFERENCE_TIME endTimeME = startTime + 1;
	//pSample->SetMediaTime(&startTimeME, &endTimeME);

	CComQIPtr<IMediaSample2> pOutSample2 = pSample;
	
	if (pOutSample2)
	{
		AM_SAMPLE2_PROPERTIES propOutSample;

		if (pOutSample2->GetProperties(sizeof(AM_SAMPLE2_PROPERTIES), (BYTE *)&propOutSample) == S_OK)
		{
			BITMAPINFOHEADER *pBitmapInfo2 = NULL;
			if (*m_mediaType.FormatType() == FORMAT_VideoInfo2) {
				VIDEOINFOHEADER2 *pVIH = (VIDEOINFOHEADER2*)m_mediaType.Format(); 
				if (pVIH->dwInterlaceFlags &AMINTERLACE_IsInterlaced)
				{
					propOutSample.dwTypeSpecificFlags |= AM_VIDEO_FLAG_FIELD1 | AM_VIDEO_FLAG_FIELD2;
					if (pVIH->dwInterlaceFlags & AMINTERLACE_Field1First)
					{
						propOutSample.dwTypeSpecificFlags |= AM_VIDEO_FLAG_FIELD1FIRST;
					}
				}
			}
			pOutSample2->SetProperties(sizeof(AM_SAMPLE2_PROPERTIES), (BYTE*)&propOutSample);
		}
	}
	return S_OK;
}

LONG CDeckLinkVideoOutputPin::getWidth()
{
	BITMAPINFOHEADER *pBitmapInfo = NULL;
	if (*m_mediaType.FormatType() == FORMAT_VideoInfo) {
		VIDEOINFOHEADER *pVIH = (VIDEOINFOHEADER*)m_mediaType.Format(); 
		pBitmapInfo = &pVIH->bmiHeader;
	}
	if (*m_mediaType.FormatType() == FORMAT_VideoInfo2) {
		VIDEOINFOHEADER2 *pVIH = (VIDEOINFOHEADER2*)m_mediaType.Format(); 
		pBitmapInfo = &pVIH->bmiHeader;
	}

	CheckPointer(pBitmapInfo, 0);
	return pBitmapInfo->biWidth;
}

LONG CDeckLinkVideoOutputPin::getHeight()
{
	BITMAPINFOHEADER *pBitmapInfo = NULL;
	if (*m_mediaType.FormatType() == FORMAT_VideoInfo) {
		VIDEOINFOHEADER *pVIH = (VIDEOINFOHEADER*)m_mediaType.Format(); 
		pBitmapInfo = &pVIH->bmiHeader;
	}
	if (*m_mediaType.FormatType() == FORMAT_VideoInfo2) {
		VIDEOINFOHEADER2 *pVIH = (VIDEOINFOHEADER2*)m_mediaType.Format(); 
		pBitmapInfo = &pVIH->bmiHeader;
	}

	CheckPointer(pBitmapInfo, 0);
	return pBitmapInfo->biHeight;
}

STDMETHODIMP CDeckLinkVideoOutputPin::Notify(IBaseFilter * pSender, Quality q)
{
	UNREFERENCED_PARAMETER(q);
	UNREFERENCED_PARAMETER(pSender);
	return E_NOTIMPL;
} //Notify


long long CDeckLinkVideoOutputPin::getDrops() const
{
	return drops;
}

// Set: Cannot set any properties.
HRESULT CDeckLinkVideoOutputPin::Set(REFGUID guidPropSet, DWORD dwID,
    void *pInstanceData, DWORD cbInstanceData, void *pPropData, 
    DWORD cbPropData)
{
    return E_NOTIMPL;
}

// Get: Return the pin category (our only property). 
HRESULT CDeckLinkVideoOutputPin::Get(
    REFGUID guidPropSet,   // Which property set.
    DWORD dwPropID,        // Which property in that set.
    void *pInstanceData,   // Instance data (ignore).
    DWORD cbInstanceData,  // Size of the instance data (ignore).
    void *pPropData,       // Buffer to receive the property data.
    DWORD cbPropData,      // Size of the buffer.
    DWORD *pcbReturned     // Return the size of the property.
)
{
    if (guidPropSet != AMPROPSETID_Pin) 
        return E_PROP_SET_UNSUPPORTED;
    if (dwPropID != AMPROPERTY_PIN_CATEGORY)
        return E_PROP_ID_UNSUPPORTED;
    if (pPropData == NULL && pcbReturned == NULL)
        return E_POINTER;
    if (pcbReturned)
        *pcbReturned = sizeof(GUID);
    if (pPropData == NULL)  // Caller just wants to know the size.
        return S_OK;
    if (cbPropData < sizeof(GUID)) // The buffer is too small.
        return E_UNEXPECTED;
	*(GUID *)pPropData = pinCategory;
    return S_OK;
}

// QuerySupported: Query whether the pin supports the specified property.
HRESULT CDeckLinkVideoOutputPin::QuerySupported(REFGUID guidPropSet, DWORD dwPropID,
    DWORD *pTypeSupport)
{
    if (guidPropSet != AMPROPSETID_Pin)
        return E_PROP_SET_UNSUPPORTED;
    if (dwPropID != AMPROPERTY_PIN_CATEGORY)
        return E_PROP_ID_UNSUPPORTED;
    if (pTypeSupport)
        // We support getting this property, but not setting it.
        *pTypeSupport = KSPROPERTY_SUPPORT_GET; 
    return S_OK;
}
