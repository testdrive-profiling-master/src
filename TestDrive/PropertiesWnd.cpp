#include "stdafx.h"

#include "PropertiesWnd.h"
#include "MainFrm.h"
#include "Main.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CPropertiesWnd::CPropertiesWnd()
{
}

CPropertiesWnd::~CPropertiesWnd()
{
}

BEGIN_MESSAGE_MAP(CPropertiesWnd, CToolBaseWnd)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_COMMAND(ID_EXPAND_ALL, OnExpandAllProperties)
	ON_UPDATE_COMMAND_UI(ID_EXPAND_ALL, OnUpdateExpandAllProperties)
	ON_COMMAND(ID_SORTPROPERTIES, OnSortProperties)
	ON_UPDATE_COMMAND_UI(ID_SORTPROPERTIES, OnUpdateSortProperties)
	ON_COMMAND(ID_PROPERTIES1, OnProperties1)
	ON_UPDATE_COMMAND_UI(ID_PROPERTIES1, OnUpdateProperties1)
	ON_COMMAND(ID_PROPERTIES2, OnProperties2)
	ON_UPDATE_COMMAND_UI(ID_PROPERTIES2, OnUpdateProperties2)
	ON_WM_SETFOCUS()
	ON_WM_SETTINGCHANGE()
	//ON_REGISTERED_MESSAGE(AFX_WM_PROPERTY_CHANGED, OnPropertyGridChange)	// http://rajent.tistory.com/237
	 
END_MESSAGE_MAP()

void CPropertiesWnd::AdjustLayout()
{
	if (GetSafeHwnd() == NULL)
	{
		return;
	}

	CRect rectClient;
	GetClientRect(rectClient);

	int cyTlb = m_wndToolBar.CalcFixedLayout(FALSE, TRUE).cy;

	m_wndToolBar.SetWindowPos(NULL, rectClient.left, rectClient.top, rectClient.Width(), cyTlb, SWP_NOACTIVATE | SWP_NOZORDER);
	g_PropertyGrid.SetWindowPos(NULL, rectClient.left, rectClient.top + cyTlb, rectClient.Width(), rectClient.Height() -(cyTlb), SWP_NOACTIVATE | SWP_NOZORDER);
}

void CPropertiesWnd::SetVSDotNetLook(BOOL bSet)
{
	g_PropertyGrid.SetVSDotNetLook(bSet);
	g_PropertyGrid.SetGroupNameFullWidth(bSet);
}

int CPropertiesWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CToolBaseWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rectDummy;
	rectDummy.SetRectEmpty();

	// 콤보 상자를 만듭니다.
	const DWORD dwViewStyle = WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST | WS_BORDER | CBS_SORT | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;

	if (!g_PropertyGrid.Create(WS_VISIBLE | WS_CHILD, rectDummy, this, 2))
	{
		TRACE0("속성 표를 만들지 못했습니다. \n");
		return -1;      // 만들지 못했습니다.
	}

	g_PropertyGrid.EnableHeaderCtrl(FALSE);
	g_PropertyGrid.EnableDescriptionArea();
	g_PropertyGrid.SetVSDotNetLook();
	g_PropertyGrid.MarkModifiedProperties();
	g_PropertyGrid.SetDescriptionRows(8);

	InitPropList();

	m_wndToolBar.Create(this, AFX_DEFAULT_TOOLBAR_STYLE, IDR_PROPERTIES);
	m_wndToolBar.LoadToolBar(IDR_PROPERTIES, 0, 0, TRUE /* 잠금 */);
	m_wndToolBar.CleanUpLockedImages();
	m_wndToolBar.LoadBitmap(theApp.m_bHiColorIcons ? IDB_PROPERTIES_HC : IDR_PROPERTIES, 0, 0, TRUE /* 잠금 */);

	m_wndToolBar.SetPaneStyle(m_wndToolBar.GetPaneStyle() | CBRS_TOOLTIPS | CBRS_FLYBY);
	m_wndToolBar.SetPaneStyle(m_wndToolBar.GetPaneStyle() & ~(CBRS_GRIPPER | CBRS_SIZE_DYNAMIC | CBRS_BORDER_TOP | CBRS_BORDER_BOTTOM | CBRS_BORDER_LEFT | CBRS_BORDER_RIGHT));
	m_wndToolBar.SetOwner(this);

	// 모든 명령은 부모 프레임이 아닌 이 컨트롤을 통해 라우팅됩니다.
	m_wndToolBar.SetRouteCommandsViaFrame(FALSE);

	AdjustLayout();
	return 0;
}

void CPropertiesWnd::OnSize(UINT nType, int cx, int cy)
{
	CToolBaseWnd::OnSize(nType, cx, cy);
	AdjustLayout();
}

void CPropertiesWnd::OnExpandAllProperties()
{
	g_PropertyGrid.ExpandAll();
}

void CPropertiesWnd::OnUpdateExpandAllProperties(CCmdUI* pCmdUI)
{
}

void CPropertiesWnd::OnSortProperties()
{
	g_PropertyGrid.SetAlphabeticMode(!g_PropertyGrid.IsAlphabeticMode());
}

void CPropertiesWnd::OnUpdateSortProperties(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(g_PropertyGrid.IsAlphabeticMode());
}

void CPropertiesWnd::OnProperties1()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	//AfxMessageBox(_T("OnProperties1"));
}

void CPropertiesWnd::OnUpdateProperties1(CCmdUI* /*pCmdUI*/)
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
}

void CPropertiesWnd::OnProperties2()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	//AfxMessageBox(_T("OnProperties2"));
}

void CPropertiesWnd::OnUpdateProperties2(CCmdUI* /*pCmdUI*/)
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
}

void CPropertiesWnd::InitPropList()
{
	SetPropListFont();

	g_PropertyGrid.RemoveAll();

	//int count = g_pDocumentList->GetCount();

	//if(!count) return;

	
	/*
	CMFCPropertyGridProperty* pProp;

	{
		CMFCPropertyGridProperty* pGroup1 = new CMFCPropertyGridProperty(_T("프로젝트"));

		pGroup1->AddSubItem(new CMFCPropertyGridProperty(_T("타이틀"), (_variant_t) _T("정보"), _T("프로젝트 타이틀을 지정합니다.")));

		pGroup1->AddSubItem(new CMFCPropertyGridProperty(_T("3D 모양"), (_variant_t) false, _T("창에 굵은 글꼴이 아닌 글꼴을 지정하고, 컨트롤에 3D 테두리를 지정합니다.")));

		pProp = new CMFCPropertyGridProperty(_T("테두리"), _T("Dialog Frame"), _T("None, Thin, Resizable 또는 Dialog Frame 중 하나를 지정합니다."));
		pProp->AddOption(_T("None"));
		pProp->AddOption(_T("Thin"));
		pProp->AddOption(_T("Resizable"));
		pProp->AddOption(_T("Dialog Frame"));
		pProp->AllowEdit(FALSE);

		pGroup1->AddSubItem(pProp);
		

		g_PropertyGrid.AddProperty(pGroup1);
	}

	{
		CMFCPropertyGridProperty* pSize = new CMFCPropertyGridProperty(_T("창 크기"), 0, TRUE);

		pProp = new CMFCPropertyGridProperty(_T("높이"), (_variant_t) 250l, _T("창의 높이를 지정합니다."));
		pProp->EnableSpinControl(TRUE, 50, 300);
		pSize->AddSubItem(pProp);

		pProp = new CMFCPropertyGridProperty( _T("너비"), (_variant_t) 150l, _T("창의 너비를 지정합니다."));
		pProp->EnableSpinControl(TRUE, 50, 200);
		pSize->AddSubItem(pProp);

		g_PropertyGrid.AddProperty(pSize);
	}

	{
		CMFCPropertyGridProperty* pGroup2 = new CMFCPropertyGridProperty(_T("글꼴"));

		LOGFONT lf;
		CFont* font = CFont::FromHandle((HFONT) GetStockObject(DEFAULT_GUI_FONT));
		font->GetLogFont(&lf);

		lstrcpy(lf.lfFaceName, _T("맑은 고딕"));

		pGroup2->AddSubItem(new CMFCPropertyGridFontProperty(_T("글꼴"), lf, CF_EFFECTS | CF_SCREENFONTS, _T("창의 기본 글꼴을 지정합니다.")));
		pGroup2->AddSubItem(new CMFCPropertyGridProperty(_T("시스템 글꼴을 사용합니다."), (_variant_t) true, _T("창에서 MS Shell Dlg 글꼴을 사용하도록 지정합니다.")));

		g_PropertyGrid.AddProperty(pGroup2);

		CMFCPropertyGridProperty* pGroup3 = new CMFCPropertyGridProperty(_T("기타"));
		pProp = new CMFCPropertyGridProperty(_T("(이름)"), _T("응용 프로그램"));
		pProp->Enable(FALSE);
		pGroup3->AddSubItem(pProp);

		{
			CMFCPropertyGridColorProperty* pColorProp = new CMFCPropertyGridColorProperty(_T("창 색상"), RGB(210, 192, 254), NULL, _T("창의 기본 색상을 지정합니다."));
			pColorProp->EnableOtherButton(_T("기타..."));
			pColorProp->EnableAutomaticButton(_T("기본값"), ::GetSysColor(COLOR_3DFACE));
			pGroup3->AddSubItem(pColorProp);

			static TCHAR BASED_CODE szFilter[] = _T("아이콘 파일(*.ico)|*.ico|모든 파일(*.*)|*.*||");
			pGroup3->AddSubItem(new CMFCPropertyGridFileProperty(_T("아이콘"), TRUE, _T(""), _T("ico"), 0, szFilter, _T("창 아이콘을 지정합니다.")));

			pGroup3->AddSubItem(new CMFCPropertyGridFileProperty(_T("프로파일"), _T("c:\\")));
		}
		g_PropertyGrid.AddProperty(pGroup3);
	}

	{
		CMFCPropertyGridProperty* pGroup4 = new CMFCPropertyGridProperty(_T("계층"));

		CMFCPropertyGridProperty* pGroup41 = new CMFCPropertyGridProperty(_T("첫번째 하위 수준"));
		pGroup4->AddSubItem(pGroup41);

		CMFCPropertyGridProperty* pGroup411 = new CMFCPropertyGridProperty(_T("두 번째 하위 수준"));
		pGroup41->AddSubItem(pGroup411);

		pGroup411->AddSubItem(new CMFCPropertyGridProperty(_T("항목 1"), (_variant_t) _T("값 1"), _T("설명입니다.")));
		pGroup411->AddSubItem(new CMFCPropertyGridProperty(_T("항목 2"), (_variant_t) _T("값 2"), _T("설명입니다.")));
		pGroup411->AddSubItem(new CMFCPropertyGridProperty(_T("항목 3"), (_variant_t) _T("값 3"), _T("설명입니다.")));

		pGroup4->Expand(FALSE);
		g_PropertyGrid.AddProperty(pGroup4);
	}*/
}

void CPropertiesWnd::OnSetFocus(CWnd* pOldWnd)
{
	CToolBaseWnd::OnSetFocus(pOldWnd);
	//g_PropertyGrid.SetFocus();
}

void CPropertiesWnd::OnSettingChange(UINT uFlags, LPCTSTR lpszSection)
{
	CDockablePane::OnSettingChange(uFlags, lpszSection);
	SetPropListFont();
}

LRESULT CPropertiesWnd::OnPropertyGridChange(WPARAM, LPARAM){

	AfxMessageBox(_T("AFx"));
	return 0;
}

void CPropertiesWnd::SetPropListFont()
{
	::DeleteObject(m_fntPropList.Detach());

	LOGFONT lf;
	afxGlobalData.fontRegular.GetLogFont(&lf);

	NONCLIENTMETRICS info;
	info.cbSize = sizeof(info);

	afxGlobalData.GetNonClientMetrics(info);

	lf.lfHeight = info.lfMenuFont.lfHeight;
	lf.lfWeight = info.lfMenuFont.lfWeight;
	lf.lfItalic = info.lfMenuFont.lfItalic;

	m_fntPropList.CreateFontIndirect(&lf);

	g_PropertyGrid.SetFont(&m_fntPropList);
}
