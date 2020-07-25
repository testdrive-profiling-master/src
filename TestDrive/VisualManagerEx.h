#pragma once
#include "Main.h"

class CVisualManagerEx : public CMFCVisualManagerOffice2007 {
	DECLARE_DYNCREATE(CVisualManagerEx)
public:
	CVisualManagerEx();
	virtual ~CVisualManagerEx();

	virtual void OnDrawRibbonCaption(CDC* pDC, CMFCRibbonBar* pBar, CRect rectCaption, CRect rectText);

	COLORREF GetBkColor();
};