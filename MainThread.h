#pragma once

#include "UFile.h"

// CMainThread

class CMainThread : public CWinThread
{
	DECLARE_DYNCREATE(CMainThread)

protected:
	CMainThread();           // 動的生成で使用される protected コンストラクター
public:
	virtual ~CMainThread();

public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

private:
	BOOL m_bEnding;
	BOOL m_bRunEnd;
	BOOL m_bRunStart;
	BOOL m_bCompelStop;
	BOOL m_bEmergencyStop;
	HWND m_hWndParent;


public:
	static CMainThread *Create(HWND hWnd, const BOOL bRun = TRUE);

	BOOL IsRuning();
	BOOL IsRunEnd()   const { return m_bRunEnd;   }
	BOOL IsRunStart() const { return m_bRunStart; }
	void Interruption();
	void End();


protected:
	BOOL    m_bHazad;
	BOOL    m_bUseHeader;
	BOOL    m_bShowFile;
	BOOL    m_bUseJson;
	BOOL    m_bUseBinary;
	DWORD   m_nRxTimeOut;
	CString m_strUrl;
	CString m_strResponsFileName;
	CStringTable m_tableHeader;

	BOOL Main(DWORD dwService, CString strHost, CString strPath, WORD wPort);
	BOOL CheckUrl(DWORD &dwService, CString &strHost, CString &strPath, WORD &wPort);
	void FileOpen(const WCHAR *pszName);
	void LogPutsError(const WCHAR *psz);


public:
	void SetParam(const WCHAR *pszUrl, CStringTable &tableHeader, const BOOL bUseHeader, const BOOL bUseJson, const BOOL bUseBinary);
	void ShowFileOpen(BOOL bShowFile);


protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual int Run();
};


