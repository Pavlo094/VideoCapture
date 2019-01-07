#include "stdafx.h"

#include "DeckLinkVideoOutputPin.h"
#include "DecklLinkSource.h"

CDeckLinkAudioOutputPin::CDeckLinkAudioOutputPin(CDeckLinkSource *pFilter, HRESULT * phr )
	: CBaseOutputPin(TEXT(""), pFilter, &pFilter->m_cStateLock, phr, L"Audio Output")
	, m_pSource(pFilter)
	, drops(0)
{
	m_pKeyProperty = new KsPropertySetImpl(this);
	UpdateFormat();
}

CDeckLinkAudioOutputPin::~CDeckLinkAudioOutputPin()
{
}

STDMETHODIMP CDeckLinkAudioOutputPin::NonDelegatingQueryInterface(REFIID riid, void **ppv)
{
	if(IsEqualGUID(riid,IID_IKsPropertySet)) {
		return GetInterface(static_cast<IKsPropertySet*>(m_pKeyProperty), ppv);		
	}
	else 	
	{
		return CBaseOutputPin::NonDelegatingQueryInterface(riid, ppv);
	}
}

HRESULT CDeckLinkAudioOutputPin::UpdateFormat()
{
	WAVEFORMATEX waveFormat;
	ZeroMemory(&waveFormat, sizeof(waveFormat));
	waveFormat.wFormatTag = WAVE_FORMAT_PCM;
	waveFormat.cbSize = sizeof(waveFormat);
	waveFormat.nSamplesPerSec = 48000;
	waveFormat.nChannels = channelsCount;
	waveFormat.wBitsPerSample = 16;
	waveFormat.nBlockAlign = (waveFormat.nChannels * waveFormat.wBitsPerSample) / 8;
	waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec * waveFormat.nBlockAlign;
	
	CreateAudioMediaType(&waveFormat, &m_mediaType, TRUE);		
	return S_OK;
}

HRESULT CDeckLinkAudioOutputPin::CheckMediaType(const CMediaType *pMediaType)
{
	CheckPointer(pMediaType,E_POINTER);
	if (*pMediaType->Type() != MEDIATYPE_Audio) 
		return VFW_E_TYPE_NOT_ACCEPTED;
	
	if(!(pMediaType->subtype == m_mediaType.subtype))
	{
		return VFW_E_TYPE_NOT_ACCEPTED;
	}

	WAVEFORMATEX* waveFormat = nullptr;
	if (*pMediaType->FormatType() == FORMAT_WaveFormatEx) {
		waveFormat = (WAVEFORMATEX*)m_mediaType.Format(); 
	}

	WAVEFORMATEX* waveFormat2 = nullptr;
	if (*m_mediaType.FormatType() == FORMAT_WaveFormatEx) {
		waveFormat2 = (WAVEFORMATEX*)m_mediaType.Format(); 
	}

	if (waveFormat && waveFormat2 &&
		memcmp(waveFormat, waveFormat2, sizeof(*waveFormat)) != 0)
	{
		return VFW_E_TYPE_NOT_ACCEPTED;
	}

	return S_OK;
}

HRESULT CDeckLinkAudioOutputPin::GetMediaType(int iPosition, CMediaType *pMediaType)
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

HRESULT CDeckLinkAudioOutputPin::DecideBufferSize(IMemAllocator *pAllocator, ALLOCATOR_PROPERTIES *pRequest)
{
	HRESULT hr;

	CheckPointer(pAllocator, E_POINTER);
	CheckPointer(pRequest, E_POINTER);

	if (pRequest->cBuffers == 0) {
		pRequest->cBuffers = 100;		
	}

	pRequest->cbBuffer = 48000 * m_mediaType.GetSampleSize();

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

	return S_OK;
}

HRESULT CDeckLinkAudioOutputPin::ProcessFrame(IDeckLinkAudioInputPacket *audioPacket)
{	
	CheckPointer(audioPacket, E_POINTER);
	CComPtr<IMediaSample> pSample;
	HRESULT hr = GetDeliveryBuffer(&pSample, NULL, NULL,AM_GBF_NOWAIT);
	if (FAILED(hr) || !pSample)
	{
		drops++;
	}
	CheckPointer(pSample, E_POINTER);
	
	FillBuffer(pSample, audioPacket);

	Deliver(pSample);
	return S_OK;
}

HRESULT CDeckLinkAudioOutputPin::FillBuffer(IMediaSample* pSample, IDeckLinkAudioInputPacket *audioPacket)
{
	CheckPointer(audioPacket, E_POINTER);
	CheckPointer(pSample, E_POINTER);

	LONG actualDataSize = 0;
	PBYTE bufferDst;
	ULONG bufferSizeDst;
	pSample->GetPointer(&bufferDst);
	bufferSizeDst = pSample->GetSize();
	

	PBYTE bufferSrc;
	ULONG bufferSizeSrc;
	const int sampleSize = channelsCount * 2;
	audioPacket->GetBytes(reinterpret_cast<void**>(&bufferSrc));
	long long samplesCount = audioPacket->GetSampleFrameCount();
	bufferSizeSrc = samplesCount * sampleSize;
	actualDataSize = bufferSizeSrc;

	if (bufferSizeSrc > bufferSizeDst)
	{
		return E_POINTER;
	}
#if 0
	PBYTE src = bufferSrc;
	PBYTE dst = bufferDst;
	for (int sample = 0; sample < samplesCount; sample++)
	{
		int index = 0;
		dst[index++] = src[0];
		dst[index++] = src[1];
		dst[index++] = src[0];
		dst[index++] = src[1];
		dst[index++] = src[0];
		dst[index++] = src[1];
		dst[index++] = src[0];
		dst[index++] = src[1];

		dst[index++] = src[2];
		dst[index++] = src[3];
		dst[index++] = src[2];
		dst[index++] = src[3];
		dst[index++] = src[2];
		dst[index++] = src[3];
		dst[index++] = src[2];
		dst[index++] = src[3];
		src += sampleSize;
		dst += sampleSize;
	}
#else
	memcpy(bufferDst, bufferSrc, actualDataSize);
#endif
	
	pSample->SetActualDataLength(actualDataSize);
	pSample->SetDiscontinuity(FALSE);

	BMDTimeValue frameTime;
	BMDTimeScale timeScale = UNITS;
	audioPacket->GetPacketTime(&frameTime, timeScale);
	
	REFERENCE_TIME startTime = frameTime;
	REFERENCE_TIME endTime = startTime + (samplesCount * 10000000ul)/ 48000ul;
	pSample->SetTime(&startTime, &endTime);

	REFERENCE_TIME startTimeM = 48ul * frameTime / 10000ul;
	REFERENCE_TIME endTimeM = startTimeM + samplesCount;
	pSample->SetMediaTime(&startTimeM, &endTimeM);

	return S_OK;
}


STDMETHODIMP CDeckLinkAudioOutputPin::Notify(IBaseFilter * pSender, Quality q)
{
	UNREFERENCED_PARAMETER(q);
	UNREFERENCED_PARAMETER(pSender);
	return E_NOTIMPL;
} //Notify
