#include "stdafx.h"
#include "Main.h"
#include "TestDriveDoc.h"

#include "VisualManagerEx.h"
#include "MainFrm.h"
#include "DocumentChildFrame.h"
#include "AccelCodeDecoder.h"
#include "DocumentCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMainFrame
CMainFrame*		g_pMainFrame	= NULL;

IMPLEMENT_DYNAMIC(CMainFrame, CMainFrameBase)

BEGIN_MESSAGE_MAP(CMainFrame, CMainFrameBase)
	ON_WM_CREATE()
	ON_WM_CLOSE()
	ON_WM_POWERBROADCAST()
	ON_WM_SIZE()
	ON_COMMAND(ID_WINDOW_MANAGER, &CMainFrame::OnWindowManager)
	ON_COMMAND(ID_LOCALE_SELECT, &CMainFrame::OnLocaleSelect)
	ON_COMMAND_RANGE(ID_VIEW_APPLOOK_WIN_2000, ID_VIEW_APPLOOK_WINDOWS_7, &CMainFrame::OnApplicationLook)
	ON_UPDATE_COMMAND_UI_RANGE(ID_VIEW_APPLOOK_WIN_2000, ID_VIEW_APPLOOK_WINDOWS_7, &CMainFrame::OnUpdateApplicationLook)
	ON_COMMAND(ID_TOOLS_OPTIONS, &CMainFrame::OnOptions)
	ON_COMMAND(ID_VIEW_TOGGLE_MAXIMIZE, &CMainFrame::OnViewToggleMaximize)
	ON_WM_SETTINGCHANGE()

	// accel key decode
	ON_COMMAND(ID_EDIT_COPY, &CMainFrame::OnEditCopy)
	ON_COMMAND(ID_EDIT_PASTE, &CMainFrame::OnEditPaste)
	ON_COMMAND(ID_EDIT_CUT, &CMainFrame::OnEditCut)
	ON_COMMAND(ID_EDIT_UNDO, &CMainFrame::OnEditUndo)
	ON_COMMAND(ID_EDIT_SELECT_ALL, &CMainFrame::OnEditSelectAll)
	ON_COMMAND(ID_EDIT_PRINT, &CMainFrame::OnEditPrint)
	ON_COMMAND(ID_EDIT_SAVE, &CMainFrame::OnFileSave)
	ON_COMMAND(ID_TOGGLE_FULL_SCREEN, &CMainFrame::OnToggleFullScreen)

	// testdrive command
	ON_COMMAND(ID_PROCESS_CHECK, &CMainFrame::OnProcessCheck)
	ON_COMMAND(ID_PROCESS_CLEANUP, &CMainFrame::OnProcessCleanUp)
	ON_COMMAND(ID_PROCESS_INITIALIZE, &CMainFrame::OnProcessIntialize)
	ON_COMMAND(ID_TESTDRIVE_POST_PROJECT_OPEN, &CMainFrame::OnPostProjectOpen)

	ON_COMMAND(ID_STATUSBAR_LINK, OnStatusBarWebSiteLink)
END_MESSAGE_MAP()

static LPCTSTR	__APPLICATION_LOOK = _T("ApplicationLook");

CMainFrame::CMainFrame()
{
	g_pMainFrame	= this;
	m_bInitialized	= FALSE;
	//m_bfxWater		= FALSE;
	theApp.m_nAppLook = theApp.GetInt(__APPLICATION_LOOK, ID_VIEW_APPLOOK_OFF_2007_SILVER);
}

CMainFrame::~CMainFrame()
{
	g_pMainFrame	= NULL;
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CMainFrameBase::OnCreate(lpCreateStruct) == -1)
		return -1;

	BOOL bNameValid;
	// 보관된 값에 따라 비주얼 관리자 및 스타일을 설정합니다.
	OnApplicationLook(theApp.m_nAppLook);

	CMDITabInfo mdiTabParams;
	mdiTabParams.m_style					= CMFCTabCtrl::STYLE_3D_VS2005; // 사용할 수 있는 다른 스타일...
	//mdiTabParams.m_bTabIcons				= FALSE;
	mdiTabParams.m_bActiveTabCloseButton	= FALSE;	// FALSE로 설정하여 탭 영역 오른쪽에 닫기 단추를 배치합니다.
	mdiTabParams.m_bEnableTabSwap			= TRUE;		// Tab 간의 자리이동이 가능하게 합니다.
	mdiTabParams.m_bTabIcons				= FALSE;	// TRUE로 설정하여 MDI 탭의 문서 아이콘을 활성화합니다.
	mdiTabParams.m_bAutoColor				= FALSE;	// FALSE로 설정하여 MDI 탭의 자동 색 지정을 비활성화합니다.
	mdiTabParams.m_bDocumentMenu			= TRUE;		// 탭 영역의 오른쪽 가장자리에 문서 메뉴를 활성화합니다.
	mdiTabParams.m_bFlatFrame				= TRUE;
	EnableMDITabbedGroups(TRUE, mdiTabParams);
	CTabbedPane::m_bTabsAlwaysTop			= TRUE;

	m_bCanConvertControlBarToMDIChild		= TRUE;	// 문서가 도킹 가능하도록 변경

	m_wndRibbonBar.Create(this);
	m_wndRibbonBar.LoadFromResource(IDR_RIBBON);

	{
		CMFCRibbonCategory* pCategory = m_wndRibbonBar.AddCategory(_S(OPTION), 0, 0, CSize(16, 16), CSize(32, 32), -1, 0);
		CMFCRibbonPanel* pPannel = pCategory->AddPanel(_S(SYSTEM));
		CMFCRibbonComboBox* pCombo	= new CMFCRibbonComboBox(ID_LOCALE_SELECT, FALSE, 100, _S(LANGUAGE));
		m_pLangCombo = pCombo;
		pPannel->Add(pCombo);
		{
			LOCALE_DESC*	pLocale;
			for (DWORD i = 0; (pLocale = g_Localization.LocaleFromList(i)) != NULL; i++) {
				pCombo->AddItem(pLocale->sName, (DWORD_PTR)pLocale);
				if (g_Localization.CurrentLocale() == pLocale)
					pCombo->SelectItem((int)i);
			}
		}
	}

	// minimize ribbon menu
	{
		/*static LPCTSTR	__sInitializeRiboon = _T("RibbonInitialized");
		if (!theApp.GetInt(__sInitializeRiboon)) {
			m_wndRibbonBar.ToggleMimimizeState();
			theApp.WriteInt(__sInitializeRiboon, 1);
		}*/
		m_wndRibbonBar.ToggleMimimizeState();
	}

	if (!m_wndStatusBar.Create(this))
	{
		TRACE0("상태 표시줄을 만들지 못했습니다.\n");
		return -1;      // 만들지 못했습니다.
	}

	CString strTitlePane1;
	CString strTitlePane2;
	bNameValid = strTitlePane1.LoadString(IDS_STATUS_PANE1);
	ASSERT(bNameValid);
	bNameValid = strTitlePane2.LoadString(IDS_STATUS_PANE2);
	ASSERT(bNameValid);
	m_wndStatusBar.AddElement(new CMFCRibbonStatusBarPane(ID_STATUSBAR_PANE1, strTitlePane1, TRUE), strTitlePane1);
	m_wndStatusBar.AddExtendedElement(new CMFCRibbonLinkCtrl(ID_STATUSBAR_LINK, _T("Questions."), _T("mailto:clonextop@gmail.com?subject=[TestDrive Question]")), _T("Question about TestDrive."));

	// Visual Studio 2005 스타일 도킹 창 동작을 활성화합니다.
	CDockingManager::SetDockingMode(DT_SMART);
	// Visual Studio 2005 스타일 도킹 창 자동 숨김 동작을 활성화합니다.
	EnableAutoHidePanes(CBRS_ALIGN_ANY);

	// 메뉴 항목 이미지를 로드합니다(표준 도구 모음에 없음).
	CMFCToolBar::AddToolBarForImageCollection(IDR_MENU_IMAGES, theApp.m_bHiColorIcons ? IDB_MENU_IMAGES_24 : 0);

	// 도킹 창을 만듭니다.
	if (!CreateDockingWindows())
	{
		TRACE0("도킹 창을 만들지 못했습니다.\n");
		return -1;
	}

	// 창 제목 표시줄에서 문서 이름 및 응용 프로그램 이름의 순서를 전환합니다.
	// 문서 이름이 축소판 그림과 함께 표시되므로 작업 표시줄의 기능성이 개선됩니다.
	ModifyStyle(0, FWS_PREFIXTITLE);
	OnUpdateFrameTitle(FALSE);

	m_bInitialized	= TRUE;

	return 0;
}

void CMainFrame::OnClose(){
	// 모든 도큐먼트의 락상태 조사
	if(!g_pDocumentList->RemoveAll()){
		g_pTestDrive->LogWarning(_T("현재 '락'상태의 도큐먼트가 존재하여, 종료할 수 없습니다. 도큐먼트 실행이 중지된 후 다시 시도하십시오."));
		return;
	}
	// Release documents
	if(g_pTestDrive->IsProjectOpen()){
		if(!g_pTestDrive->CloseProject()){
			g_pTestDrive->LogWarning(_T("현재 실행중인 프로세스가 존재합니다. 실행중인 프로세스가 종료된 후 다시 시도하십시오."));
			return;
		}
	}

	CMainFrameBase::OnClose();
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	cs.style &= ~(LONG)(FWS_ADDTOTITLE);
	if( !CMainFrameBase::PreCreateWindow(cs) )
		return FALSE;

	return TRUE;
}

CMDIChildWndEx* CMainFrame::ControlBarToTabbedDocument(CDockablePane* pBar)
{
	ASSERT_VALID(this);
	ASSERT_VALID(pBar);

	CDocumentChildFrame* pFrame = new CDocumentChildFrame;
	ASSERT_VALID(pFrame);

	pBar->m_bWasFloatingBeforeTabbed = TRUE;	// tab 해재시 강제로 floating 되도록 만듬.

	CString strName;
	pBar->GetWindowText(strName);

	if (!pFrame->Create(
		AfxRegisterWndClass(CS_DBLCLKS, 0, (HBRUSH)(COLOR_BTNFACE + 1), pBar->GetIcon(FALSE)),
		strName, WS_CHILD | WS_VISIBLE | WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, rectDefault, this))
	{
		return NULL;
	}

	pFrame->SetTitle(strName);
	pFrame->SetWindowText(strName);
	pFrame->AddTabbedPane(pBar);

	return pFrame;
}
/*
void CMainFrame::OnTimer(UINT_PTR nIDEvent)
{
	Invalidate(FALSE);

	CMainFrameBase::OnTimer(nIDEvent);
}*/

void CMainFrame::DrawClientArea(CDC* pDC){
	CRect rect;
	PAINTSTRUCT ps;
	BOOL	bDCPresent	= (pDC != NULL);
	if(!bDCPresent){
		m_wndClientArea.BeginPaint(&ps);
		pDC	= CDC::FromHandle(ps.hdc);
	}
	
	
	if (m_wndClientArea.IsWindowVisible() && m_imgBackground.IsInitialize()){
		//BOOL	bContinue = m_fxWater.Render((DWORD*)m_imgBackground.GetPointer(), (DWORD*)m_imgBackgroundWater.GetPointer());
		CRect	rect_clip;
		GetClientRect(&rect);
		m_wndClientArea.GetClientRect(&rect);
		UINT	width	= rect.right - rect.left;
		UINT	height = rect.bottom - rect.top;

		{

			int i_width = m_imgBackground.Width();
			int i_height = m_imgBackground.Height();
			if(i_width > (int)width){
				i_height	= i_height * width / i_width;
				i_width		= width;
			}
			if(i_height > (int)height){
				i_width		= i_width * height / i_height;
				i_height	= height;
			}
			int sx			= (width - i_width) / 2;
			int sy			= (height - i_height) / 2;
			CRect clip(sx, sy, sx+i_width-1, sy+i_height-1);
			m_imgBackground.Present(pDC, &clip, &rect);
		}

		{
			CFont font, *old_font;
			CString sText;
			{
				CString sVersion;
				sVersion.LoadStringW(ID_PROJECT_VERSION);
				sText.Format(_T("%s (Released at ") _T(__DATE__) _T(")"), sVersion);
			}

			font.CreateFont(13, 0, 0, 0, FW_MEDIUM, TRUE, FALSE, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
				CLEARTYPE_NATURAL_QUALITY, DEFAULT_PITCH|FF_SWISS, _T("Arial"));
			::GetWindowRect(m_hWndMDIClient, &rect);

			old_font= pDC->SelectObject(&font);
			pDC->SetBkMode(TRANSPARENT);
			CSize tx_size = pDC->GetTextExtent(sText);
			pDC->TextOut(width - tx_size.cx-2, height - tx_size.cy-2, sText);
			pDC->SetBkMode(OPAQUE);

			pDC->SelectObject(&old_font);
		}
		/*
		if (bContinue && !m_bfxWater){
			m_bfxWater = TRUE;
			SetTimer(0, 20, NULL);
		}
		else{
			if (!bContinue && m_bfxWater){
				KillTimer(0);
				m_bfxWater = FALSE;
			}
		}*/
	}
/*	else if (m_bfxWater){
		KillTimer(0);
		m_bfxWater = FALSE;
	}*/
	if(!bDCPresent){
		m_wndClientArea.ReleaseDC(pDC);
		m_wndClientArea.EndPaint(&ps);
	}
}

LRESULT CMainFrame::DefWindowProc(UINT nMsg, WPARAM wParam, LPARAM lParam){
	if(WM_SETTEXT == nMsg)
		return CWnd::DefWindowProc(nMsg, wParam, lParam);
	if(WM_PAINT == nMsg && m_hWndMDIClient){
		DrawClientArea();
	}
	return CMainFrameBase::DefWindowProc(nMsg, wParam, lParam);
}

#include <afxcontextmenumanager.h>
BOOL CMainFrame::OnShowMDITabContextMenu(CPoint point, DWORD dwAllowedItems, BOOL bTabDrop)
{
	return CMainFrameBase::OnShowMDITabContextMenu(point, dwAllowedItems, bTabDrop);
	/*
	if ((dwAllowedItems & AFX_MDI_CAN_BE_DOCKED) == 0)
	{
		return FALSE;
	}

	if (afxContextMenuManager == NULL)
	{
		return FALSE;
	}

	const UINT idTabbed = (UINT) -106;

	CMenu menu;
	menu.CreatePopupMenu();

	CString strItem;
	ENSURE(strItem.LoadString(IDS_AFXBARRES_TABBED));

	menu.AppendMenu(MF_STRING, idTabbed, strItem);
	menu.CheckMenuItem(idTabbed, MF_CHECKED);

	HWND hwndThis = GetSafeHwnd();

	int nMenuResult = afxContextMenuManager->TrackPopupMenu(menu, point.x, point.y, this);

	if (::IsWindow(hwndThis))
	{
		switch(nMenuResult)
		{
		case idTabbed:
			{
				CMDIChildWndEx* pMDIChild = DYNAMIC_DOWNCAST(CMDIChildWndEx, MDIGetActive());
				if (pMDIChild != NULL)
				{
					TabbedDocumentToControlBar(pMDIChild);
				}
			}
		}
	}

	return TRUE;
	*/
}

void CMainFrame::OnSize(UINT nType, int cx, int cy){
	CMainFrameBase::OnSize(nType, cx, cy);
	Invalidate();
}

void CMainFrame::OnUpdateFrameTitle(BOOL bAddToTitle){
	// 원본에서는 여기서 UpdateFrameTitleForDocument(LPCTSTR lpszDocName) 를 호출함.
	CString sTitle;
	if(!g_pTestDrive->IsProjectOpen())
		sTitle.Format(_T("%s - TestDrive Profiling Master"), g_pTestDrive->GetTitle());
	else sTitle = g_pTestDrive->GetTitle();
	SetWindowText(sTitle);
}

BOOL CMainFrame::CreateDockingWindows()
{
	// Profile view
	if (!m_wndProfile.Create(_S(PROFILE_VIEW), this, CRect(0, 0, 200, 500), TRUE, ID_VIEW_PROFILEWND, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_LEFT| CBRS_FLOAT_MULTI))
	{
		TRACE0("프로파일 뷰 창을 만들지 못했습니다.\n");
		return FALSE;
	}

	// output window
	if (!m_wndOutput.Create(_S(OUTPUT_WND), this, CRect(0, 0, 500, 150), TRUE, ID_VIEW_OUTPUTWND, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_BOTTOM | CBRS_FLOAT_MULTI))
	{
		TRACE0("출력 창을 만들지 못했습니다.\n");
		return FALSE;
	}

	// Properties window
	if (!m_wndProperties.Create(_S(PROPERTY_VIEW), this, CRect(0, 0, 200, 500), TRUE, ID_VIEW_PROPERTIESWND, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_RIGHT | CBRS_FLOAT_MULTI))
	{
		TRACE0("속성 창을 만들지 못했습니다.\n");
		return FALSE;
	}

	m_wndProfile.EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndProfile);
	m_wndOutput.EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndOutput);
	m_wndProperties.EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndProperties);

	// 향상된 창 관리 대화 상자를 활성화합니다.
	EnableWindowsDialog(ID_WINDOW_MANAGER, ID_WINDOW_MANAGER, TRUE);

	SetDockingWindowIcons(theApp.m_bHiColorIcons);
	{
		BOOL	bInvalidate = FALSE;
		if (!m_wndProfile.IsVisible()) {
			m_wndProfile.ShowPane(TRUE, FALSE, FALSE);
			bInvalidate = TRUE;
		}
		if (!m_wndProperties.IsVisible()) {
			m_wndProperties.ShowPane(TRUE, FALSE, FALSE);
			bInvalidate = TRUE;
		}
		if (!m_wndOutput.IsVisible()) {
			m_wndOutput.ShowPane(TRUE, FALSE, FALSE);
			bInvalidate = TRUE;
		}
		if(bInvalidate) RecalcLayout(FALSE);
	}
	

	return TRUE;
}

void CMainFrame::SetDockingWindowIcons(BOOL bHiColorIcons)
{
	HICON hProfileViewIcon = (HICON) ::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(bHiColorIcons ? IDI_FILE_VIEW_HC : IDI_FILE_VIEW), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0);
	m_wndProfile.SetIcon(hProfileViewIcon, FALSE);

	HICON hOutputBarIcon = (HICON) ::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(bHiColorIcons ? IDI_OUTPUT_WND_HC : IDI_OUTPUT_WND), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0);
	m_wndOutput.SetIcon(hOutputBarIcon, FALSE);

	HICON hPropertiesBarIcon = (HICON) ::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(bHiColorIcons ? IDI_PROPERTIES_WND_HC : IDI_PROPERTIES_WND), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0);
	m_wndProperties.SetIcon(hPropertiesBarIcon, FALSE);

	UpdateMDITabbedBarsIcons();
}

// CMainFrame 진단

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CMainFrameBase::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CMainFrameBase::Dump(dc);
}
#endif //_DEBUG


// CMainFrame 메시지 처리기

void CMainFrame::OnWindowManager()
{
	ShowWindowsDialog();
}

void CMainFrame::OnLocaleSelect() {
	LOCALE_DESC*	pLocale	= (LOCALE_DESC*)m_pLangCombo->GetItemData(m_pLangCombo->GetCurSel());
	if (pLocale != g_Localization.CurrentLocale()) {
		AfxMessageBox(_S(LANGUAGE_CHANGE));
		g_Localization.SetLocale(pLocale->dwLangID, pLocale->dwSubLangID, TRUE);
	}
}

void CMainFrame::OnApplicationLook(UINT id)
{
	CWaitCursor wait;

	theApp.m_nAppLook = id;

	m_imgBackground.ReleaseAll();
	//m_imgBackgroundWater.ReleaseAll();

	switch (theApp.m_nAppLook)
	{
	case ID_VIEW_APPLOOK_WIN_2000:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManager));
		m_wndRibbonBar.SetWindows7Look(FALSE);
		break;

	case ID_VIEW_APPLOOK_OFF_XP:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOfficeXP));
		m_wndRibbonBar.SetWindows7Look(FALSE);
		break;

	case ID_VIEW_APPLOOK_WIN_XP:
		CMFCVisualManagerWindows::m_b3DTabsXPTheme = TRUE;
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));
		m_wndRibbonBar.SetWindows7Look(FALSE);
		break;

	case ID_VIEW_APPLOOK_OFF_2003:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2003));
		CDockingManager::SetDockingMode(DT_SMART);
		m_wndRibbonBar.SetWindows7Look(FALSE);
		break;

	case ID_VIEW_APPLOOK_VS_2005:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerVS2005));
		CDockingManager::SetDockingMode(DT_SMART);
		m_wndRibbonBar.SetWindows7Look(FALSE);
		break;

	case ID_VIEW_APPLOOK_VS_2008:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerVS2008));
		CDockingManager::SetDockingMode(DT_SMART);
		m_wndRibbonBar.SetWindows7Look(FALSE);
		break;

	case ID_VIEW_APPLOOK_WINDOWS_7:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows7));
		CDockingManager::SetDockingMode(DT_SMART);
		m_wndRibbonBar.SetWindows7Look(TRUE);
		break;

	default:
		switch (theApp.m_nAppLook)
		{
		case ID_VIEW_APPLOOK_OFF_2007_BLUE:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_LunaBlue);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_BLACK:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_ObsidianBlack);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_SILVER:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_Silver);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_AQUA:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_Aqua);
			break;
		}

		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CVisualManagerEx));
		
		CDockingManager::SetDockingMode(DT_SMART);
		m_wndRibbonBar.SetWindows7Look(FALSE);
	}

	{
		m_imgBackground.UseAlphaChannel();
		m_imgBackground.CreateFromResource(IDB_BACKGROUND, COLORTYPE_ARGB_8888);
		
		m_imgBackground.EnableOutLine(FALSE);
		m_imgBackground.EnableAntialiasing();

		//m_imgBackgroundWater.CreateFromResource(IDB_BACKGROUND, COLORTYPE_ARGB_8888);
		//m_imgBackgroundWater.EnableOutLine(FALSE);
		//m_imgBackgroundWater.EnableAntialiasing();

		//m_fxWater.Create(m_imgBackground.Width(), m_imgBackground.Height());
		{
			typedef union{
				DWORD	rgba;
				struct{
					BYTE	r,g,b,a;
				};
			}COLOR;
			COLOR	out_color, color, ref;
			DWORD*	pBuffer	= (DWORD*)(m_imgBackground.GetPointer());
			DWORD	dwSize	= m_imgBackground.Width() * m_imgBackground.Height();
			ref.rgba		= ((CVisualManagerEx*)CMFCVisualManagerOffice2007::GetInstance())->GetBkColor();

			for(DWORD i=0;i<dwSize;i++){

				color.rgba	= pBuffer[i];
				out_color.b = (BYTE)((((DWORD)color.b * color.a) + ((DWORD)ref.r * (255 - color.a))) / 255);	// ref 와 color 의 rgb 순서가 다르다.
				out_color.g = (BYTE)((((DWORD)color.g * color.a) + ((DWORD)ref.g * (255 - color.a))) / 255);
				out_color.r = (BYTE)((((DWORD)color.r * color.a) + ((DWORD)ref.b * (255 - color.a))) / 255);
				out_color.a = 255;			
				pBuffer[i]	= out_color.rgba;
			}
			//m_fxWater.SetBackColor(ref.rgba);
			m_imgBackground.SetBackColor(ref.rgba);
			//m_imgBackgroundWater.SetBackColor(ref.rgba);
			//m_imgBackgroundWater.CopyFromBuffer(&m_imgBackground);
			//m_imgBackgroundWater.GetRect(&m_FxRect);
		}
	}

	RedrawWindow(NULL, NULL, RDW_ALLCHILDREN | RDW_INVALIDATE | RDW_UPDATENOW | RDW_FRAME | RDW_ERASE);

	theApp.WriteInt(__APPLICATION_LOOK, theApp.m_nAppLook);
}

void CMainFrame::OnUpdateApplicationLook(CCmdUI* pCmdUI)
{
	pCmdUI->SetRadio(theApp.m_nAppLook == pCmdUI->m_nID);
}

void CMainFrame::OnStatusBarWebSiteLink()
{
	CMFCRibbonLinkCtrl* pLink = (CMFCRibbonLinkCtrl*)m_wndStatusBar.FindByID(ID_STATUSBAR_LINK);
	if (pLink != NULL) pLink->OpenLink();
}

void CMainFrame::OnOptions()
{
	CMFCRibbonCustomizeDialog *pOptionsDlg = new CMFCRibbonCustomizeDialog(this, &m_wndRibbonBar);
	ASSERT(pOptionsDlg != NULL);

	pOptionsDlg->DoModal();
	delete pOptionsDlg;
}

void CMainFrame::OnViewToggleMaximize()
{
	BOOL	bDock[3], bVisible[3];

	BOOL bShowed = FALSE;

	bDock[0] = m_wndProfile.IsDocked();
	bDock[1] = m_wndOutput.IsDocked();
	bDock[2] = m_wndProperties.IsDocked();
	bVisible[0] = m_wndProfile.IsPaneVisible();
	bVisible[1] = m_wndOutput.IsPaneVisible();
	bVisible[2] = m_wndProperties.IsPaneVisible();

	bShowed |= bDock[0] & bVisible[0];
	bShowed |= bDock[1] & bVisible[1];
	bShowed |= bDock[2] & bVisible[2];

	if (bDock[0] && (bShowed == bVisible[0])) {
		m_wndProfile.ToggleAutoHide();
	}
	if (bDock[1] && (bShowed == bVisible[1])) {
		m_wndOutput.ToggleAutoHide();
	}
	if (bDock[2] && (bShowed == bVisible[2])) {
		m_wndProperties.ToggleAutoHide();
	}
}

void CMainFrame::OnSettingChange(UINT uFlags, LPCTSTR lpszSection)
{
	CMainFrameBase::OnSettingChange(uFlags, lpszSection);
	m_wndOutput.UpdateFonts();
}

UINT CMainFrame::OnPowerBroadcast(UINT wParam, LPARAM lParam){
	if(wParam == PBT_APMRESUMESUSPEND){
		// hibernate mode 탈출시 초기화된 look&feel 다시 적용.
		OnApplicationLook(theApp.m_nAppLook);
	}
	return TRUE;
}

BOOL CMainFrame::OnEraseMDIClientBackground(CDC* pDC){
	DrawClientArea(pDC);
	return TRUE;
}

void CMainFrame::OnEditCopy(){
	AccelCodeDecoder::DoAccel(ACCEL_CODE_COPY);
}

void CMainFrame::OnEditPaste(){
	AccelCodeDecoder::DoAccel(ACCEL_CODE_PASTE);
}

void CMainFrame::OnEditCut(){
	AccelCodeDecoder::DoAccel(ACCEL_CODE_CUT);
}

void CMainFrame::OnEditUndo(){
	AccelCodeDecoder::DoAccel(ACCEL_CODE_UNDO);
}

void CMainFrame::OnEditSelectAll(){
	AccelCodeDecoder::DoAccel(ACCEL_CODE_SELECTALL);
}

void CMainFrame::OnEditPrint(){
	AccelCodeDecoder::DoAccel(ACCEL_CODE_PRINT);
}

void CMainFrame::OnFileSave(){
	AccelCodeDecoder::DoAccel(ACCEL_CODE_SAVE);
}

void CMainFrame::OnToggleFullScreen(){
	AccelCodeDecoder::DoAccel(ACCEL_CODE_TOGGLE_FULLSCREEN);
}

void CMainFrame::OnProcessIntialize() {
	g_pTestDrive->Build(TESTDRIVE_PROFILE_INITIALIZE);
}

void CMainFrame::OnProcessCheck() {
	g_pTestDrive->Build(TESTDRIVE_PROFILE_CHECK);
}

void CMainFrame::OnProcessCleanUp() {
	g_pTestDrive->Build(TESTDRIVE_PROFILE_CLEANUP);
}

void CMainFrame::OnPostProjectOpen(){
	if (!m_bInitialized) {
		PostMessage(WM_COMMAND, ID_TESTDRIVE_POST_PROJECT_OPEN);
		return;
	}
	//GetActiveDocument()->OnOpenDocument(NULL);
	if (!g_pTestDrive->OpenProject(NULL)) {
		MessageBox(_S(PROJECT_CAN_NOT_LOADING), _S(ERROR), MB_ICONEXCLAMATION | MB_OK);
		g_pTestDrive->CloseProject(TRUE);
	}
}
