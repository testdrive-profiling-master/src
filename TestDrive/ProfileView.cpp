#include "stdafx.h"
#include "mainfrm.h"
#include "ProfileView.h"
#include "Main.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////
// CProfileView

CProfileView::CProfileView()
{
}

CProfileView::~CProfileView()
{
}

BEGIN_MESSAGE_MAP(CProfileView, CToolBaseWnd)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_PROPERTIES, OnProperties)
	ON_COMMAND(ID_OPEN, OnOpen)
	ON_COMMAND(ID_DUMMY_COMPILE, OnDummyCompile)
	//ON_NOTIFY(NM_DBLCLK, ID_PROFILE_TREE, OnItemClick)
	ON_WM_PAINT()
	ON_WM_SETFOCUS()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWorkspaceBar 메시지 처리기

int CProfileView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CToolBaseWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rectDummy;
	rectDummy.SetRectEmpty();

	// 뷰를 만듭니다.
	//const DWORD dwViewStyle = WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS;

	if (!g_ProfileTree.Create(this, 0))
	{
		TRACE0("파일 뷰를 만들지 못했습니다.\n");
		return -1;      // 만들지 못했습니다.
	}

	// 이미지 리스트를 로드합니다.
	{
		m_ImageList.Create(16,16,ILC_COLOR32,6,0);
		ASSERT(m_ImageList.m_hImageList);
		CBitmap bmp;
		bmp.LoadBitmap(IDB_PROFILE_ITEM);
		m_ImageList.Add(&bmp,RGB(0,0,0));
		g_ProfileTree.SetImageList(&m_ImageList, TVSIL_NORMAL);
	}


	m_wndToolBar.Create(this, AFX_DEFAULT_TOOLBAR_STYLE, IDR_EXPLORER);
	m_wndToolBar.LoadToolBar(IDR_EXPLORER, 0, 0, TRUE /* 잠금 */);

	OnChangeVisualStyle();

	m_wndToolBar.SetPaneStyle(m_wndToolBar.GetPaneStyle() | CBRS_TOOLTIPS | CBRS_FLYBY);
	m_wndToolBar.SetPaneStyle(m_wndToolBar.GetPaneStyle() & ~(CBRS_GRIPPER | CBRS_SIZE_DYNAMIC | CBRS_BORDER_TOP | CBRS_BORDER_BOTTOM | CBRS_BORDER_LEFT | CBRS_BORDER_RIGHT));
	m_wndToolBar.SetOwner(this);

	// 모든 명령은 부모 프레임이 아닌 이 컨트롤을 통해 라우팅됩니다.
	m_wndToolBar.SetRouteCommandsViaFrame(FALSE);

	// 정적 트리 뷰 데이터를 더미 코드로 채웁니다.
	AdjustLayout();

	return 0;
}

void CProfileView::OnSize(UINT nType, int cx, int cy)
{
	CToolBaseWnd::OnSize(nType, cx, cy);
	AdjustLayout();
}

void CProfileView::OnContextMenu(CWnd* pWnd, CPoint point)
{
	CTreeCtrl* pWndTree = (CTreeCtrl*) &g_ProfileTree;
	ASSERT_VALID(pWndTree);

	if (pWnd != pWndTree)
	{
		CToolBaseWnd::OnContextMenu(pWnd, point);
		return;
	}

	if (point != CPoint(-1, -1))
	{
		// 클릭한 항목을 선택합니다.
		CPoint ptTree = point;
		pWndTree->ScreenToClient(&ptTree);

		UINT flags = 0;
		HTREEITEM hTreeItem = pWndTree->HitTest(ptTree, &flags);
		if (hTreeItem){
			CTreeObject*	pObject = (CTreeObject*)pWndTree->GetItemData(hTreeItem);
			pWndTree->SetFocus();
			pWndTree->SelectItem(hTreeItem);
			if(!pObject) return;
			theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_PROFILE, point.x, point.y, this, TRUE);
		}
	}
}

void CProfileView::AdjustLayout()
{
	if (GetSafeHwnd() == NULL) return;

	CRect rectClient;
	GetClientRect(rectClient);

	int cyTlb = m_wndToolBar.CalcFixedLayout(FALSE, TRUE).cy;

	m_wndToolBar.SetWindowPos(NULL, rectClient.left, rectClient.top, rectClient.Width(), cyTlb, SWP_NOACTIVATE | SWP_NOZORDER);
	((CWnd*)&g_ProfileTree)->SetWindowPos(NULL, rectClient.left + 1, rectClient.top + cyTlb + 1, rectClient.Width() - 2, rectClient.Height() - cyTlb - 2, SWP_NOACTIVATE | SWP_NOZORDER);
}

void CProfileView::OnProperties()
{

}

void CProfileView::OnOpen()
{
	CTreeObject*	pObject = g_ProfileTree.GetItemDataAtCursor();
	if(!pObject) return;
	pObject->OnSelect();

}

void CProfileView::OnDummyCompile()
{
}

void CProfileView::OnPaint()
{
	CPaintDC dc(this);

	CRect rectTree;
	((CWnd*)&g_ProfileTree)->GetWindowRect(rectTree);
	ScreenToClient(rectTree);

	rectTree.InflateRect(1, 1);
	dc.Draw3dRect(rectTree, ::GetSysColor(COLOR_3DSHADOW), ::GetSysColor(COLOR_3DSHADOW));
}

void CProfileView::OnSetFocus(CWnd* pOldWnd)
{
	CToolBaseWnd::OnSetFocus(pOldWnd);

	((CWnd*)&g_ProfileTree)->SetFocus();
}

void CProfileView::OnChangeVisualStyle()
{
	m_wndToolBar.CleanUpLockedImages();
	m_wndToolBar.LoadBitmap(theApp.m_bHiColorIcons ? IDB_EXPLORER_24 : IDR_EXPLORER, 0, 0, TRUE /* 잠금 */);

//	m_ProfileViewImages.DeleteImageList();

	UINT uiBmpId = theApp.m_bHiColorIcons ? IDB_FILE_VIEW_24 : IDB_PROFILE_VIEW;

	CBitmap bmp;
	if (!bmp.LoadBitmap(uiBmpId))
	{
		TRACE(_T("비트맵을 로드할 수 없습니다. %x\n"), uiBmpId);
		ASSERT(FALSE);
		return;
	}

	/*BITMAP bmpObj;
	bmp.GetBitmap(&bmpObj);

	UINT nFlags = ILC_MASK;

	nFlags |= (theApp.m_bHiColorIcons) ? ILC_COLOR24 : ILC_COLOR4;

	m_ProfileViewImages.Create(16, bmpObj.bmHeight, nFlags, 0, 0);
	m_ProfileViewImages.Add(&bmp, RGB(255, 0, 255));*/

	//g_ProfileTree.SetImageList(&m_ProfileViewImages, TVSIL_NORMAL);
}

void CProfileView::ClearAll(void){
	g_ProfileTree.DeleteAllItems();
}

void CProfileView::OnItemClick(NMHDR* pNMHDR, LRESULT* pResult){
	AfxMessageBox(_T("A"));
}