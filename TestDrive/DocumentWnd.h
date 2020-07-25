#pragma once
#include "ViewObject.h"
#include "WatchDogFile.h"
#include "Paser.h"
#include "Memory.h"
#include "DocumentView.h"

class CDocumentWnd :
	public CDockablePane
{
	DECLARE_DYNAMIC(CDocumentWnd)

public:
	CDocumentWnd();
	virtual ~CDocumentWnd();

	BOOL Create(LPCTSTR lpszName, CPaser* pPaser);
	CDocumentView* DocuementView(void)	{return &m_View;}
	void SetTitle(LPCTSTR lpszTitle);

	virtual BOOL HasAutoHideMode() const { return FALSE; }
	virtual BOOL CanAutoHide() const {return FALSE; }
	virtual BOOL CanAcceptMiniFrame(CPaneFrameWnd* pMiniFrame) const;
	virtual BOOL CanAcceptPane(const CBasePane* pBar) const;
	virtual BOOL CanBeDocked(CBasePane* pDockBar) const;
	virtual BOOL OnShowControlBarMenu(CPoint point);
	virtual void OnAfterDock(CBasePane* pBar, LPCRECT lpRect, AFX_DOCK_METHOD dockMethod);
	virtual void OnAfterFloat();
	virtual BOOL LoadState(LPCTSTR lpszProfileName = NULL, int nIndex = -1, UINT uiID = (UINT) -1);
	virtual BOOL SaveState(LPCTSTR lpszProfileName = NULL, int nIndex = -1, UINT uiID = (UINT) -1);
	virtual void OnProcessDblClk();

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnClose();
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	DECLARE_MESSAGE_MAP()

	CDocumentView	m_View;

};

