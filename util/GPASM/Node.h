#ifndef __CLASS_CNODE__
#define __CLASS_CNODE__
#include "Str.h"
#include "define_base.h"

class CNodeData
{
protected:
	CNodeData*	m_pNext;			// ���� ��� ������
	BOOL		m_bSet;				// ��������

public:
	char*		m_pName;			// ��� �̸�
	DWORD		m_Type;				// ������ Ÿ��
	int			m_iData;			// ��� ������
	char*		m_sData;			// ���ڿ� ������
	CNodeData*	m_pLink;			// ��ũ ������

	CNodeData(const char* node_name = NULL){
		m_pName			= NULL;
		m_pNext			= NULL;
		m_bSet			= FALSE;
		m_iData			= 0;
		m_Type			= STRTYPE_NONE;
		m_sData			= NULL;

		SetName(node_name);
	}

	~CNodeData(void){
		ReleaseData();
		SAFE_DELETE(m_pNext);
	}

	void SetName(const char* node_name){
		SAFE_DELETE_ARRAY(m_pName);
		if(node_name){
			m_pName		= new char[strlen(node_name)+1];
			strcpy(m_pName, node_name);
		}
	}

	void SetString(const char* str){
		if(!str){
			if(!m_sData) m_sData = new char[1];
			*m_sData = NULL;
			return;
		}
		if(m_sData){
			if(strlen(m_sData) < strlen(str)) SAFE_DELETE_ARRAY(m_sData);
		}
		m_sData = new char[strlen(str)+1];
		m_bSet	= TRUE;
		strcpy(m_sData, str);
	}

	virtual CNodeData* Tail(void){							// ������ ��� ���
		CNodeData* tail = this;
		while(tail->m_pNext) tail = tail->m_pNext;
		return tail;
	}

	CNodeData* Add(const char* node_name){					// ��� ����
		CNodeData* node;
		// ��� ã��
		node = Find(node_name);
		if(node) return node;

		// �� ��� ã��
		node = this;
		while(node){
			if(!node->m_pName) {
				node->SetName(node_name);
				return node;
			}
			if(node->m_pNext) node = node->m_pNext;
			else break;
		}
		// ���ο� ��� �߰�
		node->m_pNext = new CNodeData(node_name);
		return node->m_pNext;
	}

	virtual CNodeData* Find(const char* node_name){
		CNodeData* node = this;
		while(node){
			if(node->m_pName)
				if(!strcmp(node->m_pName, node_name)) return node;
			node = node->m_pNext;
		}
		return NULL;
	}

	void ReleaseData(void){								// ��� ������ ���
		m_Type	= STRTYPE_NONE;
		m_bSet	= FALSE;
		SAFE_DELETE_ARRAY(m_pName);
		SAFE_DELETE_ARRAY(m_sData);
	}

	void SetData(int iData){
		m_bSet	= TRUE;
		m_iData	= iData;
	}

	/*void SetsData(char* sData, int idata){
		m_pData = (void*)sData;
		//m_pData = idata;
	}*/

	char* GetName(void){
		return m_pName;
	}

	int GetData(void){
		return m_iData;
	}

	DWORD GetType(void){
		return m_Type;
	}

	void SetType(DWORD type){
		m_Type = type;
	}

	CNodeData* GetNext(void){
		return m_pNext;
	}

	BOOL IsSet(void){
		return m_bSet;
	}

	void Set(void){
		m_bSet = TRUE;
	}

	void UnSet(void){
		m_bSet = FALSE;
	}
};

class CNodeStr
{
protected:
	char*		m_pName;			// ��� �̸�
	CNodeStr*		m_pNext;			// ���� ��� ������
	DWORD		m_type;				// ������ Ÿ��
	char*		m_pData;			// ��� ������

public:
	CNodeStr(const char* node_name = NULL){
		m_pName			= NULL;
		m_pNext			= NULL;
		m_pData			= NULL;
		m_type			= STRTYPE_NONE;

		SetName(node_name);
	}

	~CNodeStr(void){
		ReleaseData();
		SAFE_DELETE_ARRAY(m_pName);
		SAFE_DELETE(m_pNext);
	}

	void SetName(const char* node_name){
		SAFE_DELETE_ARRAY(m_pName);
		if(node_name){
			m_pName		= new char[strlen(node_name)+1];
			strcpy(m_pName, node_name);
		}
	}

	virtual CNodeStr* Tail(void){							// ������ ��� ���
		CNodeStr* tail = this;
		while(tail->m_pNext) tail = tail->m_pNext;
		return tail;
	}

	CNodeStr* Add(const char* node_name){					// ��� ����
		CNodeStr* node;
		// ��� ã��
		node = Find(node_name);
		if(node) return node;

		// �� ��� ã��
		node = this;
		while(node){
			if(!node->m_pName) {
				node->SetName(node_name);
				return node;
			}
			if(node->m_pNext) node = node->m_pNext;
			else break;
		}
		// ���ο� ��� �߰�
		node->m_pNext = new CNodeStr(node_name);
		return node->m_pNext;
	}

	virtual CNodeStr* Find(const char* node_name){
		CNodeStr* node = this;
		while(node){
			if(node->m_pName)
			if(!strcmp(node->m_pName, node_name)) return node;
			node = node->m_pNext;
		}
		return NULL;
	}

	void ReleaseData(void){								// ��� ������ ���
		SAFE_DELETE_ARRAY(m_pData);
		m_type	= STRTYPE_NONE;
	}

	void SetData(const char* str, DWORD type = STRTYPE_NAME){
		ReleaseData();
		m_pData	= new char[strlen(str)+1];
		strcpy(m_pData, str);
		m_type	= type;
	}

	char* GetName(void){
		return m_pName;
	}

	char* GetData(void){
		return m_pData;
	}

	DWORD GetType(void){
		return m_type;
	}

	CNodeStr* GetNext(void){
		return m_pNext;
	}
};

class CNodeCode
{
protected:
public:
	DWORD		m_dwCode;
	DWORD		m_dwBytes;
	CNodeCode*	m_pNext;
	
	CNodeCode(DWORD dwBytes = 4){	// default 4 bytes
		m_dwCode	= 0;
		m_dwBytes	= dwBytes;
		m_pNext		= NULL;
	}
	~CNodeCode(void){
		SAFE_DELETE(m_pNext);
	}

	void Set(DWORD op, int bitwidth = 32, int pos = 0){
		DWORD mask = 0;
		for(;bitwidth>0;bitwidth--) mask = (mask<<1) | 1;
		op		&= mask;
		mask	<<= pos;
		op		<<= pos;
		op			&= mask;
		m_dwCode	&= (~mask);
		m_dwCode	|= op;
	}

	CNodeCode* Tail(void){
		CNodeCode*	pNode = this;
		while(pNode->m_pNext) pNode = pNode->m_pNext;
		return pNode;
	}

	CNodeCode* Add(DWORD Bytes){
		return (Tail()->m_pNext = new CNodeCode(Bytes));
	}

	CNodeCode* Next(void){
		return m_pNext;
	}
};

class CNodeRelation
{
protected:
	CNodeRelation*	m_pNext;
public:
	CNodeCode*		m_pCode;
	int				m_iBitwidth;
	int				m_iPosition;

	CNodeData*		m_pSrcPos;
	CNodeData*		m_pDestPos;

	CNodeRelation(void){
		m_pCode		= NULL;
		m_iBitwidth	= 0;
		m_iPosition	= 0;
		m_pSrcPos	= NULL;
		m_pDestPos	= NULL;
		m_pNext		= NULL;
	}
	~CNodeRelation(void){
		SAFE_DELETE(m_pNext);
	}

	CNodeRelation* Tail(void){
		CNodeRelation*	pNode = this;
		while(pNode->m_pNext) pNode = pNode->m_pNext;
		return pNode;
	}

	CNodeRelation* Add(void){
		return (Tail()->m_pNext = new CNodeRelation);
	}

	CNodeRelation* Next(void){
		return m_pNext;
	}
};

#endif