// SharedMemoryFilter.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"

#include <string>


#ifdef _MANAGED
#pragma managed(push, off)
#endif

//
// DllEntryPoint
//
extern "C" BOOL WINAPI DllEntryPoint(HINSTANCE, ULONG, LPVOID);

BOOL APIENTRY DllMain(HANDLE hModule, 
					  DWORD  dwReason, 
					  LPVOID lpReserved)
{
	TCHAR buf[_MAX_PATH];	
	GetModuleFileName(NULL,buf,_MAX_PATH);
	std::wstring moduleName(buf);
	if (moduleName.find(TEXT("Skype")) != moduleName.npos ||
		moduleName.find(TEXT("msnmgr")) != moduleName.npos)
		return FALSE;
	return DllEntryPoint((HINSTANCE)(hModule), dwReason, lpReserved);
}
#ifdef _MANAGED
#pragma managed(pop)
#endif

