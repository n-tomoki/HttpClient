#pragma once

#include "UFile.h"
#include "day.h"

class CLog
{
public:
	CLog();
	virtual ~CLog();

private:
	File fp;
	File fpThread;
	BOOL m_bUse;
	BOOL m_bActive;

	SDate  m_dtToday;
	CString m_strFileName;

	void Init();

	CSemaphore m_rSemaphore;
	inline void Lock(DWORD dwTimeout = 60000) { m_rSemaphore.Lock(dwTimeout); }
	inline void Unlock()                      { m_rSemaphore.Unlock(); }


public:
	BOOL Open(BOOL bNew = FALSE);
	void Close(BOOL bNew = FALSE);

	void Puts   (const WCHAR *psz);
	void PutsErr(const WCHAR *psz);
protected:
	const WCHAR *GetLastErrMsg();
};

