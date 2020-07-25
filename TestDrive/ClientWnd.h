#pragma once

class ClientWnd : public CWnd
{
	DECLARE_DYNAMIC(ClientWnd)

public:
	ClientWnd();
	virtual ~ClientWnd();


protected:
	//afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);

	DECLARE_MESSAGE_MAP()
};

