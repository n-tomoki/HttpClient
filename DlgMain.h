
// DlgMain.h : ヘッダー ファイル
//

#pragma once

#include "ExComboBox.h"
#include "ComboCtrl.h"
#include "MainThread.h"


// CDlgMain ダイアログ
class CDlgMain : public CDialogEx
{
// コンストラクション
public:
	CDlgMain(CWnd* pParent = nullptr);	// 標準コンストラクター
	virtual ~CDlgMain();

// ダイアログ データ
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_HTTPCLIENT_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV サポート


// 実装
protected:
	HICON m_hIcon;
	BOOL  m_bEnding;

	void Init();
	void End(const int nEndCode = 0);


protected:
	CComboCtrl *m_pComboUrlList;
	CExComboBox m_cbUrlList;


	// スレッド
protected:
	BOOL m_bBusyThread;
	BOOL m_bBusyReport;

	CMainThread *m_pThread;

	void Enable(const BOOL bFlag = TRUE);



	// アイティムを移動＆ウインドウ位置を保存＆復元
protected:
	BOOL         m_bInitDialog;
	CSize        m_sizeDlgMin;
	CSize        m_sizeDlgOld;
	const TCHAR *m_pszWindowPos = _T("WindowPos");

	void InitWindowSize();
	void InitWindowPos();
	void SaveWindowPos();
	void MoveDlgItem(const UINT uID, const int l, const int t, const int r, const int b);


protected:
	// 生成された、メッセージ割り当て関数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg LRESULT OnReportMainTh(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedButtonQuit();
	afx_msg void OnBnClickedButtonGo();
	afx_msg void OnEndSession(BOOL bEnding);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnMove(int x, int y);
};
