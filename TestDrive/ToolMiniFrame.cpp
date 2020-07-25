#include "stdafx.h"
#include "Main.h"
#include "ToolMiniFrame.h"
#include "ToolTabbedPane.h"
#include <afxglobalutils.h>		// afxGlobalUtils 를 사용 가능하게 지원

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_SERIAL(CToolMiniFrame, CMultiPaneFrameWnd, VERSIONABLE_SCHEMA | 3)

BEGIN_MESSAGE_MAP(CToolMiniFrame, CMultiPaneFrameWnd)
	ON_WM_CREATE()
END_MESSAGE_MAP()


CToolMiniFrame::CToolMiniFrame()
{
}

CToolMiniFrame::~CToolMiniFrame()
{
}

BOOL CToolMiniFrame::CanBeDockedToPane(const CDockablePane* pDockingBar) const {
	return  pDockingBar->IsKindOf(RUNTIME_CLASS(CToolTabbedPane));
}
BOOL CToolMiniFrame::DockFrame(CPaneFrameWnd* pDockedFrame, AFX_DOCK_METHOD dockMethod) {
	if (!pDockedFrame->IsKindOf(RUNTIME_CLASS(CToolMiniFrame)))
		return FALSE;
	return CMultiPaneFrameWnd::DockFrame(pDockedFrame, dockMethod);
}

BOOL CToolMiniFrame::DockPane(CDockablePane* pDockedBar) {
	if (g_pDocumentList->IsDocument((const CDocumentWnd *)pDockedBar))
		return FALSE;
	return CMultiPaneFrameWnd::DockPane(pDockedBar);
}

int CToolMiniFrame::OnCreate(LPCREATESTRUCT lpCreateStruct){
	int iRet	= CMultiPaneFrameWnd::OnCreate(lpCreateStruct);
	return iRet;
}

#ifdef _DEBUG
void CToolMiniFrame::AssertValid() const
{
	CMultiPaneFrameWnd::AssertValid();
}

void CToolMiniFrame::Dump(CDumpContext& dc) const
{
	CMultiPaneFrameWnd::Dump(dc);
}
#endif //_DEBUG
