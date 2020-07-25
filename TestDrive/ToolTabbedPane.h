#pragma once

class CToolTabbedPane : public CTabbedPane
{
	DECLARE_SERIAL(CToolTabbedPane)
public:
	CToolTabbedPane();
	virtual ~CToolTabbedPane();

	virtual BOOL CanAcceptPane(const CBasePane* pBar) const;
	virtual BOOL CanAcceptMiniFrame(CPaneFrameWnd* pMiniFrame) const;
	virtual BOOL CanBeDocked(CBasePane* pDockBar) const;
	virtual BOOL CanBeClosed() const { return FALSE; }

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

	DECLARE_MESSAGE_MAP()
};
