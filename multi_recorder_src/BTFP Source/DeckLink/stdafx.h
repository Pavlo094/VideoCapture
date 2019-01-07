#ifndef __STDAFX_H__
#define __STDAFX_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

#define _STLP_USE_STATIC_LIB

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if _MSC_VER >= 1400
#define _CRT_SECURE_NO_WARNINGS
#endif

#pragma warning(disable : 4503 4786) 

#include <atlstr.h>
#include <atltypes.h>

#define TRACE TRACER
inline void TRACER (LPCSTR buf, ... ) {}

#define ASSERT assert

#include <stdio.h>
#include <windows.h>
#include <mmsystem.h>
#include <assert.h>
#include <math.h>

#define ZERO_INIT(x)     ZeroMemory(&x, sizeof(x))
#define INIT_DATA(data) { ZERO_INIT(data); data.dwSize = sizeof(data); }

#define FAIL_RET(x) { HRESULT hr; if(FAILED(hr=(x))) { assert(SUCCEEDED(hr)); return hr; } }
#define ERR_RET(x)  { if(FAILED(x)) { assert(FALSE); return FALSE; } }

#undef  ZFILE_DATA 
#define ZFILE_DATA 

#ifdef _DEBUG
#ifdef TRACE
#pragma comment (lib, "winmm.lib")
#undef TRACE
#define TRACE ::AfxTrace("%5.3f: ",timeGetTime()/1000.f),::AfxTrace
#endif
#endif

class CW2A {
private:
	LPWSTR m_wString;
	LPSTR  m_aString;
	size_t m_length;
public:
	CW2A(LPCWSTR s) {
		//		ASSERT(s);
		m_aString = NULL;
		m_length = wcslen(s);
		m_wString = new WCHAR[m_length+1];
		wcscpy(m_wString,s);
	}

	virtual ~CW2A() {
		delete [] m_wString;
		delete [] m_aString;
	}

	size_t getLength() const { return m_length; }

	operator LPCSTR () /*const*/ {
		if (!m_aString && m_wString) {
			int mulStrLen = WideCharToMultiByte(CP_ACP,0,m_wString,-1,NULL,0,0,0);
			if (mulStrLen > 0) {
				m_aString = new CHAR[mulStrLen+1];
				WideCharToMultiByte(CP_ACP,0,m_wString,-1,m_aString,mulStrLen+1,0,0);
			}
		}
		return m_aString;
	}

	operator const LPCWSTR () const { return m_wString; }
};

class CA2W {
private:
	LPWSTR m_wString;
	LPSTR  m_aString;
	size_t m_length;
public:
	CA2W(LPCSTR s) {
		//		ASSERT(s);
		m_wString = NULL;
		m_length = strlen(s);
		m_aString = new CHAR[m_length+1];
		strcpy(m_aString,s);
	}

	virtual ~CA2W() {
		delete [] m_wString;
		delete [] m_aString;
	}

	size_t getLength() const { return m_length; }

	operator LPCWSTR () /*const*/ {
		if (!m_wString && m_aString) {
			int wideStrLen = MultiByteToWideChar(CP_ACP,0,m_aString,-1,NULL,0);
			if (wideStrLen > 0) {    
				m_wString = new WCHAR[wideStrLen + 1];
				MultiByteToWideChar(CP_ACP,0,m_aString,-1,m_wString,wideStrLen + 1);        
			}  
		}
		return m_wString;
	}

	operator const LPCSTR () const { return m_aString; }
};

#include <atlconv.h>
#include <atlbase.h>
#include <atlcom.h>

#ifdef _UNICODE
#ifdef A2T
#undef A2T
#endif
#define A2T(x) (LPCWSTR)CA2W(x)  
#ifdef T2A
#undef T2A
#endif
#define T2A(x) (LPCSTR)CW2A(x)
#else
#define A2T(x) x
#define T2A(x) x  
#endif

#ifdef WIN_CE
#include <winuser.h>  
#define ON_WM_INITMENU()
#define ON_WM_CONTEXTMENU()
#define ON_WM_MOUSEWHEEL()
#define ON_WM_WINDOWPOSCHANGING()
//  #define ID_FILE_PRINT           0xA0000001
//	#define ID_FILE_PRINT_DIRECT    0xA0000002
//	#define ID_FILE_PRINT_PREVIEW   0xA0000003
#define WS_MAXIMIZE             0xA0000004
#endif

#endif

#include <streams.h>
#include <initguid.h>
#include <dvdmedia.h>
#include <comutil.h>
#include <combase.h>
#include "DeckLinkAPI_h.h"
#include "DeckLinkDS_h.h"
#include "VideoDevice_h.h"
// TODO: reference additional headers your program requires here


DEFINE_GUID(MEDIASUBTYPE_HDYC,
			0x43594448,0x0000,0x0010,0x80,0x00,0x00,0xAA,0x00,0x38,0x9B,0x71);

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
