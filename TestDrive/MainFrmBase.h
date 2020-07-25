#pragma once
#include "ProfileView.h"
#include "OutputWnd.h"
#include "PropertiesWnd.h"
#include "ClientWnd.h"
#include "Buffer.h"
#include "DocumentWnd.h"
#include "DocumentCtrl.h"

class CMainFrameBase : public CMDIFrameWndEx
{
	DECLARE_DYNAMIC(CMainFrameBase)
public:
	CMainFrameBase();
	virtual ~CMainFrameBase();

	virtual BOOL OnCloseMiniFrame(CPaneFrameWnd* pWnd);
	virtual BOOL OnCloseDockingPane(CDockablePane* pWnd);
	virtual BOOL PreTranslateMessage(MSG* pMsg);

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

	DECLARE_MESSAGE_MAP()
};
