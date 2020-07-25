#pragma once

#ifndef __AFXWIN_H__
	#error "PCH에 대해 이 파일을 포함하기 전에 'stdafx.h'를 포함합니다."
#endif

#include "resource.h"       // 주 기호입니다.
#include "DocumentCtrl.h"
#include "TestDriveImp.h"

// CTestDriveApp:
// 이 클래스의 구현에 대해서는 TestDrive.cpp을 참조하십시오.
//

class CTestDriveApp : public CWinAppEx, public TestDriveResource
{
public:
	CTestDriveApp(void);
	virtual ~CTestDriveApp(void);

// 재정의입니다.
public:
	virtual BOOL InitInstance();
	BOOL InitRegistries(void);
	virtual int ExitInstance();

// 구현입니다.
	UINT  m_nAppLook;
	BOOL  m_bHiColorIcons;

	virtual void PreLoadState();
	virtual void LoadCustomState();
	virtual void SaveCustomState();

	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()

	CTestDrive*			m_pTestDrive;
	CDocumentCtrl		m_ViewTabControl;	//@FIXME
};

extern CTestDriveApp theApp;
