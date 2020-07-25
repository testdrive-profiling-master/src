#include "stdafx.h"
#include "Main.h"
#include "VisualManagerEx.h"
#include "MainFrmBase.h"
#include "DocumentChildFrame.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNAMIC(CMainFrameBase, CMDIFrameWndEx)

BEGIN_MESSAGE_MAP(CMainFrameBase, CMDIFrameWndEx)
END_MESSAGE_MAP()

CMainFrameBase::CMainFrameBase()
{
}

CMainFrameBase::~CMainFrameBase()
{
}

BOOL CMainFrameBase::OnCloseMiniFrame(CPaneFrameWnd* pWnd) {
	if (!pWnd->GetPaneCount()) return TRUE;

	CDockingManager* pDockMan = GetDockingManager();
	if (pDockMan)
	{
		CObList allPanes;
		pDockMan->GetPaneList(allPanes, TRUE, NULL, TRUE);

		//List<CDocumentWnd*>
		// 리스트를 먼저 뽑아야 합니다.
		for (POSITION pos = allPanes.GetHeadPosition(); pos != NULL;)
		{
			CDockablePane* pPane = dynamic_cast<CDockablePane*>(allPanes.GetNext(pos));

			if (pPane && (pPane->GetParentMiniFrame() == pWnd)) {
				if (g_pDocumentList->IsDocument((const CDocumentWnd*)pPane)) {
					CDocumentWnd*	pDoc	= (CDocumentWnd*)pPane;
					CDocumentView*	pView	= pDoc->DocuementView();
					if (!pView->IsLocked())
						pDoc->DestroyWindow();
					else {
						CRect	rc;
						pDoc->GetWindowRect(&rc);
						pDoc->FloatPane(&rc);
					}
				}
			}
		}
	}
	return TRUE;
}

BOOL CMainFrameBase::OnCloseDockingPane(CDockablePane* pWnd) {
	if (g_pDocumentList->IsDocument((const CDocumentWnd*)pWnd)) {
		CDocumentWnd*	pDoc = (CDocumentWnd*)pWnd;
		CDocumentView*	pView = pDoc->DocuementView();
		if (!pView->IsLocked())
			pDoc->DestroyWindow();
			//pDoc->PostMessage(WM_CLOSE);
		else {
			CRect	rc;
			pDoc->GetWindowRect(&rc);
			pDoc->FloatPane(&rc);
			return FALSE;
		}
	}
	return TRUE;
}
#include "DocumentTabbedPane.h"
#include "MainFrm.h"

BOOL CMainFrameBase::PreTranslateMessage(MSG* pMsg) {
	switch (pMsg->message) {
	case WM_LBUTTONDBLCLK:
		CWnd* pWnd = FromHandle(pMsg->hwnd);
		CMFCTabCtrl* pTabCtrl = dynamic_cast<CMFCTabCtrl*>(pWnd);
		if (pTabCtrl) {
			CPoint click_pos = pMsg->pt;
			pTabCtrl->ScreenToClient(&click_pos);
			int iTab = pTabCtrl->GetTabFromPoint(click_pos);
			if (iTab != -1) {	// 탭 더블클릭
				pWnd = pTabCtrl->GetTabWnd(iTab);
				if (pWnd->IsKindOf(RUNTIME_CLASS(CDocumentChildFrame))) {
					// tabbed document 제거
					CDocumentWnd*	pDocWnd	= dynamic_cast<CDocumentWnd*>((dynamic_cast<CDocumentChildFrame*>(pWnd))->GetWindow(GW_CHILD));
					if (pDocWnd) {
						if (!pDocWnd->DocuementView()->IsLocked()) {
							pDocWnd->DestroyWindow();
						}
						return TRUE;
					}
				}else if (pWnd->IsKindOf(RUNTIME_CLASS(CDocumentWnd))) {
					// floating
					CDocumentWnd*	pDocWnd =dynamic_cast<CDocumentWnd*>(pWnd);
					if (!pDocWnd->DocuementView()->IsLocked()) {
						pDocWnd->DestroyWindow();
					}
					return TRUE;
				}
			}
			else {	// tab 이외의 영역 더블클릭
				static int aaa = 0;
				if ((pWnd = pTabCtrl->GetParent())) {
					if (pWnd->IsKindOf(RUNTIME_CLASS(CDocumentTabbedPane))) {
						// floating 된 도큐먼트 모음에서 tab 외부영역 더블클릭
						//g_pTestDrive->LogInfo(_T("CDocumentTabbedPane(%d)"), aaa++);
					}

					if ((pWnd = pWnd->GetParent()) && pWnd->IsKindOf(RUNTIME_CLASS(CMainFrame))) {
							// 메인프레임쪽 탭뷰 더블클릭할 때.
							AfxGetMainWnd()->PostMessage(WM_COMMAND, ID_VIEW_TOGGLE_MAXIMIZE);
					}
				}
				
			}
		}
	}
	return CMDIFrameWndEx::PreTranslateMessage(pMsg);
}

#ifdef _DEBUG
void CMainFrameBase::AssertValid() const
{
	CMDIFrameWndEx::AssertValid();
}

void CMainFrameBase::Dump(CDumpContext& dc) const
{
	CMDIFrameWndEx::Dump(dc);
}
#endif //_DEBUG

