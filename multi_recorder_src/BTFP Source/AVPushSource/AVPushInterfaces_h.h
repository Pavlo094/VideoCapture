#ifndef DECKLINK_INTERFACES_H
#define DECKLINK_INTERFACES_H

// {7251FAD4-45CD-4604-A03B-390147894C39}
static const GUID CLSID_CustomMemAllocator = 
{ 0x7251fad4, 0x45cd, 0x4604, { 0xa0, 0x3b, 0x39, 0x1, 0x47, 0x89, 0x4c, 0x39 } };

// {15148D35-9942-4BD2-B9BA-30A4C1204071}
static const GUID CLSID_DecklinkPushSource = 
{ 0x15148d35, 0x9942, 0x4bd2, { 0xb9, 0xba, 0x30, 0xa4, 0xc1, 0x20, 0x40, 0x71 } };

// {7FB052B4-E14E-43DA-B8BD-B9004CAFEE87}
static const GUID CLSID_DecklinkVideoSource = 
{ 0x7fb052b4, 0xe14e, 0x43da, { 0xb8, 0xbd, 0xb9, 0x0, 0x4c, 0xaf, 0xee, 0x87 } };

// {521BD957-D0E4-4BF3-A7EE-1D5DBA376428}
static const GUID CLSID_DecklinkAudioSource = 
{ 0x521bd957, 0xd0e4, 0x4bf3, { 0xa7, 0xee, 0x1d, 0x5d, 0xba, 0x37, 0x64, 0x28 } };



// {A3E69173-A6AF-448D-8770-9FB6A8143EE8}
DEFINE_GUID(IID_IAVPushSource, 
0xa3e69173, 0xa6af, 0x448d, 0x87, 0x70, 0x9f, 0xb6, 0xa8, 0x14, 0x3e, 0xe8);

// {E5A7DBA7-B09F-4564-AC2A-E0975232091A}
DEFINE_GUID(IID_IAVPushSource2, 
0xe5a7dba7, 0xb09f, 0x4564, 0xac, 0x2a, 0xe0, 0x97, 0x52, 0x32, 0x9, 0x1a);

// {B1B6A2BF-577F-4A7C-A19F-B08A93ACE343}
DEFINE_GUID(IID_IAVPushSource3, 
0xb1b6a2bf, 0x577f, 0x4a7c, 0xa1, 0x9f, 0xb0, 0x8a, 0x93, 0xac, 0xe3, 0x43);


// {43594448-0000-0010-8000-00AA00389B71}
DEFINE_GUID(IID_MEDIASUBTYPE_HDYC,
0x43594448, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xAA, 0x00, 0x38, 0x9B, 0x71);

// {30313272-0000-0010-8000-00AA00389B71}
DEFINE_GUID(IID_MEDIASUBTYPE_r210,
0x30313272, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xAA, 0x00, 0x38, 0x9B, 0x71);

// {30313276-0000-0010-8000-00AA00389B71}
DEFINE_GUID(IID_MEDIASUBTYPE_v210a,
0x30313276, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xAA, 0x00, 0x38, 0x9B, 0x71);


#ifdef __cplusplus
extern "C"{
#endif


// define IAVPushSource interfaces
MIDL_INTERFACE("A3E69173-A6AF-448D-8770-9FB6A8143EE8")
IAVPushSource : public IUnknown
{
public:
	STDMETHOD(GetFrameBuffer)(unsigned char** ppBuffer, unsigned long* pSize) PURE;
	STDMETHOD(Deliver)(unsigned char* pBuffer) PURE;
};


MIDL_INTERFACE("E5A7DBA7-B09F-4564-AC2A-E0975232091A")
IAVPushSource2 : public IUnknown
{
public:
	STDMETHOD(GetFrameBuffer)(IMediaSample** ppSample) PURE;
	STDMETHOD(Deliver)( IMediaSample* pSample) PURE;
};


MIDL_INTERFACE("B1B6A2BF-577F-4A7C-A19F-B08A93ACE343")
IAVPushSource3 : public IUnknown
{
public:
	STDMETHOD(GetFrameBufferEx)(unsigned char** ppBuffer, unsigned long* pSize) PURE;
	STDMETHOD(DeliverEx)(unsigned char* pBuffer, unsigned long cbSize) PURE;
};


#ifdef __cplusplus
}
#endif

#endif//DECKLINK_INTERFACES_H