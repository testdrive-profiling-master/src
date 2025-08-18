#pragma once
#include "commandlist.h"

//--------------------------------------------------------
// CTreeObject
class CTreeObject{
public:
	CTreeObject();
	virtual ~CTreeObject();

	virtual void OnSelect(void) = 0;
};

//--------------------------------------------------------
// CViewTree
class CViewTree : protected CTreeCtrl
{
public:
	typedef enum{
		ITEM_SCREEN,
		ITEM_CHART,
		ITEM_REPORT,
		ITEM_HTML,
		ITEM_PROFILE,
		ITEM_TREE_OPEN,
		ITEM_TREE_CLOSE,
	}ITEM_TYPE;
	CViewTree();
	virtual ~CViewTree();

	BOOL Create(CWnd* pParentWnd, UINT nID);

	void DeleteItem(HTREEITEM item);
	BOOL DeleteAllItems(void);
	void DeleteSubItems(HTREEITEM item);
	HTREEITEM Root(void);
	HTREEITEM FindChildItem(HTREEITEM parent, LPCTSTR name);
	HTREEITEM SetCurrentRootItem(LPCTSTR name);
	HTREEITEM InsertTree(LPCTSTR sName, HTREEITEM parent = NULL);
	void InsertData(HTREEITEM parent, TD_TREE_ITEM type, LPCTSTR sName, LPCTSTR sCommand);
	void AddItem(TD_TREE_ITEM type, LPCTSTR szName = NULL);
	void SetRootItem(HTREEITEM parent = NULL);
	BOOL SetCurrentItemData(CTreeObject* pObject);
	void ExpandTree(HTREEITEM item, bool bExpand = true);
	void ExpandAll(void);
	void ExpandCurrent(void);
	CTreeObject* GetItemDataAtCursor(void);

	CImageList* SetImageList(CImageList* pImageList, int nImageList);

	virtual operator CWnd*(){return this;}

protected:
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);

	HTREEITEM			m_CurrentItem;
	HTREEITEM			m_ParentItem;

protected:
	DECLARE_MESSAGE_MAP()

	afx_msg void OnItemDblClick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnItemReturn(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnTVSelChanged(NMHDR* pNMHDR, LRESULT* pResult);

	afx_msg UINT OnGetDlgCode(void);
};

extern CViewTree	g_ProfileTree;
extern CViewTree	g_SystemTree;
