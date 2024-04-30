#include "stdafx.h"
#include "Output.h"
#include "TestDriveResource.h"
#include "TestDriveImp.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

COutput g_Output[COutput::TD_OUTPUT_SIZE];

#define MESSAGE_USE_MASK	(CFE_BOLD|CFE_ITALIC|CFE_UNDERLINE|CFE_STRIKEOUT|CFE_LINK)

//-----------------------------------------------------------------------
// COutputCtrl
COutput::COutput(){
	memset(&m_cf, 0, sizeof(CHARFORMAT));
	m_cf.cbSize = sizeof(CHARFORMAT);
	m_cf.dwMask = CFM_COLOR | CFM_FACE | MESSAGE_USE_MASK;
	m_cf.dwEffects = 0;//(unsigned long)~(CFE_UNDERLINE | CFE_BOLD | CFE_AUTOCOLOR);
	m_cf.crTextColor = RGB(0,0,0);

	m_pParent = NULL;
}

COutput::~COutput(){
}

BEGIN_MESSAGE_MAP(COutput, CListBox)
	ON_NOTIFY_REFLECT_EX(EN_LINK, OnLink)
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
	ON_WM_MOUSEWHEEL()
	//ON_WM_CONTEXTMENU()
	//ON_WM_COPYDATA()
	//ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	//ON_WM_WINDOWPOSCHANGING()
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// COutputList 메시지 처리기

/*void COutput::OnContextMenu(CWnd* pWnd, CPoint point)
{
	CMenu menu;
	menu.LoadMenu(IDR_OUTPUT_POPUP);

	CMenu* pSumMenu = menu.GetSubMenu(0);

	//if (AfxGetMainWnd()->IsKindOf(RUNTIME_CLASS(CMDIFrameWndEx)))
	{
		CMFCPopupMenu* pPopupMenu = new CMFCPopupMenu;

		if (!pPopupMenu->Create(this, point.x, point.y, (HMENU)pSumMenu->m_hMenu, FALSE, TRUE))
			return;
		((CMDIFrameWndEx*)AfxGetMainWnd())->OnShowPopupMenu(pPopupMenu);
		//UpdateDialogControls(this, FALSE);
	}

	SetFocus();
}*/

void COutput::OnEditCopy()
{
	/*CString str = GetSelText();
	if(OpenClipboard()){
		EmptyClipboard();
		HGLOBAL hClipboardData;
		hClipboardData = GlobalAlloc(GMEM_DDESHARE,  (str.GetLength()+1)*sizeof(TCHAR));
		TCHAR * pchData = (TCHAR*)GlobalLock(hClipboardData);
		_tcscpy(pchData, str);
		GlobalUnlock(hClipboardData);
		SetClipboardData(CF_TEXT,hClipboardData);
		CloseClipboard();
	}*/
	//AfxMessageBox(_T("출력 복사"));
}

BOOL COutput::Create(COutputWnd* pPaneWnd, CWnd* pParentWnd, CFont* pFont, UINT nID){
	CRect rectDummy;
	rectDummy.SetRectEmpty();
	BOOL bRet	= COleRichEditCtrl::Create(LBS_NOINTEGRALHEIGHT | WS_CHILD | WS_VISIBLE | ES_MULTILINE | WS_HSCROLL | WS_VSCROLL | WS_CLIPSIBLINGS, rectDummy, pParentWnd, nID);

	if (!bRet) return FALSE;

	PostInitialize();
	SetOptions(ECOOP_OR, ECO_AUTOWORDSELECTION|ECO_READONLY|ECO_AUTOVSCROLL|ECO_AUTOHSCROLL);
	//SetFont(pFont);

	m_pParent = pPaneWnd;

	SetAutoURLDetect(TRUE);
	SetEventMask(ENM_LINK | ENM_PROTECTED);

	{	// 폰트크기 10으로 고정
		CDC * pDC	= GetDC();
		if(pDC){
			int iLogPixelSy	= pDC->GetDeviceCaps(LOGPIXELSY);
			m_cf.dwMask		|= CFM_SIZE;
			m_cf.yHeight	= MulDiv(10, iLogPixelSy, 72);
			m_cf.yHeight	*= (1440 / iLogPixelSy);
			pDC->Detach();
		}
	}

	// 고정 폭 글꼴
	/*switch (g_Localization.CurrentLocale()->dwLangID) {
	case LANG_KOREAN:		_tcscpy(m_cf.szFaceName, _T("Dotumche"));			break;
	default:				_tcscpy(m_cf.szFaceName, _T("Cascadia Mono"));		break;
	}*/
	_tcscpy(m_cf.szFaceName, _T("Cascadia Mono"));
	CRichEditCtrl::SetSel(0, -1);
	SetSelectionCharFormat(m_cf);
	Clear();

	return bRet;
}

void COutput::LogOut(LPCTSTR szMsg, COLORREF color, DWORD dwEffects){
	if(!szMsg || !IsWindow(m_hWnd)) return;

	CString sMsg(szMsg);

	{	// backspace 처리
		int i;
		while((i=sMsg.Find(_T("\b")))>=0){		
			sMsg.Delete(i,1);
			if(i){
				sMsg.Delete(i-1,1);
			}else if(GetTextLength()){
				SetSel(GetTextLength()-1,-1);
				ReplaceSel(_T(""));
			}
		}
	}
	SetSel(-1,-1);
	sMsg.Replace(_T("\r\n"), _T("\n"));
	if(sMsg.GetAt(0) == _T('\r')){
		sMsg.Delete(0,1);
		SetSel(LineIndex(-1), -1);
	}

	//if(m_cf.crTextColor != color || ((m_cf.dwEffects ^ dwEffects) & MESSAGE_USE_MASK))
	{
		m_cf.crTextColor	= color;
		m_cf.dwEffects		= dwEffects;
		SetSelectionCharFormat(m_cf);
	}

	ReplaceSel(sMsg);
	
	HideSelection(TRUE, FALSE);

	PostMessage(WM_VSCROLL, SB_BOTTOM, 0);

	if(m_pParent && m_pParent->IsAutoHideMode()){
		if(!m_pParent->IsVisible()) m_pParent->ShowPane(TRUE, TRUE, TRUE);	// bActive 가 true 이어야 계속 출력시 show 상태가 유지됨.
	}
}

void COutput::LogLink(LPCTSTR szLink, LPCTSTR szName, COLORREF color){
	CString sText;
	{
		CString sName(szName);
		CString sLink(szLink);
		sName.Replace(_T("{"), _T("\\{"));
		sName.Replace(_T("}"), _T("\\}"));
		sName.Replace(_T("\\"), _T("\\\\"));
		sLink.Replace(_T("\""), _T("\\\""));
		sText.Format(_T("{\\rtf1{\\field{\\*\\fldinst{HYPERLINK \"%s\"}}{\\fldrslt{{\\colortbl;\\red%d\\green%d\\blue%d;}\\cf1\\ul %s}}}}"),
			sLink,
			(color)&0xFF, (color>>8)&0xFF, (color>>16)&0xFF,
			sName);
	}
	SetSel(-1,-1);
	SetTextEx(sText);
}

void COutput::Clear(void){
	if(IsWindow(m_hWnd)){
		CRichEditCtrl::SetSel(0,-1);
		CRichEditCtrl::ReplaceSel(_T(""));
	}
}

BOOL COutput::OnLink(NMHDR *pNotifyHeader, LRESULT *pResult) 
{
	ENLINK	  *pENLink = (ENLINK *) pNotifyHeader;
	CString	  URL;
	CHARRANGE CharRange ;

	*pResult = 0;

	switch (pNotifyHeader->code)
	{
	case EN_LINK:
		pENLink = (ENLINK *) pNotifyHeader;

		switch (pENLink->msg)
		{
		case WM_LBUTTONDOWN:
			GetSel(CharRange);
			{	// text 얻기
				TEXTRANGEW m_tr;
				int iLen		= int(pENLink->chrg.cpMax - pENLink->chrg.cpMin + 1);
				TCHAR* pStr		= new TCHAR[iLen];
				m_tr.chrg		= pENLink->chrg;
				m_tr.lpstrText	= pStr;
				SendMessage(EM_GETTEXTRANGE, 0,  (LPARAM) &m_tr);
				URL	= pStr;
				delete [] pStr;
			}
			SetSel(CharRange);
			if(!URL.IsEmpty()){
				CWaitCursor WaitCursor;
				if(!URL.Find(_T("document://"))){
					int				iPos			= 11;
					CString			sDocumentName	= URL.Tokenize(_T("?"), iPos);
					ITDDocument*	pDoc			= g_pTestDrive->GetDocument(sDocumentName);
					if(pDoc){
						DWORD	dwCommand	= 0;
						CString	sArg;
						if(iPos>0) dwCommand	= (DWORD)_tstoi(URL.Tokenize(_T("?"), iPos));
						if(iPos>0) sArg			= URL.Tokenize(_T("?"), iPos);

						pDoc->GetImplementation()->OnCommand(dwCommand, (WPARAM)((LPCTSTR)sArg));
					}else{
						g_pTestDrive->LogInfo(_S(DOCUEMT_IS_NOT_FOUND), sDocumentName);
					}
				}else{
					ShellExecute(GetSafeHwnd(), _T("open"), URL, NULL, NULL, SW_SHOWNORMAL);
				}
				*pResult = 1;
			}
			break;

		case WM_LBUTTONUP:
			*pResult = 1;
			break ;
		}

		break;
	}
	return 0;
}
