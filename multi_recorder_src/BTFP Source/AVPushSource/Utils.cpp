//------------------------------------------------------------------------------------
// Utils.cpp
//
// Desc: DirectShow utility class implementation
//
// Copyright (c) Blackmagic Design 2005. All rights reserved.
//------------------------------------------------------------------------------------

#include <streams.h>
#include "Utils.h"

//------------------------------------------------------------------------------------
// CUtils - utility class
//------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// GetBMIHeader
// Returns the BITMAPINFOHEADER structure from media type format extension
BITMAPINFOHEADER* CUtils::GetBMIHeader(const AM_MEDIA_TYPE* pamt)
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

//-----------------------------------------------------------------------------
// GetBMIHeader
// Returns the BITMAPINFOHEADER structure from media type format extension
BITMAPINFOHEADER* CUtils::GetBMIHeader(const CMediaType& mt)
{
	BITMAPINFOHEADER* pbmih = NULL;

	if (FORMAT_VideoInfo == mt.formattype)
	{
		VIDEOINFOHEADER* pvih = reinterpret_cast<VIDEOINFOHEADER*>(mt.pbFormat);
		ASSERT(sizeof(VIDEOINFOHEADER) <= mt.cbFormat);
		pbmih = &pvih->bmiHeader;
	}
	else if (FORMAT_VideoInfo2 == mt.formattype)
	{
		VIDEOINFOHEADER2* pvih = reinterpret_cast<VIDEOINFOHEADER2*>(mt.pbFormat);
		ASSERT(sizeof(VIDEOINFOHEADER2) <= mt.cbFormat);
		pbmih = &pvih->bmiHeader;
	}

	return pbmih;
}

//-----------------------------------------------------------------------------
// GetAvgTimePerFrame
// Returns the average time per frame from media type format extension
REFERENCE_TIME CUtils::GetAvgTimePerFrame(const AM_MEDIA_TYPE* pamt)
{
	REFERENCE_TIME rtAvgTimePerFrame = 0;

	if (pamt)
	{
		if (FORMAT_VideoInfo == pamt->formattype)
		{
			VIDEOINFOHEADER* pvih = reinterpret_cast<VIDEOINFOHEADER*>(pamt->pbFormat);
			ASSERT(sizeof(VIDEOINFOHEADER) <= pamt->cbFormat);
			rtAvgTimePerFrame = pvih->AvgTimePerFrame;
		}
		else if (FORMAT_VideoInfo2 == pamt->formattype)
		{
			VIDEOINFOHEADER2* pvih = reinterpret_cast<VIDEOINFOHEADER2*>(pamt->pbFormat);
			ASSERT(sizeof(VIDEOINFOHEADER2) <= pamt->cbFormat);
			rtAvgTimePerFrame = pvih->AvgTimePerFrame;
		}
	}

	return rtAvgTimePerFrame;
}

//------------------------------------------------------------------------------------
// GetImageSize
// Calculates the image size
unsigned long CUtils::GetImageSize(const BITMAPINFOHEADER* pbmih)
{
	unsigned long dwImageSize = 0;

	if (pbmih)
	{
		switch (pbmih->biCompression)
		{
			default:
			case BI_RGB:
				dwImageSize = (pbmih->biWidth * abs(pbmih->biHeight) * pbmih->biBitCount) >> 3;
				break;

			case 'YVYU':
			case '2YUY':
				dwImageSize = (pbmih->biWidth * abs(pbmih->biHeight) * 16) >> 3;
				break;
		}
	}

	return dwImageSize;
}
