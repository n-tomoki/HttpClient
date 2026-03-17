#pragma once

//=======================================================================
// 文字入力で、自動的に右スクロールする為には、
// ・「ComboBoxプロパティ」→「動作」→「自動」を「True」にする
// ・「xxxxxx.rc」の「COMBOBOX」のスタイルに「CBS_AUTOHSCROLL」を追加する
//=======================================================================

/*=======================================================================
// セレクトされた時のメッセージの受け方(例)
BOOL CDlgMain::OnCommand(WPARAM wParam, LPARAM lParam)
{
	if (wParam == IDC_COMBOX_SELCHECNG) {
		if (m_cbUrlList.m_hWnd == (HWND)lParam)
		{
			LoadUrlBody(m_pComboUrlList->GetSelectString());
		}
	}

	return CDialogEx::OnCommand(wParam, lParam);
}
=======================================================================*/


class CExComboBox : public CComboBox
{
	DECLARE_DYNAMIC(CExComboBox)

public:
	CExComboBox();
	virtual ~CExComboBox();

private:
	CWnd *m_pParentWnd;

public:
	int  GetWindowTextS(const TCHAR *pstr, const int nMaxCount);
	void GetWindowTextS(CString &str);

	void SetWindowTextS(const TCHAR *pstr);

	void SetParent(CWnd *pParentWnd);


protected:
	DECLARE_MESSAGE_MAP()
public:
//	afx_msg void OnCbnSelchange();
};


