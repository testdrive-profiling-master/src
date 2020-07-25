#include "stdafx.h"
#include "VisualManagerEx.h"

IMPLEMENT_DYNCREATE(CVisualManagerEx, CMFCVisualManagerOffice2007)

CVisualManagerEx::CVisualManagerEx(){

}

CVisualManagerEx::~CVisualManagerEx(){

}

void CVisualManagerEx::OnDrawRibbonCaption(CDC* pDC, CMFCRibbonBar* pBar, CRect rectCaption, CRect rectText){
	if (!CanDrawImage())
	{
		CMFCVisualManagerOffice2003::OnDrawRibbonCaption(pDC, pBar, rectCaption, rectText);
		return;
	}

	ASSERT_VALID(pDC);
	ASSERT_VALID(pBar);

	CWnd* pWnd = pBar->GetParent();
	ASSERT_VALID(pWnd);

	const DWORD dwStyle   = pWnd->GetStyle();
	const DWORD dwStyleEx = pWnd->GetExStyle();

	const BOOL bIsRTL     = (dwStyleEx & WS_EX_LAYOUTRTL) == WS_EX_LAYOUTRTL;
	const BOOL bActive    = IsWindowActive(pWnd);
	const BOOL bGlass	  = pBar->IsTransparentCaption();

	//int nExtraWidth = 0;
	{
		CSize szSysBorder(GetSystemBorders(TRUE));
		CRect rectCaption1(rectCaption);
		CRect rectBorder(m_ctrlMainBorderCaption.GetParams().m_rectSides);
		CRect rectQAT = pBar->GetQuickAccessToolbarLocation();

		if (rectQAT.left > rectQAT.right)
		{
			rectText.left = rectQAT.left + 1;
		}
		
		rectCaption1.InflateRect(szSysBorder.cx, szSysBorder.cy, szSysBorder.cx, 0);

		BOOL bHide  = (pBar->GetHideFlags() & AFX_RIBBONBAR_HIDE_ALL) != 0;
		BOOL bExtra = !bHide && pBar->IsQuickAccessToolbarOnTop() && rectQAT.left < rectQAT.right && (!pBar->IsQATEmpty() || IsBeta1());

		if (!bGlass)
		{
			if (IsBeta())
			{
				COLORREF clr1  = bActive ? m_clrAppCaptionActiveStart : m_clrAppCaptionInactiveStart;
				COLORREF clr2  = bActive ? m_clrAppCaptionActiveFinish : m_clrAppCaptionInactiveFinish;

				CRect rectCaption2(rectCaption1);
				rectCaption2.DeflateRect(rectBorder.left, rectBorder.top, rectBorder.right, rectBorder.bottom);

				CDrawingManager dm(*pDC);
				dm.Fill4ColorsGradient(rectCaption2, clr1, clr2, clr2, clr1, FALSE);

				m_ctrlMainBorderCaption.DrawFrame(pDC, rectCaption1, bActive ? 0 : 1);
			}
			else
			{
				m_ctrlMainBorderCaption.Draw(pDC, rectCaption1, bActive ? 0 : 1);
			}
		}
		
		if (bExtra)
		{
			CMFCControlRenderer* pCaptionQA = bGlass ? &m_ctrlRibbonCaptionQA_Glass : &m_ctrlRibbonCaptionQA;

			if (pCaptionQA->IsValid())
			{
				const CMFCControlRendererInfo& params = pCaptionQA->GetParams();

				CRect rectQAFrame(rectQAT);
				rectQAFrame.InflateRect(params.m_rectCorners.left - 2, 1, 0, 1);
				rectQAFrame.right   = pBar->GetQATCommandsLocation().right + GetRibbonQuickAccessToolBarRightMargin() + 1;

				if (rectQAFrame.Height() < params.m_rectImage.Height())
				{
					rectQAFrame.top = rectQAFrame.bottom - params.m_rectImage.Height();
				}

				if (bGlass)
				{
					const int dxFrame = GetSystemMetrics(SM_CXSIZEFRAME) / 2;

					const int nTop = afxGlobalData.GetRibbonImageScale () != 1. ? -2 : 1;
					rectQAFrame.DeflateRect (1, nTop, dxFrame, 0);
				}

				pCaptionQA->Draw(pDC, rectQAFrame, bActive ? 0 : 1);
			}
		}
		else if (bHide)
		{
			/*
			HICON hIcon = afxGlobalUtils.GetWndIcon(pWnd);

			if (hIcon != NULL)
			{
				CSize szIcon(::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON));

				CRect rectIcon(rectCaption.TopLeft(), CSize(min(::GetSystemMetrics(SM_CYCAPTION), rectCaption.Height()), rectCaption.Height()));

				long x = rectCaption.left + max(0, (rectIcon.Width()  - szIcon.cx) / 2);
				long y = rectCaption.top  + max(0, (rectIcon.Height() - szIcon.cy) / 2);

				pDC->DrawState(CPoint(x, y), szIcon, hIcon, DSS_NORMAL, (CBrush*)NULL);

				if (rectText.left < rectIcon.right)
				{
					rectText.left = rectIcon.right;
				}
			}
			*/
		}
	}

	CString strText;
	pWnd->GetWindowText(strText);

	CFont* pOldFont = (CFont*)pDC->SelectObject(&m_AppCaptionFont);
	ENSURE(pOldFont != NULL);

	CString strTitle(strText);
	CString strDocument;

	BOOL bPrefix = FALSE;
	if ((dwStyle & FWS_ADDTOTITLE) == FWS_ADDTOTITLE)
	{
		bPrefix = (dwStyle & FWS_PREFIXTITLE) == FWS_PREFIXTITLE;
		CFrameWnd* pFrameWnd = DYNAMIC_DOWNCAST(CFrameWnd, pWnd);

		if (pFrameWnd != NULL)
		{
			strTitle = pFrameWnd->GetTitle();

			if (!strTitle.IsEmpty())
			{
				int pos = strText.Find(strTitle);

				if (pos != -1)
				{
					if (strText.GetLength() > strTitle.GetLength())
					{
						if (pos == 0)
						{
							bPrefix = FALSE; // avoid exception
							strTitle = strText.Left(strTitle.GetLength() + 3);
							strDocument = strText.Right(strText.GetLength() - strTitle.GetLength());
						}
						else
						{
							strTitle = strText.Right(strTitle.GetLength() + 3);
							strDocument = strText.Left(strText.GetLength() - strTitle.GetLength());
						}
					}
				}
			}
			else
			{
				strDocument = strText;
			}
		}
	}

	DrawNcText(pDC, rectText, strTitle, strDocument, bPrefix, bActive, bIsRTL, m_bNcTextCenter, bGlass,
		10, RGB(16, 16, 16));

	pDC->SelectObject(pOldFont);
}


COLORREF CVisualManagerEx::GetBkColor()
{
	return m_bIsStandardWinXPTheme ? m_clrBarGradientDark : m_colorToolBarCornerBottom;
}