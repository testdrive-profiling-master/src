#include "stdafx.h"
#include "TestDriveImp.h"
#include "ReportView.h"
#include "DocumentWnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
// CReportView

IMPLEMENT_DYNAMIC(CReportView, CViewObject)

CReportView::CReportView(){
	ZeroMemory(&m_CharFormat, sizeof(CHARFORMAT2));
	ZeroMemory(&m_ParaFormat, sizeof(PARAFORMAT));
	m_CharFormat.cbSize	= sizeof(CHARFORMAT2);
	m_ParaFormat.cbSize	= sizeof(PARAFORMAT);

	//SetFont(_T("System"));

	m_CharStart	= 0;
	m_CharEnd	= 0;

	m_pManager		= NULL;
	m_dwManagerID	= 0;
}

CReportView::~CReportView()
{
}

BOOL CReportView::Create(CWnd* pParentWnd){
	CRect rc;
	m_Layout.GetViewRect(&rc);
	BOOL bRet = CWnd::Create(NULL, m_sViewName, WS_VISIBLE|WS_CHILD|WS_CLIPCHILDREN|WS_CLIPSIBLINGS, rc, pParentWnd, GetLastWindowID(pParentWnd));
	if(bRet){
		GetClientRect(&rc);
		m_Edit.Create(WS_VISIBLE|WS_CHILD|WS_VSCROLL|WS_HSCROLL|ES_AUTOVSCROLL|ES_AUTOHSCROLL|ES_MULTILINE|ES_WANTRETURN, rc, this, AFX_IDW_PANE_FIRST);
	}
	m_Edit.SetEventMask(ENM_LINK | ENM_SELCHANGE | ENM_PROTECTED);
	BringWindowToTop();
	//m_Edit.SendMessage(EM_AUTOURLDETECT, TRUE, 0);
	{
		int tabsize = 14;
		m_Edit.SendMessage(EM_SETTABSTOPS, 1, (LPARAM)&tabsize);
	}

	// 자동 폰트 변경 (키 입력이 들어올시 default format 이 적용되도록 함.)
	m_Edit.SendMessage( EM_SETLANGOPTIONS , 0, (LPARAM)(m_Edit.SendMessage( EM_GETLANGOPTIONS, 0, 0 ) & ~IMF_AUTOFONT) );
	m_Edit.LimitText(-1);	//Set the control to accept the maximum amount of text
	//m_Edit.SetOptions(ECOOP_OR, ECO_AUTOVSCROLL | ECO_AUTOHSCROLL | ECO_NOHIDESEL | ECO_SAVESEL | ECO_SELECTIONBAR);

	// 고정 폭 글꼴
	/*switch (g_Localization.CurrentLocale()->dwLangID) {
	case LANG_KOREAN:		SetFont(_T("Dotumche"));			break;
	default:				SetFont(_T("Cascadia Mono"));		break;
	}*/
	SetFont(_T("Cascadia Mono"));

	return bRet;
}

ITDObject* CReportView::GetObject(void){
	return this;
}

void CReportView::SetManager(ITDReportManager* pManager, DWORD dwID){
	m_pManager		= pManager;
	m_dwManagerID	= dwID;
}

void CReportView::UpdateLayout(void){
	SetWindowPos(NULL, m_Layout.ViewX(), m_Layout.ViewY(), m_Layout.ViewWidth(), m_Layout.ViewHeight(), SWP_NOACTIVATE | SWP_NOZORDER);
	//OnSize(SIZE_RESTORED, m_Layout.GetWidth(), m_Layout.GetHeight());
	//Invalidate(FALSE);
}

BOOL CReportView::SetDescription(LPCTSTR lpszFileName){
	//return m_Edit.Open(lpszFileName);
	BOOL bRet = m_Edit.Open(lpszFileName);
	//m_Edit.AddWatchingFile(_T("test.xlsx"), 1);
	//m_Edit.AddWatchingFile(_T("a.txt"));
	return bRet;
}

BOOL CReportView::Open(LPCTSTR lpszFileName, BOOL bAutoUpdate){
	//m_Edit.ModifyStyle(ES_AUTOVSCROLL|ES_AUTOHSCROLL,0);
	return m_Edit.Open(lpszFileName, bAutoUpdate);
}

BOOL CReportView::Save(LPCTSTR lpszFileName){
	return m_Edit.Save(lpszFileName);
}

void CReportView::Clear(BOOL bClose){
	m_Edit.Clear(bClose);
}


void CReportView::UpdateFormat(void){
	if(m_CharStart == m_CharEnd) return;

	if(m_CharFormat.dwMask){
		// set format
		m_Edit.SetSel(m_CharStart, m_CharEnd > m_Edit.GetTextLength() ? -1 : m_CharEnd);
		m_Edit.SetSelectionCharFormat(m_CharFormat);
		m_CharFormat.dwEffects	&= ~(CFE_LINK|CFE_PROTECTED);
		m_Edit.SetDefaultCharFormat(m_CharFormat);


		/*
		DWORD dwLength = m_sHiperLink.GetLength();

		if(dwLength){
			CString sHiper;
			m_Edit.SetSel(m_CharStart, m_CharStart);
			sHiper.Format(_T("%s"),m_sHiperLink);
			m_Edit.ReplaceSel(sHiper);
			m_sHiperLink.Empty();
			//dwLength+=1;
		}

		// set format
		m_Edit.SetSel(m_CharStart, m_CharEnd+dwLength);
		m_Edit.SetSelectionCharFormat(m_CharFormat);
		m_CharFormat.dwEffects	&= ~(CFE_LINK|CFE_PROTECTED);
		m_Edit.SetDefaultCharFormat(m_CharFormat);

		if(dwLength){
			m_Edit.SetSel(m_CharStart, m_CharStart + dwLength);
			m_CharFormat.dwMask		|= (CFM_HIDDEN);
			m_CharFormat.dwEffects	|= (CFE_HIDDEN);
			m_Edit.SetSelectionCharFormat(m_CharFormat);
			m_CharFormat.dwEffects	&= ~(CFE_HIDDEN);
			m_CharEnd				+= dwLength;
		}
		*/

		/*DWORD dwLength = m_sHiperLink.GetLength();

		if(dwLength){
			CString sHiper;
			m_Edit.SetSel(m_CharStart, m_CharStart);
			sHiper.Format(_T("%s"),m_sHiperLink);
			m_Edit.ReplaceSel(sHiper);
			m_sHiperLink.Empty();
			//dwLength+=1;
		}

		// set format
		m_Edit.SetSel(m_CharStart, m_CharEnd+dwLength);
		m_Edit.SetSelectionCharFormat(m_CharFormat);
		m_CharFormat.dwEffects	&= ~(CFE_LINK|CFE_PROTECTED);
		m_Edit.SetDefaultCharFormat(m_CharFormat);

		if(dwLength){
			m_Edit.SetSel(m_CharStart, m_CharStart + dwLength);
			m_CharFormat.dwMask		|= (CFM_HIDDEN);
			m_CharFormat.dwEffects	|= (CFE_HIDDEN);
			m_Edit.SetSelectionCharFormat(m_CharFormat);
			m_CharFormat.dwEffects	&= ~(CFE_HIDDEN);
			m_CharEnd				+= dwLength;
		}*/
	}
	if(m_ParaFormat.dwMask){
		m_Edit.SetParaFormat(m_ParaFormat);
		m_ParaFormat.dwMask	= 0;
	}
}

void CReportView::AppendText(LPCTSTR lpszFormat, ...){
	CString str;
	va_list args;
	va_start(args, lpszFormat);
	str.FormatV(lpszFormat,args);
	va_end(args);

	m_CharStart = GetTextLength();
	{
		int iLen = m_sHiperLink.GetLength();
		if(iLen){
			CString sLink, sStr(str);
			sStr.Replace(_T("{"), _T("\\{"));
			sStr.Replace(_T("}"), _T("\\}"));
			sStr.Replace(_T("\\"), _T("\\\\"));
			m_sHiperLink.Replace(_T("\""), _T("\\\""));

			sLink.Format(_T("{\\rtf1{\\field{\\*\\fldinst{HYPERLINK \"%s\"}}{\\fldrslt{\\cf1\\ul %s}}}}"), m_sHiperLink, sStr);
			m_Edit.SetSel(-1,-1);
			m_Edit.SetTextEx(sLink);
			m_sHiperLink.Empty();

			/*
			'{\\field{\\*\\fldinst  HYPERLINK \\\\l "%s"} {\\fldrslt %s}}'%(bookmark, text)
			"{\\field{\\*\\fldinst  PAGEREF %s }}"%bookmark
			*/

		}else{
			m_Edit.AppendText(str);
		}
	}
	m_CharEnd	= GetTextLength();
	
	UpdateFormat();
}

void CReportView::SetText(LPCTSTR lpszFormat, ...){

	CString str;
	va_list args;
	va_start(args, lpszFormat);
	str.FormatV(lpszFormat,args);
	va_end(args);

	m_CharStart	= 0;
	m_Edit.SetSel(0, -1);
	m_Edit.ReplaceSel(str);

	m_CharEnd	= GetTextLength();

	UpdateFormat();
}

void CReportView::ReplaceText(long nStartChar, long nEndChar, LPCTSTR lpszFormat, ...){
	CString str;
	va_list args;
	long	length;
	va_start(args, lpszFormat);
	str.FormatV(lpszFormat,args);
	va_end(args);

	m_Edit.SetSel(nStartChar, nEndChar);
	m_Edit.ReplaceSel(_T(""));

	length = GetTextLength();
	if(length<m_CharStart) m_CharStart = length;
	else m_CharStart	= nStartChar;

	m_Edit.SetSel(nStartChar, nStartChar);
	m_Edit.ReplaceSel(str);

	m_CharEnd	= nStartChar + GetTextLength() - length;
	UpdateFormat();
}

void CReportView::SetSel(long nStartChar, long nEndChar){
	m_Edit.SetSel(nStartChar, nEndChar);
}

void CReportView::GetSel(long& nStartChar, long& nEndChar){
	m_Edit.GetSel(nStartChar, nEndChar);
}

int CReportView::GetLineCount(void){
	return m_Edit.GetLineCount();
}
void CReportView::SetBackgroundColor(COLORREF crBasckColor){
	m_Edit.SetBackgroundColor(FALSE, crBasckColor);
}

BOOL CReportView::GetText(LPTSTR lpszBuffer, DWORD dwSize, long nStartChar, long nEndChar){
	m_Edit.SetSel(nStartChar, nEndChar);
	CString buff	= m_Edit.GetSelText();
	if(!_tcscpy_s(lpszBuffer, dwSize, buff)) return TRUE;
	return FALSE;
}

void CReportView::ReplaceSel(LPCTSTR lpszNewText){
	m_Edit.ReplaceSel(lpszNewText);
}

void CReportView::SetStyle(DWORD dwAdd, DWORD dwRemove){
	/*CHARFORMAT	cf;
	cf.cbSize		= sizeof(cf);
	cf.dwMask		= dwAdd | dwRemove;
	cf.dwEffects	= dwAdd & (~dwRemove);
	m_Edit.SetSelectionCharFormat(cf);*/

	m_CharFormat.dwMask			|= dwAdd|dwRemove;
	m_CharFormat.dwEffects		|= dwAdd;
	m_CharFormat.dwEffects		&= (~dwRemove);
}

void CReportView::SetColor(COLORREF crTextColor){
	/*CHARFORMAT	cf;
	cf.cbSize		= sizeof(cf);
	cf.dwMask		= CFM_COLOR;
	cf.dwEffects	= 0;
	cf.crTextColor	= crTextColor;
	m_Edit.SetSelectionCharFormat(cf);*/
	m_CharFormat.dwMask			|= CFM_COLOR;
	m_CharFormat.crTextColor	= crTextColor;
}

void CReportView::SetFont(LPCTSTR sFontName){
	/*CHARFORMAT	cf;
	cf.cbSize		= sizeof(cf);
	cf.dwMask		= CFM_FACE;
	_tcscpy(cf.szFaceName, sFontName);
	m_Edit.SetSelectionCharFormat(cf);*/
	m_CharFormat.dwMask			|= CFM_FACE;
	_tcscpy(m_CharFormat.szFaceName, sFontName);

}

void CReportView::SetOffset(LONG yOffset){
	/*CHARFORMAT	cf;
	cf.cbSize		= sizeof(cf);
	cf.dwMask		= CFM_OFFSET;
	cf.yOffset		= yOffset;
	m_Edit.SetSelectionCharFormat(cf);*/
	m_CharFormat.dwMask			|= CFM_OFFSET;
	m_CharFormat.yOffset		= yOffset;
}

void CReportView::SetHeight(LONG yHeight){
	/*CHARFORMAT	cf;
	cf.cbSize		= sizeof(cf);
	cf.dwMask		= CFM_SIZE;
	cf.yHeight		= yHeight;
	m_Edit.SetSelectionCharFormat(cf);*/
	m_CharFormat.dwMask			|= CFM_SIZE;
	CDC* pDC = m_Edit.GetDC();
	int iLogPixelSy = pDC->GetDeviceCaps(LOGPIXELSY);
	pDC->Detach();

	m_CharFormat.yHeight         = MulDiv(yHeight, iLogPixelSy, 72);
	m_CharFormat.yHeight        *= (1440 / iLogPixelSy);
}

void CReportView::SetAlign(TEXT_ALIGN align){
	/*PARAFORMAT pf;
	pf.cbSize		= sizeof(PARAFORMAT);
	pf.dwMask		= PFM_ALIGNMENT;
	pf.wAlignment	= align;
	m_Edit.SetParaFormat(pf);*/
	//m_Edit.
	m_ParaFormat.dwMask			|= PFM_ALIGNMENT;
	m_ParaFormat.wAlignment		= align;
}

void CReportView::SetLink(LPCTSTR sLink){
	
	if(sLink) m_sHiperLink = sLink;
	else{
		m_CharFormat.dwMask			|= CFM_LINK|CFM_PROTECTED;
		m_CharFormat.dwEffects		|= CFE_LINK|CFE_PROTECTED;
		m_sHiperLink.Empty();
	}
}

void CReportView::ShowScrollBar(BOOL bShow){
	m_Edit.SendMessage(EM_SHOWSCROLLBAR, SB_VERT, bShow);
	m_Edit.SendMessage(EM_SHOWSCROLLBAR, SB_HORZ, bShow);
// 	m_Edit.ModifyStyleEx(	bShow ? 0 : WS_HSCROLL|ES_AUTOHSCROLL,
// 							bShow ? WS_HSCROLL|ES_AUTOHSCROLL : 0);
}

void CReportView::EnableEdit(BOOL bEditable){
	m_Edit.m_bEditable	= bEditable;
}

void CReportView::ShowOutline(BOOL bShow){
	m_bOutline	= bShow;
// 	if(bShow) ModifyStyleEx(0, WS_EX_CLIENTEDGE, SWP_DRAWFRAME);
// 	else ModifyStyleEx(WS_EX_CLIENTEDGE, 0, SWP_DRAWFRAME);
 	if(bShow) ModifyStyleEx(0, WS_EX_STATICEDGE, SWP_DRAWFRAME);
 	else ModifyStyleEx(WS_EX_STATICEDGE, 0, SWP_DRAWFRAME);
}

void CReportView::CalculateHeight(void){
	// Auto height
	m_Edit.SetEventMask(ENM_REQUESTRESIZE);
	m_Edit.RequestResize();
}

int CReportView::GetFirstVisibleLine(void){
	return m_Edit.GetFirstVisibleLine();
}

void CReportView::LineScroll(int iLines, int iChars){
	m_Edit.LineScroll(iLines, iChars);
}

void CReportView::ScrollToLastLine(void){
	m_Edit.PostMessage(WM_VSCROLL, SB_BOTTOM, 0);
}

long CReportView::GetTextLength(void){
	long line_count = m_Edit.GetLineCount();
	if(line_count) line_count--;
	return (m_Edit.GetTextLength()-line_count);
}

BEGIN_MESSAGE_MAP(CReportView, CViewObject)
	ON_WM_SIZE()
	ON_WM_NCPAINT()
	ON_WM_LBUTTONUP()
	ON_WM_SETFOCUS()
	ON_NOTIFY(EN_REQUESTRESIZE, AFX_IDW_PANE_FIRST, OnRequestResize)
	ON_NOTIFY(EN_LINK, AFX_IDW_PANE_FIRST, OnLink)
	ON_NOTIFY(EN_PROTECTED, AFX_IDW_PANE_FIRST, OnProtected)

END_MESSAGE_MAP()

void CReportView::OnNcPaint(){
	CWnd::OnNcPaint();
}
void CReportView::OnSetFocus(CWnd *pOldWnd){
	m_Edit.SetFocus();
}

void CReportView::OnSize(UINT nType, int cx, int cy){
	if(nType == SIZE_MINIMIZED) return;
	CWnd::OnSize(nType, cx, cy);

	if ( m_Edit.m_hWnd == NULL ) return;

	CRect rcCtrl;
	rcCtrl.SetRect(0, 0, cx, cy);
	m_Edit.MoveWindow( &rcCtrl, FALSE );
}

void CReportView::OnLButtonUp(UINT nFlags, CPoint point){
	CWnd::OnLButtonUp(nFlags,point);
	((CDocumentWnd*)GetParent())->SetFocus();
	//((CDocumentWnd`*)GetParent())->InvokeClickEvent(m_iControlID);
}

void CReportView::OnRequestResize(NMHDR* pNMHDR, LRESULT* pResult){
	REQRESIZE* prr = (REQRESIZE*)pNMHDR;
	//m_dimRtf.cx = prr->rc.right - prr->rc.left;

	m_Layout.m_iHeight = prr->rc.bottom - prr->rc.top + 10;
	UpdateLayout();

	/*CString ar;
	ar.Format(_T("%d"), m_Layout.m_iHeight);
	MessageBox(ar);*/

	*pResult = NULL;
}

void CReportView::OnLink(NMHDR* pNMHDR, LRESULT* pResult){
	ENLINK *pLink = reinterpret_cast<ENLINK *>(pNMHDR);

	//FINDTEXTEX findText;
	*pResult = 0;
	
	if( pLink->msg == WM_LBUTTONDOWN )
	{
		{
			int iLen = int(pLink->chrg.cpMax - pLink->chrg.cpMin + 1);
			
			TCHAR* pStr = new TCHAR[iLen];
			{	// text 얻기
				TEXTRANGEW m_tr;
				m_tr.chrg		= pLink->chrg;
				m_tr.lpstrText	= pStr;
				m_Edit.SendMessage(EM_GETTEXTRANGE, 0,  (LPARAM) &m_tr);
			}
			// 메니저 도큐먼트에 리포팅
			if(m_pManager) m_pManager->OnReportLink(m_dwManagerID, (LPCTSTR)pStr, pLink->chrg.cpMin, pLink->chrg.cpMax);
			delete [] pStr;
		}

		// 링크 문자열 얻기
// 		m_Edit.SetSel(pLink->chrg);
// 		CString strLink = m_Edit.GetSelText();
// // 		{	// '\1' 문자가 있으면 그 뒤의 문자열만 반환한다.
// // 			int iHiperPos = strLink.Find(_T("\1"));
// // 			if(iHiperPos) strLink.Delete(0,iHiperPos+1);
// // 		}
// 		// 메니저 도큐먼트에 리포팅
// 		if(m_pManager) m_pManager->OnReportLink(m_dwManagerID, (LPCTSTR)strLink, pLink->chrg.cpMin, pLink->chrg.cpMax);
// 		else{
// 			// @todo : 메니저가 없을 경우 http:// 로 시작할 때 Shell 로 연다.
// 		}
	}
}

// BOOL CReportView::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult){
// 	ENLINK* pLink = (ENLINK*)lParam;
// 
// 
// 
// 	if(pLink && pLink->msg == WM_LBUTTONUP)
// 	{
// 		m_Edit.SetSel(pLink->chrg);
// 		g_pTestDrive->LogInfo(_T("[%s]"),m_Edit.GetSelText());
// 	}
// 
// 	return CWnd::OnNotify(wParam, lParam, pResult);
// }

void CReportView::OnProtected(NMHDR *pNMHDR, LRESULT *pResult){
	ENPROTECTED *pEnProtected = (ENPROTECTED *)pNMHDR;

	//return 0 to allow modification, 1 otherwise
	//allow copy, but deny link modification
	*pResult = pEnProtected->msg != WM_COPY;
}

typedef enum{
	CMD_REPORT_OPEN,
	CMD_REPORT_EDITABLE,
	CMD_REPORT_OUTLINE,
	CMD_REPORT_SIZE
}CMD_REPORT;

const TCHAR* g_sReportCommand[CMD_REPORT_SIZE]={
	_T("Open"),
	_T("EnableEdit"),
	_T("ShowOutline"),
};

BOOL CReportView::Paser(CPaser* pPaser, int x, int y){
	BOOL			bRet	= FALSE;
	int				iLoop	= 1;
	TCHAR			token[MAX_PATH];
	TD_TOKEN_TYPE	type;
	int				i;

	m_Layout.Move(x, y);

	if(pPaser)
		if(pPaser->TokenOut(TD_DELIMITER_LOPEN)){
			while(iLoop){
				if(!pPaser->IsTokenable()) break;
				type = pPaser->GetToken(token);
				switch(type){
			case TD_TOKEN_NAME:
				{
					i = CheckCommand(token, g_sReportCommand, CMD_REPORT_SIZE);
					if(!pPaser->TokenOut(TD_DELIMITER_SOPEN)) goto ERROR_OUT;
					switch(i){
			case CMD_REPORT_OPEN:
				{
					BOOL bUpdate	= TRUE;
					if(!pPaser->GetTokenString(token)) goto ERROR_OUT;
					if(pPaser->TokenOut(TD_DELIMITER_COMMA)){
						if(!pPaser->GetTokenInt((int*)&bUpdate)) goto ERROR_OUT;
					}
					m_Edit.Open(token, bUpdate);
				}
				break;
			case CMD_REPORT_EDITABLE:
				m_Edit.m_bEditable	= TRUE;
				break;
			case CMD_REPORT_OUTLINE:
				ShowOutline();
				break;
			default: goto ERROR_OUT;
					}
					if(!pPaser->TokenOut(TD_DELIMITER_SCLOSE)) goto ERROR_OUT;
					if(!pPaser->TokenOut(TD_DELIMITER_SEMICOLON)) goto ERROR_OUT;
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
