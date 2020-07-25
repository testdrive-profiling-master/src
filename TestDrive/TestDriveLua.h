#pragma once
#include "lua_tinker.h" 

class CTestDriveLua
{
public:
	CTestDriveLua(void);
	~CTestDriveLua(void);

	void Initialize(void);
	void Release(void);
	BOOL Run(const char* sFileName);
	static inline lua_State* GetCurrent(void)	{return m_pCurrentLua->m_pLua;}

private:
	static CTestDriveLua*	m_pCurrentLua;
	lua_State*				m_pLua;
};

