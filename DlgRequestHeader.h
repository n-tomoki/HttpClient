#pragma once
#include "afxdialogex.h"


// CDlgRequestHeader ダイアログ

#define REQUEST_EDIT_SIZE 300



class CDlgRequestHeader : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgRequestHeader)

public:
	CDlgRequestHeader(CWnd* pParent = nullptr);   // 標準コンストラクター
	virtual ~CDlgRequestHeader();

// ダイアログ データ
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_REQUEST_HEADER };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート

public:

protected:
	HICON m_hIcon;
	BOOL  m_bEnding;

	void Init();
	void End(const int nEndCode = 0);

protected:
	CFont *m_pFont;

	void MakeFont();

	// ウインドウ位置を保存＆復元＆アイティム移動
protected:
	BOOL         m_bInitDialog;
	CSize        m_sizeDlgMin;
	CSize        m_sizeDlgOld;
	const WCHAR *m_pszWindowPos = L"DlgRequestPos";

	void InitWindowSize();
	void InitWindowPos();
	void SaveWindowPos();
	void MoveDlgItem(const UINT uID, const int l, const int t, const int r, const int b);



protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedButtonQuit();
	afx_msg void OnBnClickedButtonSave();
	virtual BOOL OnInitDialog();
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnMove(int x, int y);
};
