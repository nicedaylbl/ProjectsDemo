// MyStatic.cpp : 实现文件
//

#include "stdafx.h"
#include "MyStatic.h"
#include ".\mystatic.h"


// CMyStatic

IMPLEMENT_DYNAMIC(CMyStatic, CStatic)
CMyStatic::CMyStatic()
{
	m_clrText = RGB(0,0,0);
	m_clrBack = ::GetSysColor(COLOR_3DFACE);
	m_brBKgnd.CreateSolidBrush(m_clrBack);
}

CMyStatic::~CMyStatic()
{
}


BEGIN_MESSAGE_MAP(CMyStatic, CStatic)
	ON_WM_CTLCOLOR_REFLECT()
END_MESSAGE_MAP()



// CMyStatic 消息处理程序


void CMyStatic::SetTextColor(COLORREF clrText)
{
	m_clrText = clrText;
	Invalidate();
}

void CMyStatic::SetBKColor(COLORREF clrBack)
{
	m_clrBack = clrBack;
	m_brBKgnd.DeleteObject();
	m_brBKgnd.CreateSolidBrush(clrBack);
	Invalidate();
}

HBRUSH CMyStatic::CtlColor(CDC* pDC, UINT nCtlColor)
{
	// TODO:  在此更改 DC 的任何属性
	pDC->SetTextColor(m_clrText);
	pDC->SetBkColor(m_clrBack);
	return (HBRUSH)m_brBKgnd;

	// TODO:  如果不应调用父级的处理程序，则返回非空画笔
	//return NULL;
}
