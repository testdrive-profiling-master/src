#pragma once
#include "ToolBaseWnd.h"
#include "Paser.h"
#include "ProfileData.h"
#include "ViewTree.h"

class CProfileViewToolBar : public CMFCToolBar
{
	virtual void OnUpdateCmdUI(CFrameWnd* /*pTarget*/, BOOL bDisableIfNoHndler)
	{
		CMFCToolBar::OnUpdateCmdUI((CFrameWnd*) GetOwner(), bDisableIfNoHndler);
	}

	virtual BOOL AllowShowOnList() const { return FALSE; }
};

class CProfileView : public CToolBaseWnd
{
public:
	CProfileView();
	virtual ~CProfileView();

	void AdjustLayout();
	void OnChangeVisualStyle();
	void ClearAll(void);

// 특성입니다.
protected:
	CImageList				m_ImageList;
	CProfileViewToolBar		m_wndToolBar;

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnProperties();
	afx_msg void OnOpen();
	afx_msg void OnDummyCompile();
	afx_msg void OnPaint();
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnItemClick(NMHDR* pNMHDR, LRESULT* pResult);

	DECLARE_MESSAGE_MAP()
};