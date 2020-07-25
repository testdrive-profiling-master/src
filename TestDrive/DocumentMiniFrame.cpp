#include "stdafx.h"
#include "Main.h"
#include "DocumentMiniFrame.h"
#include "DocumentTabbedPane.h"
#include <afxglobalutils.h>		// afxGlobalUtils 를 사용 가능하게 지원

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_SERIAL(CDocumentMiniFrame, CMultiPaneFrameWnd, VERSIONABLE_SCHEMA | 1)

BEGIN_MESSAGE_MAP(CDocumentMiniFrame, CMultiPaneFrameWnd)
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()

CDocumentMiniFrame::CDocumentMiniFrame()
{
}

CDocumentMiniFrame::~CDocumentMiniFrame()
{
}

BOOL CDocumentMiniFrame::CanBeDockedToPane(const CDockablePane* pDockingBar) const {
	return  pDockingBar && (pDockingBar->IsKindOf(RUNTIME_CLASS(CDocumentWnd)) || pDockingBar->IsKindOf(RUNTIME_CLASS(CDocumentTabbedPane)));
}
void CDocumentMiniFrame::OnDockToRecentPos() {
	ConvertToTabbedDocument();
}

BOOL CDocumentMiniFrame::DockFrame(CPaneFrameWnd* pDockedFrame, AFX_DOCK_METHOD dockMethod) {
	if (!pDockedFrame->IsKindOf(RUNTIME_CLASS(CDocumentMiniFrame)))
		return FALSE;
	return CMultiPaneFrameWnd::DockFrame(pDockedFrame, dockMethod);
}

BOOL CDocumentMiniFrame::DockPane(CDockablePane* pDockedBar) {
	if (!g_pDocumentList->IsDocument((const CDocumentWnd *)pDockedBar))
		return FALSE;
	return CMultiPaneFrameWnd::DockPane(pDockedBar);
}

void CDocumentMiniFrame::OnMouseMove(UINT nFlags, CPoint point)
{
	if (m_bBlockMove)
	{
		m_bBlockMove = false;
		return;
	}

	if (m_bCaptured)
	{
		if (!m_bIsMoving)
		{
			CPoint ptCurrent;
			GetCursorPos(&ptCurrent);

			if (ptCurrent.x != m_ptHot.x || ptCurrent.y != m_ptHot.y)
			{
				m_bIsMoving = TRUE;

				CDockablePane* pFirstBar = DYNAMIC_DOWNCAST(CDockablePane, GetFirstVisiblePane());

				if ((GetDockingMode() & DT_SMART) != 0 && pFirstBar != NULL && (pFirstBar->GetEnabledAlignment() & CBRS_ALIGN_ANY) != 0)
				{
					CDockingManager* pDockManager = m_pDockManager != NULL ? m_pDockManager : afxGlobalUtils.GetDockingManager(GetParent());
					if (pDockManager != NULL)
					{
						//pDockManager->StartSDocking(this);
						// Docking 윈도우 maker 를 보이지 않게 합니다.
						CSmartDockingManager* pSDManager	= pDockManager->GetSmartDockingManager();
						if(pSDManager){
							pSDManager->SetOuterRect(CRect(-65535, -65535, -65535, -65535));	// hide out rect guid.
							pSDManager->Start(this);
						}
					}
				}
			}
			else
			{
				return;
			}
		}

		afxGlobalUtils.m_bIsDragging = TRUE;

		AFX_DOCK_TYPE docktype = GetDockingMode();

		CSmartDockingManager* pSDManager = NULL;
		CDockingManager* pDockManager = NULL;
		if ((docktype & DT_SMART) != 0)
		{
			pDockManager = m_pDockManager != NULL ? m_pDockManager : afxGlobalUtils.GetDockingManager(GetParent());
			if (pDockManager != NULL)
			{
				CPoint pt(point);
				ClientToScreen(&pt);

				pSDManager = pDockManager->GetSmartDockingManager();
				if (pSDManager != NULL)
				{
					pSDManager->OnMouseMove(pt);
				}
			}
		}

		if ((docktype & DT_STANDARD) != 0)
		{
			m_dragFrameImpl.MoveDragFrame();
		}
		else if ((docktype & DT_IMMEDIATE) != 0)
		{
			CPoint ptScreen = point;
			ClientToScreen(&ptScreen);
			BOOL bSDockingIsOn = (docktype & DT_SMART) != 0 && pSDManager != NULL && pSDManager->IsStarted();
			CSmartDockingStandaloneGuide::SDMarkerPlace nHilitedSideNo = bSDockingIsOn ? pSDManager->GetHighlightedGuideNo() : CSmartDockingStandaloneGuide::sdNONE;
			BOOL bCtrlHeld = (GetKeyState(VK_CONTROL) < 0);
			BOOL bOnCaption = FALSE;
			CPaneFrameWnd* pOtherMiniFrameWnd = NULL;
			if (bSDockingIsOn)
			{
				CBasePane* pThisControlBar = NULL;
				CDockablePane* pBar = NULL;
				CWnd* pBarToPlaceTo = NULL;

				if (!bCtrlHeld)
				{
					pOtherMiniFrameWnd = pDockManager->FrameFromPoint(ptScreen, this, FALSE);
					pThisControlBar = DYNAMIC_DOWNCAST(CBasePane, GetPane());
					if (pOtherMiniFrameWnd != NULL)
					{
						CDockablePane* pOtherDockingControlBar = DYNAMIC_DOWNCAST(CDockablePane, pOtherMiniFrameWnd->GetFirstVisiblePane());
						if (pOtherDockingControlBar != NULL && pOtherDockingControlBar->CanBeAttached() && pThisControlBar->CanBeAttached() &&
							pOtherDockingControlBar->CanAcceptMiniFrame(this) && afxGlobalUtils.CanPaneBeInFloatingMultiPaneFrameWnd(pThisControlBar) &&
							pOtherDockingControlBar->GetEnabledAlignment() == pThisControlBar->GetEnabledAlignment())
						{
							CRect rcCaption;
							pOtherMiniFrameWnd->GetCaptionRect(rcCaption);
							CRect rcWnd;
							pOtherMiniFrameWnd->GetWindowRect(rcWnd);
							rcCaption.OffsetRect(rcWnd.TopLeft());
							if (rcCaption.PtInRect(ptScreen))
							{
								bOnCaption = TRUE;
								pBarToPlaceTo = pOtherDockingControlBar;
							}
						}
					}

					if (!bOnCaption)
					{
						pBar = DYNAMIC_DOWNCAST(CDockablePane, pDockManager->PaneFromPoint(ptScreen, CDockingManager::m_nDockSensitivity, true, NULL, TRUE, pThisControlBar));
						if (pBar != NULL)
						{
							BOOL bCanBeTabbed = (pBar->GetParentMiniFrame() != NULL && afxGlobalUtils.CanPaneBeInFloatingMultiPaneFrameWnd(pThisControlBar) || pBar->GetParentMiniFrame() == NULL) &&
								(pBar->GetEnabledAlignment() == pThisControlBar->GetEnabledAlignment() && pBar->CanAcceptPane(pThisControlBar) && pThisControlBar->CanBeAttached() && pBar->CanBeAttached());
							CRect rcWnd;
							pBar->GetWindowRect(&rcWnd);
							if (rcWnd.PtInRect(ptScreen) &&(ptScreen.y - rcWnd.top) < pBar->GetCaptionHeight())
							{
								bOnCaption = bCanBeTabbed;
								pBarToPlaceTo = pBar;
							}
							else
							{
								CRect rcTabsTop;
								CRect rcTabsBottom;
								pBar->GetTabArea(rcTabsTop, rcTabsBottom);
								if (rcTabsTop.PtInRect(ptScreen) || rcTabsBottom.PtInRect(ptScreen))
								{
									bOnCaption = bCanBeTabbed;
									pBarToPlaceTo = pBar;
								}
							}
						}
					}
				}

				if (bCtrlHeld)
				{
					pSDManager->Show(FALSE);
				}
				else if (bOnCaption)
				{
					pSDManager->Show(FALSE);
					m_dragFrameImpl.PlaceTabPreDocking(pBarToPlaceTo);
				}
				else
				{
					pSDManager->Show(TRUE);
					BOOL bShowCentralGroup = FALSE;
					if (pBar != NULL)
					{
						BOOL bCanFloatMulti = afxGlobalUtils.CanPaneBeInFloatingMultiPaneFrameWnd(pThisControlBar);
						BOOL bIsTargetBarFloatingMulti = pBar->IsInFloatingMultiPaneFrameWnd();

						if (bIsTargetBarFloatingMulti && !bCanFloatMulti)
						{
						}
						else
							if (pBar->CanAcceptPane(pThisControlBar) && ((pThisControlBar->GetEnabledAlignment() & pBar->GetCurrentAlignment()) != 0 &&
								pBar->GetDefaultPaneDivider() != NULL || pBar->GetParentMiniFrame() != NULL && pThisControlBar->GetEnabledAlignment() == pBar->GetEnabledAlignment()))
							{
								int nShowMiddleMarker = 0;
								if (CanBeAttached() && pBar->CanBeAttached() && pThisControlBar->GetEnabledAlignment() == pBar->GetEnabledAlignment())
								{
									nShowMiddleMarker = 1;
								}
								CRect rcBar;
								pBar->GetWindowRect(rcBar);

								DWORD dwEnabledAlignment = CDockingManager::m_bIgnoreEnabledAlignment ? CBRS_ALIGN_ANY : pThisControlBar->GetEnabledAlignment();

								pSDManager->MoveCentralGroup(rcBar, nShowMiddleMarker, dwEnabledAlignment);
								pSDManager->ShowCentralGroup(TRUE);
								bShowCentralGroup = TRUE;
							}
					}
					if (!bShowCentralGroup)
					{
						CRect rcClient;
						pDockManager->GetClientAreaBounds(rcClient);
						GetParent()->ClientToScreen(&rcClient);
						if (rcClient.PtInRect(ptScreen))
						{
							int nShowMiddleMarker = 0;

							CMDIFrameWndEx* pDockSite = DYNAMIC_DOWNCAST(CMDIFrameWndEx, pThisControlBar->GetDockSiteFrameWnd());
							if (pDockSite != NULL && pDockSite->AreMDITabs() && pDockSite->CanConvertControlBarToMDIChild() && pThisControlBar->CanBeTabbedDocument())
							{
								nShowMiddleMarker = 1;
							}

							pSDManager->MoveCentralGroup(rcClient, nShowMiddleMarker, pThisControlBar->GetEnabledAlignment());
							pSDManager->ShowCentralGroup(TRUE, pThisControlBar->GetEnabledAlignment());
							bShowCentralGroup = TRUE;
						}
					}
					if (!bShowCentralGroup && !(nHilitedSideNo >= CSmartDockingStandaloneGuide::sdLEFT && nHilitedSideNo <= CSmartDockingStandaloneGuide::sdBOTTOM))
					{
						pSDManager->ShowCentralGroup(FALSE, pThisControlBar->GetEnabledAlignment());
					}
				}
			}

			AFX_DOCK_TYPE dockType = GetDockingMode();

			if (bSDockingIsOn
				&& nHilitedSideNo != CSmartDockingStandaloneGuide::sdNONE
				&& !bCtrlHeld)
			{
				CPoint ptLastHot = m_dragFrameImpl.m_ptHot;   // save
				m_dragFrameImpl.MoveDragFrame();
				m_dragFrameImpl.m_ptHot = ptLastHot;  // restore
			}
			else
			{
				if (bSDockingIsOn && !bOnCaption)
				{
					m_dragFrameImpl.RemoveTabPreDocking();
					pSDManager->HidePlace();
				}

				if (!bOnCaption && MoveMiniFrame())
				{
					CPoint ptMouse;
					GetCursorPos(&ptMouse);

					CPoint ptOffset = ptMouse - m_dragFrameImpl.m_ptHot;

					CRect rect;
					GetWindowRect(&rect);
					rect.OffsetRect(ptOffset);
					//MoveWindow(rect);
					SetWindowPos(NULL, rect.left, rect.top, -1, -1, SWP_NOZORDER | SWP_NOSIZE | SWP_SHOWWINDOW);

					m_dragFrameImpl.m_ptHot = ptMouse;
					m_dragFrameImpl.m_rectDrag = rect;

				}
			}

			if ((dockType & DT_SMART) == 0)
			{
				SetCursor(AfxGetApp()->LoadStandardCursor(IDC_SIZEALL));
			}
		}

		afxGlobalUtils.m_bIsDragging = FALSE;
		return;
	}

	CPoint ptScreen = point;
	ClientToScreen(&ptScreen);

	OnTrackCaptionButtons(ptScreen);
	CWnd::OnMouseMove(nFlags, point);
}

#ifdef _DEBUG
void CDocumentMiniFrame::AssertValid() const
{
	CMultiPaneFrameWnd::AssertValid();
}

void CDocumentMiniFrame::Dump(CDumpContext& dc) const
{
	CMultiPaneFrameWnd::Dump(dc);
}
#endif //_DEBUG
