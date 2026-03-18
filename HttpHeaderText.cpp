
#include "pch.h"
#include "HttpClient.h"
#include "HttpHeaderText.h"


CHttpHeaderText::CHttpHeaderText()
{
	m_bInit = FALSE;
}


CHttpHeaderText::~CHttpHeaderText()
{
	Clear();
}


/// <summary>記録の解放</summary>
void CHttpHeaderText::Clear()
{
	int nSize = GetSize();

	for (int i = 0; i < nSize; i++) {
		SBase *p = (SBase *)m_arrList.GetAt(0);
		delete p;
		m_arrList.RemoveAt(0);
	}
}


/// <summary>記録数を返す</summary>
/// <returns></returns>
int CHttpHeaderText::GetSize()
{
	return (int)m_arrList.GetSize();
}


/// <summary>パラメータの読み込み</summary>
void CHttpHeaderText::Init()
{
	if (m_bInit) { return; }
	m_bInit = TRUE;

	int nSize = App.GetParamFileInt(L"HttpHeaderText", L"Size", 0);
	if (nSize) {
		for (int i = 0; i < nSize; i++) {
			WCHAR sz[20];

			swprintf_s(sz, L"USE%02d", i);
			BOOL bUse = App.GetParamFileBOOL(L"HttpHeaderText", sz);

			swprintf_s(sz, L"KEY%02d", i);
			CString strKey = App.GetParamFileString(L"HttpHeaderText", sz);

			AddParam(bUse, strKey);
		}
	} else {
		AddParam(TRUE, L"Content-Type: application/json : charset=\"utf-8\"");
	}
}


void CHttpHeaderText::Save()
{
	int nSize = GetSize();

	App.WriteParamFileInt(L"HttpHeaderText", L"Size", nSize);

	for (int i = 0; i < nSize; i++) {
		SBase *p = (SBase *)m_arrList.GetAt(i);

		WCHAR sz[20];

		swprintf_s(sz, L"USE%02d", i);
		App.WriteParamFileBOOL(L"HttpHeaderText", sz, p->m_bUse);

		swprintf_s(sz, L"KEY%02d", i);
		App.WriteParamFileString(L"HttpHeaderText", sz, p->m_strText);
	}
}


BOOL CHttpHeaderText::AddParam(const BOOL bUse, const WCHAR *pszText)
{
	SBase *p = new SBase;
	p->m_bUse = bUse;
	p->m_strText = pszText;

	m_arrList.Add((void *)p);

	return TRUE;
}


BOOL CHttpHeaderText::GetParamUse(const int nNum)
{
	int nSize = GetSize();

	if (nNum < 0 || nNum >= nSize) { return FALSE; }

	SBase *p = (SBase *)m_arrList.GetAt(nNum);

	return p->m_bUse;
}


CString CHttpHeaderText::GetParamText(const int nNum)
{
	int nSize = GetSize();

	if (nNum < 0 || nNum >= nSize) { return L""; }

	SBase *p = (SBase *)m_arrList.GetAt(nNum);

	return p->m_strText;
}

