#pragma once
#include <wtypes.h>
#include <map>
#include "wxdebug.h"

using namespace std;

enum EFILED_ORDER
{
	foProgressive = 0,
	foUpFieldFirst,
	foLowFieldFirst,
	foProgressiveSegmented
};

class CVideoProperty 
{
public:
	CVideoProperty(void);
	~CVideoProperty(void);

public:
	DWORD Width() const { return m_dwWidth; }
	void Width(DWORD val) { m_dwWidth = val; }
	DWORD Height() const { return m_dwHeight; }
	void Height(DWORD val) { m_dwHeight = val; }
	DWORD Bpp() const { return m_dwBpp; }
	void Bpp(DWORD val) { m_dwBpp = val; }
	EFILED_ORDER FieldOrder() const { return m_eFieldOrder; }
	void FieldOrder(EFILED_ORDER val) { m_eFieldOrder = val; }
	DWORD FrameTime() const { return m_dwFrameTime; }
	void FrameTime(DWORD val) { m_dwFrameTime = val; }

	void AddPixelFormat(DWORD val)
	{
		m_vPixelFormat[val] = 1;		
	}
	void RemovePixelFormat(DWORD val)
	{
		m_vPixelFormat[val] = 0;		
	}

	HRESULT GetPixelFormatSupproted(DWORD val)
	{
		if (m_vPixelFormat[val] == 1)
		{
			return S_OK;
		}
		else 
		{
			return S_FALSE;
		}
	}

private:
	DWORD m_dwWidth;
	DWORD m_dwHeight;	
	DWORD m_dwBpp;
	DWORD m_dwFrameTime;
	EFILED_ORDER m_eFieldOrder;	
	map<DWORD, int> m_vPixelFormat;
};


// {2148ADAC-D36C-4c29-A787-EAE8FFFFE215}
DEFINE_GUID(IID_IVideoPropertyHolder,
			0x2148adac, 0xd36c, 0x4c29, 0xa7, 0x87, 0xea, 0xe8, 0xff, 0xff, 0xe2, 0x15);
//struct __declspec(uuid("2148ADAC-D36C-4c29-A787-EAE8FFFFE215")) __declspec(novtable);


class IVideoPropertyHolder :
	public IUnknown
{
public:
	virtual HRESULT GetConfig(CVideoProperty *pConfig) PURE;
	virtual HRESULT SetConfig(CVideoProperty pConfig) PURE;
	
};