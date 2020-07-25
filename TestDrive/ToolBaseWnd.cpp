#include "stdafx.h"
#include "DocumentWnd.h"
#include "ToolBaseWnd.h"
#include "ToolMiniFrame.h"
#include "ToolTabbedPane.h"
#include "Resource.h"
#include "MainFrm.h"
#include "Main.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(CToolBaseWnd, CDockablePane)
CToolBaseWnd::CToolBaseWnd()
{
}

CToolBaseWnd::~CToolBaseWnd()
{
}

BEGIN_MESSAGE_MAP(CToolBaseWnd, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
END_MESSAGE_MAP()

int CToolBaseWnd::OnCreate(LPCREATESTRUCT lpCreateStruct){
	int iRet = CDockablePane::OnCreate(lpCreateStruct);
	SetMiniFrameRTC(RUNTIME_CLASS(CToolMiniFrame));
	SetTabbedPaneRTC(RUNTIME_CLASS(CToolTabbedPane));
	return iRet;
}

void CToolBaseWnd::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);

	if(theApp.GetMainWnd()) theApp.GetMainWnd()->Invalidate();
}

BOOL CToolBaseWnd::CanAcceptMiniFrame(CPaneFrameWnd* pMiniFrame) const {
	return (pMiniFrame->IsKindOf(RUNTIME_CLASS(CToolMiniFrame)));
}

BOOL CToolBaseWnd::CanAcceptPane(const CBasePane* pBar) const {
	return (pBar->IsKindOf(RUNTIME_CLASS(CToolBaseWnd)) || pBar->IsKindOf(RUNTIME_CLASS(CToolTabbedPane)));
}

BOOL CToolBaseWnd::CanBeDocked(CBasePane* pDockBar) const {
	return (pDockBar->IsKindOf(RUNTIME_CLASS(CToolBaseWnd)) || pDockBar->IsKindOf(RUNTIME_CLASS(CToolTabbedPane)));
}
