// warning disable
#pragma warning(disable: 4097 4511 4512 4514 4705)

#include <InitGuid.h>
#include "iAudioDownMixer.h"


class CAudioDownMixer : public CTransformFilter
{
public:
	// Declare IUnknown interface
	DECLARE_IUNKNOWN;
	// Constructor
	CAudioDownMixer(LPUNKNOWN pUnk, HRESULT *phr);
	// Destructor
	~CAudioDownMixer();
	// Create instance
	static CUnknown * WINAPI CreateInstance(LPUNKNOWN pUnk, HRESULT *phr);
public:
	// This method is called on each transform with samples.
    HRESULT Transform(IMediaSample *pIn, IMediaSample *pOut);
	// Checks if we support input type
    HRESULT CheckInputType(const CMediaType* pMediaType) ;
	// Checks if we support transformation between input and output type.
	HRESULT CheckTransform(const CMediaType *mtIn, const CMediaType *mtOut);
	// Get media type
    HRESULT GetMediaType(int iPosition, CMediaType *pMediaType);
	// Decide Buffer Size
	HRESULT DecideBufferSize(IMemAllocator *pAlloc,
                             ALLOCATOR_PROPERTIES *pProperties);
private:
	CMediaType MakeOutputCMediaTypeFromParams(const CMediaType *InputMediaType = NULL);
	HRESULT DownMix(IMediaSample *pIn, IMediaSample *pOut);

private:
	int m_Channels;
	int m_SampleRate;
	int m_BitsPerSample;
};
