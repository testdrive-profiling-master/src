#ifndef __CLASS_CNODELIST__
#define __CLASS_CNODELIST__

#include "Node.h"
//////////////////////////////////////////////////////////////////////////
// Node list base class
//////////////////////////////////////////////////////////////////////////
/*
template <class TYPE_DATA>
class CNodeList
{
protected:
	DWORD				m_dwNodeCount;
	CNode<TYPE_DATA>*	m_pTopNode;
	CNode<TYPE_DATA>*	m_pCurNode;
public:
	CNodeList(void){
		m_pTopNode	= NULL;
		Release();
	}

	~CNodeList(void){
		Release();
	}

	void Release(void){
		SAFE_DELETE(m_pTopNode);
		m_pCurNode		= NULL;
		m_dwNodeCount	= 0;
	}

	void Add(const char* name){
		if(m_pTopNode)	m_pCurNode = m_pTopNode->New(name);
		else			m_pCurNode = m_pTopNode = new CNodeData<TYPE_DATA>(name);
		m_dwNodeCount++;
	}

	BOOL Delete(void){
		if(!m_pCurNode) return FALSE;
		{
			CNodeData<TYPE_DATA>* pPrevNode;
			pPrevNode = m_pTopNode;

			while(pPrevNode && (pPrevNode->GetNext() != m_pCurNode))
				pPrevNode = pPrevNode->GetNext();

			if(pPrevNode) pPrevNode->SetNext(m_pCurNode->GetNext());
		}
		m_pCurNode->SetNext(NULL);
		if(m_pCurNode == m_pTopNode) m_pTopNode = NULL;
		SAFE_DELETE(m_pCurNode);
		m_dwNodeCount--;
		return TRUE;
	}

	BOOL Find(const char* name){
		if(!m_pTopNode) return FALSE;
		m_pCurNode = m_pTopNode->Find(name);
		if(!m_pCurNode) return FALSE;
		return TRUE;
	}

	DWORD GetCount(void){
		return m_dwNodeCount;
	}

	CNode<TYPE_DATA>* GetNode(void){
		return m_pCurNode;
	}
};

//////////////////////////////////////////////////////////////////////////
// String node list class
//////////////////////////////////////////////////////////////////////////
class CNodeListString :
	public CNodeList<char>
{
public:
	CNodeListString(void){}
	~CNodeListString(void){}

	BOOL SetString(const char* str){
		if(!m_pCurNode) return FALSE;
		m_pCurNode->SetString(str);
		return TRUE;
	}

	char* GetString(void){
		if(!m_pCurNode) return NULL;
		return m_pCurNode->Get();
	}
};

//////////////////////////////////////////////////////////////////////////
// Data type node list class
//////////////////////////////////////////////////////////////////////////
template <class TYPE_DATA>
class CNodeListData :
	public CNodeList<TYPE_DATA>
{
public:
	CNodeListData(void){}
	~CNodeListData(void){}

	BOOL SetData(TYPE_DATA& data){
		if(!m_pCurNode) return FALSE;
		m_pCurNode->Copy(&data);
		return TRUE;
	}

	TYPE_DATA GetData(void){
		if(!m_pCurNode) return NULL;
		return m_pCurNode->Get();
	}
};*/

#endif