#include "StdAfx.h"
#include "TestDriveLua.h"
#include "TestDriveImp.h"

using namespace luabridge;

#define LUA_ERROR	lua_error(CTestDriveLua::GetCurrent());
CTestDriveLua*	CTestDriveLua::m_pCurrentLua	= NULL;

CTestDriveLua::CTestDriveLua(void){
	m_pLua	= NULL;
}

CTestDriveLua::~CTestDriveLua(void){
	Release();
}

namespace Lua_System {
	void SetTitle(ITDSystem::TITLE_ID id, const char* name){
		if (g_pTestDrive) {
			g_pTestDrive->LogInfo(_T("id = %d\n"), id);
			g_pTestDrive->SetTitle(id, CString(name));
		}
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

static Lua_Memory	g_Lua_Memory;

#define LUA_ENUM(L, name, val) \
	lua_pushlstring(L, #name, sizeof(#name)-1); \
	lua_pushnumber(L, val); \
	lua_settable(L, -3);

BOOL CTestDriveLua::Initialize(void){
	Release();

	if (!(m_pLua = luaL_newstate()))
		return FALSE;

	luaL_openlibs(m_pLua);	// load all standard libraries

	getGlobalNamespace(m_pLua)
		.beginNamespace("System")
		.addFunction("SetTitle", Lua_System::SetTitle)
		.addFunction("LogInfo", Lua_System::LogInfo)
		.addFunction("LogError", Lua_System::LogError)
		.addFunction("ClearLog", Lua_System::ClearLog)
		.addFunction("Call", Lua_System::Call)
		.endNamespace();

	return TRUE;
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
		Lua_System::LogError(lua_tostring(m_pLua,-1));
		return FALSE;
	}

	return TRUE;
}
