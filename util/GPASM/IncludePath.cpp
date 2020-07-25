#include "IncludePath.h"
#include <Windows.h>

IncludePath*	IncludePath::m_pHead	= NULL;

IncludePath::IncludePath(const char* sIncludePath)
{
	if(!m_pHead)
		m_pHead	= this;
	else{
		// Add to last include path
		IncludePath*	pNode	= m_pHead;
		while(pNode->Next())
			pNode	= pNode->Next();

		pNode->m_pNext	= this;
	}
	m_pNext		= NULL;
	SetEnable();
	Set(sIncludePath);
}

IncludePath::~IncludePath(void)
{
	if(m_pHead == this){
		m_pHead	= Next();
	}else{
		IncludePath*	pNode	= m_pHead;
		while(pNode->Next() != this)
			pNode	= pNode->Next();
		pNode->m_pNext	= Next();
	}
}

void IncludePath::ReleaseAll(void){
	while(m_pHead) delete m_pHead;
}

void IncludePath::Set(const char* sIncludePath){
	char	sFullPath[4096];
	ExpandEnvironmentStrings(sIncludePath, sFullPath, 4096);
	m_sPath	= sFullPath;
}
