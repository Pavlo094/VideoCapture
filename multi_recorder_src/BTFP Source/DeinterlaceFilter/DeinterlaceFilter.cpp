#include <streams.h>
#include "DeinterlaceFilter.h"
// VideoInfoHeader2 support
#include <dvdmedia.h>

#define MIDL_DEFINE_GUID(type,name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8) \
        const type name = {l,w1,w2,{b1,b2,b3,b4,b5,b6,b7,b8}}

MIDL_DEFINE_GUID(IID, IID_MEDIASUBTYPE_HDYC,0x43594448,0x0000,0x0010,0x80,0x00,0x00,0xAA,0x00,0x38,0x9B,0x71);
MIDL_DEFINE_GUID(IID, IID_MEDIASUBTYPE_v210a,0x30313276,0x0000,0x0010,0x80,0x00,0x00,0xAA,0x00,0x38,0x9B,0x71);
MIDL_DEFINE_GUID(IID, IID_MEDIASUBTYPE_r210,0x30313272,0x0000,0x0010,0x80,0x00,0x00,0xAA,0x00,0x38,0x9B,0x71);

// AMOVIESETUP_FILTER
const AMOVIESETUP_FILTER sudDeinterlaceFilter =
{
    &CLSID_DeinterlaceFilter,		  // GUID of the filter.
    L"Deinterlace Filter",			  // Name of the filter
    MERIT_DO_NOT_USE,         // dwMerit
    0,                        // nPins
    0                         // lpPin
};

// Global data
CFactoryTemplate g_Templates[]= {
	{L"Deinterlace Filter", &CLSID_DeinterlaceFilter, CDeinterlaceFilter::CreateInstance, NULL, &sudDeinterlaceFilter},
};

// g_cTemplates
int g_cTemplates = sizeof(g_Templates) / sizeof(g_Templates[0]);

//
// Create instance used by COM to create instance of us
CUnknown * WINAPI CDeinterlaceFilter::CreateInstance(LPUNKNOWN pUnk, HRESULT * phr)
{
	 CheckPointer(phr, NULL);
    
    CDeinterlaceFilter* pNewObject = new CDeinterlaceFilter(NAME("Deinterlace Filter"), pUnk, phr);

    if (NULL == pNewObject)
    {
        *phr = E_OUTOFMEMORY;
    }

    return pNewObject;
}
//------------------------------------------------------------------------------
// CheckInputType
// Some basic checks on preferred formats.  In this sample enable field swapping for PAL sized frames.
HRESULT CDeinterlaceFilter::CheckInputType(const CMediaType* mtIn)
{
	// Make some very rudimentary checks on the media type
	// This would be improved by examining the VIDEOINFOHEADER2 interlace flags
	// and processing accordingly.
	if (MEDIATYPE_Video != mtIn->majortype)
		return S_FALSE;
		
	if (FORMAT_VideoInfo != mtIn->formattype)
		return S_FALSE;

	if ((MEDIASUBTYPE_UYVY != mtIn->subtype) && (IID_MEDIASUBTYPE_HDYC != mtIn->subtype) && (MEDIASUBTYPE_YUY2 != mtIn->subtype) &&
		(IID_MEDIASUBTYPE_v210a != mtIn->subtype) && (IID_MEDIASUBTYPE_r210 != mtIn->subtype) &&
		(MEDIASUBTYPE_RGB24 != mtIn->subtype) && (MEDIASUBTYPE_RGB32 != mtIn->subtype) && (MEDIASUBTYPE_ARGB32 != mtIn->subtype))
		return S_FALSE;
		
	return S_OK;
}
//------------------------------------------------------------------------------
// Transform
// This is where the field swap actually takes place.  Its a very simple trans-in-place operation,
// if we've flagged field swapping, move the buffer contents down by one line.  We lose one active
// line in this process and have to black out the first line.  The MoveMemory operation is fast but
// still not ideal if there is a lot of other processing in the filter graph.
HRESULT CDeinterlaceFilter::Transform(IMediaSample* pSample)
{
	HRESULT hr = S_OK;
	
	if (pSample)
	{
		AM_MEDIA_TYPE amt = {0};
		if (SUCCEEDED(m_pInput->ConnectionMediaType(&amt)))
		{
			BITMAPINFOHEADER* pbmih = GetBMIHeader(&amt);

			if (576 == pbmih->biHeight)
			{
				// field swap the buffer contents
				long cbData = pSample->GetActualDataLength();
				BYTE* pFrame = NULL;
				hr = pSample->GetPointer(&pFrame);
				if (SUCCEEDED(hr))
				{
					long rowBytes = cbData / pbmih->biHeight;
					// move the buffer contents down by one line to achieve field swap
					MoveMemory(pFrame + rowBytes, pFrame, cbData - rowBytes);
					
					
					// black out the first line
					unsigned long Black[2];
					
					// TODO: Add more complete range of FCCs, e.g. Apple equivalents
					if ((FOURCC('UYVY') == pbmih->biCompression) || (FOURCC('HDYC') == pbmih->biCompression))
					{
						// 8-bit YUV black
						Black[0] = Black[1] = 0x10801080;
					}
					else if (FOURCC('YUY2') == pbmih->biCompression)
					{
						// 8-bit YUV black
						Black[0] = Black[1] = 0x80108010;
					}
					else if (FOURCC('v210') == pbmih->biCompression)
					{
						// 10-bit YUV
						Black[0] = 0x20010200;
						Black[1] = 0x04080040;
					}
					else if (FOURCC('r210') == pbmih->biCompression)
					{
						// 10-bit RGB
						Black[0] = Black[1] = 0x40000104;
					}
					else
					{
						// just assume BI_RGB for all others
						Black[0] = Black[1] = 0x00000000;
					}
					
					unsigned long* pData = reinterpret_cast<unsigned long*>(pFrame);
					int iCount = rowBytes / sizeof(Black);
					for (int i=0; i<iCount; ++i, ++pData)
					{
						CopyMemory(pData, &Black, sizeof(Black));
					}
				}
			}
			else
			{
				// do nothing
			}
			
			FreeMediaType(amt);
		}
		else
		{
			// not connected
		}
	}
	else
	{
		hr = E_POINTER;
	}
	
	return hr;
}
//-----------------------------------------------------------------------------
// GetBMIHeader
// Returns the BITMAPINFOHEADER structure from media type format extension
BITMAPINFOHEADER* CDeinterlaceFilter::GetBMIHeader(const AM_MEDIA_TYPE* pamt)
{
	BITMAPINFOHEADER* pbmih = NULL;

	if (pamt)
	{
		if (FORMAT_VideoInfo == pamt->formattype)
		{
			VIDEOINFOHEADER* pvih = reinterpret_cast<VIDEOINFOHEADER*>(pamt->pbFormat);
			ASSERT(sizeof(VIDEOINFOHEADER) <= pamt->cbFormat);
			pbmih = &pvih->bmiHeader;
		}
		else if (FORMAT_VideoInfo2 == pamt->formattype)
		{
			VIDEOINFOHEADER2* pvih = reinterpret_cast<VIDEOINFOHEADER2*>(pamt->pbFormat);
			ASSERT(sizeof(VIDEOINFOHEADER2) <= pamt->cbFormat);
			pbmih = &pvih->bmiHeader;
		}
	}

	return pbmih;
}

////////////////////////////////////////////////////////////////////////
//
// Exported entry points for registration and unregistration 
// (in this case they only call through to default implementations).
//
////////////////////////////////////////////////////////////////////////

STDAPI DllRegisterServer()
{
	return AMovieDllRegisterServer2(TRUE);
}

STDAPI DllUnregisterServer()
{
	return AMovieDllRegisterServer2(FALSE);
}

//
// DllEntryPoint
//
extern "C" BOOL WINAPI DllEntryPoint(HINSTANCE, ULONG, LPVOID);

BOOL APIENTRY DllMain(HANDLE hModule, 
                      DWORD  dwReason, 
                      LPVOID lpReserved)
{
	return DllEntryPoint((HINSTANCE)(hModule), dwReason, lpReserved);
}