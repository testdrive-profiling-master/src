#pragma once

class CDocumentMiniFrame : public CMultiPaneFrameWnd
{
	DECLARE_SERIAL(CDocumentMiniFrame)
public:
	CDocumentMiniFrame();
	virtual ~CDocumentMiniFrame();

	virtual BOOL CanBeDockedToPane(const CDockablePane* pDockingBar) const;
	virtual void OnDockToRecentPos();
	virtual BOOL DockFrame(CPaneFrameWnd* pDockedFrame, AFX_DOCK_METHOD dockMethod);
	virtual BOOL DockPane(CDockablePane* pDockedBar);

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);

	DECLARE_MESSAGE_MAP()
};
