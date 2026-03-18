#pragma once

#include "UFile.h"

class CJsonMolding
{
public:
	CJsonMolding();
	virtual ~CJsonMolding();


private:
	BOOL m_bTabIndent;

	BOOL Execution(const WCHAR *pszExportName, CMemory &memBody, CMemory &memHeader);
	BOOL Analyze(CMemory *pRD, File *pWR, int nWeit);
	void TopSpaceCut(CMemory *pMem);
	int  SetIndent(char *cp, int nWeit);
	BOOL AllTabCrLfDelete(CMemory &memBody, CMemory &memTemp);
	void FileOpen(const WCHAR *pszName);
	CString CreateExportName(const WCHAR *pszName);


public:
	BOOL Run(const WCHAR *pszFileName, CMemory memBody, CMemory memHeader);
};


