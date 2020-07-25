// TestDriveView.h : CTestDriveView Ŭ������ �������̽�
//

#pragma once
#include "TestDriveDoc.h"

class CTestDriveView : public CView
{
protected: // serialization������ ��������ϴ�.
	CTestDriveView();
	DECLARE_DYNCREATE(CTestDriveView)

// Ư���Դϴ�.
public:
	CTestDriveDoc* GetDocument() const;

// �۾��Դϴ�.
public:

// �������Դϴ�.
public:
	virtual void OnDraw(CDC* pDC);  // �� �並 �׸��� ���� �����ǵǾ����ϴ�.
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:

// �����Դϴ�.
public:
	virtual ~CTestDriveView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// ������ �޽��� �� �Լ�
protected:
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // TestDriveView.cpp�� ����� ����
inline CTestDriveDoc* CTestDriveView::GetDocument() const
   { return reinterpret_cast<CTestDriveDoc*>(m_pDocument); }
#endif

