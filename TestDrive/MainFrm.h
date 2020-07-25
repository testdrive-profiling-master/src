// 이 MFC 샘플 소스 코드는 MFC Microsoft Office Fluent 사용자 인터페이스("Fluent UI")를 
// 사용하는 방법을 보여 주며, MFC C++ 라이브러리 소프트웨어에 포함된 
// Microsoft Foundation Classes Reference 및 관련 전자 문서에 대해 
// 추가적으로 제공되는 내용입니다.  
// Fluent UI를 복사, 사용 또는 배포하는 데 대한 사용 약관은 별도로 제공됩니다.  
// Fluent UI 라이선싱 프로그램에 대한 자세한 내용은 
// http://msdn.microsoft.com/officeui를 참조하십시오.
//
// Copyright (C) Microsoft Corporation
// 모든 권리 보유.

// MainFrm.h : CMainFrame 클래스의 인터페이스
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

protected:  // 컨트롤 모음이 포함된 멤버입니다.
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

// 생성된 메시지 맵 함수
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
