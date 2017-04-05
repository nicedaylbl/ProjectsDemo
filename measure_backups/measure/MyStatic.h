#pragma once
#include "afxwin.h"


// CMyStatic

class CMyStatic : public CStatic
{
	DECLARE_DYNAMIC(CMyStatic)

public:
	CMyStatic();
	virtual ~CMyStatic();

protected:
	DECLARE_MESSAGE_MAP()
public:
	COLORREF m_clrText;
	COLORREF m_clrBack;
	CBrush m_brBKgnd;
	void SetTextColor(COLORREF clrText);
	void SetBKColor(COLORREF clrBack);
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
};


