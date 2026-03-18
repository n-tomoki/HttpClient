// DlgRequestHeader.cpp : 実装ファイル
//

#include "pch.h"
#include "HttpClient.h"
#include "afxdialogex.h"
#include "DlgRequestHeader.h"
#include "lib.h"


// CDlgRequestHeader ダイアログ

IMPLEMENT_DYNAMIC(CDlgRequestHeader, CDialogEx)

CDlgRequestHeader::CDlgRequestHeader(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_REQUEST_HEADER, pParent)
{
	m_hIcon = AfxGetApp()->LoadIconW(IDR_MAINFRAME);

	m_bEnding     = FALSE;
	m_bInitDialog = FALSE;

	m_pFont = NULL;
}

CDlgRequestHeader::~CDlgRequestHeader()
{
	if (m_pFont != NULL) { delete m_pFont; m_pFont = NULL; }
}

void CDlgRequestHeader::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgRequestHeader, CDialogEx)
	ON_BN_CLICKED(IDOK, &CDlgRequestHeader::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CDlgRequestHeader::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BUTTON_QUIT, &CDlgRequestHeader::OnBnClickedButtonQuit)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, &CDlgRequestHeader::OnBnClickedButtonSave)
	ON_WM_GETMINMAXINFO()
	ON_WM_SIZE()
	ON_WM_MOVE()
END_MESSAGE_MAP()

/// <summary>
/// 初期化処理
/// </summary>
/// <returns></returns>
BOOL CDlgRequestHeader::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		pSysMenu->RemoveMenu(SC_RESTORE , MF_BYCOMMAND);  // ウィンドウを通常の位置とサイズに戻します。
		pSysMenu->RemoveMenu(SC_MAXIMIZE, MF_BYCOMMAND);  // ウィンドウを最大化します。
		pSysMenu->RemoveMenu(SC_MINIMIZE, MF_BYCOMMAND);  // ウィンドウを最小化します。
	}

	SetIcon(m_hIcon, TRUE);
	SetIcon(m_hIcon, FALSE);

	Init();
	InitWindowSize();
	InitWindowPos();

	return TRUE;
}


// CDlgRequestHeader メッセージ ハンドラー


void CDlgRequestHeader::OnBnClickedOk()
{
	CWnd *pWnd = GetFocus();

	if      (pWnd == GetDlgItem(IDC_BUTTON_QUIT)) { PostMessageW(WM_COMMAND, IDC_BUTTON_QUIT); }
	else if (pWnd == GetDlgItem(IDC_BUTTON_SAVE)) { PostMessageW(WM_COMMAND, IDC_BUTTON_SAVE); }
	else {
		NextDlgCtrl();
	}
}


void CDlgRequestHeader::OnBnClickedCancel()
{
	PostMessageW(WM_COMMAND, IDC_BUTTON_QUIT);
}

/////////////////////////////////////////////////////////////////////////////
//===========================================================================
// 開始＆終了処理
//===========================================================================

/// <summary>初期化処理</summary>
void CDlgRequestHeader::Init()
{
	SetWindowTextW(L"リクエストヘッダーの設定");

	GetDlgItem(IDC_BUTTON_QUIT)->SetWindowTextW(L"閉じる");
	GetDlgItem(IDC_BUTTON_SAVE)->SetWindowTextW(L"保存して閉じる");

	MakeFont();

	int nCheck = IDC_CHECK_HEADER0;
	int nEdit  = IDC_EDIT_HEADER0;
	int nSize  = App.m_HttpHeader.GetSize();
	for (int i = 0; i < 10; i++) {
		if (i < nSize) {
			((CButton *)(GetDlgItem(nCheck)))->SetCheck(App.m_HttpHeader.GetParamUse(i));
			((CEdit   *)(GetDlgItem(nEdit) ))->SetWindowTextW(App.m_HttpHeader.GetParamText(i));
		} else {
			((CButton *)(GetDlgItem(nCheck)))->SetCheck(0);
			((CEdit   *)(GetDlgItem(nEdit) ))->SetWindowTextW(L"");
		}
		((CEdit   *)(GetDlgItem(nEdit) ))->SetFont(m_pFont);
		((CEdit   *)(GetDlgItem(nEdit) ))->LimitText(REQUEST_EDIT_SIZE);
		nCheck++;
		nEdit++;
	}

	// ボタンのフォーカスを設定
	GetDlgItem(IDC_BUTTON_SAVE)->SetFocus();
}


/// <summary>フォントの作成</summary>
void CDlgRequestHeader::MakeFont()
{
	if (m_pFont != NULL) {
		delete m_pFont;
	}

	m_pFont = new CFont();
	m_pFont->CreateFontW(
		12,							// int nHeight
		0,							// int nWidth
		0,							// int nEscapement
		0,							// int nOrientation
		FW_NORMAL,					// int nWeight
		0,							// BYTE bItalic
		0,							// BYTE bUnderline
		0,							// BYTE cStrikeOut
		DEFAULT_CHARSET,			// BYTE nCharSet
		OUT_DEFAULT_PRECIS,			// BYTE nOutPrecision
		CLIP_DEFAULT_PRECIS,		// BYTE nClipPrecision
		NONANTIALIASED_QUALITY,		// BYTE nQuality or 'DEFAULT_QUALITY'
		FIXED_PITCH | FF_MODERN,	// BYTE nPitchAndFamily
		L"ＭＳ ゴシック");		// LPCTSTR lpszFacename
}




/// <summary>保存せず閉じる</summary>
void CDlgRequestHeader::OnBnClickedButtonQuit()
{
	End();
}


/// <summary>設定を保存して閉じる</summary>
void CDlgRequestHeader::OnBnClickedButtonSave()
{
	End(1);
}


void CDlgRequestHeader::End(int nEndCode)
{
	if (m_bEnding) { return; }
	m_bEnding = TRUE;

	if (nEndCode) {
		App.m_HttpHeader.Clear();
		int nCheck = IDC_CHECK_HEADER0;
		int nEdit  = IDC_EDIT_HEADER0;
		WCHAR sz[REQUEST_EDIT_SIZE + 2];

		for (int i = 0; i < 10; i++) {
			int nLen = GetDlgItem(nEdit)->GetWindowTextW(sz, REQUEST_EDIT_SIZE);
			sz[nLen] = '\0';

			LIB.TrimSpaceCut(sz);
			if (wcslen(sz)) {
				int nChk = ((CButton *)GetDlgItem(nCheck))->GetCheck();

				App.m_HttpHeader.AddParam(nChk, sz);
			}
			nCheck++;
			nEdit++;
		}
	}

	EndDialog(nEndCode);
}



/////////////////////////////////////////////////////////////////////////////
//===========================================================================
// ウインドウ位置を保存＆復元＆アイティム移動
//===========================================================================

/// <summary>最初のウインドウ位置を保存</summary>
void CDlgRequestHeader::InitWindowSize()
{
	CRect rc;
	GetWindowRect(rc) ; m_sizeDlgMin = rc.Size();
	ScreenToClient(rc); m_sizeDlgOld = rc.Size();

	m_bInitDialog = TRUE;
}


/// <summary>最小のウインドウサイズ</summary>
void CDlgRequestHeader::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	CDialogEx::OnGetMinMaxInfo(lpMMI);

	if (m_bInitDialog) {
		lpMMI->ptMinTrackSize.x = m_sizeDlgMin.cx;
		lpMMI->ptMinTrackSize.y = m_sizeDlgMin.cy;
		// lpMMI->ptMaxTrackSize.x = m_sizeDlgMin.cx;
		lpMMI->ptMaxTrackSize.y = m_sizeDlgMin.cy;
	}
}


/// <summary>アイテムの移動</summary>
/// <param name="uID">アイテムコード</param>
/// <param name="l">左上の移動距離</param>
/// <param name="t">左下の移動距離</param>
/// <param name="r">右上の移動距離</param>
/// <param name="b">右下の移動距離</param>
void CDlgRequestHeader::MoveDlgItem(const UINT uID, const int l, const int t, const int r, const int b)
{
	CRect rc;

	GetDlgItem(uID)->GetWindowRect(rc);
	ScreenToClient(rc);
	rc.InflateRect(-l, -t, r, b);
	GetDlgItem(uID)->MoveWindow(rc);
}


/// <summary>ダイアログのサイズが変更された</summary>
void CDlgRequestHeader::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	switch (nType) {
	case SIZE_MINIMIZED:
	//	ShowWindow(SW_MINIMIZE);  <--- アイコン化
	//	ShowWindow(SW_HIDE);      <--- アイコン化
		break;
		
	case SIZE_RESTORED:
		if (m_bInitDialog && !IsIconic()) {
			CRect rcNew;
			GetWindowRect(rcNew);
			ScreenToClient(rcNew);
			int dx = rcNew.Width() - m_sizeDlgOld.cx;
			int dy = rcNew.Height() - m_sizeDlgOld.cy;
			m_sizeDlgOld = rcNew.Size();

			int nEdit = IDC_EDIT_HEADER0;
			for (int i = 0; i < 10; i++) {
				MoveDlgItem(nEdit, 0, 0, dx, 0);
				nEdit++;
			}

			//MoveDlgItem(IDC_BUTTON_QUIT     ,  0,  0,  0,  0);
			//MoveDlgItem(IDC_BUTTON_SAVE     ,  0,  0,  0,  0);

			SaveWindowPos();

			Invalidate(FALSE);
		}
		break;
	}
}


/// <summary>
/// ウインドウが移動された
/// </summary>
/// <param name="x">移動量</param>
/// <param name="y">移動量</param>
void CDlgRequestHeader::OnMove(int x, int y)
{
	CDialogEx::OnMove(x, y);
	SaveWindowPos();
}


/// <summary>ウインドウ位置の保存</summary>
void CDlgRequestHeader::SaveWindowPos()
{
	if (m_bInitDialog && !IsIconic()) {
		CRect rc;
		GetWindowRect(rc);
		App.WriteParamFileInt(m_pszWindowPos, _T("x") , rc.left);
		App.WriteParamFileInt(m_pszWindowPos, _T("y") , rc.top);
		App.WriteParamFileInt(m_pszWindowPos, _T("cx"), rc.Width());
		App.WriteParamFileInt(m_pszWindowPos, _T("cy"), rc.Height());
	}
}


/// <summary>ウインドウ位置の移動</summary>
void CDlgRequestHeader::InitWindowPos()
{
	int nVirtualPosX = GetSystemMetrics(SM_XVIRTUALSCREEN);
	int nVirtualPosY = GetSystemMetrics(SM_YVIRTUALSCREEN);
	int nViewX       = GetSystemMetrics(SM_CXVIRTUALSCREEN);
	int nViewY       = GetSystemMetrics(SM_CYVIRTUALSCREEN);

	int x  = App.GetParamFileInt(m_pszWindowPos, _T("x") );
	int y  = App.GetParamFileInt(m_pszWindowPos, _T("y") );
	int cx = App.GetParamFileInt(m_pszWindowPos, _T("cx"));
	int cy = App.GetParamFileInt(m_pszWindowPos, _T("cy"));

	CRect rtVirtual = CRect(nVirtualPosX, nVirtualPosY, nVirtualPosX + nViewX, nVirtualPosY + nViewY);

	// ディスプレイ内にウインドウの左上が無い場合は、ウインドウの移動はしない。
	if (rtVirtual.PtInRect(CPoint(x,y)) == 0 || GetAsyncKeyState(VK_SHIFT) & 0x8000) { return; }

	// 表示するウインドウサイズが、初期値のウィンドウサイズより小さい場合は、ウインドウの移動はしない
	if (m_sizeDlgMin.cx > cx || m_sizeDlgMin.cy > cy) { return; }

	MoveWindow(x, y, cx, cy);
}

