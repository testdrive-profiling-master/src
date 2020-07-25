// ButtonCtrl.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "TestDriveImp.h"
#include "ButtonCtrl.h"
#include "DocumentWnd.h"

// CButtonCtrl

IMPLEMENT_DYNAMIC(CButtonCtrl, CViewObject)

CButtonCtrl::CButtonCtrl() : m_pManager(NULL), m_dwID(0)
{
}

CButtonCtrl::~CButtonCtrl()
{
}


BEGIN_MESSAGE_MAP(CButtonCtrl, CViewObject)
	ON_WM_NCPAINT()
	ON_WM_SIZE()
	ON_COMMAND(IDOK, OnClicked)
END_MESSAGE_MAP()

void CButtonCtrl::OnNcPaint(){
	CWnd::OnNcPaint();
}

void CButtonCtrl::OnSize(UINT nType, int cx, int cy){
	CRect rc;
	GetClientRect(&rc);
	if(IsWindow(m_Button.m_hWnd)) m_Button.SetWindowPos(NULL, 0, 0, cx, cy, SWP_NOACTIVATE | SWP_NOZORDER);
	CWnd::OnSize(nType, cx, cy);
}

void CButtonCtrl::OnClicked(){
	if(m_pManager){
		m_pManager->OnButtonClick(m_dwID);
	}
}

BOOL CButtonCtrl::Create(CWnd* pParentWnd){
	CRect rc;
	m_Layout.GetViewRect(&rc);
	CWnd::Create(NULL,NULL,WS_CHILD | WS_CLIPCHILDREN | WS_VISIBLE | WS_CLIPSIBLINGS, rc, pParentWnd,GetLastWindowID(pParentWnd));
	GetClientRect(&rc);
	BringWindowToTop();
	//return CButton::Create(m_sViewName, BS_TEXT|BS_CENTER|BS_AUTO3STATE|BS_PUSHBUTTON, rc, pParentWnd, 0);
	return m_Button.Create(m_sViewName, WS_VISIBLE|WS_CHILD|WS_CLIPSIBLINGS|BS_PUSHBUTTON, rc, this, IDOK);
}

void CButtonCtrl::UpdateLayout(void){
	if(IsWindow(m_Button.m_hWnd)) SetWindowPos(NULL, m_Layout.ViewX(), m_Layout.ViewY(), m_Layout.ViewWidth(), m_Layout.ViewHeight(), SWP_NOACTIVATE | SWP_NOZORDER);
}

ITDObject* CButtonCtrl::GetObject(void){
	return this;
}

void CButtonCtrl::SetManager(ITDButtonManager* pManager, DWORD dwID){
	m_pManager	= pManager;
	m_dwID		= dwID;
}

void CButtonCtrl::SetEnable(BOOL bEnable){
	m_Button.EnableWindow(bEnable);
}

void CButtonCtrl::SetText(LPCTSTR lpszText){
	m_Button.SetWindowText(lpszText);
}

void CButtonCtrl::SetStyle(BUTTON_STYLE style){
	UINT bs_style;
	switch(style){
	case BUTTON_STYLE_PUSHBUTTON:	bs_style	= BS_PUSHBUTTON;		break;
	case BUTTON_STYLE_CHECKBOX:		bs_style	= BS_CHECKBOX;			break;
	case BUTTON_STYLE_RADIOBUTTON:	bs_style	= BS_RADIOBUTTON;		break;
	}
	m_Button.SetButtonStyle(bs_style);
}

void CButtonCtrl::SetCheck(BOOL bCheck){
	m_Button.SetCheck(bCheck ? BST_CHECKED : BST_UNCHECKED);
}

typedef enum{
	CMD_BUTTON_SET_TEXT,
	CMD_BUTTON_SET_ENABLE,
	CMD_BUTTON_SET_BITMAP,
	CMD_BUTTON_SIZE,
}CMD_BUTTON;

static const TCHAR* g_sButtonCommand[CMD_BUTTON_SIZE]={
	_T("SetText"),
	_T("SetEnable"),
	_T("SetBitmap"),
};

BOOL CButtonCtrl::Paser(CPaser* pPaser, int x, int y){
	BOOL			bRet	= FALSE;
	int				iLoop	= 1;
	TCHAR			token[MAX_PATH];
	TD_TOKEN_TYPE	type;
	int				id;

	m_Layout.Move(x, y);

	if(pPaser)
	if(pPaser->TokenOut(TD_DELIMITER_LOPEN)){
		while(iLoop){
			if(!pPaser->IsTokenable()) break;
			type = pPaser->GetToken(token);
			switch(type){
		case TD_TOKEN_NAME:
			{
				id = CheckCommand(token, g_sButtonCommand, CMD_BUTTON_SIZE);
				//if(id < CMD_BUFFER_LINKED_SCREEN)
					if(!pPaser->TokenOut(TD_DELIMITER_SOPEN)) goto ERROR_OUT;
				switch(id){
				case CMD_BUTTON_SET_TEXT:
					if(!pPaser->GetTokenString(token)) goto ERROR_OUT;
					SetText(token);
					break;
				case CMD_BUTTON_SET_ENABLE:
					{
						BOOL bEnable;
						if(!pPaser->GetTokenInt((int*)&bEnable)) goto ERROR_OUT;
						SetEnable(bEnable);
					}break;
				case CMD_BUTTON_SET_BITMAP:
					{
						if(!pPaser->GetTokenString(token)) goto ERROR_OUT;

						HBITMAP hBmp = (HBITMAP)LoadImage(AfxGetInstanceHandle(), token, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE );
						if(!hBmp) break;
						BITMAP bm;
						::GetObject(hBmp, sizeof(BITMAP), &bm );
						m_Layout.m_iWidth = bm.bmWidth+4;	// add outline spaces
						m_Layout.m_iHeight = bm.bmHeight+4;
						m_Button.ModifyStyle(BS_PUSHBUTTON, BS_BITMAP);
						 
						m_Button.SetBitmap(hBmp);
						//m_Button.SetImageList(BUTTON_IMAGELIST)
						UpdateLayout();
					}break;
				default:
					goto ERROR_OUT;
				}
				//if(id < CMD_BUFFER_LINKED_SCREEN){
					if(!pPaser->TokenOut(TD_DELIMITER_SCLOSE)) goto ERROR_OUT;
					if(!pPaser->TokenOut(TD_DELIMITER_SEMICOLON)) goto ERROR_OUT;
				//}
			}break;
		case TD_TOKEN_DELIMITER:
			if(*token != *g_PaserDelimiter[TD_DELIMITER_LCLOSE]) goto ERROR_OUT;
			iLoop--;
			if(!iLoop) bRet = TRUE;
			break;
			}
		}
	}
ERROR_OUT:
	return bRet;
}
