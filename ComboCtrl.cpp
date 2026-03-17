
//#include "StdAfx.h"
#include "pch.h"
#include "HttpClient.h"
#include "ComboCtrl.h"


/// <summary>コンストラクタ</summary>
CComboCtrl::CComboCtrl()
{
	m_pCtrl = NULL;

	m_bWrite      = FALSE;
	m_bEnding     = FALSE;
	m_bOnlyNumber = FALSE;

	m_nLimitSize = COMBOX_SIZE_DEF;

	m_pFont = NULL;
}


/// <summary>デストラクタ</summary>
CComboCtrl::~CComboCtrl()
{
	Save(TRUE);
	if (m_pFont != NULL) { delete m_pFont; m_pFont = NULL; }
}



/// <summary>
/// パラメータの保存
/// </summary>
/// <param name="bConstructor">終了時の表示項目を保存しない</param>
void CComboCtrl::Save(const BOOL bConstructor)
{
	if (m_bEnding)              { return; }
	if (m_strSection.IsEmpty()) { return; }

	if (!bConstructor) {
		CString str;

		GetString(str);
		if (!str.IsEmpty()) {
			App.WriteParamFileString(m_strSection, _T("LastItem"), str);
		}
	}

	if (!m_bWrite) { return; }
	m_bWrite = FALSE;

	App.WriteParamFileString(m_strSection, _T("DefaultItem"), m_strDefaultItem);

	int  nSize = m_pCtrl->GetCount();
	int  nCnt  = 0;
	int  nLen;
	TCHAR szKey[50];
	TCHAR *pTemp = new TCHAR[m_nLimitSize + 5];

	App.WriteParamFileInt(m_strSection, _T("ItemCount"), nSize);

	if (nSize == 0)      { return; }
	if (nSize == CB_ERR) { return; }

	for (int i = 0; i < nSize; i++) {
		if (CB_ERR != (nLen = m_pCtrl->GetLBText(i, pTemp))) {
#ifdef _UNICODE
			swprintf_s(szKey, L"Item%03d", nCnt++);
#else
			sprintf_s(szKey, "Item%03d", nCnt++);
#endif
			App.WriteParamFileString(m_strSection, szKey, pTemp);
		}
	}

	App.WriteParamFileInt (m_strSection, _T("LimitSize")  , m_nLimitSize);
	App.WriteParamFileBOOL(m_strSection, _T("OnlyNumvers"), m_bOnlyNumber);

	delete []pTemp;
}


/// <summary>パラメータの読み込み</summary>
void CComboCtrl::Load()
{
	TCHAR szKey[50];
	CString str;

	int nSize = App.GetParamFileInt(m_strSection, _T("ItemCount"));

	for (int i = 0; i < nSize; i++) {
#ifdef _UNICODE
		swprintf_s(szKey, L"Item%03d", i);
#else
		sprintf_s(szKey, "Item%03d", i);
#endif
		str = App.GetParamFileString(m_strSection, szKey);
		if (str.IsEmpty()) { break; }

		m_pCtrl->AddString(str);
	}

	m_nLimitSize  = App.GetParamFileInt (m_strSection, _T("LimitSize")  , m_nLimitSize);
	m_bOnlyNumber = App.GetParamFileBOOL(m_strSection, _T("OnlyNumvers"), m_bOnlyNumber);

	if (m_nLimitSize <= 0)               { m_nLimitSize = COMBOX_SIZE_DEF; }
	if (m_nLimitSize <  COMBOX_SIZE_MIN) { m_nLimitSize = COMBOX_SIZE_MIN; }
	if (m_nLimitSize >  COMBOX_SIZE_MAX) { m_nLimitSize = COMBOX_SIZE_MAX; }

	m_strDefaultItem = App.GetParamFileString(m_strSection, _T("DefaultItem"));
	if (m_strDefaultItem.IsEmpty()) {
		str = App.GetParamFileString(m_strSection, _T("LastItem"));
	} else {
		str = m_strDefaultItem;
	}
	m_pCtrl->SetWindowTextS(str);
}


//===========================================================================
// 開始処理＆終了処理
//===========================================================================

/// <summary>
/// 初期設定
/// </summary>
/// <param name="pCtrl">コンボボックス</param>
/// <param name="pszSection">セクション名</param>
/// <param name="nLimitSize">入力出来る文字数</param>
/// <param name="bOnlyNumber"></param>
void CComboCtrl::Init(CExComboBox *pCtrl, const TCHAR *pszSection, const int nLimitSize, const BOOL bOnlyNumber)
{
	m_pCtrl       = pCtrl;
	m_bOnlyNumber = bOnlyNumber;
	m_nLimitSize  = nLimitSize;
	Initialization(pszSection);
}


/// <summary>
/// 初期設定その２
/// </summary>
/// <param name="pszSection">セクション名</param>
/// <param name="nLimitSize">入力出来る文字数</param>
void CComboCtrl::Initialization(const TCHAR *pszSection)
{
	m_strSection = pszSection;

	if (m_pCtrl == NULL)        { return; }
	if (m_strSection.IsEmpty()) { return; }

	Load();
	m_pCtrl->LimitText(m_nLimitSize);
}



/// <summary>
/// 終了処理
/// </summary>
/// <param name="bNoLastItem">最後に表示していた文字列を保存しない</param>
void CComboCtrl::End(const BOOL bNoLastItem)
{
	if (m_pCtrl == NULL) { return; }

	Save(bNoLastItem);
	m_bEnding = TRUE;
}


/// <summary>
/// フォントサイズを変更する
/// </summary>
/// <param name="nSize">フォントのサイズ</param>
void CComboCtrl::SetFontSize(const int nSize)
{
	if (m_pCtrl == NULL) { return; }

	if (m_pFont != NULL) { delete m_pFont; }

	m_pFont = new CFont();
#ifdef _UNICODE
	m_pFont->CreateFontW(
#else
	m_pFont->CreateFontA(
#endif
		nSize,                    // int nHeight
		0,                        // int nWidth
		0,                        // int nEscapement
		0,                        // int nOrientation
		FW_NORMAL,                // int nWeight
		0,                        // BYTE bItalic
		0,                        // BYTE bUnderline
		0,                        // BYTE cStrikeOut
		DEFAULT_CHARSET,          // BYTE nCharSet
		OUT_DEFAULT_PRECIS,       // BYTE nOutPrecision
		CLIP_DEFAULT_PRECIS,      // BYTE nClipPrecision
		NONANTIALIASED_QUALITY,   // BYTE nQuality or 'DEFAULT_QUALITY'
		FIXED_PITCH | FF_MODERN,  // BYTE nPitchAndFamily
		_T("ＭＳ ゴシック"));     // LPCTSTR lpszFacename

	m_pCtrl->SetFont(m_pFont);
}



/// <summary>
/// 表示の有効/無効(灰色)切り替え
/// </summary>
/// <param name="bEnable">フラグ</param>
void CComboCtrl::Enable(const BOOL bEnable)
{
	if (m_pCtrl == NULL) { return; }

	m_pCtrl->EnableWindow(bEnable);
}


/// <summary>
/// 表示している文字をセレクト状態にする
/// </summary>
/// <param name="nStartPos">最初の文字位置(0)</param>
/// <param name="nEndPos">最後の文字位置(-1)</param>
void CComboCtrl::SetEditSel(const int nStartPos, const int nEndPos)
{
	if (m_pCtrl == NULL) { return; }

	m_pCtrl->SetEditSel(nStartPos, nEndPos);
}


//===========================================================================
//===========================================================================

/// <summary>
/// 文字列をセットする
/// </summary>
/// <param name="pstr">文字列</param>
void CComboCtrl::SetString(const TCHAR *pstr)
{
	if (m_pCtrl == NULL) { return; }

	if (CheckSetString(pstr)) { return; }

	m_pCtrl->SetWindowTextS(pstr);
}


/// <summary>
/// 文字列を追加する
/// </summary>
/// <param name="pstr">文字列</param>
void CComboCtrl::AddString(const TCHAR *pstr)
{
	if (m_pCtrl == NULL) { return; }

	TCHAR *pItem = new TCHAR[m_nLimitSize + 5];
	TCHAR *pTemp = new TCHAR[m_nLimitSize + 5];

	if (StrLen(pstr) == 0) {
		GetString(pItem, m_nLimitSize);
	} else  {
		StrCpy_s(pItem, m_nLimitSize, pstr);
	}

	TopSpaceCut(pItem);
	BackSpaceCut(pItem);

	if (StrLen(pItem) == 0)    { goto FUNC_END; }
	if (CheckSetString(pItem)) { goto FUNC_END; }

	BOOL bWrite = TRUE;
	int  i;
	int  nLen;
	int  nSize = m_pCtrl->GetCount();

	if (nSize == CB_ERR) {
	} else {
		for (i = 0; i < nSize; i++) {
			if (CB_ERR != (nLen = m_pCtrl->GetLBText(i, pTemp))) {
				pTemp[nLen] = '\0';
				if (FALSE == StrCmpi(pItem, pTemp)) {
					bWrite = FALSE;
					break;
				}
			}
		}
	}

	if (bWrite) {
		m_bWrite = TRUE;
		SetString(pItem);
		m_pCtrl->AddString(pItem);
	}

FUNC_END:
	delete []pItem;
	delete []pTemp;
}


/// <summary>
/// 書き込む文字列の確認
/// </summary>
/// <param name="pstr">文字列</param>
/// <returns>TRUE:ダメ/FALSE:いいよ</returns>
BOOL CComboCtrl::CheckSetString(const TCHAR *pstr)
{
	if (!m_bOnlyNumber) { return FALSE; }

	BOOL bRet = FALSE;
	const TCHAR *cp = pstr;

	while (*cp) {
		if (!('0' <= *cp && *cp <= '9')) {
			bRet = TRUE;
			break;
		}
		cp++;
	}

	return bRet;
}


/// <summary>
/// 文字列の比較
/// </summary>
/// <param name="pstr1">文字列１</param>
/// <param name="pstr2">文字列２</param>
/// <returns>TRUE:違う</returns>
BOOL CComboCtrl::StrCmpi(const TCHAR *pstr1, const TCHAR *pstr2)
{
	BOOL bVal = FALSE;
	const TCHAR *p1 = pstr1;
	const TCHAR *p2 = pstr2;
	TCHAR c1;
	TCHAR c2;

	while (1) {
		c1 = *p1++;
		c2 = *p2++;

		if (c1 == '\0' && c2 == '\0') { break; }

		if ('a' <= c1 && c1 <= 'z') { c1 -= 0x20; }
		if ('a' <= c2 && c2 <= 'z') { c2 -= 0x20; }

		if (c1 != c2) { bVal = TRUE; break; }
	}

	return bVal;
}


/// <summary>
/// 表示している文字列を返す
/// </summary>
/// <param name="pBuf">書き込むポインタ</param>
/// <param name="nMaxLen">バッファサイズ</param>
/// <returns>FALSE</returns>
BOOL CComboCtrl::GetString(TCHAR *pBuf, const int nMaxLen)
{
	if (m_pCtrl == NULL) { *pBuf = '\0'; return FALSE; }

	TCHAR *pTemp = new TCHAR[m_nLimitSize + 5];

	*pBuf = '\0';
	int nLen = m_pCtrl->GetWindowTextS(pTemp, m_nLimitSize);
	*(pTemp + nLen) = '\0';

	if (nLen < nMaxLen) {
		StrCpy_s(pBuf, nMaxLen, pTemp);
	} else {
		memcpy_s(pBuf, nMaxLen * sizeof(TCHAR), pTemp, (nMaxLen - 1) * sizeof(TCHAR));
		pBuf[(nMaxLen - 1) * sizeof(TCHAR)] = '\0';
	}

	delete []pTemp;

	return FALSE;
}


/// <summary>
/// 表示している文字列を返す
/// </summary>
/// <param name="str">ここに返す</param>
/// <returns>FALSE</returns>
BOOL CComboCtrl::GetString(CString &str)
{
	if (m_pCtrl == NULL) { str.Empty(); return FALSE; }

	m_pCtrl->GetWindowTextS(str);

	return FALSE;
}



/// <summary>
/// 表示している文字列を返す
/// </summary>
/// <returns>文字列</returns>
CString CComboCtrl::GetString()
{
	CString str;

	GetString(str);

	return str;
}



/// <summary>
/// 選択された文字列を返す
/// </summary>
/// <param name="pBuf">書き込むポインタ</param>
/// <param name="nMaxLen">バッファサイズ</param>
/// <returns>FALSE</returns>
BOOL CComboCtrl::GetSelectString(TCHAR *pBuf, const int nMaxLen)
{
	if (m_pCtrl == NULL) { *pBuf = '\0'; return FALSE; }

	int n = m_pCtrl->GetCurSel();

	if (n == CB_ERR ) { *pBuf = '\0'; return FALSE; }

	TCHAR *pTemp = new TCHAR[m_nLimitSize + 5];

	*pBuf = '\0';
	int nLen = m_pCtrl->GetLBText(n, pTemp);
	*(pTemp + nLen) = '\0';

	if (nLen < nMaxLen) {
		StrCpy_s(pBuf, nMaxLen, pTemp);
	} else {
		memcpy_s(pBuf, nMaxLen * sizeof(TCHAR), pTemp, (nMaxLen - 1) * sizeof(TCHAR));
		pBuf[(nMaxLen - 1) * sizeof(TCHAR)] = '\0';
	}

	delete []pTemp;

	return FALSE;
}


/// <summary>
/// 選択された文字列を返す
/// </summary>
/// <param name="str">ここに返す</param>
/// <returns>FALSE</returns>
BOOL CComboCtrl::GetSelectString(CString &str)
{
	if (m_pCtrl == NULL) { str.Empty(); return FALSE; }

	int n = m_pCtrl->GetCurSel();

	if (n == CB_ERR) { str.Empty(); return FALSE; }

	m_pCtrl->GetLBText(n, str);

	return FALSE;
}



/// <summary>
/// 選択された文字列を返す
/// </summary>
/// <returns>文字列</returns>
CString CComboCtrl::GetSelectString()
{
	CString str;

	GetSelectString(str);

	return str;
}



/// <summary>
/// 指定された文字列を削除する
/// </summary>
/// <param name="pstr">文字列</param>
/// <returns>FALSE:失敗/TRUE:成功</returns>
BOOL CComboCtrl::DeleteString(const TCHAR *pstr)
{
	if (m_pCtrl == NULL)   { return FALSE; }
	if (StrLen(pstr) == 0) { return FALSE; }

	BOOL bRet    = FALSE;
	TCHAR *pItem = new TCHAR[m_nLimitSize + 5];
	StrCpy_s(pItem, m_nLimitSize, pstr);

	TopSpaceCut(pItem);
	BackSpaceCut(pItem);

	if (StrLen(pItem) == 0) { goto FUNC_END; }

	int  nIndex = -1;
	if (IsExistString(pItem, &nIndex)) {
		m_bWrite = TRUE;
		bRet     = TRUE;
		m_pCtrl->DeleteString(nIndex);
	}

FUNC_END:
	delete []pItem;

	return bRet;
}



/// <summary>全ての文字列を削除する</summary>
void CComboCtrl::DeleteAllString()
{
	int nSize = m_pCtrl->GetCount();

	if (nSize) {
		for (int i = 0; i < nSize; i++) {
			m_pCtrl->DeleteString(0);
		}
		m_bWrite = TRUE;
	}
}


/// <summary>
/// 指定された文字列があるか？調べる
/// </summary>
/// <param name="pstr">文字列</param>
/// <param name="pIndex">見つけた文字列のインデックス番号がセットされる</param>
/// <returns>無いorエラー:FALSE/見つけた:TRUE</returns>
BOOL CComboCtrl::IsExistString(const TCHAR *pstr, int *pIndex)
{
	BOOL bRet = FALSE;

	if (pIndex != NULL)    { *pIndex = -1; }
	if (m_pCtrl == NULL)   { return FALSE; }
	if (StrLen(pstr) == 0) { return FALSE; }

	TCHAR *pItem = new TCHAR[m_nLimitSize + 5];
	TCHAR *pTemp = new TCHAR[m_nLimitSize + 5];

	StrCpy_s(pItem, m_nLimitSize, pstr);

	TopSpaceCut(pItem);
	BackSpaceCut(pItem);

	if (StrLen(pItem) == 0) { goto FUNC_END; }

	int nLen;
	int nSize = m_pCtrl->GetCount();

	if (nSize == CB_ERR) {
	} else {
		for (int i = 0; i < nSize; i++) {
			if (CB_ERR != (nLen = m_pCtrl->GetLBText(i, pTemp))) {
				*(pTemp + nLen) = '\0';
				if (FALSE == StrCmpi(pItem, pTemp)) {
					bRet = TRUE;
					if (pIndex != NULL) { *pIndex = i; }
					break;
				}
			}
		}
	}

FUNC_END:
	delete []pItem;
	delete []pTemp;

	return bRet;
}

//===========================================================================
// 初期値の設定
//===========================================================================

/// <summary>デフォルト文字列を表示する</summary>
void CComboCtrl::SetDefaultItem()
{
	SetString(m_strDefaultItem);
	m_bWrite = TRUE;
}


/// <summary>デフォルト文字列をクリアする</summary>
void CComboCtrl::ClearDefaultItem()
{
	m_strDefaultItem.Empty();
	m_bWrite = TRUE;
}


/// <summary>
/// デフォルト文字列をセットする
/// </summary>
/// <param name="pstr"></param>
void CComboCtrl::AddDefaultItem(const TCHAR *pstr)
{
	if (m_pCtrl == NULL) { return; }

	TCHAR *pItem = new TCHAR[m_nLimitSize + 5];

	if (StrLen(pstr) == 0) {
		GetString(pItem, m_nLimitSize);
	} else  {
		StrCpy_s(pItem, m_nLimitSize, pstr);
	}

	TopSpaceCut(pItem);
	BackSpaceCut(pItem);

	if (StrLen(pItem) == 0)    { goto FUNC_END; }
	if (CheckSetString(pItem)) { goto FUNC_END; }

	SetString(pItem);
	m_strDefaultItem = pItem;

FUNC_END:
	delete []pItem;
}



//===========================================================================
// 文字列の空白を削除する
//===========================================================================

/// <summary>
/// 文字列の頭の空白を削除する 
/// </summary>
/// <param name="pBuf">文字列</param>
void CComboCtrl::TopSpaceCut(char *pBuf)
{
	char *rp = pBuf;
	char *wp = pBuf;

	while (*rp != '\0') {
		if (*rp == ' ') {
			rp++;
		} else if (*rp == '\t') {
			rp++;
		} else if ((BYTE)(*rp) == 0x81 && (BYTE)(*(rp + 1)) == 0x40) {
			rp += 2;
		} else {
			break;
		}
	}

	if (wp != rp) {
		do {
			*wp++ = *rp++;
		} while (*(wp - 1) != '\0');
	}
}


/// <summary>
/// 文字列の頭の空白を削除する 
/// </summary>
/// <param name="pBuf">文字列</param>
void CComboCtrl::TopSpaceCut(WCHAR *pBuf)
{
	WCHAR *rp = pBuf;
	WCHAR *wp = pBuf;

	while (*rp != '\0') {
		if (*rp == ' ') {
			rp++;
		} else if (*rp == '　') {
			rp++;
		} else if (*rp == '\t') {
			rp++;
		} else {
			break;
		}
	}

	if (wp != rp) {
		do {
			*wp++ = *rp++;
		} while (*(wp - 1) != '\0');
	}
}


/// <summary>
/// 文字列の後ろの空白を削除する
/// </summary>
/// <param name="pBuf">文字列</param>
void CComboCtrl::BackSpaceCut(char *pBuf)
{
	char *cp;

	cp = pBuf + strlen(pBuf) - 1;
	while(cp >= pBuf) {
		if (*cp == '\t' || *cp == ' ' || *cp == '\n' || *cp == '\r') {
		} else if ((BYTE)(*cp) == 0x40 && (BYTE)(*(cp - 1)) == 0x81) {
			cp--;
		} else {
			break;
		}
		*cp-- = '\0';
	}
}


/// <summary>
/// 文字列の後ろの空白を削除する
/// </summary>
/// <param name="pBuf">文字列</param>
void CComboCtrl::BackSpaceCut(WCHAR *pBuf)
{
	WCHAR *cp;

	cp = pBuf + wcslen(pBuf) - 1;
	while(cp >= pBuf) {
		if (*cp == '\t' || *cp == ' ' || *cp == '\n' || *cp == '\r' || *cp == '　') {
		} else {
			break;
		}
		*cp-- = '\0';
	}
}


//===========================================================================
// 文字列の長さを返す
//===========================================================================

/// <summary>
/// 文字列の長さを返す
/// </summary>
/// <param name="pstr">文字列</param>
/// <returns>長さ</returns>
int CComboCtrl::StrLen(const char  *pstr)
{
	return (int)strlen(pstr);
}


/// <summary>
/// 文字列の長さを返す
/// </summary>
/// <param name="pstr">文字列</param>
/// <returns>長さ</returns>
int CComboCtrl::StrLen(const WCHAR *pstr)
{
	return (int)wcslen(pstr);
}


//===========================================================================
// 文字列のコピー
//===========================================================================

/// <summary>
/// 文字列のコピー
/// </summary>
/// <param name="pBuf">コピー先</param>
/// <param name="nSize">コピー先のサイズ</param>
/// <param name="pstr">コピー元</param>
void CComboCtrl::StrCpy_s(char  *pBuf, const int nSize, const char  *pstr)
{
	strcpy_s(pBuf, nSize, pstr);
}


/// <summary>
/// 文字列のコピー
/// </summary>
/// <param name="pBuf">コピー先</param>
/// <param name="nSize">コピー先のサイズ</param>
/// <param name="pstr">コピー元</param>
void CComboCtrl::StrCpy_s(WCHAR *pBuf, const int nSize, const WCHAR *pstr)
{
	wcscpy_s(pBuf, nSize, pstr);
}



//===========================================================================
// 扱うアイテムを変更する
//===========================================================================

/// <summary>
/// 扱うアイテムを変更する
/// </summary>
/// <param name="pszSection">セクション名</param>
/// <param name="bNoLastItem">最後に表示していた文字列を保存しない</param>
/// <param name="nLimitSize">入力出来る文字数</param>
/// <returns>FALSE:失敗/TRUE:成功</returns>
BOOL CComboCtrl::RestartItem(const TCHAR *pszSection, const BOOL bNoLastItem)
{
	// 文字列の保存
	Save(bNoLastItem);

	// 文字列の削除
	DeleteAllString();
	// 書き込みフラグを落とす
	m_bWrite = FALSE;

	// 文字列の再設定
	Initialization(pszSection);

	return TRUE;
}


/// <summary>
/// 現在のセッション名を返す
/// </summary>
/// <returns>セッション名</returns>
CString CComboCtrl::GetSectionName() const
{
	return m_strSection;
}


/// <summary>現在のセッション名を返す</summary>
void  CComboCtrl::GetSectionName(CString &str)
{
	str = m_strSection;
}
