
#include "pch.h"
#include "HttpClient.h"
#include "JsonMolding.h"


/// <summary>コンストラクタ</summary>
CJsonMolding::CJsonMolding()
{
	m_bTabIndent = FALSE;
}


/// <summary>デストラクタ</summary>
CJsonMolding::~CJsonMolding()
{
}


/// <summary>実行</summary>
/// <param name="pszFileName">ファイル名</param>
BOOL CJsonMolding::Run(const WCHAR *pszFileName, CMemory memBody, CMemory memHeader)
{
	CString strExportName = CreateExportName(pszFileName);

	if (!Execution(strExportName, memBody, memHeader)) { return FALSE; }

	FileOpen(strExportName);

	return TRUE;
}


/// <summary>実際の処理する</summary>
/// <param name="pszImportName">インポートファイル名</param>
/// <param name="pszExportName">エクスポートファイル名</param>
/// <returns>FALSE:失敗/TRUE:成功</returns>
BOOL CJsonMolding::Execution(const WCHAR *pszExportName, CMemory &memBody, CMemory &memHeader)
{
	File  wrFile;

	if (!wrFile.Open(pszExportName, File::_WRITE)) { return FALSE; }

	wrFile.Write(memHeader, memHeader.GetSize());

	if (memBody[0] == '{' || memBody[0] == '[') {
		CMemory memTemp;
	
		// 一旦「TAB/CR/LF」を削除する
		AllTabCrLfDelete(memBody, memTemp);


		Analyze(&memTemp, &wrFile, 0);
	} else {
		wrFile.Write(memBody, memBody.GetSize());
	}

	wrFile.Close();

	return TRUE;
}

#define BUFFER_SIZE 1000
BOOL CJsonMolding::Analyze(CMemory *pRD, File *pWR, int nWeit)
{
	int nChar;
	int nCnt = 0;
	int nNum;
	char *pBuf = new char[BUFFER_SIZE + 10];
	char *cp = pBuf;
	BOOL bWrite = FALSE;

	nNum = SetIndent(cp, nWeit); cp += nNum; nCnt += nNum;
	TopSpaceCut(pRD);

	while (nChar = pRD->Read()) {
		if (nChar == _NO_DATA) { break; }

		if (nChar == '\\')
		{
			continue;
		}
		else if (nChar == '[' || nChar == '{')
		{
			*cp++ = (char)nChar; nCnt++;
			int n = pRD->Read();

			if (n == _NO_DATA) {
				break;
			} else if ((nChar == '[' && n == ']') || (nChar == '{' && n == '}')) {
				*cp++ = (char)n; nCnt++;
				continue;
			} else {
				pRD->SeekOff(-1);
			}

			pWR->Write(pBuf, nCnt);
			pWR->Write("\r\n");
			bWrite = FALSE;
			Analyze(pRD, pWR, nWeit + 1);

			cp   = pBuf;
			nCnt = 0;
			nNum = SetIndent(cp, nWeit); cp += nNum; nCnt += nNum;
		}
		else if (nChar == ',')
		{
			*cp++ = (char)nChar; nCnt++;
			pWR->Write(pBuf, nCnt);
			pWR->Write("\r\n");
			bWrite = FALSE;
			TopSpaceCut(pRD);

			cp   = pBuf;
			nCnt = 0;
			nNum = SetIndent(cp, nWeit); cp += nNum; nCnt += nNum;
		}
		else if (nChar == '}' || nChar == ']')
		{
			if (bWrite) {
				pWR->Write(pBuf, nCnt);
				pWR->Write("\r\n");
			}
			TopSpaceCut(pRD);

			cp = pBuf;
			nCnt = 0;
			nNum = SetIndent(cp, nWeit - 1); cp += nNum; nCnt += nNum;

			*cp++ = (char)nChar; nCnt++;
			int n = pRD->Read();

			if (n == ',')
			{
				*cp++ = (char)n; nCnt++;
			}
			else if (n == '\"')
			{
				*cp++ = (char)n; nCnt++;
				n = pRD->Read();
				if (n == _NO_DATA) {
				} else if (n == ',') {
					*cp++ = (char)n; nCnt++;
				} else {
					pRD->SeekOff(-1);
				}
			}
			else if (n == _NO_DATA)
			{
			}
			else
			{
				pRD->SeekOff(-1);
			}
			pWR->Write(pBuf, nCnt);
			pWR->Write("\r\n");
			TopSpaceCut(pRD);
			break;
		}
		else if (nChar == '\"')
		{
			*cp++ = (char)nChar; nCnt++;
			
			nChar = pRD->Read();
			if (nChar == _NO_DATA) { break; }
			if (nChar == '[') {
				*cp++ = (char)nChar; nCnt++;
				pWR->Write(pBuf, nCnt);
				pWR->Write("\r\n");
				bWrite = FALSE;
				Analyze(pRD, pWR, nWeit + 1);

				cp   = pBuf;
				nCnt = 0;
				nNum = SetIndent(cp, nWeit); cp += nNum; nCnt += nNum;
			} else {
				do {
					//if (nChar == '\\') {
					//	int n = pRD->Read();
					//	if (n != '\\') {
					//		pRD->SeekOff(-1);
					//		continue;
					//	}
					//} else
					if (nChar == '\"') {
						*cp++ = (char)nChar; nCnt++;
						break;
					} else if (nCnt >= BUFFER_SIZE) {
						pWR->Write(pBuf, nCnt);
						pWR->Write("\r\n");
						cp = pBuf;
						nCnt = 0;
						nNum = SetIndent(cp, nWeit); cp += nNum; nCnt += nNum;
					}
					*cp++ = (char)nChar; nCnt++;
				} while ((nChar = pRD->Read()) != _NO_DATA);
				bWrite = TRUE;
			}
		}
		else
		{
			*cp++ = (char)nChar; nCnt++;
			bWrite = TRUE;
		}

		if (nCnt >= BUFFER_SIZE) {
			pWR->Write(pBuf, nCnt);
			pWR->Write("\r\n");
			cp = pBuf;
			nCnt = 0;
			nNum = SetIndent(cp, nWeit); cp += nNum; nCnt += nNum;
			bWrite = FALSE;
		}
	}

	delete[]pBuf;

	return TRUE;
}


/// <summary>
/// 先頭のスペースを読み飛ばす
/// </summary>
/// <param name="mem">メモリー・クラス</param>
void CJsonMolding::TopSpaceCut(CMemory *pMem)
{
	int nChar;

	while (nChar = pMem->Read())
	{
		if (nChar == _NO_DATA) { break; }
		if (nChar != ' ')      { pMem->SeekOff(-1); break; }
	}
}

/// <summary>インデットする</summary>
/// <param name="cp">書き込み位置</param>
/// <param name="nWeit">重み</param>
/// <returns>書き込んだバイト数</returns>
int CJsonMolding::SetIndent(char *cp, int nWeit)
{
	int nCnt = 0;

	if (nWeit < 0) { nWeit = 0; }

	for (int i = 0; i < nWeit; i++) {
		if (m_bTabIndent) {
			*cp++ = '\t';
			nCnt++;
		} else {
			memcpy(cp, "    ", 4);
			cp += 4;
			nCnt += 4;
		}
	}

	return nCnt;
}



//---------------------------------------------------------------------------
// 一旦「TAB/CR/LF」を削除する
//---------------------------------------------------------------------------
#define FILE_BUFFER_SIZE 1024

/// <summary>
/// 一旦「TAB/CR/LF」を削除する
/// </summary>
/// <param name="memBody">変更元</param>
/// <param name="memTemp">変更先</param>
/// <retuen>FALSE:失敗/TRUE:成功</return>
BOOL CJsonMolding::AllTabCrLfDelete(CMemory &memBody, CMemory &memTemp)
{
	int nCnt = 0;
	int n    = 0;
	char *pBuf = new char[FILE_BUFFER_SIZE + 1];
	int nSize  = memBody.GetSize();
	const char *cp = memBody;

	for (int i = 0; i < nSize; i++) {
		if (*cp == C_LF || *cp == C_CR || *cp == '\t') { cp++; continue; }
		*(pBuf + nCnt) = *cp++;
		if (++nCnt >= FILE_BUFFER_SIZE) {
			memTemp.Add(pBuf, nCnt);
			nCnt = 0;
		}
	}
	memTemp.Add(pBuf, nCnt);

	delete []pBuf;

	return TRUE;
}



/// <summary>
/// 出力ファイル名の作成
/// </summary>
/// <param name="pszName"></param>
/// <returns></returns>
CString CJsonMolding::CreateExportName(const WCHAR *pszName)
{
	WCHAR *pDrv  = new WCHAR[12];
	WCHAR *pPath = new WCHAR[MAX_PATH + 2];
	WCHAR *pName = new WCHAR[MAX_PATH + 2];
	WCHAR *pExt  = new WCHAR[12];

	CString str;

	_wsplitpath_s(pszName, pDrv, 10, pPath, MAX_PATH, pName, MAX_PATH, pExt, 10);

	str  = pDrv;
	str += pPath;
	str += pName;
	str += L"_s";
	str += pExt;

	delete []pDrv;
	delete []pPath;
	delete []pName;
	delete []pExt;

	return str;
}


/// <summary>
/// ファイルを開く
/// </summary>
/// <param name="pszName">ファイルを開く</param>
void CJsonMolding::FileOpen(const WCHAR *pszName)
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

