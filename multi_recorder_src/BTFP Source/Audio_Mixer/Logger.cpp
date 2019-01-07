#include "Logger.h"


CLogger::CLogger(const TCHAR* pFile)
: m_hFile(NULL)
{
	// to turn this on, create the file c:\GMFBridge.txt

	if (GetFileAttributes(pFile) != INVALID_FILE_ATTRIBUTES)
	{
		m_hFile = CreateFile(pFile, GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS, 0, NULL);
		if (m_hFile == INVALID_HANDLE_VALUE)
		{
			m_hFile = NULL;
		} else
		{
			SetFilePointer(m_hFile, 0, NULL, FILE_END);
		}
		m_msBase = timeGetTime();

		SYSTEMTIME st;
		GetLocalTime(&st);
		Log(TEXT("Started %04d-%02d-%02d %02d:%02d:%02d"),
			st.wYear,
			st.wMonth,
			st.wDay,
			st.wHour,
			st.wMinute,
			st.wSecond);
	}
}

CLogger::~CLogger()
{
	if (m_hFile != NULL)
	{
		CloseHandle(m_hFile);
	}
}

void 
CLogger::Log(const TCHAR* pFormat, ...)
{
	if (m_hFile != NULL)
	{
		va_list va;
		va_start(va, pFormat);
		TCHAR  ach[4096];
		int cchTime = wsprintf(ach, TEXT("%d:\t"), timeGetTime() - m_msBase);
		int cch = cchTime + wvsprintf(ach+cchTime, pFormat, va);
		va_end(va);

		// debug output without newline and without time (added by existing debug code)
		DbgLog((LOG_TRACE, 0, "%s", ach+cchTime));

		// add time at start and newline at end for file output
		ach[cch++] = '\r';
		ach[cch++] = '\n';

		CAutoLock lock(&m_csLog);
		DWORD cActual;
		WriteFile(m_hFile, ach, cch * sizeof(TCHAR), &cActual, NULL);
	}
}