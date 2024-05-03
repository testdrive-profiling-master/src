#include "StdAfx.h"
#include "TestDriveLua.h"
#include "TestDriveImp.h"
#include "ViewTree.h"
#include "SplashScreenEx.h"
#include "FullPath.h"

using namespace luabridge;

static const TCHAR* MMFileName = _T("TESTDRIVE_MEMORY_MAPPED");	// default paged memory name


#define LUA_ERROR	lua_error(TestDriveLua::GetCurrent());
TestDriveLua*	TestDriveLua::m_pCurrentLua	= NULL;

TestDriveLua::TestDriveLua(void){
	m_pLua	= NULL;
}

TestDriveLua::~TestDriveLua(void){
	Release();
}

namespace Lua_System {
	void Log(const char* msg) {
		g_pTestDrive->LogOut(CString(msg), ITDSystem::SYSMSG_NONE);
	}

	void LogInfo(const char* msg){
		g_pTestDrive->LogInfo(_T("%s"), CString(msg));
	}

	void LogError(const char* msg){
		g_pTestDrive->LogError(_T("%s"), CString(msg));
	}

	void LogWarning(const char* msg) {
		g_pTestDrive->LogWarning(_T("%s"), CString(msg));
	}

	void ClearLog(void){
		g_pTestDrive->ClearLog();
	}

	bool Call(const char* filename){
		lua_State* pLua	= TestDriveLua::GetCurrent();
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

bool TestDriveLua::Initialize(void){
	Release();

	if (!(m_pLua = luaL_newstate()))
		return false;

	luaL_openlibs(m_pLua);	// load all standard libraries

	getGlobalNamespace(m_pLua)
		.beginNamespace("System")
		.addFunction("ClearLog", std::function<void(void)>([](void) {
				g_pTestDrive->ClearLog();
			}))
		.addFunction("SetTitle", std::function<void(const char* sTitle)>([](const char* sTitle) {
				g_pTestDrive->SetTitle(ITDSystem::TITLE_MAIN, CString(sTitle));
				COutput* pMsg	= g_pTestDrive->GetMsgOutput();
				pMsg->LogOut(CString(_S(MAIN_TITLE)) + _T(" : "), RGB(0, 0, 255));
				pMsg->LogOut(_TEXT_(_T("\"%s\"\n"), (LPCTSTR)CString(sTitle)), RGB(0, 0, 255), CFE_BOLD);
			}))
		.addFunction("SetSubTitle", std::function<void(const char* sTitle)>([](const char* sTitle) {
				g_pTestDrive->SetTitle(ITDSystem::TITLE_WORK, CString(sTitle));
				COutput* pMsg = g_pTestDrive->GetMsgOutput();
				pMsg->LogOut(CString(_S(SUB_TITLE)) + _T(" : "), RGB(0, 0, 255));
				pMsg->LogOut(_TEXT_(_T("\"%s\"\n"), (LPCTSTR)CString(sTitle)), RGB(0, 0, 255), CFE_BOLD);
			}))
		.addFunction("ShowSplash", std::function<bool(const char* sFileName)>([](const char* sFileName) -> bool {
				CSplashScreenEx* pSplash = new CSplashScreenEx();
				if (pSplash->Create(AfxGetMainWnd()->m_hWnd, CString(sFileName))) {
					pSplash->Start(2000, 15);
					return true;
				}
				else {
					delete pSplash;
					g_pTestDrive->LogError(CString(_S(LOAD_FILE_IS_FAILED)) + _T(" : \"%s\""), (LPCTSTR)CString(sFileName));
				}
				return false;
			}))
		.addFunction("CreateMemory", std::function<bool(uint64_t uMemSize, const char* sName)>([](uint64_t uMemSize, const char* sName) -> bool {
				CString	sMemName(sName);
				if (!sName) sMemName = MMFileName;

				CMemory* pMemory = CMemory::Find(sMemName);
				if(pMemory) {
					if (pMemory->GetSize() != uMemSize) {	// 메모리가 존재하면서 사이즈가 다를 때...
						g_pTestDrive->LogError(_T("%s : %s"), _S(ALREADY_MEMORY_EXIST), (LPCTSTR)sMemName);
						return false;
					}
				}

				COutput*	pMsg	= g_pTestDrive->GetMsgOutput();
				pMsg->LogOut(_S(CREATE_MEMORY_MODEL), RGB(0, 0, 255));

				{
					CString sBytes;	// 1000 자리 구분자 ',' 추가
					sBytes.Format(_T("%lld"), uMemSize);
					for (int i = sBytes.GetLength() - 3; i > 0; i -= 3) {
						sBytes.Insert(i, _T(','));
					}
					pMsg->LogOut(_TEXT_(_T("'%s' : %s Bytes "), (LPCTSTR)sMemName, (LPCTSTR)sBytes), RGB(0, 0, 255), CFE_BOLD);
				}

				if ((uMemSize / 0x100000))		pMsg->LogOut(_TEXT_(_T("(%.1fMB)\n"), (double)uMemSize / 0x100000), RGB(0, 0, 255), CFE_BOLD);
				else if ((uMemSize / 0x400))	pMsg->LogOut(_TEXT_(_T("(%.1fKB)\n"), (double)uMemSize / 0x400), RGB(0, 0, 255), CFE_BOLD);
				else pMsg->LogOut(_T("\n"));

				{
					CMemory* pMemory = new CMemory;
					if (!pMemory->Create(uMemSize, sMemName)) {
						delete pMemory;
						return false;
					}
				}
				return true;
			}))
		.addFunction("RunProfile", std::function<bool(const char* sFileName)>([](const char* sFileName) -> bool {
				TCHAR cur_path[8192];
				// 현재 폴더위치 저장
				GetCurrentDirectory(8192, cur_path);
				bool bRet	= g_pTestDrive->Build(CString(sFileName), 0, g_pTestDrive->IsThreaded());
				// 현재 폴더 위치 복구
				SetCurrentDirectory(cur_path);
				return bRet;
			}))
		.addFunction("Execute", std::function<int(const char* sCmd, const char* sArg, const char* sWorkPath)>([](const char* sCmd, const char* sArg, const char* sWorkPath) -> int {
				if (!sCmd) return -1;
				if (!sArg) sArg = "";
				if (!sWorkPath) sWorkPath = "";

				CFullPath	work_path(CString(sWorkPath));

				// run batch file
				//CRedirectExecute* pExec = new CRedirectExecute(CString(sCmd), g_pTestDrive->GetMsgOutput(), sArg ? CString(sArg) : NULL, (LPCTSTR)work_path);
				// 에러 문자열 지정
				/*while (paser.TokenOut(TD_DELIMITER_COMMA)) {
					int error_code = -1;
					if (!paser.GetTokenString(temp)) {
						SAFE_DELETE(pExec);
						goto ERROR_OCCUR;
					}
					if (paser.TokenOut(TD_DELIMITER_COLON)) {
						TCHAR sCode[MAX_PATH];
						switch (paser.GetToken(sCode)) {
						case TD_TOKEN_INT:
							error_code = StrToInt(temp);
							break;
						case TD_TOKEN_NAME:
							if (!_tcscmp(sCode, _T("W"))) {
								error_code = INT_MAX;
							}
							else goto ERROR_OCCUR;
							break;
						default:
							goto ERROR_OCCUR;
						}
					}
					pExec->AddErrorToken(temp, error_code);
				}*/
				/*pExec->AddErrorToken(_T("##"), 0);
				if (!pExec->Run()) {
					SAFE_DELETE(pExec);
					return -1;
				}
				int iRet = pExec->GetErrorCode();
				SAFE_DELETE(pExec);

				return iRet;*/
				return 0;
			}))
		.addFunction("SetProfilePath", std::function<bool(const char* sID, const char* sPath)>([](const char* sID, const char* sPath) -> bool {
				if (!sID) sID = "root";

				CString sProfileID(sID);
				if (!sPath) sPath = "";
				
				if (sProfileID == "root") {
					TCHAR fullpath[8192];
					GetFullPathName(CString(sPath), 8192, fullpath, NULL);
					{	// set root profile path
						CString sPath(fullpath);
						sPath.Replace(_T('\\'), _T('/'));
						SetGlobalEnvironmentVariable(_T("TESTDRIVE_PROFILE"), sPath);
					}
					{	// set profile common binary path
						CString sPath;
						sPath.Format(_T("%scommon\\bin\\"), fullpath);
						GetFullPathName(sPath, 1024, fullpath, NULL);
						{
							int iLen = (int)_tcslen(fullpath);
							if (fullpath[iLen - 1] == _T('\\'))
								fullpath[iLen - 1] = _T('\0');
						}
						ModifyGlobalEnvironmentPath(fullpath);
					}
				} else {
					int id = CheckCommand(sProfileID, g_sCmdProfileSet, TESTDRIVE_PROFILE_SIZE);

					if (id >= TESTDRIVE_PROFILE_SIZE) return false;

					if (!g_pTestDrive->SetProfilePath((TESTDRIVE_PROFILE)id, CString(sPath))) {
						g_pTestDrive->LogError(CString(_S(INVALID_FILE_PATH)) + _T(" : %s"), (LPCTSTR)CString(sPath));
						//goto ERROR_OCCUR;
					}
				}

				return true;
			}))
		.addFunction("ClearProfile", std::function<void()>([]() {
				g_ProfileTree.DeleteAllItems();
			}))
		.endNamespace()
		.addFunction("print", Lua_System::Log)
		.addFunction("LOGI", Lua_System::LogInfo)
		.addFunction("LOGE", Lua_System::LogError)
		.addFunction("LOGW", Lua_System::LogWarning)
		.addFunction("LOG_CLEAR", Lua_System::ClearLog);

	return true;
}

void TestDriveLua::Release(void){
	if(m_pLua){
		lua_close(m_pLua);
		m_pLua	= NULL;
	}
}

bool TestDriveLua::Run(const char* sFileName){
	if(!m_pLua) return false;

	m_pCurrentLua	= this;
	//lua_tinker::dofile(m_pLua, sFileName);
	if(luaL_dofile(m_pLua, sFileName)){
		Lua_System::LogError(lua_tostring(m_pLua,-1));
		return false;
	}

	return true;
}

bool TestDriveLua::RunBuffer(const char* sBuffer, const char* sFileName, int iLineNumber)
{
	if (sBuffer && sFileName) {
		//TODO: setup line number!
		if(luaL_loadbuffer(m_pLua, sBuffer, strlen(sBuffer), sFileName) || lua_pcall(m_pLua, 0, LUA_MULTRET, 0)) {
			const char* sError = luaL_checkstring(m_pLua, -1);
			CStringA sMsg;
			sMsg.Format("Error on running script : %s", sError);
			Lua_System::LogError(sMsg);
			lua_pop(m_pLua, 1); // pop out error message
		}
		else return true;
	}
	return false;
}