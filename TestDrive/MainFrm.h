// �� MFC ���� �ҽ� �ڵ�� MFC Microsoft Office Fluent ����� �������̽�("Fluent UI")�� 
// ����ϴ� ����� ���� �ָ�, MFC C++ ���̺귯�� ����Ʈ��� ���Ե� 
// Microsoft Foundation Classes Reference �� ���� ���� ������ ���� 
// �߰������� �����Ǵ� �����Դϴ�.  
// Fluent UI�� ����, ��� �Ǵ� �����ϴ� �� ���� ��� ����� ������ �����˴ϴ�.  
// Fluent UI ���̼��� ���α׷��� ���� �ڼ��� ������ 
// http://msdn.microsoft.com/officeui�� �����Ͻʽÿ�.
//
// Copyright (C) Microsoft Corporation
// ��� �Ǹ� ����.

// MainFrm.h : CMainFrame Ŭ������ �������̽�
//

#pragma once
#include "ProfileView.h"
#include "OutputWnd.h"
#include "PropertiesWnd.h"
#include "ClientWnd.h"
#include "Buffer.h"
#include "DocumentWnd.h"
#include "DocumentCtrl.h"
#include "MainFrmBase.h"
//#include "FxWaterEffect.h"

class CMainFrame : public CMainFrameBase
{
	DECLARE_DYNAMIC(CMainFrame)
public:
	CMainFrame();

public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual CMDIChildWndEx* ControlBarToTabbedDocument(CDockablePane* pBar);
	virtual LRESULT DefWindowProc(UINT nMsg, WPARAM wParam, LPARAM lParam);
	virtual void OnUpdateFrameTitle(BOOL bAddToTitle);
	virtual BOOL OnEraseMDIClientBackground(CDC* pDC);
	virtual BOOL OnShowMDITabContextMenu(CPoint point, DWORD dwAllowedItems, BOOL bTabDrop);

	void DrawClientArea(CDC* pDC = NULL);
	CMDIClientAreaWnd* GetClientAreaWnd(void) {return &m_wndClientArea;}
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // ��Ʈ�� ������ ���Ե� ����Դϴ�.
	CMFCRibbonBar				m_wndRibbonBar;
	CMFCRibbonApplicationButton	m_MainButton;
	CMFCToolBarImages			m_PanelImages;
	CMFCRibbonStatusBar			m_wndStatusBar;
	CProfileView				m_wndProfile;
	COutputWnd					m_wndOutput;
	CPropertiesWnd				m_wndProperties;
	CBuffer						m_imgBackground;
	//CBuffer						m_imgBackgroundWater;
	//CFxWaterEffect				m_fxWater;
	//CRect						m_FxRect;
	//BOOL						m_bfxWater;
	BOOL						m_bInitialized;

// ������ �޽��� �� �Լ�
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnClose();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg UINT OnPowerBroadcast(UINT wParam, LPARAM lParam);
	afx_msg void OnWindowManager();
	afx_msg void OnApplicationLook(UINT id);
	afx_msg void OnUpdateApplicationLook(CCmdUI* pCmdUI);
	afx_msg void OnViewToggleMaximize();
	afx_msg void OnOptions();
	afx_msg void OnLocaleSelect();
	afx_msg void OnSettingChange(UINT uFlags, LPCTSTR lpszSection);
	afx_msg void OnStatusBarWebSiteLink();

	afx_msg void OnEditCopy();
	afx_msg void OnEditPaste();
	afx_msg void OnEditCut();
	afx_msg void OnEditUndo();
	afx_msg void OnEditSelectAll();
	afx_msg void OnEditPrint();
	afx_msg void OnFileSave();
	afx_msg void OnToggleFullScreen();

	// testdrive commands
	afx_msg void OnProcessCheck();
	afx_msg void OnProcessCleanUp();
	afx_msg void OnProcessIntialize();
	afx_msg void OnPostProjectOpen();
	DECLARE_MESSAGE_MAP()

	BOOL CreateDockingWindows();
	void SetDockingWindowIcons(BOOL bHiColorIcons);
public:
	CMFCRibbonComboBox*	m_pLangCombo;
};

extern CMainFrame*		g_pMainFrame;
