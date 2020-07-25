#include "StdAfx.h"
#include "TestDriveLua.h"
#include "TestDriveImp.h"
#include "lua/lua-5.1.4/src/lua.h"
#include "lua/lua_typed_enums.h"

#define LUA_ERROR	lua_error(CTestDriveLua::GetCurrent());
CTestDriveLua*	CTestDriveLua::m_pCurrentLua	= NULL;

CTestDriveLua::CTestDriveLua(void){
	m_pLua	= NULL;
}

CTestDriveLua::~CTestDriveLua(void){
	Release();
}

class Lua_System{
public:
	Lua_System(void){}
	~Lua_System(void){}

	void SetTitle(ITDSystem::TITLE_ID id, const char* name){
		g_pTestDrive->LogInfo(_T("id = %d\n"), id);
		g_pTestDrive->SetTitle(id, CString(name));
	}

	void LogInfo(const char* msg){
		g_pTestDrive->LogInfo(_T("%s"), CString(msg));
	}

	void LogError(const char* msg){
		g_pTestDrive->LogError(_T("%s"), CString(msg));
	}

	void ClearLog(void){
		g_pTestDrive->ClearLog();
	}

	bool Call(const char* filename){
		lua_State* pLua	= CTestDriveLua::GetCurrent();
		if(luaL_dofile(pLua, filename)){
			LogError(lua_tostring(pLua,-1));
			return false;
		}
		return true;
	}
};

static Lua_System	g_Lua_System;

class Lua_Memory{
public:
	Lua_Memory(void){}
	~Lua_Memory(void){}

	void Create(const char* name, unsigned int byte_size){
		CString mem_name(name);
		CMemory* pMemory = CMemory::Find(mem_name);
		if(pMemory){
			if(pMemory->GetSize() != byte_size){
				// 메모리가 이미 존재하면서 사이즈가 다를 때. (에러발생)
				g_pTestDrive->LogOut(_S(ALREADY_MEMORY_EXIST), ITDSystem::SYSMSG_WARNING);
				g_pTestDrive->LogError(_T("%s : %s"), _S(ALREADY_MEMORY_EXIST), mem_name);
				LUA_ERROR
				return;
			}else{	// 이미 같은 메모리가 존재함.
				return;
			}
		}
	}
};


static int LUA_SetTitle(lua_State *L){
	// 첫번째 파라미터 얻기
	if(check_enum_type(L, "title", -1)){
		// error
		g_pTestDrive->LogInfo(_T("err\n"));
	}
	int id = get_enum_value(L, -1);
	g_pTestDrive->LogInfo(_T("id    = %d\n"), id);

	// 두번째 파라미터 얻기 : 실패하고 있다.
	lua_pushvalue(L, -1);
	lua_tostring(L, 1);
	lua_pop(L, 1);

	/*lua_pushvalue(L, 1);
	lua_gettable(L, lua_upvalueindex(1));
	int id = (int)lua_tonumber(L, 1);
	lua_pop(L, 1);*/
	/*
	lua_pushvalue(L, 2);
	CString name(lua_tostring(L, 2));
	lua_pop(L, 1);

	g_pTestDrive->LogInfo(_T("id    = %d, %s\n"), id, name);
	*/

//	safe get table
// 	luaL_check_type(L, index, LUA_TTABLE);
// 	lua_gettable(L, index);
// 	if (lua_isnil(L, -1)) { /* why nil? */ }
// 	else { /* okay */ }




	return 0;
}

// LUA enum http://lua-users.org/wiki/BindingEnumsToLua 참조
static const luaL_reg __system_funcs[]={
	{"SetTitle", LUA_SetTitle},
	{NULL, NULL}
};

static Lua_Memory	g_Lua_Memory;

#define LUA_ENUM(L, name, val) \
	lua_pushlstring(L, #name, sizeof(#name)-1); \
	lua_pushnumber(L, val); \
	lua_settable(L, -3);

void CTestDriveLua::Initialize(void){
	Release();
	m_pLua = lua_open();
	luaopen_base(m_pLua);	// Lua 기본 함수들을 로드한다.- print() 사용
	luaopen_string(m_pLua);	// Lua 문자열 함수들을 로드한다.- string 사용
	luaopen_table(m_pLua);
	//luaopen_io(m_pLua);
	//luaopen_math(m_pLua);

// 	lua_newtable(m_pLua);
// 	LUA_ENUM(m_pLua, TITLE_MAIN, (int)ITDSystem::TITLE_MAIN);
// 	LUA_ENUM(m_pLua, TITLE_WORK, (int)ITDSystem::TITLE_WORK);
// 	LUA_ENUM(m_pLua, TITLE_DOC,  (int)ITDSystem::TITLE_DOC);

	lua_tinker::dostring(m_pLua, "enum TITLE{MAIN, WORK, DOC}");
	add_enum_to_lua(m_pLua, "title",
		"main", ITDSystem::TITLE_MAIN,
		"work", ITDSystem::TITLE_WORK,
		"document", ITDSystem::TITLE_DOC,
		NULL);
	

	luaL_openlib(m_pLua, "sys", __system_funcs, 1);

	lua_tinker::class_add<Lua_System>(m_pLua, "TestDriveSystem");
	lua_tinker::class_con<Lua_System>(m_pLua, lua_tinker::constructor<void>());
	lua_tinker::class_def<Lua_System>(m_pLua, "SetTitle",			&Lua_System::SetTitle);
	lua_tinker::class_def<Lua_System>(m_pLua, "LogInfo",			&Lua_System::LogInfo);
	lua_tinker::class_def<Lua_System>(m_pLua, "LogError",			&Lua_System::LogError);
	lua_tinker::class_def<Lua_System>(m_pLua, "ClearLog",			&Lua_System::ClearLog);
	lua_tinker::class_def<Lua_System>(m_pLua, "Call",				&Lua_System::Call);

	lua_tinker::set(m_pLua, "System", &g_Lua_System);

	lua_tinker::class_add<Lua_Memory>(m_pLua, "TestDriveMemory");
	lua_tinker::class_con<Lua_Memory>(m_pLua, lua_tinker::constructor<void>());
	lua_tinker::class_def<Lua_Memory>(m_pLua, "Create",				&Lua_Memory::Create);

	lua_tinker::set(m_pLua, "Memory", &g_Lua_Memory);
}

void CTestDriveLua::Release(void){
	if(m_pLua){
		lua_close(m_pLua);
		m_pLua	= NULL;
	}
}

BOOL CTestDriveLua::Run(const char* sFileName){
	if(!m_pLua) return FALSE;

	m_pCurrentLua	= this;
	//lua_tinker::dofile(m_pLua, sFileName);
	if(luaL_dofile(m_pLua, sFileName)){
		g_Lua_System.LogError(lua_tostring(m_pLua,-1));
		return FALSE;
	}

	return TRUE;
}
