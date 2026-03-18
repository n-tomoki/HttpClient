#pragma once
class CHttpHeaderText
{
public:
	CHttpHeaderText();
	virtual ~CHttpHeaderText();

public:
	struct SBase {
		SBase(){
			m_bUse = FALSE;
		};

		virtual ~SBase() {
		};

		BOOL    m_bUse;
		CString m_strText;
	};
	
private:
	BOOL m_bInit;
	CPtrArray m_arrList;
public:
	void Init();
	void Clear();
	void Save();

	BOOL AddParam(const BOOL bUse, const WCHAR *pszText);

	int     GetSize();
	BOOL    GetParamUse(const int nNum);
	CString GetParamText(const int nNum);
};


