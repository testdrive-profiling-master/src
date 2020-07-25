#pragma once
#include "ToolBaseWnd.h"

class COutputWnd : public CToolBaseWnd
{
public:
	COutputWnd();
	virtual ~COutputWnd();

	void UpdateFonts();

protected:
	CImageList	m_TabImgList;
	CMFCTabCtrl	m_wndTabs;

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);

	DECLARE_MESSAGE_MAP()
};

