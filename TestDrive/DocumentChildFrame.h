#pragma once

class CDocumentChildFrame : public CMDIChildWndEx
{
	DECLARE_DYNCREATE(CDocumentChildFrame)
public:
	CDocumentChildFrame();
	virtual ~CDocumentChildFrame();

	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void ActivateFrame(int nCmdShow = -1);
	virtual BOOL CanShowOnTaskBarTabs() { return FALSE; }
	

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	DECLARE_MESSAGE_MAP()
};
