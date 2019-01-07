//------------------------------------------------------------------------------------
// Utils.h
//
// Desc: DirectShow utility class header
//------------------------------------------------------------------------------------

#pragma once

#include <string>
#include <list>
using namespace std;

#include <dvdmedia.h>

//------------------------------------------------------------------------------------
// CUtils - utility class
//------------------------------------------------------------------------------------
class CUtils
{
public:
	static BITMAPINFOHEADER* GetBMIHeader(const AM_MEDIA_TYPE* pamt);
	static BITMAPINFOHEADER* GetBMIHeader(const CMediaType& mt);
	static REFERENCE_TIME GetAvgTimePerFrame(const AM_MEDIA_TYPE* pamt);
	static unsigned long GetImageSize(const BITMAPINFOHEADER* pbmih);
};

//------------------------------------------------------------------------------
// UNITS = 10 ^ 7  
// UNITS / 30 = 30 fps;
// UNITS / 20 = 20 fps, etc
const REFERENCE_TIME FPS_60 = UNITS / 60;
const REFERENCE_TIME FPS_5994 = UNITS * 1001 / 60000;
const REFERENCE_TIME FPS_50 = UNITS / 50;
const REFERENCE_TIME FPS_30 = UNITS / 30;
const REFERENCE_TIME FPS_2997 = UNITS * 1001 / 30000;
const REFERENCE_TIME FPS_25 = UNITS / 25;
const REFERENCE_TIME FPS_24 = UNITS / 24;
const REFERENCE_TIME FPS_23976 = UNITS * 1001 / 24000;
const REFERENCE_TIME FPS_20 = UNITS / 20;
const REFERENCE_TIME FPS_10 = UNITS / 10;
const REFERENCE_TIME FPS_5  = UNITS / 5;
const REFERENCE_TIME FPS_4  = UNITS / 4;
const REFERENCE_TIME FPS_3  = UNITS / 3;
const REFERENCE_TIME FPS_2  = UNITS / 2;
const REFERENCE_TIME FPS_1  = UNITS / 1;

//------------------------------------------------------------------------------
#define SAFE_DELETE(p)			{ if(p) { delete (p);     (p)=NULL; } }
#define SAFE_DELETE_ARRAY(p)	{ if(p) { delete [] (p);     (p)=NULL; } }
#define SAFE_RELEASE(p)			{ if(p) { (p)->Release(); (p)=NULL; } }
#define SIZEOF_ARRAY(x)			(sizeof(x)/sizeof((x)[0]))
