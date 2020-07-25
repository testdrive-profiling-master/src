#pragma once
#include "DocumentWnd.h"
#include "TestDrive.h"

typedef list<CDocumentView*>	DOCUMENT_LIST;

class CDocumentCtrl
{
	DOCUMENT_LIST		m_List;
public:
	CDocumentCtrl();
	virtual ~CDocumentCtrl();

	BOOL IsDocument(const CDocumentWnd* pWnd);
	CDocumentView* Find(LPCTSTR lpszName);
	CDocumentView* Add(LPCTSTR lpszName, CPaser* pPaser);
	void Add(CDocumentView* pDoc);
	BOOL Remove(LPCTSTR lpszName = NULL, BOOL bForce = FALSE);
	void Remove(CDocumentView* pDoc);
	BOOL RemoveAll(BOOL bForceToExit = FALSE);
	int GetCount(void);
	void SetActive(CDocumentView* pDoc);
	BOOL SetTitle(LPCTSTR lpszDocName, LPCTSTR lpszTitle);
	void Show(LPCTSTR lpszName, BOOL bShow = TRUE);

protected:
	//virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);

	//afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);

private:
	CImageList	m_ImgListProfile;
	CSize		m_WndSize;
};

extern CDocumentCtrl*		g_pDocumentList;