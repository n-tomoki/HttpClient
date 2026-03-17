// ExComboBox.cpp : 実装ファイル
//

//#include "stdafx.h"
#include "pch.h"
#include "resource.h"
#include "ExComboBox.h"


// CExComboBox

IMPLEMENT_DYNAMIC(CExComboBox, CComboBox)

CExComboBox::CExComboBox()
{
	m_pParentWnd = NULL;
}

CExComboBox::~CExComboBox()
{
}


BEGIN_MESSAGE_MAP(CExComboBox, CComboBox)
//	ON_CONTROL_REFLECT(CBN_SELCHANGE, &CExComboBox::OnCbnSelchange)
END_MESSAGE_MAP()



// CExComboBox メッセージ ハンドラー


/// <summary>
/// 「GetWindowTextA/W」の切り替え
/// </summary>
/// <param name="pstr">受け取るバッファ</param>
/// <param name="nMaxCount">バッファのサイズ</param>
/// <returns>文字列の長さ</returns>
int CExComboBox::GetWindowTextS(const TCHAR *pstr, const int nMaxCount)
{
#ifdef _UNICODE
	return CComboBox::GetWindowTextW((LPWSTR)pstr, nMaxCount);
#else
	return CComboBox::GetWindowTextA((LPSTR)pstr, nMaxCount);
#endif
}

/// <summary>
/// 「GetWindowTextA/W」の切り替え
/// </summary>
/// <param name="str">受け取るバッファ</param>
void CExComboBox::GetWindowTextS(CString &str)
{
#ifdef _UNICODE
	CComboBox::GetWindowTextW(str);
#else
	CComboBox::GetWindowTextA(str);
#endif
}


/// <summary>
/// 「SetWindowTextA/W」の切り替え
/// </summary>
/// <param name="pstr">設定する文字列</param>
/// <returns>なし</returns>
void CExComboBox::SetWindowTextS(const TCHAR *pstr)
{
#ifdef _UNICODE
	CComboBox::SetWindowTextW(pstr);
#else
	CComboBox::SetWindowTextA(pstr);
#endif
}

void CExComboBox::SetParent(CWnd *pParentWnd)
{
	m_pParentWnd = pParentWnd;

	CComboBox::SetParent(pParentWnd);
}


/// <summary>選択された</summary>
//void CExComboBox::OnCbnSelchange()
//{
//#ifdef _UNICODE
//	m_pParentWnd->PostMessageW(WM_COMMAND, IDC_COMBOX_SELCHECNG, (LPARAM)this->m_hWnd);
//#else
//	m_pParentWnd->PostMessageA(WM_COMMAND, IDC_COMBOX_SELCHECNG, (LPARAM)this->m_hWnd);
//#endif
//}

