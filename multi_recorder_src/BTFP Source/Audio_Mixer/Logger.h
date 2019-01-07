#pragma once
//#include <tchar.h>
#include <stdio.h>
#include "streams.h"

// writes log output to c:\AudioMixer.txt if present.
class CLogger
{
public:
	CLogger(const TCHAR* pFile);
	~CLogger();
	void Log(const TCHAR* pFormat, ...);
private:
	CCritSec m_csLog;
	DWORD m_msBase;
	HANDLE m_hFile;
};
extern CLogger logger;
#define LOG(x)	logger.Log x