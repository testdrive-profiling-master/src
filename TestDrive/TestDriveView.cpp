// TestDriveView.cpp : CTestDriveView Ŭ������ ����
//

#include "stdafx.h"
// SHARED_HANDLERS�� �̸� ����, ����� �׸� �� �˻� ���� ó���⸦ �����ϴ� ATL ������Ʈ���� ������ �� ������
// �ش� ������Ʈ�� ���� �ڵ带 �����ϵ��� �� �ݴϴ�.
#ifndef SHARED_HANDLERS
#include "Main.h"
#endif

#include "TestDriveDoc.h"
#include "TestDriveView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CTestDriveView

IMPLEMENT_DYNCREATE(CTestDriveView, CView)

BEGIN_MESSAGE_MAP(CTestDriveView, CView)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
END_MESSAGE_MAP()

// CTestDriveView ����/�Ҹ�

CTestDriveView::CTestDriveView()
{
	// TODO: ���⿡ ���� �ڵ带 �߰��մϴ�.

}

CTestDriveView::~CTestDriveView()
{
}

BOOL CTestDriveView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: CREATESTRUCT cs�� �����Ͽ� ���⿡��
	//  Window Ŭ���� �Ǵ� ��Ÿ���� �����մϴ�.

	return CView::PreCreateWindow(cs);
}

// CTestDriveView �׸���

void CTestDriveView::OnDraw(CDC* /*pDC*/)
{
	CTestDriveDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: ���⿡ ���� �����Ϳ� ���� �׸��� �ڵ带 �߰��մϴ�.
}

void CTestDriveView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CTestDriveView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	//theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}

// CTestDriveView ����

#ifdef _DEBUG
void CTestDriveView::AssertValid() const
{
	CView::AssertValid();
}

void CTestDriveView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CTestDriveDoc* CTestDriveView::GetDocument() const // ����׵��� ���� ������ �ζ������� �����˴ϴ�.
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CTestDriveDoc)));
	return (CTestDriveDoc*)m_pDocument;
}
#endif //_DEBUG


// CTestDriveView �޽��� ó����
