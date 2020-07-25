#include "stdafx.h"
#include "TestDriveImp.h"
#include "DocumentWnd.h"
#include "Resource.h"
#include "MainFrm.h"
#include "ArchiveFile.h"
#include "DocumentCtrl.h"

#include "BufferCtrl.h"
#include "ReportView.h"
#include "ChartCtrl.h"
#include "ButtonCtrl.h"
#include "HtmlCtrl.h"
#include "PropertyGridCtrl.h"
#include "FullPath.h"
#include "DocumentMiniFrame.h"
#include "DocumentTabbedPane.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNAMIC(CDocumentWnd, CDockablePane)

CDocumentWnd::CDocumentWnd()
{
}

CDocumentWnd::~CDocumentWnd()
{
}

BOOL CDocumentWnd::Create(LPCTSTR lpszName, CPaser* pPaser){
	CRect rect;
	g_pMainFrame->GetClientAreaWnd()->GetClientRect(&rect);

	if(!CDockablePane::Create(lpszName, g_pMainFrame, rect, TRUE, GetLastWindowID(g_pMainFrame), WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_FLOAT_MULTI))
		return FALSE;

	// Set docking...
	EnableDocking(CBRS_ALIGN_ANY);
	EnableAutohideAll(FALSE);

	if (!m_View.Create(this, lpszName, pPaser)) {
		return FALSE;
	}

	if(IsVisible()) ConvertToTabbedDocument();

	return TRUE;
}

void CDocumentWnd::SetTitle(LPCTSTR lpszTitle){
	if (!IsWindow(m_hWnd)) return;
	SetWindowText(lpszTitle);
	CWnd* pParent	= GetParent();
	if (pParent->IsKindOf(RUNTIME_CLASS(CDocumentTabbedPane))) {
		((CDocumentTabbedPane*)pParent)->SetWindowText(lpszTitle);
	}
}

BEGIN_MESSAGE_MAP(CDocumentWnd, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_CLOSE()
	ON_WM_DESTROY()
END_MESSAGE_MAP()

void CDocumentWnd::OnClose() {
	m_View.CloseWindow();
	CDockablePane::OnClose();
}

void CDocumentWnd::OnDestroy(){
	m_View.DestroyWindow();
	CDockablePane::OnDestroy();
	delete this;
}

int CDocumentWnd::OnCreate(LPCREATESTRUCT lpCreateStruct){
	int iRet	= CDockablePane::OnCreate(lpCreateStruct);
	SetMiniFrameRTC(RUNTIME_CLASS(CDocumentMiniFrame));
	SetTabbedPaneRTC(RUNTIME_CLASS(CDocumentTabbedPane));
	return iRet;
}

void CDocumentWnd::OnSize(UINT nType, int cx, int cy){
	CDockablePane::OnSize(nType, cx, cy);
	if(!cy || !cx) return;
	if (IsWindow(m_View.m_hWnd) && IsVisible())
		m_View.SetWindowPos(NULL, 0, 0, cx, cy, SWP_FRAMECHANGED);
}

BOOL CDocumentWnd::CanAcceptMiniFrame(CPaneFrameWnd* pMiniFrame) const {
	return (pMiniFrame != NULL) && (pMiniFrame->IsKindOf(RUNTIME_CLASS(CDocumentMiniFrame)));
}

BOOL CDocumentWnd::CanAcceptPane(const CBasePane* pBar) const {
	return (pBar != NULL) && (pBar->IsKindOf(RUNTIME_CLASS(CDocumentWnd)) || pBar->IsKindOf(RUNTIME_CLASS(CDocumentTabbedPane)));
}

BOOL CDocumentWnd::CanBeDocked(CBasePane* pDockBar) const {
	return FALSE;
}

BOOL CDocumentWnd::OnShowControlBarMenu(CPoint point){
	//CDockablePane::OnShowControlBarMenu(point);
	/*if (afxContextMenuManager == NULL)
	{
		return FALSE;
	}

	const UINT idFloating = (UINT) -102;
	const UINT idDocking = (UINT) -103;
	const UINT idAutoHide = (UINT) -104;
	const UINT idHide = (UINT) -105;
	const UINT idTabbed = (UINT) -106;

	CMenu menu;
	menu.CreatePopupMenu();

	{
		CString strItem;

		ENSURE(strItem.LoadString(IDS_AFXBARRES_FLOATING));
		menu.AppendMenu(MF_STRING, idFloating, strItem);

		ENSURE(strItem.LoadString(IDS_AFXBARRES_DOCKING));
		menu.AppendMenu(MF_STRING, idDocking, strItem);

		ENSURE(strItem.LoadString(IDS_AFXBARRES_TABBED));
		menu.AppendMenu(MF_STRING, idTabbed, strItem);

		ENSURE(strItem.LoadString(IDS_AFXBARRES_AUTOHIDE));
		menu.AppendMenu(MF_STRING, idAutoHide, strItem);

		ENSURE(strItem.LoadString(IDS_AFXBARRES_HIDE));
		menu.AppendMenu(MF_STRING, idHide, strItem);
	}

	if (!CanFloat())
	{
		menu.EnableMenuItem(idFloating, MF_GRAYED);
	}

	if (!CanAutoHide() || GetParentMiniFrame() != NULL)
	{
		menu.EnableMenuItem(idAutoHide, MF_GRAYED);
	}

	if (IsAutoHideMode())
	{
		menu.EnableMenuItem(idFloating, MF_GRAYED);
		menu.EnableMenuItem(idDocking, MF_GRAYED);
		menu.CheckMenuItem(idAutoHide, MF_CHECKED);
		menu.EnableMenuItem(idHide, MF_GRAYED);
	}

	CMDIFrameWndEx* pFrame = DYNAMIC_DOWNCAST(CMDIFrameWndEx, GetDockSiteFrameWnd());
	if (!CanBeTabbedDocument() || pFrame != NULL && pFrame->IsFullScreen())
	{
		menu.EnableMenuItem(idTabbed, MF_GRAYED);
	}

	if (IsMDITabbed())
	{
		menu.EnableMenuItem(idFloating, MF_GRAYED);
		menu.EnableMenuItem(idDocking, MF_GRAYED);
		menu.CheckMenuItem(idTabbed, MF_CHECKED);
	}

	if (IsFloating())
	{
		menu.CheckMenuItem(idFloating, MF_CHECKED);
	}
	else if (!IsAutoHideMode() && !IsMDITabbed())
	{
		menu.CheckMenuItem(idDocking, MF_CHECKED);
	}

	if ((GetEnabledAlignment() & CBRS_ALIGN_ANY) == 0)
	{
		menu.EnableMenuItem(idDocking, MF_GRAYED);
	}

	if (!CanBeClosed())
	{
		menu.EnableMenuItem(idHide, MF_GRAYED);
	}

	if (!OnBeforeShowPaneMenu(menu))
	{
		return FALSE;
	}

	HWND hwndThis = GetSafeHwnd();

	int nMenuResult = afxContextMenuManager->TrackPopupMenu(
		menu, point.x, point.y, this);

	if (!::IsWindow(hwndThis))
	{
		return TRUE;
	}

	if (!OnAfterShowPaneMenu(nMenuResult))
	{
		return TRUE;
	}

	switch(nMenuResult)
	{
	case idDocking:
		if (IsFloating())
		{
			CPaneFrameWnd* pMiniFrame = GetParentMiniFrame();
			if (pMiniFrame != NULL)
			{
				pMiniFrame->OnDockToRecentPos();
			}
		}
		break;

	case idFloating:
		{
			BOOL bWasFloated = FALSE;

			CBaseTabbedPane* pTabbedBar = DYNAMIC_DOWNCAST(CBaseTabbedPane, IsTabbed() ? GetParentTabbedPane() : (CDockablePane*)this);

			if (pTabbedBar != NULL)
			{
				ASSERT_VALID(pTabbedBar);

				CMFCBaseTabCtrl* pTabWnd = pTabbedBar->GetUnderlyingWindow();
				if (pTabWnd != NULL)
				{
					ASSERT_VALID(pTabWnd);

					const int nTabID = pTabWnd->GetActiveTab();
					CWnd* pWnd = pTabWnd->GetTabWnd(nTabID);

					if (pWnd != NULL && pTabWnd->IsTabDetachable(nTabID))
					{
						bWasFloated = pTabbedBar->DetachPane(pWnd, FALSE);
						if (bWasFloated)
						{
							if (pTabWnd->GetTabsNum() > 0 &&
								pTabWnd->GetVisibleTabsNum() == 0)
							{
								pTabbedBar->ShowPane(FALSE, FALSE, FALSE);
							}
						}
					}
				}
			}

			if (!bWasFloated)
			{
				FloatPane(m_recentDockInfo.m_rectRecentFloatingRect);
			}
		}
		break;

	case idAutoHide:
		ToggleAutoHide();
		break;

	case idHide:
		OnPressCloseButton();
		break;

	case idTabbed:
		if (IsMDITabbed())
		{
			CMDIChildWndEx* pMDIChild = DYNAMIC_DOWNCAST(CMDIChildWndEx, GetParent());
			if (pMDIChild == NULL)
			{
				ASSERT(FALSE);
				return FALSE;
			}

			CMDIFrameWndEx* pTabbedFrame = DYNAMIC_DOWNCAST(CMDIFrameWndEx, GetDockSiteFrameWnd());
			if (pTabbedFrame == NULL)
			{
				ASSERT(FALSE);
				return FALSE;
			}

			pTabbedFrame->TabbedDocumentToControlBar(pMDIChild);
		}
		else
		{
			ConvertToTabbedDocument();
		}
	}*/

	return TRUE;
}

void CDocumentWnd::OnAfterDock(CBasePane* pBar, LPCRECT lpRect, AFX_DOCK_METHOD dockMethod){
	CDockablePane::OnAfterDock(pBar, lpRect, dockMethod);
	CBaseTabbedPane* tabbedPane = GetParentTabbedPane();
	if (!tabbedPane) return;

	CMFCTabCtrl* tabCtrl = (CMFCTabCtrl*)(tabbedPane->GetUnderlyingWindow());
	if (!tabCtrl) return;

	if(!tabCtrl->IsVS2005Style()){
		tabCtrl->ModifyTabStyle(CMFCTabCtrl::STYLE_3D_VS2005);
	}
}

void CDocumentWnd::OnAfterFloat(){
	CDockablePane::OnAfterFloat();
	{
		CRect rect;
		g_pMainFrame->GetClientAreaWnd()->GetWindowRect(&rect);
		GetParent()->SetWindowPos(NULL, rect.left + 24, rect.top + 24, rect.Width(), rect.Height(), SWP_NOZORDER|SWP_FRAMECHANGED);
	}
}

BOOL CDocumentWnd::LoadState(LPCTSTR lpszProfileName, int nIndex, UINT uiID){
	return TRUE;
}

BOOL CDocumentWnd::SaveState(LPCTSTR lpszProfileName, int nIndex, UINT uiID){
	return TRUE;
}

void CDocumentWnd::OnProcessDblClk(){
	CDockablePane::OnProcessDblClk();
}