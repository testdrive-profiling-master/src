#include "Paser.h"

CPaser::CPaser(void)
{
	m_pNext		= NULL;
	m_sPaser	= NULL;
	id			= PASER_ID_NULL;
}

CPaser::~CPaser(void)
{
	if(m_pNext) delete m_pNext;
}

CPaser* CPaser::Next(void)
{
	return m_pNext;
}

CPaser* CPaser::Delete(void)
{
	CPaser* pPaser = m_pNext;
	m_pNext = NULL;
	delete this;
	return pPaser;
}

void CPaser::Paser(char* line)
{

}
