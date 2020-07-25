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

	// �޺� ���ڸ� ����ϴ�.
	const DWORD dwViewStyle = WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST | WS_BORDER | CBS_SORT | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;

	if (!g_PropertyGrid.Create(WS_VISIBLE | WS_CHILD, rectDummy, this, 2))
	{
		TRACE0("�Ӽ� ǥ�� ������ ���߽��ϴ�. \n");
		return -1;      // ������ ���߽��ϴ�.
	}

	g_PropertyGrid.EnableHeaderCtrl(FALSE);
	g_PropertyGrid.EnableDescriptionArea();
	g_PropertyGrid.SetVSDotNetLook();
	g_PropertyGrid.MarkModifiedProperties();
	g_PropertyGrid.SetDescriptionRows(8);

	InitPropList();

	m_wndToolBar.Create(this, AFX_DEFAULT_TOOLBAR_STYLE, IDR_PROPERTIES);
	m_wndToolBar.LoadToolBar(IDR_PROPERTIES, 0, 0, TRUE /* ��� */);
	m_wndToolBar.CleanUpLockedImages();
	m_wndToolBar.LoadBitmap(theApp.m_bHiColorIcons ? IDB_PROPERTIES_HC : IDR_PROPERTIES, 0, 0, TRUE /* ��� */);

	m_wndToolBar.SetPaneStyle(m_wndToolBar.GetPaneStyle() | CBRS_TOOLTIPS | CBRS_FLYBY);
	m_wndToolBar.SetPaneStyle(m_wndToolBar.GetPaneStyle() & ~(CBRS_GRIPPER | CBRS_SIZE_DYNAMIC | CBRS_BORDER_TOP | CBRS_BORDER_BOTTOM | CBRS_BORDER_LEFT | CBRS_BORDER_RIGHT));
	m_wndToolBar.SetOwner(this);

	// ��� ����� �θ� �������� �ƴ� �� ��Ʈ���� ���� ����õ˴ϴ�.
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
	// TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.
	//AfxMessageBox(_T("OnProperties1"));
}

void CPropertiesWnd::OnUpdateProperties1(CCmdUI* /*pCmdUI*/)
{
	// TODO: ���⿡ ��� ������Ʈ UI ó���� �ڵ带 �߰��մϴ�.
}

void CPropertiesWnd::OnProperties2()
{
	// TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.
	//AfxMessageBox(_T("OnProperties2"));
}

void CPropertiesWnd::OnUpdateProperties2(CCmdUI* /*pCmdUI*/)
{
	// TODO: ���⿡ ��� ������Ʈ UI ó���� �ڵ带 �߰��մϴ�.
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
		CMFCPropertyGridProperty* pGroup1 = new CMFCPropertyGridProperty(_T("������Ʈ"));

		pGroup1->AddSubItem(new CMFCPropertyGridProperty(_T("Ÿ��Ʋ"), (_variant_t) _T("����"), _T("������Ʈ Ÿ��Ʋ�� �����մϴ�.")));

		pGroup1->AddSubItem(new CMFCPropertyGridProperty(_T("3D ���"), (_variant_t) false, _T("â�� ���� �۲��� �ƴ� �۲��� �����ϰ�, ��Ʈ�ѿ� 3D �׵θ��� �����մϴ�.")));

		pProp = new CMFCPropertyGridProperty(_T("�׵θ�"), _T("Dialog Frame"), _T("None, Thin, Resizable �Ǵ� Dialog Frame �� �ϳ��� �����մϴ�."));
		pProp->AddOption(_T("None"));
		pProp->AddOption(_T("Thin"));
		pProp->AddOption(_T("Resizable"));
		pProp->AddOption(_T("Dialog Frame"));
		pProp->AllowEdit(FALSE);

		pGroup1->AddSubItem(pProp);
		

		g_PropertyGrid.AddProperty(pGroup1);
	}

	{
		CMFCPropertyGridProperty* pSize = new CMFCPropertyGridProperty(_T("â ũ��"), 0, TRUE);

		pProp = new CMFCPropertyGridProperty(_T("����"), (_variant_t) 250l, _T("â�� ���̸� �����մϴ�."));
		pProp->EnableSpinControl(TRUE, 50, 300);
		pSize->AddSubItem(pProp);

		pProp = new CMFCPropertyGridProperty( _T("�ʺ�"), (_variant_t) 150l, _T("â�� �ʺ� �����մϴ�."));
		pProp->EnableSpinControl(TRUE, 50, 200);
		pSize->AddSubItem(pProp);

		g_PropertyGrid.AddProperty(pSize);
	}

	{
		CMFCPropertyGridProperty* pGroup2 = new CMFCPropertyGridProperty(_T("�۲�"));

		LOGFONT lf;
		CFont* font = CFont::FromHandle((HFONT) GetStockObject(DEFAULT_GUI_FONT));
		font->GetLogFont(&lf);

		lstrcpy(lf.lfFaceName, _T("���� ���"));

		pGroup2->AddSubItem(new CMFCPropertyGridFontProperty(_T("�۲�"), lf, CF_EFFECTS | CF_SCREENFONTS, _T("â�� �⺻ �۲��� �����մϴ�.")));
		pGroup2->AddSubItem(new CMFCPropertyGridProperty(_T("�ý��� �۲��� ����մϴ�."), (_variant_t) true, _T("â���� MS Shell Dlg �۲��� ����ϵ��� �����մϴ�.")));

		g_PropertyGrid.AddProperty(pGroup2);

		CMFCPropertyGridProperty* pGroup3 = new CMFCPropertyGridProperty(_T("��Ÿ"));
		pProp = new CMFCPropertyGridProperty(_T("(�̸�)"), _T("���� ���α׷�"));
		pProp->Enable(FALSE);
		pGroup3->AddSubItem(pProp);

		{
			CMFCPropertyGridColorProperty* pColorProp = new CMFCPropertyGridColorProperty(_T("â ����"), RGB(210, 192, 254), NULL, _T("â�� �⺻ ������ �����մϴ�."));
			pColorProp->EnableOtherButton(_T("��Ÿ..."));
			pColorProp->EnableAutomaticButton(_T("�⺻��"), ::GetSysColor(COLOR_3DFACE));
			pGroup3->AddSubItem(pColorProp);

			static TCHAR BASED_CODE szFilter[] = _T("������ ����(*.ico)|*.ico|��� ����(*.*)|*.*||");
			pGroup3->AddSubItem(new CMFCPropertyGridFileProperty(_T("������"), TRUE, _T(""), _T("ico"), 0, szFilter, _T("â �������� �����մϴ�.")));

			pGroup3->AddSubItem(new CMFCPropertyGridFileProperty(_T("��������"), _T("c:\\")));
		}
		g_PropertyGrid.AddProperty(pGroup3);
	}

	{
		CMFCPropertyGridProperty* pGroup4 = new CMFCPropertyGridProperty(_T("����"));

		CMFCPropertyGridProperty* pGroup41 = new CMFCPropertyGridProperty(_T("ù��° ���� ����"));
		pGroup4->AddSubItem(pGroup41);

		CMFCPropertyGridProperty* pGroup411 = new CMFCPropertyGridProperty(_T("�� ��° ���� ����"));
		pGroup41->AddSubItem(pGroup411);

		pGroup411->AddSubItem(new CMFCPropertyGridProperty(_T("�׸� 1"), (_variant_t) _T("�� 1"), _T("�����Դϴ�.")));
		pGroup411->AddSubItem(new CMFCPropertyGridProperty(_T("�׸� 2"), (_variant_t) _T("�� 2"), _T("�����Դϴ�.")));
		pGroup411->AddSubItem(new CMFCPropertyGridProperty(_T("�׸� 3"), (_variant_t) _T("�� 3"), _T("�����Դϴ�.")));

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
