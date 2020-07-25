#pragma once
#include "define_base.h"

typedef enum{
	PASER_ID_NULL,
	PASER_ID_NAME,
	PASER_ID_STRING,
	PASER_ID_NUMERIC,
//	PASER_ID_FLOAT,
	PASER_ID_SPECIAL,
} PASER_ID;

class CPaser
{
	CPaser*		m_pNext;
	char*		m_sPaser;
	PASER_ID	id;

public:
	CPaser(void);
	~CPaser(void);
	CPaser* Next(void);
	CPaser* Delete(void);
	void Paser(char* line);
};
