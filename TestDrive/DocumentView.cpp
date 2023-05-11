#include "stdafx.h"
#include "TestDriveImp.h"
#include "DocumentView.h"
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

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define		VIEW_DOCUMENT_MAGIC_CODE		0x44564454	// "TDVD"

static LPCTSTR	__sPROPERTY_EXPANDED	= _T("TESTDRIVE_PROPERTY_VIEW_EXPANDED");

static void __webview2_setup_fn(CViewObject* pObj) {
	((CHtmlCtrl*)pObj)->SetWebView2();
}

IMPLEMENT_DYNAMIC(CDocumentView, CWnd)

CDocumentView::CDocumentView() : m_dwRefCount(0), m_bAutoClose(FALSE), m_bShow(TRUE)
{
	m_hModule		= NULL;
	m_pDocumentImp	= NULL;
	m_BasePoint.SetPoint(0,0);
	m_DrawSize.SetSize(0,0);

	m_pProperty	= new CMFCPropertyGridProperty(NULL);
	m_pProperty->SetData((DWORD_PTR)this);

}

CDocumentView::~CDocumentView()
{
	ReleaseAll();

	if(m_pProperty){
		if(m_pProperty->GetSubItemsCount()){
			BOOL	bExpand	= m_pProperty->IsExpanded();
			if(bExpand != GetConfigInt(__sPROPERTY_EXPANDED, 1))
				SetConfigInt(__sPROPERTY_EXPANDED, bExpand);
		}
		ClearAllProperty();
		g_PropertyGrid.DeleteProperty(m_pProperty);
		SAFE_DELETE(m_pProperty);
	}
	TRACE(_T("Document(%s) is destroyed.\n"), m_sTitle);
}

BOOL CDocumentView::Create(CWnd* pParent, LPCTSTR lpszName, CPaser* pPaser){
	if(!pParent || !lpszName || !pPaser) return FALSE;

	m_sTitle	=
	m_sName		= lpszName;
	g_PropertyGrid.AddProperty(m_pProperty);
	m_pProperty->Show(FALSE);

	if(!CWnd::Create(NULL, NULL, WS_CHILD | WS_VISIBLE | ES_MULTILINE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, CRect(0,0,300,300), pParent, GetLastWindowID(pParent)))
		return FALSE;

	g_pDocumentList->Add(this);

	EnableScrollBar(SB_BOTH);
	EnableScrollBarCtrl(SB_BOTH);
	DocumentTitle(lpszName);

	// parsing
	if (pPaser && !Load(pPaser)) return FALSE;
	
	{	// normalize position & initialize size
		CRect	rc_sum;
		GetDrawRect(&rc_sum);

		m_DrawSize.SetSize(rc_sum.Width(), rc_sum.Height());

		GetClientRect(&rc_sum);
		m_ViewSize.SetSize(rc_sum.Width(), rc_sum.Height());
	}

	UpdateLayout();
	m_pProperty->Expand(GetConfigInt(__sPROPERTY_EXPANDED, 1));

	TRACE(_T("Document(%s) is created.\n"), m_sTitle);

	return TRUE;
}

void CDocumentView::GetDrawRect(LPRECT pRect){
	if(!pRect) return;
	CRect* pRC	= (CRect*)pRect;
	BOOL bInit	= FALSE;
	pRC->SetRectEmpty();

	for(list<CViewObject*>::iterator iter = m_ViewObjectList.begin();iter != m_ViewObjectList.end();iter++){
		CRect rc;
		if(!((*iter)->IsVisible())) continue;

		(*iter)->m_Layout.GetViewRect(&rc);
		if(!bInit){
			*pRC = rc;
			bInit = TRUE;
		}else{
			if(pRC->left > rc.left)		pRC->left	= rc.left;
			if(pRC->right < rc.right)	pRC->right	= rc.right;
			if(pRC->top > rc.top)		pRC->top	= rc.top;
			if(pRC->bottom < rc.bottom)	pRC->bottom	= rc.bottom;
		}	
	}
}

void CDocumentView::UpdateOrigin(void){
	for(list<CViewObject*>::iterator iter = m_ViewObjectList.begin();iter != m_ViewObjectList.end();iter++){
		(*iter)->m_Layout.SetOrigin(m_BasePoint.x, m_BasePoint.y);
	}
}

void CDocumentView::UpdateLayout(void){
	{
		CRect	rc_sum;
		GetDrawRect(&rc_sum);
		m_DrawSize.SetSize(rc_sum.Width(), rc_sum.Height());
	}
	
	int ViewWidth, ViewHeight;
	int DrawWidth, DrawHeight;

	{
		CRect rc;
		GetClientRect(&rc);
		ViewWidth	= rc.Width();
		ViewHeight	= rc.Height();
		GetDrawRect(&rc);
		m_DrawSize.SetSize(rc.Width(), rc.Height());
		DrawWidth	= m_DrawSize.cx;
		DrawHeight	= m_DrawSize.cy;

		ShowScrollBar(SB_HORZ, (ViewWidth < DrawWidth));
		ShowScrollBar(SB_VERT, (ViewHeight < DrawHeight));
		// 스크롤바 변경으로 클라이언트 영역 크기가 변경될 수 있다.
		GetClientRect(&rc);
		ViewWidth	= rc.Width();
		ViewHeight	= rc.Height();
	}

	{
		SCROLLINFO	info;
		ZeroMemory(&info, sizeof(SCROLLINFO));
		info.cbSize		= sizeof(SCROLLINFO);
		info.fMask		= SIF_ALL;


		if(ViewWidth >= DrawWidth){	// 그릴 크기가 뷰영역보다 작으면 중간에 맞춤.
			m_BasePoint.x = (ViewWidth - DrawWidth) / 2;
		}else{	// 그릴 크기가 이전의 뷰영역보다 클 때,
			// 왼쪽이 비어있으면, 왼쪽에 맞춤
			if(m_BasePoint.x>0) m_BasePoint.x = 0;
			// 오른쪽이 비어있으면, 오른쪽에 맞춤
			else if(m_BasePoint.x+DrawWidth < ViewWidth) m_BasePoint.x = ViewWidth - DrawWidth;

			// 스크롤바 셋팅
			info.nPage	= ViewWidth;
			info.nMax	= DrawWidth;
			info.nPos	= -m_BasePoint.x;
			SetScrollInfo(SB_HORZ, &info);
		}

		if(ViewHeight >= DrawHeight){	// 그릴 크기가 뷰영역보다 작으면 중간에 맞춤.
			m_BasePoint.y = (ViewHeight - DrawHeight) / 2;
		}else{	// 그릴 크기가 이전의 뷰영역보다 클 때,
			// 스케일일 경우
			/*if(m_ViewSize.cy){
				int delta		= (m_ViewSize.cy/2) - m_BasePoint.y;
				m_BasePoint.y	= (ViewHeight/2) + (delta * ViewHeight) / m_ViewSize.cy;
				m_ViewSize.cy	= ViewHeight;
			}*/

			// 위쪽이 비어있으면, 위쪽에 맞춤
			if(m_BasePoint.y>0) m_BasePoint.y = 0;
			// 아래쪽이 비어있으면, 아래쪽에 맞춤
			else if(m_BasePoint.y+DrawHeight < ViewHeight) m_BasePoint.y = ViewHeight - DrawHeight;

			// 스크롤바 셋팅
			info.nPage	= ViewHeight;
			info.nMax	= DrawHeight;
			info.nPos	= -m_BasePoint.y;
			SetScrollInfo(SB_VERT, &info);
		}
	}
	for(list<CViewObject*>::iterator iter = m_ViewObjectList.begin();iter != m_ViewObjectList.end();iter++){
		(*iter)->m_Layout.SetOrigin(m_BasePoint.x, m_BasePoint.y);
		(*iter)->UpdateLayout();
	}
}

const CString& CDocumentView::GetName(void){
	return m_sName;
}

void CDocumentView::ReleaseAll(void){
	if (m_ViewObjectList.size()) {
		for (list<CViewObject*>::iterator iter = m_ViewObjectList.begin();
			iter != m_ViewObjectList.end(); iter++) {
			delete (*iter);
		}
		m_ViewObjectList.clear();
	}

	SAFE_RELEASE(m_pDocumentImp);
	if (m_hModule) {
		FreeLibrary(m_hModule);
		m_hModule = NULL;
	}

	g_pDocumentList->Remove(this);
}

CViewObject* CDocumentView::GetObject(OBJECT_TYPE Type, LPCTSTR lpszName){
	CViewObject* pVO	= NULL;
	for(list<CViewObject*>::iterator iter = m_ViewObjectList.begin();
		iter!=m_ViewObjectList.end();iter++){
		pVO = (*iter);
		if(!pVO->m_sViewName.Compare(lpszName) && pVO->GetType() == Type)
			return pVO;
	}
	return NULL;
}

CMFCPropertyGridProperty* CDocumentView::GetProperty(void){
	return m_pProperty;
}

ITDBuffer* CDocumentView::GetBuffer(LPCTSTR lpszName){
	return (CBufferCtrl*)GetObject(TDOBJECT_BUFFER, lpszName);
}

ITDReport* CDocumentView::GetReport(LPCTSTR lpszName){
	return (CReportView*)GetObject(TDOBJECT_REPORT, lpszName);
}

ITDChart* CDocumentView::GetChart(LPCTSTR lpszName){
	return (CChartCtrl*)GetObject(TDOBJECT_CHART, lpszName);
}

ITDButton* CDocumentView::GetButton(LPCTSTR lpszName){
	return (CButtonCtrl*)GetObject(TDOBJECT_BUTTON, lpszName);
}

ITDHtml* CDocumentView::GetHtml(LPCTSTR lpszName){
	return (CHtmlCtrl*)GetObject(TDOBJECT_HTML, lpszName);
}

ITDImplDocument* CDocumentView::GetImplementation(void){
	return m_pDocumentImp;
}

LPCTSTR CDocumentView::DocumentName(void){
	return m_sName;
}

LPCTSTR CDocumentView::DocumentTitle(LPCTSTR lpszTitle){
	if(lpszTitle){
		m_sTitle = lpszTitle;
		m_pProperty->SetName(lpszTitle);
		((CDocumentWnd*)GetParent())->SetTitle(lpszTitle);
// 
// 		if(!g_pDocumentList->SetTitle(m_sName, lpszTitle)){
// 			g_pTestDrive->LogError(_S(DOCUMENT_TITLE_DUPLICATED), m_sName, lpszTitle);
// 		}else{
// 			m_sTitle = lpszTitle;
// 			m_pProperty->SetName(lpszTitle);
// 		}
	}
	return m_sTitle;
}

ITDSystem* CDocumentView::GetSystem(void){
	return g_pTestDrive;
}

ITDBuffer* CDocumentView::CreateBuffer(LPCTSTR lpszName, int x, int y, int width, int height){
	CViewObject* pVO	= CViewObject::New(TDOBJECT_BUFFER, this);
	if(pVO){
		pVO->GetLayout()->SetPosition(x, y);
		pVO->GetLayout()->SetSize(width, height);
		m_ViewObjectList.push_back(pVO);
		CBufferCtrl* pCtrl = (CBufferCtrl*)pVO;
		return (ITDBuffer*)pCtrl;
	}
	return NULL;
}

ITDReport* CDocumentView::CreateReport(LPCTSTR lpszName, int x, int y, int width, int height){
	CViewObject* pVO	= CViewObject::New(TDOBJECT_REPORT, this);
	if(pVO){
		pVO->GetLayout()->SetPosition(x, y);
		pVO->GetLayout()->SetSize(width, height);
		m_ViewObjectList.push_back(pVO);
		CReportView* pCtrl = (CReportView*)pVO;
		return (ITDReport*)pCtrl;
	}
	return NULL;
}

ITDButton* CDocumentView::CreateButton(LPCTSTR lpszName, int x, int y, int width, int height){
	CViewObject* pVO	= CViewObject::New(TDOBJECT_BUTTON, this);
	if(pVO){
		pVO->GetLayout()->SetPosition(x, y);
		pVO->GetLayout()->SetSize(width, height);
		m_ViewObjectList.push_back(pVO);
		CButtonCtrl* pCtrl = (CButtonCtrl*)pVO;
		return (ITDButton*)pCtrl;
	}
	return NULL;
}

ITDHtml* CDocumentView::CreateHtml(LPCTSTR lpszName, int x, int y, int width, int height, BOOL bWebView2){
	CViewObject* pVO	= CViewObject::New(TDOBJECT_HTML, this, NULL, bWebView2 ? __webview2_setup_fn : nullptr);
	if(pVO){
		pVO->GetLayout()->SetPosition(x, y);
		pVO->GetLayout()->SetSize(width, height);
		m_ViewObjectList.push_back(pVO);
		CHtmlCtrl* pCtrl = (CHtmlCtrl*)pVO;
		return (ITDHtml*)pCtrl;
	}
	return NULL;
}

DWORD CDocumentView::GetReferenceCount(void){
	return m_dwRefCount;
}

BOOL CDocumentView::TryUnlock(BOOL bForce){
	if(!IsLocked()) return TRUE;
	if(GetImplementation()){
		GetImplementation()->OnCommand((DWORD)-1, (WPARAM)bForce);
		if(!IsLocked()) return TRUE;
	}
	return FALSE;
}

void CDocumentView::Lock(void){
	m_dwRefCount++;
	if (!IsWindow(m_hWnd))
		return;
	CDocumentWnd*	pWnd	= (CDocumentWnd*)GetParent();
	pWnd->ModifyStyle(AFX_CBRS_CLOSE, 0);
	//m_dwControlBarStyle		&= ~AFX_CBRS_CLOSE;
	if(pWnd->IsVisible()){
		Invalidate();
		pWnd->AdjustLayout();
	}
}

void CDocumentView::UnLock(void){
	if(m_dwRefCount) m_dwRefCount--;
	if(!m_dwRefCount){
		CDocumentWnd*	pWnd	= (CDocumentWnd*)GetParent();
		pWnd->ModifyStyle(0, AFX_CBRS_CLOSE);
		if(pWnd->IsVisible()){
			Invalidate();
			pWnd->AdjustLayout();
		}
	}
	if(m_bAutoClose && !m_dwRefCount){
		CDocumentWnd*	pWnd	= (CDocumentWnd*)GetParent();
		pWnd->DestroyWindow();
	}
}

BOOL CDocumentView::IsLocked(void){
	return (m_dwRefCount!=0);
}


void CDocumentView::SetAutoClose(BOOL bClose){
	m_bAutoClose	= bClose;
	Lock();
	UnLock();
}

void CDocumentView::SetDescription(LPCTSTR lpszDesc){
	m_pProperty->SetDescription(CString(lpszDesc));
}

int CDocumentView::GetConfigInt(LPCTSTR lpszKeyName, int iDefaultValue){
	return g_pTestDrive->m_Config.GetInt(m_sName, lpszKeyName, iDefaultValue);
}

void CDocumentView::SetConfigInt(LPCTSTR lpszKeyName, int iSetValue){
	g_pTestDrive->m_Config.SetInt(m_sName, lpszKeyName, iSetValue);
}

void CDocumentView::GetConfigString(LPCTSTR lpszKeyName, LPTSTR lpszStr, DWORD dwSize, LPCTSTR lpszDefault){
	g_pTestDrive->m_Config.GetString(m_sName, lpszKeyName, lpszStr, dwSize, lpszDefault);
}

void CDocumentView::SetConfigString(LPCTSTR lpszKeyName, LPCTSTR lpszStr){
	g_pTestDrive->m_Config.SetString(m_sName, lpszKeyName, lpszStr);
}

void CDocumentView::ClearAllProperty(void){
	int count = m_pProperty->GetSubItemsCount();

	for(int count = m_pProperty->GetSubItemsCount();count>0;count--){
		CMFCPropertyGridProperty* pSub	= m_pProperty->GetSubItem(0);
		m_pProperty->RemoveSubItem(pSub);
	}
}

ITDPropertyData* CDocumentView::AddPropertyData(PROPERTY_TYPE Type, DWORD iID, LPCTSTR lpszName, DWORD_PTR pData, LPCTSTR lpszDesc){
	if(!lpszName) return NULL;
	//*pInt = g_pTestDrive->m_Config.GetInt(m_sName, lpszName, *pInt);
	CPropertyDataBase* pSub	= NULL;

	switch(Type){
	case PROPERTY_TYPE_INT:
		pSub	= new CPropertyData(iID, lpszName, (_variant_t)((int)(*(int*)pData)), lpszDesc, pData);
		break;
	case PROPERTY_TYPE_BOOL:
		pSub	= new CPropertyData(iID, lpszName, (_variant_t)(*(bool*)pData), lpszDesc, pData);
		break;
	case PROPERTY_TYPE_FLOAT:
		pSub	= new CPropertyData(iID, lpszName, (_variant_t)(*(float*)pData), lpszDesc, pData);
		break;
	case PROPERTY_TYPE_DOUBLE:
		pSub	= new CPropertyData(iID, lpszName, (_variant_t)(*(double*)pData), lpszDesc, pData);
		break;
	case PROPERTY_TYPE_STRING:
		pSub	= new CPropertyData(iID, lpszName, (_variant_t)(const wchar_t*)pData, lpszDesc, pData);
		break;
	case PROPERTY_TYPE_PASSWORD:
		pSub	= new CPropertyPasswordData(iID, lpszName, (_variant_t)(const wchar_t*)pData, lpszDesc, pData);
		break;
	case PROPERTY_TYPE_DIRECTORY:
		pSub	= new CPropertyDirectoryData(iID, lpszName, (_variant_t)(const wchar_t*)pData, lpszDesc, pData);
		break;
	case PROPERTY_TYPE_FILE_PATH:
		{
			static LPCTSTR __sDelim	= _T(";");
			int		iTokPos	= 0;
			CString	sDesc(lpszName), sName, sDefaultExt, sFilter;
			sName	= sDesc.Tokenize(__sDelim, iTokPos);
			if(!sName.IsEmpty())		sDefaultExt	= sDesc.Tokenize(__sDelim, iTokPos);
			if(!sDefaultExt.IsEmpty())	sFilter		= sDesc.Tokenize(__sDelim, iTokPos);
			pSub	= new CPropertyFileData(iID, sName, (_variant_t)(const wchar_t*)pData,
				OFN_OVERWRITEPROMPT|OFN_READONLY,
				sDefaultExt,
				sFilter,
				lpszDesc, pData);
		}
		break;
	}
	//pSub->EnableSpinControl(TRUE, 0, 300);
	m_pProperty->Show();
	m_pProperty->AddSubItem(pSub->GetObject());

	g_PropertyGrid.AdjustLayout();
	return pSub;
}

BEGIN_MESSAGE_MAP(CDocumentView, CWnd)
	ON_WM_CLOSE()
	ON_WM_DESTROY()
	ON_WM_PAINT()
	ON_WM_MOUSEWHEEL()
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_WM_TIMER()
	ON_WM_SIZE()
	ON_WM_SHOWWINDOW()
	ON_MESSAGE(WM_USER, OnUser)
	ON_MESSAGE(WM_USER_COMMAND, OnUserCommand)
	ON_MESSAGE(CWM_FILE_CHANGED_NOTIFICATION, OnWatchDog)
END_MESSAGE_MAP()

void CDocumentView::OnPaint(){
	CPaintDC dc(this);
	CRect rc;
	GetClientRect(&rc);
	dc.FillSolidRect(&rc, RGB(255,255,255));
}

void CDocumentView::OnClose(){
	ReleaseAll();
	CWnd::OnClose();
}

void CDocumentView::OnDestroy(){
	ReleaseAll();
	CWnd::OnDestroy();
}

void CDocumentView::OnTimer(UINT_PTR nIDEvent){
	CWnd::OnTimer(nIDEvent);
	if(m_pDocumentImp) m_pDocumentImp->OnCommand((DWORD)nIDEvent);
}

void CDocumentView::SetTimer(UINT nIDEvent, UINT nElapse){
	CWnd::SetTimer(nIDEvent, nElapse, NULL);
}

void CDocumentView::KillTimer(UINT nIDEvent){
	CWnd::KillTimer(nIDEvent);
}

void CDocumentView::OnSize(UINT nType, int cx, int cy){
	CWnd::OnSize(nType, cx, cy);
	if(!cy || !cx) return;
	if(m_pDocumentImp)
		m_pDocumentImp->OnSize(cx, cy);

	UpdateLayout();
}

BOOL CDocumentView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt){
	CWnd::OnMouseWheel(nFlags, zDelta, pt);

	if(!zDelta) return 0;

	if(nFlags&MK_SHIFT){	// Horizontal scroll
		OnHScroll(zDelta>0 ? SB_LINELEFT : SB_LINERIGHT, 0, 0);
	}else {						// Vertical scroll
		OnVScroll(zDelta>0 ? SB_LINEUP : SB_LINEDOWN, 0, 0);
	}

	return 0;
}

void CDocumentView::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar){
	int		PreviousPos = GetScrollPos(SB_HORZ);
	int		CurPos		= PreviousPos;
	BOOL	bUpdate		= TRUE;

	switch(nSBCode){
	case SB_LINELEFT:
		m_BasePoint.x += 20;
		break;
	case SB_LINERIGHT:
		m_BasePoint.x -= 20;
		break;
	case SB_PAGELEFT:
		m_BasePoint.x += 50;
		break;
	case SB_PAGERIGHT:
		m_BasePoint.x -= 50;
		break;
	case SB_ENDSCROLL:
		bUpdate = FALSE;
		break;
	case SB_THUMBPOSITION:
	case SB_THUMBTRACK:
		m_BasePoint.x	= -(int)nPos;
		break;
	}

	CWnd::OnHScroll(nSBCode, nPos, pScrollBar);
	if(bUpdate)
		UpdateLayout();
}

void CDocumentView::OnShowWindow(BOOL bShow, UINT nStatus){
	CWnd::OnShowWindow(bShow, nStatus);
	if(!nStatus && m_pDocumentImp) m_pDocumentImp->OnShow(bShow);
	if(m_pDocumentImp) m_pDocumentImp->OnShow(bShow);
}

void CDocumentView::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar){
	int		PreviousPos = GetScrollPos(SB_VERT);
	int		CurPos		= PreviousPos;
	BOOL	bUpdate		= TRUE;

	switch(nSBCode){
	case SB_LINEUP:
		m_BasePoint.y += 20;
		break;
	case SB_LINEDOWN:
		m_BasePoint.y -= 20;
		break;
	case SB_PAGEUP:
		m_BasePoint.y += 50;
		break;
	case SB_PAGEDOWN:
		m_BasePoint.y -= 50;
		break;
	case SB_ENDSCROLL:
		bUpdate = FALSE;
		break;
	case SB_THUMBPOSITION:
	case SB_THUMBTRACK:
		m_BasePoint.y	= -(int)nPos;
		break;
	}
	CWnd::OnVScroll(nSBCode, nPos, pScrollBar);
	if(bUpdate)
		UpdateLayout();
}

LRESULT CDocumentView::OnWatchDog(WPARAM wParam, LPARAM lParam){
	DWORD			dwID	= (DWORD)wParam;
	if((int)wParam ==-1){
		if(!m_dwRefCount){
			LPCTSTR sFileName	= (*m_WatchDogFolderList.begin())->GetPath();
			g_pDocumentList->Remove(m_sName);
			g_pTestDrive->Build(sFileName);
		}else{
			g_pTestDrive->LogOut(_TEXT_(_S(DOCUMENT_UPDATE_NOT_ALLOWED), m_sTitle));
		}
	}else{
		CWatchDogFile* pWatchDogFile = (CWatchDogFile*)wParam;
		if(m_pDocumentImp){
			m_pDocumentImp->OnCommand(pWatchDogFile->GetID(), (WPARAM)pWatchDogFile->GetPath(), lParam);
		}
	}
	return 0;
}

LRESULT CDocumentView::OnUser(WPARAM wParam, LPARAM lParam){
	if(m_pDocumentImp) m_pDocumentImp->OnCommand(TD_EXTERNAL_COMMAND, wParam, lParam);
	return 0;
}

LRESULT CDocumentView::OnUserCommand(WPARAM wParam, LPARAM lParam){
	if(m_pDocumentImp){
		USER_COMMAND* pCommand = (USER_COMMAND*)lParam;
		m_pDocumentImp->OnCommand(pCommand->command, pCommand->wParam, pCommand->lParam);
	}
	return 0;
}

BOOL CDocumentView::CheckUpdateFile(LPCTSTR lpszDllPath, CString* pUpdateFile){
	CString sUpdatFilePath(lpszDllPath);
	sUpdatFilePath	= sUpdatFilePath.Left(sUpdatFilePath.GetLength()-3);
	sUpdatFilePath	+= _T("update.sp");

	if(pUpdateFile) *pUpdateFile	= sUpdatFilePath;

	return IsFileExist(sUpdatFilePath);
}

typedef ITDImplDocument* (__cdecl *RegisterDocumentT)(ITDDocument* pDoc);

BOOL CDocumentView::SetProgram(LPCTSTR lpszFileName){
	BOOL		bSecondChance	= TRUE;
	BOOL		bModified		= FALSE;	// 최신 소스 빌드가 아님.
	CString		sFullPath		= g_pTestDrive->RetrieveFullPath(lpszFileName);

TRY_RELOAD_DOCUMENT:
	SAFE_RELEASE(m_pDocumentImp);

	if(m_hModule){
		FreeLibrary(m_hModule);
		m_hModule = NULL;
	}
	if(lpszFileName){
		m_hModule = LoadLibrary(sFullPath);

		if(m_hModule && CheckUpdateFile(sFullPath)){	// 최신 빌드된 도큐먼트인지 검색
			CString			sSearchPath;
			LPCTSTR		sExt[]	= {
				_T("h"),
				_T("c"),
				_T("cpp"),
				NULL
			};
			{	// 검색 폴더 지정
				CWorkDirectory	SetWorkDir(sFullPath);
				sSearchPath		= SetWorkDir.m_sCurrentPath;
				sSearchPath		+= _T("\\Document\\");
			}
			bModified	= IsSourceTreeChanged(sSearchPath, sExt, sFullPath);
		}
		if(bSecondChance){
			if(m_hModule && bModified){	// 최신 빌드가 아니면 해제한다.
				FreeLibrary(m_hModule);
				m_hModule	= NULL;
			}
			if(!m_hModule) goto TRY_SECOND_CHANCE;	// 존재하지 않으면 다시 시도
		}
	}
	if(m_hModule){
		// 도큐먼트 등록하기
		RegisterDocumentT pFunc = (RegisterDocumentT)GetProcAddress(m_hModule, "RegisterDocument");
		// 구현을 얻는다.
		if (pFunc) {
			m_pDocumentImp = pFunc(this);
		}
		// 구현 얻기가 실패
		if(!m_pDocumentImp){
			FreeLibrary(m_hModule);
			m_hModule	= NULL;

			if(bSecondChance){	// 빌드 재시도
TRY_SECOND_CHANCE:
				bSecondChance	= FALSE;
				{
					CString sUpdatFilePath;

					// 다시 빌드 프로파일이 있다면 다시 빌드 수행
					if(CheckUpdateFile(sFullPath, &sUpdatFilePath)){
						g_pTestDrive->LogInfo(_S(DOCUMENT_UPDATE), lpszFileName);
						if(g_pTestDrive->RunProfile(sUpdatFilePath, TRUE)>=0){
							g_pTestDrive->LogInfo(_S(DOCUMENT_UPDATE_DONE), lpszFileName);
						}
						goto TRY_RELOAD_DOCUMENT;
					}else{
						goto THERE_IS_NO_DOC_ENTRY;
					}
				}
			}
THERE_IS_NO_DOC_ENTRY:
			g_pTestDrive->LogOut(_S(LIBRARY_NO_DOC_ENTRY), CTestDrive::SYSMSG_ERROR);
		}
	}else{
		g_pTestDrive->LogOut(_TEXT_(_S(LIBRARY_LOAD_FAILED), lpszFileName), CTestDrive::SYSMSG_ERROR);
	}

	if(m_pDocumentImp){
		CRect rc;
		GetDrawRect(&rc);
		m_DrawSize.SetSize(rc.Width(), rc.Height());
		GetClientRect(&rc);
		OnSize(NULL, rc.Width(), rc.Height());
	}

	return (m_hModule != NULL);
}

typedef enum{
	VIEWDOCUMENT_CMD_SCREEN,
	VIEWDOCUMENT_CMD_CHART,
	VIEWDOCUMENT_CMD_REPORT,
	VIEWDOCUMENT_CMD_BUTTON,
	VIEWDOCUMENT_CMD_HTML,
	VIEWDOCUMENT_CMD_HTML2,
	VIEWDOCUMENT_CMD_POSITION,
	VIEWDOCUMENT_CMD_PROGRAM,
	VIEWDOCUMENT_CMD_DESCRIPTION,
	VIEWDOCUMENT_USE_EDITMODE,
	VIEWDOCUMENT_CMD_SIZE
}VIEWDOCUMENT_CMD;

const TCHAR*	g_sViewDocumentCmd[VIEWDOCUMENT_CMD_SIZE]={
	_T("screen"),
	_T("chart"),
	_T("report"),
	_T("button"),
	_T("html"),
	_T("html2"),
	_T("position"),
	_T("SetProgram"),
	_T("SetDescription"),
	_T("USE_EDIT_MODE"),
};

void CDocumentView::AddObject(CViewObject*	pVO){
	if(!pVO) return;
	m_ViewObjectList.push_back(pVO);
}

void CDocumentView::DeleteObject(CViewObject* pVO){
	if(!pVO) return;
	CViewObject* pObj	= NULL;
	for(list<CViewObject*>::iterator iter = m_ViewObjectList.begin();
		iter!=m_ViewObjectList.end();iter++){
			if(pVO == (*iter)){
				m_ViewObjectList.erase(iter);
				delete pVO;
				break;
			}
	}
}

BOOL CDocumentView::Load(CPaser* pPaser){
	BOOL			bRet	= FALSE;
	int				iLoop	= 1;
	TCHAR			token[MAX_PATH];
	TD_TOKEN_TYPE	type;
	int				i;
	list<CPoint>	position;
	int				x = 0, y = 0;
	CString			sProgramPath;

	if(pPaser->TokenOut(TD_DELIMITER_LOPEN)){
		while(iLoop){
			if(!pPaser->IsTokenable()) break;
			type = pPaser->GetToken(token);
			switch(type){
			case TD_TOKEN_NAME:
				{
					CViewObject*	pVO = NULL;
					i = CheckCommand(token, g_sViewDocumentCmd, VIEWDOCUMENT_CMD_SIZE);
					switch(i){
					case VIEWDOCUMENT_CMD_POSITION:
						if(!pPaser->TokenOut(TD_DELIMITER_SOPEN)) goto ERROR_OUT;
						{
							CPoint	point;
							int cx, cy;
							if(!pPaser->GetTokenInt(&cx)) goto ERROR_OUT;
							if(!pPaser->TokenOut(TD_DELIMITER_COMMA)) goto ERROR_OUT;
							if(!pPaser->GetTokenInt(&cy)) goto ERROR_OUT;
							x+=cx;
							y+=cy;
							point.SetPoint(cx,cy);
							position.push_front(point);
						}
						if(!pPaser->TokenOut(TD_DELIMITER_SCLOSE)) goto ERROR_OUT;
						if(!pPaser->TokenOut(TD_DELIMITER_LOPEN)) goto ERROR_OUT;
						iLoop++;
						break;
					case VIEWDOCUMENT_CMD_PROGRAM:
					case VIEWDOCUMENT_CMD_DESCRIPTION:
						if(!pPaser->TokenOut(TD_DELIMITER_SOPEN)) goto ERROR_OUT;
						if(!pPaser->GetTokenString(token)) goto ERROR_OUT;
						if(!pPaser->TokenOut(TD_DELIMITER_SCLOSE)) goto ERROR_OUT;
						if(!pPaser->TokenOut(TD_DELIMITER_SEMICOLON)) goto ERROR_OUT;
						if(i == VIEWDOCUMENT_CMD_PROGRAM) sProgramPath	= token;
						else m_pProperty->SetDescription(token);
						break;
					case VIEWDOCUMENT_USE_EDITMODE:
						AddWatchingFile(pPaser->GetFilePath(), m_hWnd, -1, FALSE);
						break;
					case VIEWDOCUMENT_CMD_SCREEN:
					case VIEWDOCUMENT_CMD_CHART:
					case VIEWDOCUMENT_CMD_REPORT:
					case VIEWDOCUMENT_CMD_BUTTON:
					case VIEWDOCUMENT_CMD_HTML:
						pVO	= CViewObject::New((OBJECT_TYPE)i, this, pPaser);
						if(pVO){
							m_ViewObjectList.push_back(pVO);
							pVO->Paser(pPaser, x, y);
							break;
						}
						goto ERROR_OUT;
					case VIEWDOCUMENT_CMD_HTML2:
						pVO = CViewObject::New(TDOBJECT_HTML, this, pPaser, __webview2_setup_fn);
						if (pVO) {
							m_ViewObjectList.push_back(pVO);
							pVO->Paser(pPaser, x, y);
							break;
						}
						goto ERROR_OUT;
					default:
						goto ERROR_OUT;
					}
				}break;
			case TD_TOKEN_DELIMITER:
				if(*token != *g_PaserDelimiter[TD_DELIMITER_LCLOSE]) goto ERROR_OUT;
				iLoop--;
				if(iLoop){
					CPoint point = position.front();
					x -= point.x;
					y -= point.y;
					position.pop_front();
				}
				if(!iLoop) bRet = TRUE;
				break;
			}
		}
	}
ERROR_OUT:
	position.clear();

	if(bRet && !sProgramPath.IsEmpty()){
		if (!SetProgram(sProgramPath)) return FALSE;
	}
	return bRet;
}

HWND CDocumentView::GetWindowHandle(void){
	return this->m_hWnd;
}

void CDocumentView::SetForegroundDocument(void){
	g_pDocumentList->SetActive(this);
}

void CDocumentView::AddWatchDogPath(LPCTSTR lpszPath, DWORD dwID, BOOL bSearchSubDir){
	if(!lpszPath) return;
	{
		TCHAR	path[MAX_PATH];
		GetFullPathName(lpszPath, MAX_PATH, path, NULL);
		AddWatchingFile(path, m_hWnd, dwID, bSearchSubDir);
	}
}

void CDocumentView::ClearWatchDogPath(DWORD dwID){
	if(dwID==(DWORD)-1) ClearWatchingFileAll();
	else ClearWatchingFile(dwID);
}

void CDocumentView::InvalidateLayout(void){
	if(m_pDocumentImp){
		CRect rc;
		GetClientRect(&rc);
		m_pDocumentImp->OnSize(rc.Width(), rc.Height());
		UpdateLayout();
		RedrawWindow();
	}
}

void CDocumentView::Show(BOOL bShow){
	m_bShow = bShow;
	g_pDocumentList->Show(m_sName, bShow);
}
