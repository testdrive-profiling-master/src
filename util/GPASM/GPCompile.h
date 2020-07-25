#ifndef __CLASS_CGPCOMPILE__
#define __CLASS_CGPCOMPILE__

#include "Source.h"
#include "Log.h"
#include "Node.h"

const char g_CUR_ADDRESS_TAG_NAME[] = "*pc";

class CCompileResource{
private:
	DWORD				m_iPC;
	
public:
	CLog				m_Log;
	CStr				m_Str;
	CNodeStr*			m_pSourceFile;
	CNodeStr*			m_pNodeDefine;
	CNodeStr*			m_pNodeFunction;
	CNodeData*			m_pNodeVariable;
	CNodeCode*			m_pNodeCode;
	CNodeRelation*		m_pNodeRelation;

	CNodeData*			m_pCurPC;
	CNodeCode*			m_pCurCode;

	CCompileResource(void){
		m_pSourceFile	= new CNodeStr();
		m_pNodeDefine	= new CNodeStr();
		m_pNodeFunction	= new CNodeStr();
		m_pNodeVariable	= new CNodeData(g_CUR_ADDRESS_TAG_NAME);
		m_pNodeCode		= new CNodeCode();
		m_pNodeRelation	= new CNodeRelation();

		m_pNodeCode->Set(0x41505047);	// magic code : GPPA
		
		m_pCurPC		= m_pNodeVariable;
		m_pCurPC->SetData(0);
		m_pCurPC->SetType(STRTYPE_ADDRESSTAG);
		m_pCurCode		= m_pNodeCode;

	}

	~CCompileResource(void){
		SAFE_DELETE(m_pSourceFile);
		SAFE_DELETE(m_pNodeDefine);
		SAFE_DELETE(m_pNodeFunction);
		SAFE_DELETE(m_pNodeVariable);
		SAFE_DELETE(m_pNodeCode);
		SAFE_DELETE(m_pNodeRelation);
	}

	CNodeData* FindVariable(const char* name){
		return m_pNodeVariable->Find(name);
	}

	CNodeData* NewVariable(const char* name){
		return m_pNodeVariable->Add(name);
	}
	void NextCode(DWORD Bytes){
		if(m_pCurCode->m_pNext) m_pCurCode = m_pCurCode->m_pNext;
		else m_pCurCode = m_pCurCode->Add(Bytes);
	}

	void PrevCode(void){
		CNodeCode *pCode;
		pCode = m_pNodeCode;
		do{
			if(pCode->m_pNext == m_pNodeCode){
				m_pNodeCode = pCode; break;
			}
			pCode = pCode->m_pNext;
		}while(pCode);
	}
};

class CGPCompile
{
protected:
	CCompileResource*	m_rsc;
	CStr*				m_pStr;
	CNodeData*			m_pCurVar;
	int					m_iPhase, m_iUnit;
	DWORD				m_dwDelimCount;		// '{','}' 구분자 카운트
	DWORD				m_dwFuncCount;		// 함수 개수
	DWORD				m_dwFuncDepth;		// 현재 함수 깊이
	BOOL				m_bFuncCallFirst;
public:
	CGPCompile(CCompileResource* rsc);
	~CGPCompile(void);

	BOOL Compile(void);
	BOOL Preprocess(void);
	BOOL Function(const char* func_name);

	BOOL Link(void);
	BOOL SaveObject(const char* file_name);
	BOOL GetLine(void);
};

#endif
