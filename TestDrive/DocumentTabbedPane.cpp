#include "stdafx.h"
#include "Main.h"

#include "DocumentTabbedPane.h"
#include "DocumentMiniFrame.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_SERIAL(CDocumentTabbedPane, CTabbedPane, VERSIONABLE_SCHEMA | 2)

BEGIN_MESSAGE_MAP(CDocumentTabbedPane, CTabbedPane)
	ON_WM_CREATE()
END_MESSAGE_MAP()


CDocumentTabbedPane::CDocumentTabbedPane()
{
}

CDocumentTabbedPane::~CDocumentTabbedPane()
{
}

BOOL CDocumentTabbedPane::CanAcceptPane(const CBasePane* pBar) const {
	return pBar && (pBar->IsKindOf(RUNTIME_CLASS(CDocumentWnd)) || pBar->IsKindOf(RUNTIME_CLASS(CDocumentTabbedPane)));
}

BOOL CDocumentTabbedPane::CanAcceptMiniFrame(CPaneFrameWnd* pMiniFrame) const {
	return pMiniFrame && (pMiniFrame->IsKindOf(RUNTIME_CLASS(CDocumentMiniFrame)));
}

BOOL CDocumentTabbedPane::CanBeDocked(CBasePane* pDockBar) const {
	return FALSE;
}

int CDocumentTabbedPane::OnCreate(LPCREATESTRUCT lpCreateStruct){
	int iRet	= CTabbedPane::OnCreate(lpCreateStruct);
	SetMiniFrameRTC(RUNTIME_CLASS(CDocumentMiniFrame));
	SetTabbedPaneRTC(RUNTIME_CLASS(CDocumentTabbedPane));
	return iRet;
}

#ifdef _DEBUG
void CDocumentTabbedPane::AssertValid() const
{
	CTabbedPane::AssertValid();
}

void CDocumentTabbedPane::Dump(CDumpContext& dc) const
{
	CTabbedPane::Dump(dc);
}
#endif //_DEBUG

