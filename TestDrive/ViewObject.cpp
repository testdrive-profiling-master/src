#include "stdafx.h"
#include "Main.h"
#include "ViewObject.h"
#include "BufferCtrl.h"
#include "ChartCtrl.h"
#include "ReportView.h"
#include "ButtonCtrl.h"
#include "HtmlCtrl.h"
#include "DocumentView.h"

IMPLEMENT_DYNAMIC(CViewObject, CWnd)

CViewObject::CViewObject()	: m_bOutline(FALSE), m_iViewType(TDOBJECT_NONE), m_bShow(TRUE)
{
}

CViewObject::~CViewObject(){
	if(IsWindow(m_hWnd)) DestroyWindow();
}

ITDLayout* CViewObject::GetLayout(void){
	return &m_Layout;
}

OBJECT_TYPE CViewObject::GetType(void){
	return m_iViewType;
}

void CViewObject::SetName(LPCTSTR lpszName){
	if(!lpszName) m_sViewName.Empty();
	else m_sViewName = lpszName;
}

void CViewObject::SetEnable(BOOL bEnable){
	EnableWindow(bEnable);
}

void CViewObject::Show(BOOL bEnable){
	ShowWindow(bEnable ? SW_SHOW : SW_HIDE);
	CDocumentView* pDoc = (CDocumentView*)GetParent();
	m_bShow = bEnable;
	pDoc->UpdateLayout();
}

BOOL CViewObject::IsVisible(void){
	return m_bShow;
}

void CViewObject::Release(void){
	CDocumentView* pDoc = (CDocumentView*)GetParent();
	if(pDoc) pDoc->DeleteObject(this);
}

CViewObject* CViewObject::New(OBJECT_TYPE iViewType, CWnd* pParentWnd, CPaser* pPaser){
	CViewObject* pVO	= NULL;

	switch(iViewType){
	case TDOBJECT_BUFFER:		pVO = new CBufferCtrl;		break;
	case TDOBJECT_CHART:		pVO = new CChartCtrl;		break;
	case TDOBJECT_REPORT:		pVO = new CReportView;		break;
	case TDOBJECT_BUTTON:		pVO = new CButtonCtrl;		break;
	case TDOBJECT_HTML:			pVO = new CHtmlCtrl;		break;
	default:
ERROR_OUT:
		if(pVO) delete pVO;
		return NULL;
	}

	if(pVO){
		pVO->m_iViewType	= iViewType;
	
		if(pPaser){	// Name & Layout parsing
			
			if(!pPaser->TokenOut(TD_DELIMITER_SOPEN)) goto ERROR_OUT;
			{
				TCHAR			token[MAX_PATH];
				if(!pPaser->GetTokenString(token)) goto ERROR_OUT;
				pVO->m_sViewName	= token;
			}
			{
				int x, y;
				if(!pPaser->TokenOut(TD_DELIMITER_COMMA)) goto ERROR_OUT;
				if(!pPaser->GetTokenInt(&x)) goto ERROR_OUT;
				if(!pPaser->TokenOut(TD_DELIMITER_COMMA)) goto ERROR_OUT;
				if(!pPaser->GetTokenInt(&y)) goto ERROR_OUT;
				pVO->m_Layout.SetPosition(x, y);
			}
			{
				int width, height;
				if(!pPaser->TokenOut(TD_DELIMITER_COMMA)) goto ERROR_OUT;
				if(!pPaser->GetTokenInt(&width)) goto ERROR_OUT;
				if(!pPaser->TokenOut(TD_DELIMITER_COMMA)) goto ERROR_OUT;
				if(!pPaser->GetTokenInt(&height)) goto ERROR_OUT;
				pVO->m_Layout.SetSize(width, height);
			}
			
			if(!pPaser->TokenOut(TD_DELIMITER_SCLOSE)) goto ERROR_OUT;
		}

		if(!pVO->Create(pParentWnd)) goto ERROR_OUT;
	}

	return pVO;
}

BEGIN_MESSAGE_MAP(CViewObject, CWnd)
	ON_WM_NCPAINT()
END_MESSAGE_MAP()

void CViewObject::OnNcPaint(){
	CDC* pDC = GetWindowDC();

	CRect rect;
	GetWindowRect(&rect);
	rect.OffsetRect(-rect.left, -rect.top);

	int color = m_bOutline ? 128 : 255;
	CBrush brush(RGB(color,color,color));
	pDC->FrameRect(&rect, &brush);

	ReleaseDC(pDC);
}
