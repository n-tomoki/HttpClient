// MainThread.cpp : 実装ファイル
//

#include "pch.h"
#include "HttpClient.h"
#include "MainThread.h"
#include "winhttp.h"
#include "afxinet.h"
#include "lib.h"

#pragma comment (lib, "winhttp.lib")


// CMainThread

IMPLEMENT_DYNCREATE(CMainThread, CWinThread)

CMainThread::CMainThread()
{
	m_bAutoDelete = FALSE;

	m_bEnding        = FALSE;
	m_bRunEnd        = FALSE;
	m_bRunStart      = FALSE;
	m_bCompelStop    = FALSE;
	m_bEmergencyStop = FALSE;
	m_hWndParent     = NULL;

	m_bHazad     = FALSE;
	m_bUseHeader = FALSE;
	m_bShowFile  = FALSE;
	m_bUseJson   = FALSE;
	m_nRxTimeOut = 0;
}

CMainThread::~CMainThread()
{
}

BOOL CMainThread::InitInstance()
{
	m_nRxTimeOut = RX_TIMEOUT_DEF;
	m_bUseHeader = TRUE;
	m_bShowFile  = TRUE;
	m_bUseJson   = TRUE;

	return TRUE;
}

int CMainThread::ExitInstance()
{
	// TODO:    スレッドごとの後処理をここで実行します。
	return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(CMainThread, CWinThread)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CMainThread メッセージ ハンドラ

/// <summary>Create</summary>
/// <param name="hWnd">親ハンドル</param>
/// <param name="bRun">すぐに実行(Defult:TRUE)</param>
/// <returns>スレッド・ハンドル</returns>
CMainThread *CMainThread::Create(HWND hWnd, const BOOL bRun)
{
	CMainThread *pThread = (CMainThread *)AfxBeginThread(RUNTIME_CLASS(CMainThread), THREAD_PRIORITY_NORMAL, 0, CREATE_SUSPENDED);

	pThread->m_hWndParent = hWnd;
	if (bRun) {
		pThread->ResumeThread();
	}
	return pThread;
}


/// <summary>終了処理</summary>
void CMainThread::End()
{
	if (m_bEnding) { return; }
	m_bEnding = TRUE;

	DWORD dwExitCode = 0;

	m_bEmergencyStop = TRUE;	// 終了メッセージを送らない
	m_bCompelStop    = TRUE;
	do {
		Sleep(10);
		GetExitCodeThread(m_hThread, &dwExitCode);
	} while(dwExitCode == STILL_ACTIVE);
}


/// <summary>実行中？？</summary>
/// <returns>TRUE:実行中/FALSE:停止</returns>
BOOL CMainThread::IsRuning()
{
	DWORD dwExitCode = 0;

	GetExitCodeThread(m_hThread, &dwExitCode);

	return (dwExitCode == STILL_ACTIVE);
}


/// <summary>中断</summary>
void CMainThread::Interruption()
{
	m_bCompelStop = TRUE;
}


void CMainThread::SetParam(const WCHAR *pszUrl)
{
	m_strUrl = pszUrl;
}



//===========================================================================
// Run 
//===========================================================================

int CMainThread::Run()
{
	m_bRunStart = TRUE;
	m_bHazad    = FALSE;

	WORD    wPort;      // ポート番号
	DWORD   dwService;  // サービス
	CString strHost;    // ドメイン
	CString strPath;    // パス

	if (!CheckUrl(dwService, strHost, strPath, wPort)) {
		Main(dwService, strHost, strPath, wPort);
	}

	ExitInstance();
	if (!m_bEmergencyStop) { // 緊急停止の場合は送らない
		App.m_pMainWnd->PostMessageW(WM_REPOER_MAIN_THREAD, m_bCompelStop);
	}
	m_bRunEnd = TRUE;
	return 0;
}


/// <summary>
/// 指定されたURLへ接続する
/// </summary>
/// <param name="dwService">サービスタイプ</param>
/// <param name="strHost">ホスト名</param>
/// <param name="strPath">パス</param>
/// <param name="wPort">ポート</param>
/// <returns>FALSE</returns>
BOOL CMainThread::Main(DWORD dwService, CString strHost, CString strPath, WORD wPort)
{
	// リクエストヘッダー
	CString strRequestHeader;

	m_tableHeader.SetIndexTop();
	int nSize = m_tableHeader.GetSize();
	for (int i = 0; i < nSize; i++) {
		strRequestHeader.Append(m_tableHeader);
		strRequestHeader.Append(L"\r\n");
	}

	// リクエスト・ボティ
	int nBodySize = 0;

	// セキュリティ
	DWORD dwRequestFlag = wPort == 443 ? INTERNET_FLAG_SECURE : 0;

	Log.Puts(L"スレッド開始");

	m_strResponsFileName = L"ResponsBody.txt";


	// レスポンスファイルを削除する
	File::UnlinkFile(m_strResponsFileName, TRUE);

	// ちょっと待つ
	Sleep(LIB.RandomA(100));

	// コマンド発行
	Log.Puts(L"コマンド発行");

	// セッションを開始します。
	BOOL      bResults = FALSE;
	HINTERNET hSession = NULL;
	HINTERNET hConnect = NULL;
	HINTERNET hRequest = NULL;

	hSession = WinHttpOpen(NULL, WINHTTP_ACCESS_TYPE_NO_PROXY, L"", L"", WINHTTP_FLAG_ASYNC);

	if (hSession) {
		DWORD protocols = WINHTTP_FLAG_SECURE_PROTOCOL_TLS1_2;
		WinHttpSetOption(hSession, WINHTTP_OPTION_SECURE_PROTOCOLS, &protocols, sizeof(protocols));
	}

	if (hSession) {
		hConnect = WinHttpConnect(hSession, strHost, wPort, 0);
		if (hConnect == NULL) {
			LogPutsError(L"WinHttpConnect");
		}
	}

	if (hConnect) {
		hRequest = WinHttpOpenRequest(hConnect, L"POST", strPath,
			NULL, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, WINHTTP_FLAG_REFRESH|dwRequestFlag);
		if (hRequest == NULL) {
			LogPutsError(L"WinHttpOpenRequest");
		}
	}

	// 受信タイムアウトの設定
	DWORD dwTimeout = m_nRxTimeOut;
	WinHttpSetOption(hRequest, WINHTTP_OPTION_RECEIVE_TIMEOUT, &dwTimeout, sizeof(dwTimeout));

	// リクエストを送信します。
	if (hRequest) {
		Log.Puts(L"リクエスト送信");
		bResults = WinHttpSendRequest(hRequest,
			strRequestHeader, -1, L"", nBodySize, nBodySize, 0);

		if (bResults == FALSE) {
			LogPutsError(L"WinHttpSendRequest");
		}
	}


	// レスポンスを受け取ります。
	if (bResults) {
		Log.Puts(L"受信開始");
		bResults = WinHttpReceiveResponse(hRequest, NULL);

		if (bResults == FALSE) {
			LogPutsError(L"WinHttpReceiveResponse");
		}
	}


	// レスポンスデータを表示します。
	CMemory memResponsBody;
	CMemory memResponsHeader;

	if (bResults) {
		if (m_bUseHeader) {
			Log.Puts(L"ヘッダ受信");

			DWORD dwSize = 0;

			WinHttpQueryHeaders(hRequest, WINHTTP_QUERY_RAW_HEADERS_CRLF, WINHTTP_HEADER_NAME_BY_INDEX, WINHTTP_NO_OUTPUT_BUFFER, &dwSize, WINHTTP_NO_HEADER_INDEX);
	
			WCHAR *pBuf = new WCHAR[dwSize/sizeof(WCHAR)];
	
			if(WinHttpQueryHeaders(hRequest, WINHTTP_QUERY_RAW_HEADERS_CRLF, WINHTTP_HEADER_NAME_BY_INDEX, pBuf, &dwSize, WINHTTP_NO_HEADER_INDEX)) {
				memResponsHeader.Add ("----<ResponsHeadrs>----\r\n");
				memResponsHeader.AddW(pBuf);
				memResponsHeader.Add ("----<ResponsHeadrs>----\r\n");
			} else {
				memResponsHeader.Add("----<ResponsHeadrs>----\r\n");
				memResponsHeader.Add("<< Header Error >>\r\n");
				memResponsHeader.Add("----<ResponsHeadrs>----\r\n");

				LogPutsError(L"WinHttpQueryHeaders");
			}
			delete []pBuf;
		}

		Log.Puts(L"ボティ受信開始");
		BOOL bRecive = TRUE;
		do  {
			DWORD dwBufSize = 0;  // 受け取る予定のデータサイズ
			DWORD dwReadSize= 0;  // 実際に受け取ったデータサイズ
			CStringA str;

			// データのサイズを確認します。
			if (!WinHttpQueryDataAvailable(hRequest, &dwBufSize)) {
				str.Format("Error %u in WinHttpQueryDataAvailable.\r\n", GetLastError());
				memResponsBody.Add(str);
					
				LogPutsError(L"WinHttpQueryDataAvailable");
			} else if (dwBufSize > 0) {
				char *pBuf = new char[dwBufSize + 1];

				if (!WinHttpReadData(hRequest, pBuf, dwBufSize, &dwReadSize)) {
					str.Format("Error %u in WinHttpReadData.\r\n", GetLastError());
					memResponsBody.Add(str);

					LogPutsError(L"WinHttpReadData");
				} else {
					memResponsBody.Add(pBuf, dwReadSize);
				}

				delete []pBuf;
			} else {
				bRecive = FALSE;
			}
			if (m_bCompelStop) { break; }
		} while (bRecive);
		Log.Puts(L"ボティ受信完了");
	}

	// クリーンアップします。
	if (hRequest) { WinHttpCloseHandle(hRequest); }
	if (hConnect) { WinHttpCloseHandle(hConnect); }
	if (hSession) { WinHttpCloseHandle(hSession); }


	if (memResponsBody.GetSize() || memResponsHeader.GetSize()) {
		File fp;

		if (fp.Open(m_strResponsFileName, File::_WRITE)) {
			if (memResponsHeader.GetSize()) {
				fp.Write(memResponsHeader, memResponsHeader.GetSize());
			}
			fp.Write(memResponsBody, memResponsBody.GetSize());
			fp.Close();
		}

		if (m_bShowFile) {
			if (m_bUseJson) {
				//CJsonMolding json;
				//json.Run(m_strResponsFileName, memResponsBody, memResponsHeader);
			} else {
				FileOpen(m_strResponsFileName);
			}

		}
	}

	// レスポンスを確認する
	Log.Puts(L"レスポンス確認");
	// ResponsCheck();


	return FALSE;
}



/// <summary>
/// 接続先URLの確認
/// </summary>
/// <param name="dwService">サービスタイプ</param>
/// <param name="strHost">ホスト名</param>
/// <param name="strPath">パス</param>
/// <param name="wPort">ポート</param>
/// <returns>TRUE:間違いがある/FALSE:合っているみたい</returns>
BOOL CMainThread::CheckUrl(DWORD &dwService, CString &strHost, CString &strPath, WORD &wPort)
{
	BOOL bVal = FALSE;
	const WCHAR *pszMsg = NULL;

	if (m_strUrl.IsEmpty())     { pszMsg = L"URLを指定してください";  }
	if (AfxParseURL(m_strUrl, dwService, strHost, strPath, wPort) == 0) { pszMsg = L"URLが異常です"; }

	if (pszMsg != NULL) {
		bVal = TRUE;
		AfxMessageBox(pszMsg, MB_ICONINFORMATION|MB_OK);
	}

	return bVal;
}


/// <summary>
/// ファイルを開く
/// </summary>
/// <param name="pszName">ファイルを開く</param>
void CMainThread::FileOpen(const WCHAR *pszName)
{
	HINSTANCE hInst;

	hInst = ShellExecuteW(App.m_pMainWnd->m_hWnd, L"open", pszName, NULL, NULL, SW_NORMAL);
	if ((unsigned long long)hInst <= 32) {
		WCHAR szTmp[MAX_PATH + 10];
		WCHAR szBuf[MAX_PATH + 10];

		if (GetWindowsDirectoryW(szTmp, MAX_PATH) > 0) {
			swprintf_s(szBuf, L"%s\\notepad.exe %s", szTmp, pszName);
			{
				STARTUPINFO si;
				PROCESS_INFORMATION pi;

				RtlZeroMemory(&si, sizeof(STARTUPINFO));
				si.cb          = sizeof(STARTUPINFO);
				si.dwFlags     = STARTF_USESHOWWINDOW;
				si.wShowWindow = SW_SHOWNORMAL;

				CreateProcessW(
					NULL,
					szBuf,
					NULL,
					NULL,
					FALSE,
					0,
					NULL,
					NULL,
					&si,
					&pi);

				CloseHandle(pi.hThread);
				CloseHandle(pi.hProcess);
			}
		}
	}
}

void CMainThread::LogPutsError(const WCHAR *psz)
{
	Log.PutsErr(psz);
	m_bHazad = TRUE;
}
