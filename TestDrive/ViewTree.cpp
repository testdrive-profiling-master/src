#include "stdafx.h"
#include "ViewTree.h"
#include "ProfileData.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CViewTree	g_ProfileTree;
CViewTree	g_SystemTree;

//--------------------------------------------------------
// CTreeObject
CTreeObject::CTreeObject(){
}

CTreeObject::~CTreeObject(){
}


//--------------------------------------------------------
// CViewTree

CViewTree::CViewTree(){
	m_CurrentItem	= NULL;
	m_ParentItem	= NULL;
}

CViewTree::~CViewTree(){
	DeleteAllItems();
}

BEGIN_MESSAGE_MAP(CViewTree, CTreeCtrl)
	//REFLECTED_NOTIFY_CODE_HANDLER(TVN_GETDISPINFO, OnGetDispInfo)
	//ON_NOTIFY_REFLECT(TVN_SELCHANGED, OnTVSelChanged)
	ON_NOTIFY_REFLECT(NM_DBLCLK, OnItemDblClick)
	ON_NOTIFY_REFLECT(NM_RETURN, OnItemReturn)
	ON_WM_GETDLGCODE()
END_MESSAGE_MAP()

UINT CViewTree::OnGetDlgCode(void){
	// return 키를 받을 수 있게 한다.
	return DLGC_WANTALLKEYS;
}

BOOL CViewTree::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	BOOL bRes = CTreeCtrl::OnNotify(wParam, lParam, pResult);

	LPNMHDR			pNMHDR	= (LPNMHDR)lParam;
	//LPNMTREEVIEW	pNTV	= (LPNMTREEVIEW)lParam;

	ASSERT(pNMHDR != NULL);

	if (pNMHDR && pNMHDR->code == TTN_SHOW && GetToolTips() != NULL)
	{
		GetToolTips()->SetWindowPos(&wndTop, -1, -1, -1, -1, SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOSIZE);
	}

	return bRes;
}

BOOL CViewTree::Create(CWnd* pParentWnd, UINT nID){
	CRect rectDummy;
	rectDummy.SetRectEmpty();
	return CTreeCtrl::Create(WS_CHILD | WS_VISIBLE | TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, rectDummy, pParentWnd, nID);
}

void CViewTree::DeleteItem(HTREEITEM item){
	if(!item) return;
	if(m_ParentItem==item) m_ParentItem = NULL;
	CTreeObject* pData = (CTreeObject*)GetItemData(item);
	SAFE_DELETE(pData);
	CTreeCtrl::DeleteItem(item);
}

BOOL CViewTree::DeleteAllItems(void){
	if(!IsWindow(m_hWnd)) return FALSE;
	DeleteSubItems(NULL);
	m_ParentItem	= NULL;
	m_CurrentItem	= NULL;
	return TRUE;
}

void CViewTree::DeleteSubItems(HTREEITEM item){
	HTREEITEM	item_next;
	m_ParentItem = item;

	if(m_ParentItem) Select(m_ParentItem, TVGN_CARET);

	m_CurrentItem = GetNextItem(m_ParentItem, m_ParentItem ? TVGN_CHILD : TVGN_ROOT);
	while(m_CurrentItem != m_ParentItem){
		item_next = GetNextItem(m_CurrentItem, TVGN_CHILD);
		if(item_next) m_CurrentItem = item_next;
		else{
			item_next = GetNextItem(m_CurrentItem, TVGN_NEXT);
			if(!item_next) item_next = GetNextItem(m_CurrentItem, TVGN_PARENT);
			DeleteItem(m_CurrentItem);
			m_CurrentItem = item_next;
		}
	}
}

HTREEITEM CViewTree::FindChildItem(HTREEITEM parent, LPCTSTR name){
	HTREEITEM	child	= NULL;
	CString		child_name;
	
	child	= parent ? GetNextItem(parent, TVGN_CHILD) : GetNextItem(NULL, TVGN_ROOT);
	while(child){
		child_name = GetItemText(child);
		if(!child_name.Compare(name)) break;
		child = GetNextItem(child, TVGN_NEXT);
	}

	return child;
}

HTREEITEM CViewTree::SetCurrentRootItem(LPCTSTR szName){
	HTREEITEM	item_next;
	CString		name;

	m_ParentItem	= NULL;
	m_CurrentItem	= NULL;

	if(szName){
		TCHAR	szTag[1024], *pCurName, *pNextName;
		_tcscpy(szTag, szName);
		pCurName	= szTag;

		while(pCurName){
			pNextName	= _tcsstr(pCurName, _T("\\"));
			if(pNextName){
				*pNextName	= NULL;
				pNextName++;
			}
			item_next	= FindChildItem(m_ParentItem, pCurName);
			if(item_next){
				m_ParentItem = item_next;
			}else{
				m_ParentItem = InsertItem(pCurName, TREE_ITEM_BRANCH, TREE_ITEM_BRANCH, m_ParentItem ? m_ParentItem : TVI_ROOT, TVI_LAST);
			}
			pCurName = pNextName;
		}
	}
	m_CurrentItem	= m_ParentItem;
	return m_CurrentItem;
}

HTREEITEM CViewTree::InsertTree(LPCTSTR sName, HTREEITEM parent) {
	// 이미 트리에 존재하는 경우는 생성하지 않는다.
	HTREEITEM	child = FindChildItem(parent, sName);
	if (child) return child;

	// branch 는 Item 보다 상위에 리스트를 만든다.
	child = GetNextItem(parent, TVGN_CHILD);

	if (child && !GetItemData(child)) {
		HTREEITEM	next_child;
		for (;;) {
			next_child = GetNextItem(child, TVGN_NEXT);
			if (!next_child || GetItemData(next_child)) break;
			child = next_child;
		}
		if (!child) child = TVI_LAST;
	} else {
		child = TVI_FIRST;
	}

	return InsertItem(sName, TREE_ITEM_BRANCH, TREE_ITEM_BRANCH, parent ? parent : TVI_ROOT, child);
}

void CViewTree::InsertData(HTREEITEM parent, TD_TREE_ITEM type, LPCTSTR sName, LPCTSTR sCommand) {
	HTREEITEM	child	= FindChildItem(parent, sName);

	// add new child
	if (!child) {
		child = InsertItem(sName, type, type, parent, TVI_LAST);
	}

	{	// delete previous profile data
		CProfileData* pPrevData = (CProfileData*)GetItemData(child);
		if (pPrevData) delete pPrevData;
	}

	SetItemData(child, (DWORD_PTR)(new CProfileData(sCommand)));
}

void CViewTree::AddItem(TD_TREE_ITEM type, LPCTSTR szName){
	switch (type){
	case TREE_ITEM_BRANCH:
		{	// 이미 트리에 branch 가 존재하는 경우는 생성하지 않는다.
			HTREEITEM	child	= FindChildItem(m_ParentItem, szName);
			if(child && !GetItemData(child)){
				m_ParentItem	=
				m_CurrentItem	= child;
				break;
			}
		}
		{	// branch 는 Item 보다 상위에 리스트를 만든다.
			HTREEITEM	insert_after	= NULL;
			insert_after	= GetNextItem(m_ParentItem, TVGN_CHILD);
			if(insert_after && !GetItemData(insert_after)){
				HTREEITEM	child;
				for(;;){
					child			= GetNextItem(insert_after, TVGN_NEXT);
					if(!child || GetItemData(child)) break;
					insert_after	= child;
				}
			}else{
				insert_after	= TVI_FIRST;
			}

			m_CurrentItem	= InsertItem(szName, type, type, m_ParentItem ? m_ParentItem : TVI_ROOT, insert_after ? insert_after : TVI_LAST);
		}
		
		m_ParentItem	= m_CurrentItem;
		break;
	case TREE_ITEM_TREE_CLOSE:
		m_ParentItem	= GetParentItem(m_ParentItem);
		m_CurrentItem	= m_ParentItem;
		break;
	/*case TREE_ITEM_PROFILE:
	case TREE_ITEM_SCREEN:
	case TREE_ITEM_CHART:
	case TREE_ITEM_REPORT:*/
	default:
		m_CurrentItem	= InsertItem(szName, type, type, m_ParentItem ? m_ParentItem : TVI_ROOT, TVI_LAST);
		break;
	}
}

void CViewTree::SetRootItem(HTREEITEM parent){
	m_CurrentItem	= parent ? NULL : GetRootItem();
	m_ParentItem	= parent;
}

BOOL CViewTree::SetCurrentItemData(CTreeObject* pObject){
	if(!m_CurrentItem) return FALSE;
	if(!SetItemData(m_CurrentItem, (DWORD_PTR)pObject))	return FALSE;
	return TRUE;
}

void CViewTree::ExpandTree(HTREEITEM item, bool bExpand) {
	Expand(item, bExpand ? TVE_EXPAND : TVE_COLLAPSE);
}

void CViewTree::ExpandAll(void){
	HTREEITEM item = GetNextItem(NULL, TVGN_ROOT);
	while(item){
		CTreeCtrl::Expand(item, TVE_EXPAND);
		item = GetNextItem(item, TVGN_NEXT);
	}
}

void CViewTree::ExpandCurrent(void){
	if(m_ParentItem) Expand(m_ParentItem, TVE_EXPAND);
}

CTreeObject* CViewTree::GetItemDataAtCursor(void){
	CPoint pt;
	GetCursorPos((LPPOINT)&pt);
	ScreenToClient((LPPOINT)&pt);
	HTREEITEM hItem	= HitTest(pt);
	if(!hItem) return NULL;
	return (CTreeObject*)GetItemData(hItem);
}

CImageList* CViewTree::SetImageList(CImageList* pImageList, int nImageList){
	return CTreeCtrl::SetImageList(pImageList, nImageList);
}

void CViewTree::OnItemDblClick(NMHDR* pNMHDR, LRESULT* pResult){
	if(pNMHDR->hwndFrom != m_hWnd) return;
	{
		CPoint pt;
		GetCursorPos((LPPOINT)&pt);
		ScreenToClient((LPPOINT)&pt);
		HTREEITEM hItem = HitTest(pt);
		if(!hItem) return;

		CTreeObject*	pObject = (CTreeObject*)GetItemData(hItem);
		if(!pObject) return;

		pObject->OnSelect();
	}
}

void CViewTree::OnItemReturn(NMHDR* pNMHDR, LRESULT* pResult){
	if(pNMHDR->hwndFrom != m_hWnd) return;
	{
		HTREEITEM hItem = GetSelectedItem();
		if(!hItem) return;
		CTreeObject*	pObject = (CTreeObject*)GetItemData(hItem);
		if(!pObject){
			Expand(hItem, TVE_TOGGLE);
			return;
		}

		pObject->OnSelect();
	}
	
}

void CViewTree::OnTVSelChanged(NMHDR* pNMHDR, LRESULT* pResult){
	LPNMTREEVIEW lpTV = (LPNMTREEVIEW)pNMHDR;

	CTreeObject*	pObject = (CTreeObject*)GetItemData(lpTV->itemNew.hItem);
	if(!pObject) return;

	pObject->OnSelect();

	//SelectItem(NULL);
}