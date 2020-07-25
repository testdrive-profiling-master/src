#pragma once

class CDocumentTabbedPane : public CTabbedPane
{
	DECLARE_SERIAL(CDocumentTabbedPane)
public:
	CDocumentTabbedPane();
	virtual ~CDocumentTabbedPane();

	virtual BOOL CanAcceptPane(const CBasePane* pBar) const;
	virtual BOOL CanAcceptMiniFrame(CPaneFrameWnd* pMiniFrame) const;
	virtual BOOL CanBeDocked(CBasePane* pDockBar) const;

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	DECLARE_MESSAGE_MAP()
};
