#pragma once

class CToolBaseWnd : public CDockablePane
{
	DECLARE_DYNAMIC(CToolBaseWnd)
public:
	CToolBaseWnd();
	virtual ~CToolBaseWnd();

	virtual BOOL CanBeTabbedDocument() const {return FALSE;}
	virtual BOOL CanBeClosed() const {return FALSE;}
	virtual BOOL CanAcceptMiniFrame(CPaneFrameWnd* pMiniFrame) const;
	virtual BOOL CanAcceptPane(const CBasePane* pBar) const;
	virtual BOOL CanBeDocked(CBasePane* pDockBar) const;

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);

	DECLARE_MESSAGE_MAP()
};

