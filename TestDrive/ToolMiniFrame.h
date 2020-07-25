#pragma once

class CToolMiniFrame : public CMultiPaneFrameWnd
{
	DECLARE_SERIAL(CToolMiniFrame)
public:
	CToolMiniFrame();
	virtual ~CToolMiniFrame();

	virtual BOOL CanBeDockedToPane(const CDockablePane* pDockingBar) const;
	virtual BOOL DockFrame(CPaneFrameWnd* pDockedFrame, AFX_DOCK_METHOD dockMethod);
	virtual BOOL DockPane(CDockablePane* pDockedBar);

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	//afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

	DECLARE_MESSAGE_MAP()
};
