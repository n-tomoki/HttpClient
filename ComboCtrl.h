#pragma once

#include "ExComboBox.h"

#define COMBOX_SIZE_MIN   10
#define COMBOX_SIZE_DEF  200
#define COMBOX_SIZE_MAX 2000

class CComboCtrl
{
public:
	CComboCtrl();
	virtual ~CComboCtrl();

private:
	BOOL    m_bWrite;
	BOOL    m_bEnding;
	BOOL    m_bOnlyNumber;
	int     m_nLimitSize;
	CString m_strDefaultItem;
	CString m_strSection;
//public:
	CExComboBox *m_pCtrl;
	CFont *m_pFont;


	// 初期設定
public:
	void Init(CExComboBox *pCtrl, const TCHAR *pszSection, const int nLimitSize = 0, const BOOL bOnlyNumber = FALSE);
private:
	void Initialization(const TCHAR *pszSection);

public:
	void Load();
	void Save(const BOOL bConstructor = FALSE);

	void End(const BOOL bNoLastItem = FALSE);

	void SetString(const TCHAR *pstr);
	void SetFontSize(const int nSize);
private:
	BOOL CheckSetString(const TCHAR *pstr);

public:
	void SetDefaultItem();
	void ClearDefaultItem();
	void AddDefaultItem(const TCHAR *pstr = _T(""));
	void SetEditSel(const int nStartPos = 0, const int nEndPos = -1);


	void AddString(const TCHAR *pstr = _T(""));

	BOOL    GetString(TCHAR *pBuf, const int nMaxLen);
	BOOL    GetString(CString &str);
	CString GetString();
	BOOL    GetSelectString(TCHAR *pBuf, const int nMaxLen);
	BOOL    GetSelectString(CString &str);
	CString GetSelectString();

	BOOL DeleteString(const TCHAR *pstr);
	void DeleteAllString();

	BOOL IsExistString(const TCHAR *pstr, int *pIndex = NULL);

	void Enable(const BOOL bEnable = TRUE);
private:
	BOOL StrCmpi(const TCHAR *pstr1, const TCHAR *pstr2);

	void TopSpaceCut(char  *pstr);
	void TopSpaceCut(WCHAR *pstr);
	void BackSpaceCut(char *pstr);
	void BackSpaceCut(WCHAR *pstr);

	int StrLen(const char  *pstr);
	int StrLen(const WCHAR *pstr);

	void StrCpy_s(char  *pBuf, const int nSize, const char  *pstr);
	void StrCpy_s(WCHAR *pBuf, const int nSize, const WCHAR *pstr);


public:
	int SysBufSize() const { return m_nLimitSize; }

	// 現在のセッション名を返す
	CString GetSectionName() const;
	void    GetSectionName(CString &str);

	// 扱うアイテムを変更する
	BOOL RestartItem(const TCHAR *pszSection, const BOOL bNoLastItem = FALSE);
};

