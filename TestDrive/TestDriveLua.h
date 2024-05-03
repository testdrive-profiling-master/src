#pragma once
#include "LuaLibrary.h"
#include "LuaBridge/LuaBridge.h"

class TestDriveLua
{
public:
	TestDriveLua(void);
	~TestDriveLua(void);

	bool Initialize(void);
	void Release(void);
	bool Run(const char* sFileName);
	bool RunBuffer(const char* sBuffer, const char* sFileName, int iLineNumber = 1);
	static inline lua_State* GetCurrent(void)	{return m_pCurrentLua->m_pLua;}

private:
	static TestDriveLua*	m_pCurrentLua;
	lua_State*				m_pLua;
};

