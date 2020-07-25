#pragma once

#ifndef __AFXWIN_H__
	#error "PCH�� ���� �� ������ �����ϱ� ���� 'stdafx.h'�� �����մϴ�."
#endif

#include "resource.h"       // �� ��ȣ�Դϴ�.
#include "DocumentCtrl.h"
#include "TestDriveImp.h"

// CTestDriveApp:
// �� Ŭ������ ������ ���ؼ��� TestDrive.cpp�� �����Ͻʽÿ�.
//

class CTestDriveApp : public CWinAppEx, public TestDriveResource
{
public:
	CTestDriveApp(void);
	virtual ~CTestDriveApp(void);

// �������Դϴ�.
public:
	virtual BOOL InitInstance();
	BOOL InitRegistries(void);
	virtual int ExitInstance();

// �����Դϴ�.
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
