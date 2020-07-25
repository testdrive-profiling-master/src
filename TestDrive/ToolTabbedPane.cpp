#include "stdafx.h"
#include "Main.h"

#include "ToolTabbedPane.h"
#include "ToolMiniFrame.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_SERIAL(CToolTabbedPane, CTabbedPane, VERSIONABLE_SCHEMA | 4)

BEGIN_MESSAGE_MAP(CToolTabbedPane, CTabbedPane)
	ON_WM_CREATE()
END_MESSAGE_MAP()


CToolTabbedPane::CToolTabbedPane()
{
}

CToolTabbedPane::~CToolTabbedPane()
{
}

BOOL CToolTabbedPane::CanAcceptPane(const CBasePane* pBar) const {
	return pBar && (pBar->IsKindOf(RUNTIME_CLASS(CToolBaseWnd)) || pBar->IsKindOf(RUNTIME_CLASS(CToolTabbedPane)));
}

BOOL CToolTabbedPane::CanAcceptMiniFrame(CPaneFrameWnd* pMiniFrame) const {
	return pMiniFrame && (pMiniFrame->IsKindOf(RUNTIME_CLASS(CToolMiniFrame)));
}

BOOL CToolTabbedPane::CanBeDocked(CBasePane* pDockBar) const {
	return FALSE;
}

int CToolTabbedPane::OnCreate(LPCREATESTRUCT lpCreateStruct){
	int iRet = CTabbedPane::OnCreate(lpCreateStruct);
	SetMiniFrameRTC(RUNTIME_CLASS(CToolMiniFrame));
	SetTabbedPaneRTC(RUNTIME_CLASS(CToolTabbedPane));
	return iRet;
}

#ifdef _DEBUG
void CToolTabbedPane::AssertValid() const
{
	CTabbedPane::AssertValid();
}

void CToolTabbedPane::Dump(CDumpContext& dc) const
{
	CTabbedPane::Dump(dc);
}
#endif //_DEBUG

