// ViewTabCtrl.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "Main.h"
#include "MainFrm.h"
#include "DocumentCtrl.h"
#include "TestDriveImp.h"
#include "DocumentTabbedPane.h"
#include "DocumentMiniFrame.h"
#include "DocumentChildFrame.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CDocumentCtrl*		g_pDocumentList	= NULL;

CDocumentCtrl::CDocumentCtrl(){
	g_pDocumentList = this;
}

CDocumentCtrl::~CDocumentCtrl(){
	g_pDocumentList = NULL;
}

void CDocumentCtrl::SetActive(CDocumentView* pDoc){
	if(pDoc->IsVisible()){
		CDocumentWnd*	pWnd	= (CDocumentWnd*)(pDoc->GetParent());

		pWnd->SetForegroundWindow();

		CWnd* pParent = pWnd->GetParent();
		if (pParent->IsKindOf(RUNTIME_CLASS(CDocumentChildFrame))) {
			CDocumentChildFrame* pFrame = (CDocumentChildFrame*)pParent;
			pParent	= pFrame->GetParent();
			if (pParent->IsKindOf(RUNTIME_CLASS(CMDIClientAreaWnd))){
				CMDIClientAreaWnd* pClinetArea = (CMDIClientAreaWnd*)pParent;
				pClinetArea->SetActiveTab(pFrame->m_hWnd);
			}
		}else
		if (pParent->IsKindOf(RUNTIME_CLASS(CDocumentMiniFrame))) {
			CDocumentMiniFrame* pFrame = (CDocumentMiniFrame*)pParent;
			pParent	= pFrame->GetParent();
			// floating windows
			if (pParent->IsKindOf(RUNTIME_CLASS(CMainFrame))){
				pFrame->SetForegroundWindow();
				pFrame->SetActiveWindow();
			}
		}else
		if (pParent->IsKindOf(RUNTIME_CLASS(CMainFrame))) {
			// split window to mainframe
			pWnd->SetActiveWindow();
		}else
			// paned floating miniframe
		if (pParent->IsKindOf(RUNTIME_CLASS(CMFCTabCtrl))) {
			CMFCTabCtrl* pTabControl = (CMFCTabCtrl*)pParent;
			pTabControl->SetActiveTab(pTabControl->GetTabFromHwnd(pWnd->m_hWnd));

			pParent = pTabControl->GetParent();

			if (pParent->IsKindOf(RUNTIME_CLASS(CDocumentTabbedPane))) {
				pParent = ((CDocumentTabbedPane*)pParent)->GetParent();

				if (pParent->IsKindOf(RUNTIME_CLASS(CDocumentMiniFrame))) {
					CDocumentMiniFrame* pMiniFrame = (CDocumentMiniFrame*)pParent;
					pParent = pMiniFrame->GetParent();

					if (pParent->IsKindOf(RUNTIME_CLASS(CMainFrame))) {
						pMiniFrame->SetForegroundWindow();
					}
				}
			}
		}
	}
}

BOOL CDocumentCtrl::SetTitle(LPCTSTR lpszDocName, LPCTSTR lpszTitle){
	CDocumentView* pDoc	= Find(lpszDocName);
	if(!pDoc) return FALSE;
	pDoc->DocumentTitle(lpszTitle);
	return TRUE;
}

BOOL CDocumentCtrl::IsDocument(const CDocumentWnd* pWnd){
	if (pWnd->IsKindOf(RUNTIME_CLASS(CDocumentTabbedPane)) ||
		pWnd->IsKindOf(RUNTIME_CLASS(CDocumentMiniFrame))) {
		return TRUE;
	}
	for(DOCUMENT_LIST::iterator i= m_List.begin();i!= m_List.end();i++){
		const CDocumentWnd*	pDocWnd	= (const CDocumentWnd*)((*i)->GetParent());
		if(pWnd	== pDocWnd)
			return TRUE;
	}
	return FALSE;
}

CDocumentView* CDocumentCtrl::Find(LPCTSTR lpszName){
	if (lpszName) {
		for (DOCUMENT_LIST::iterator i = m_List.begin(); i != m_List.end(); i++) {
			CDocumentView* pDoc = *i;
			if (!pDoc->GetName().Compare(lpszName))
				return pDoc;
		}
	}
	else if(m_List.size()) return *m_List.begin();

	return NULL;
}

#include "Main.h"
CDocumentView* CDocumentCtrl::Add(LPCTSTR lpszName, CPaser* pPaser){
	CDocumentView* pDoc	= Find(lpszName);
	if(pDoc){
		pDoc->SetForegroundDocument();
		return pDoc;
	}

	CDocumentWnd*	pWnd	= new CDocumentWnd;

	if(pWnd){
		pDoc	= pWnd->DocuementView();
		pDoc->Lock();
		
		if(!pWnd->Create(lpszName, pPaser)){
			if (IsWindow(pWnd->m_hWnd)) {
				pWnd->DestroyWindow();
			} else {
				delete pWnd;
			}
			
			return NULL;
		}

		pDoc->UnLock();
	}

	return pDoc;
}

void CDocumentCtrl::Add(CDocumentView* pDoc){
	m_List.push_back(pDoc);
}

BOOL CDocumentCtrl::Remove(LPCTSTR lpszName, BOOL bForce){
	CDocumentView* pDoc = NULL;
	while((pDoc = Find(NULL))){
		if(lpszName) if(pDoc->GetName().Compare(lpszName)) continue;
		if(!pDoc->TryUnlock(bForce)){
			g_pTestDrive->LogOut(_TEXT_(_S(DOCUMENT_UPDATE_NOT_ALLOWED), pDoc->DocumentTitle()));
			return FALSE;
		}
		{	// delete window
			CDocumentWnd*	pDocWnd = (CDocumentWnd*)(pDoc->GetParent());
			CWnd*			pParent	= pDocWnd->GetParent();
			if (!pParent->IsKindOf(RUNTIME_CLASS(CDocumentChildFrame))) {
				pDocWnd->ConvertToTabbedDocument(FALSE);
			}

			m_List.remove(pDoc);
			pDocWnd->DestroyWindow();
		}
	}
	return TRUE;
}

void CDocumentCtrl::Remove(CDocumentView* pDoc){
	m_List.remove(pDoc);
}

BOOL CDocumentCtrl::RemoveAll(BOOL bForceToExit){
	return Remove(NULL, bForceToExit);
}

int CDocumentCtrl::GetCount(void){
	return (int)m_List.size();
}

void CDocumentCtrl::Show(LPCTSTR lpszName, BOOL bShow){
	CDocumentView* pDoc = Find(lpszName);
	if(pDoc){
		CDocumentWnd* pWnd	= (CDocumentWnd*)pDoc->GetParent();
		if (pWnd->IsVisible() == bShow) return;

		if (pWnd->IsMDITabbed() && !bShow) {
			pWnd->FloatPane(CRect(0, 0, 10, 10));
		}
		
		pWnd->ShowPane(bShow, FALSE, bShow);

		if (pWnd->IsFloating() && bShow) {
			pWnd->ConvertToTabbedDocument();
		}
	}
}

/*
BOOL CDocumentCtrl::OnCommand(WPARAM wParam, LPARAM lParam){
	//this->OnNotify()
	switch(wParam){
	case 65534:	// 특정 탭 지우기
		{
			int iTab;
			CDocumentView* pDoc = Find((HWND)lParam, &iTab);
			if(pDoc){
				Remove(pDoc->GetName());
			}
		}
		break;
	case 65535:	// 현재 활성 탭 지우기
		{
			CDocumentView* pDoc	= (CDocumentView*)GetTabWnd(GetActiveTab());
			if(!pDoc) return 0;
			if(pDoc->IsLocked()){
				CString msg;
				if(pDoc->TryUnlock()) return 0;
				msg.LoadString(IDS_DOCUMENT_AUTO_CLOSE);
				int iRet = AfxMessageBox(msg, MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2);
				if(iRet == IDYES) pDoc->SetAutoClose(TRUE);
				else if(iRet == IDNO) pDoc->SetAutoClose(FALSE);
				return 0;
			}
			Remove(pDoc->GetName());
		}
		break;
	default:
		return CMFCTabCtrl::OnCommand(wParam, lParam);
	}
	return 0;

}*/

/*
void CDocumentCtrl::OnLButtonDblClk(UINT nFlags, CPoint point){
	// 더블 클릭시 도큐먼트 제거
	if(!IsPtInTabArea(point)) return;	// 탭 영역 밖이면 반환

	int iTab = GetTabFromPoint(point);
	CDocumentView* pDoc;
	if(iTab<0){
		// 탭 구역의 빈구간
		//AfxGetMainWnd()->PostMessage(WM_COMMAND, ID_TOGGLE_VIEW_MAXIMIZE);
	}else{
		// 탭이 존재하는 구간
		pDoc = GetView(iTab);
		if(pDoc){
			if(iTab) MoveTab(iTab, 0);
			CString sName(pDoc->GetName());
			Remove(sName);
		}
	}
}
*/