#include "StdAfx.h"
#include "TestDriveLua.h"
#include "TestDriveImp.h"
#include "ViewTree.h"
#include "SplashScreenEx.h"
#include "FullPath.h"
#include "UACElevate.h"
#include <stdio.h>
#include <stdlib.h>
#include "lua_extra/cstring.h"
#include "lua_extra/TextFile.h"

using namespace luabridge;

static const TCHAR* MMFileName = _T("TESTDRIVE_MEMORY_MAPPED");	// default paged memory name


#define LUA_ERROR	lua_error(TestDriveLua::GetCurrent());
TestDriveLua*	TestDriveLua::m_pCurrentLua	= NULL;

class lua_cstring : public cstring
{
	int iTokenizePos;

public:
	lua_cstring(LuaRef v)
	{
		if (v.isString()) {
			m_sStr = v.tostring();
		}

		iTokenizePos = 0;
	}
	lua_cstring(const cstring& s)
	{
		m_sStr = s.c_str();
		iTokenizePos = 0;
	}
	lua_cstring(const lua_cstring& s)
	{
		m_sStr = s.c_str();
		iTokenizePos = 0;
	}
	~lua_cstring(void) {}

	int Compare(const char* s)
	{
		return cstring::Compare(s);
	}
	bool CompareFront(const char* s) const
	{
		return cstring::CompareFront(s);
	}
	bool CompareBack(const char* s) const
	{
		return cstring::CompareBack(s);
	}
	bool IsEmpty(void) const
	{
		return cstring::IsEmpty();
	}
	bool CutFront(const char* s, bool bRecursive = false)
	{
		iTokenizePos = 0;
		return cstring::CutFront(s, bRecursive);
	}
	bool CutBack(const char* s, bool bRecursive = false)
	{
		iTokenizePos = 0;
		return cstring::CutBack(s, bRecursive);
	}
	bool DeleteFront(const char* s)
	{
		iTokenizePos = 0;
		return cstring::DeleteFront(s);
	}
	bool DeleteBack(const char* s)
	{
		iTokenizePos = 0;
		return cstring::DeleteBack(s);
	}
	bool DeleteBlock(const char* sExpression, int iPos = 0)
	{
		iTokenizePos = 0;
		return cstring::DeleteBlock(sExpression, iPos);
	}
	void MakeUpper(void)
	{
		cstring::MakeUpper();
	}
	void MakeLower(void)
	{
		cstring::MakeLower();
	}
	bool Replace(const char* sSearch, const char* sReplace, bool bRecursive = false)
	{
		iTokenizePos = 0;
		return cstring::Replace(sSearch, sReplace, bRecursive);
	}
	bool ReplaceVariable(const char* sSearch, const char* sReplace)
	{
		iTokenizePos = 0;
		return cstring::ReplaceVariable(sSearch, sReplace);
	}
	void TrimLeft(const char* sDelim)
	{
		iTokenizePos = 0;
		cstring::TrimLeft(sDelim);
	}
	void TrimRight(const char* sDelim)
	{
		iTokenizePos = 0;
		cstring::TrimRight(sDelim);
	}
	void Trim(const char* sDelim)
	{
		iTokenizePos = 0;
		cstring::Trim(sDelim);
	}
	bool ReadFile(const char* sFile, bool bUseComment = false)
	{
		TextFile f;
		cstring	 sFileName(sFile);
		clear();

		if (f.Open(sFileName.c_str())) {
			f.GetAll(*(cstring*)this, bUseComment);
			return true;
		}

		return false;
	}
	int GetTokenizeLuaPos(void) const
	{
		return iTokenizePos;
	}
	void SetTokenizeLuaPos(int iPos)
	{
		iTokenizePos = iPos;
	}
	lua_cstring TokenizeLua(const char* sDelim)
	{
		return cstring::Tokenize(iTokenizePos, sDelim);
	}

	cstring		m_sVariable;
	lua_cstring TokenizeVariable(const char* sExpression)
	{
		cstring sTok;

		if (iTokenizePos >= 0) {
			int iPrevPos = iTokenizePos;
			iTokenizePos = FindVariableString(m_sVariable, sExpression, iTokenizePos);
			sTok = m_sStr.c_str() + iPrevPos;

			if (iTokenizePos >= 0) {
				sTok = m_sStr.c_str() + iPrevPos;
				sTok.erase(iTokenizePos - iPrevPos, -1);
			}
			else {
				m_sVariable.clear();
			}
		}
		else {
			m_sVariable.clear();
			sTok.clear();
		}

		return sTok;
	}

	lua_cstring GetVariable(void)
	{
		return m_sVariable;
	}

	int Length(void) const
	{
		return cstring::Length();
	}
	void Set(const char* sStr)
	{
		iTokenizePos = 0;
		cstring::Set(sStr);
	}

	int CheckFileExtensionLua(const char* sExtList)
	{
		if (sExtList) {
			const char* sDelim = " .,;";
			cstring		 sExts(sExtList);
			list<string> sList;
			{
				// make ext list
				int iPos = 0;

				while (1) {
					cstring sExt = sExts.Tokenize(iPos, sDelim);

					if (iPos > 0) {
						sList.push_back(sExt.c_str());
					}
					else
						break;
				}
			}

			if (sList.size()) {
				const char** sExtPrivateList = new const char* [sList.size() + 1];
				int			 t = 0;

				for (auto& i : sList) {
					sExtPrivateList[t] = i.c_str();
					t++;
				}

				sExtPrivateList[t] = NULL;
				int id = CheckFileExtension(sExtPrivateList);
				delete[] sExtPrivateList;
				return id;
			}
		}

		return -1;
	}

	bool GetEnvironment(const char* sKey)
	{
		return cstring::GetEnvironment(sKey);
	}

	void SetEnvironment(const char* sKey)
	{
		cstring::SetEnvironment(sKey);
	}

	void FormatDate(const char* sFormat, int iDayShift)
	{
		char	  sTime[1024];
		time_t	  now = time(0);
		struct tm t = *localtime(&now);

		if (iDayShift) {
			t.tm_mday += iDayShift;
			mktime(&t);
		}

		strftime(sTime, sizeof(sTime), sFormat, &t);
		m_sStr = sTime;
	}

	void Append(const char* sStr)
	{
		cstring::Append(sStr);
	}
	inline const char* c_str(void) const
	{
		return m_sStr.c_str();
	}
	inline char get(int iPos) const
	{
		if (iPos < 0 || iPos >= m_sStr.length())
			return 0;

		return m_sStr.c_str()[iPos];
	}
	inline int find_ch(char ch, int pos = 0)
	{
		return m_sStr.find(ch, pos);
	}
	inline int find(const char* s, int pos = 0)
	{
		return m_sStr.find(s, pos);
	}
	inline int rfind(const char* s)
	{
		return m_sStr.rfind(s);
	}
	inline int rfind_ch(char ch)
	{
		return m_sStr.rfind(ch);
	}
	inline int size(void)
	{
		return m_sStr.size();
	}
	inline int length(void)
	{
		return m_sStr.length();
	}
	inline void clear(void)
	{
		m_sStr.clear();
		iTokenizePos = 0;
	}
	inline void erase(int iPos, int iSize)
	{
		m_sStr.erase(iPos, iSize);
	}
	inline void insert(int iPos, const char* s)
	{
		if (iPos < iTokenizePos)
			iTokenizePos += strlen(s);

		m_sStr.insert(iPos, s);
	}
	inline void replace(int iPos, int iSize, const char* s)
	{
		m_sStr.replace(iPos, iSize, s);
	}
};

static string __exec(const char* cmd)
{
	string sCmd(cmd);
	char	buffer[128];
	string	result = "";
	sCmd += " 2>&1"; // redirect catch stderr
	FILE* pipe = _popen(sCmd.c_str(), "r");
	if (!pipe)
		throw runtime_error("popen() failed!");

	try {
		while (fgets(buffer, sizeof(buffer), pipe) != NULL) {
			result += buffer;
		}
	}
	catch (...) {
		_pclose(pipe);
		throw;
	}

	_pclose(pipe);
	return result;
}

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

	void SetTitle(const char* sTitle) {
		g_pTestDrive->SetTitle(ITDSystem::TITLE_MAIN, CString(sTitle));
		COutput* pMsg = g_pTestDrive->GetMsgOutput();
		pMsg->LogOut(CString(_S(MAIN_TITLE)) + _T(" : "), RGB(0, 0, 255));
		pMsg->LogOut(_TEXT_(_T("\"%s\"\n"), (LPCTSTR)CString(sTitle)), RGB(0, 0, 255), CFE_BOLD);
	}

	void SetSubTitle(const char* sTitle) {
		g_pTestDrive->SetTitle(ITDSystem::TITLE_WORK, CString(sTitle));
		COutput* pMsg = g_pTestDrive->GetMsgOutput();
		pMsg->LogOut(CString(_S(SUB_TITLE)) + _T(" : "), RGB(0, 0, 255));
		pMsg->LogOut(_TEXT_(_T("\"%s\"\n"), (LPCTSTR)CString(sTitle)), RGB(0, 0, 255), CFE_BOLD);
	}

	bool ShowSplash(const char* sFileName) {
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
	}

	bool CreateMemory(uint64_t uMemSize, const char* sName) {
		CString	sMemName(sName);
		if (!sName) sMemName = MMFileName;

		CMemory* pMemory = CMemory::Find(sMemName);
		if (pMemory) {
			if (pMemory->GetSize() != uMemSize) {	// 메모리가 존재하면서 사이즈가 다를 때...
				g_pTestDrive->LogError(_T("%s : %s"), _S(ALREADY_MEMORY_EXIST), (LPCTSTR)sMemName);
				return false;
			}
		}

		COutput* pMsg = g_pTestDrive->GetMsgOutput();
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
	}

	bool RunProfile(const char* sFileName) {
		CString sCurPath;

		// 현재 폴더위치 저장
		GetCurrentDirectory(8192, sCurPath.GetBuffer(8192));
		bool bRet = g_pTestDrive->Build(CString(sFileName), 0, g_pTestDrive->IsThreaded());
		// 현재 폴더 위치 복구
		SetCurrentDirectory(sCurPath);
		return bRet;
	}

	int Execute(const char* sCmd, const char* sArg, const char* sWorkPath, const char* sErrorTokens) {
		if (!sCmd) return -1;
		if (!sArg) sArg = "";
		if (!sWorkPath) sWorkPath = ".";

		CString		sArgument(sArg);
		CFullPath	work_path(sWorkPath);

		// run batch file
		CRedirectExecute* pExec = new CRedirectExecute(CString(sCmd), g_pTestDrive->GetMsgOutput(), sArg ? (LPCTSTR)sArgument : NULL, work_path);
		// 에러 문자열 지정
		if (sErrorTokens) {
			int			iPos = 0;
			CString		sTokens(sErrorTokens);
			CString		sTok = sTokens.Tokenize(_T(";,"), iPos);

			while (iPos > 0) {
				int iErrorPos = 0;
				CString	sError = sTok.Tokenize(_T("|"), iErrorPos);
				CString	sNum = sTok.Tokenize(NULL, iErrorPos);
				int iErrorCode = (iErrorPos > 0) ? StrToInt(sNum) : 0;
				if (sNum == _T("W")) iErrorCode = INT_MAX;	// for warning

				pExec->AddErrorToken(sError, iErrorCode);
				sTok = sTokens.Tokenize(NULL, iPos);
			}
		}
		pExec->AddErrorToken(_T("##"), 0);
		if (!pExec->Run()) {
			SAFE_DELETE(pExec);
			return -1;
		}
		int iRet = pExec->GetErrorCode();
		SAFE_DELETE(pExec);

		return iRet;
	}

	bool SetProfilePath(const char* sID, const char* sPath) {
		if (!sID) sID = "root";

		CString sProfileID(sID);
		if (!sPath) sPath = "";

		if (sProfileID == "root") {
			CFullPath	full_path(sPath);
			{	// set root profile path
				CString sPath(full_path);
				sPath.Replace(_T('\\'), _T('/'));
				SetGlobalEnvironmentVariable(_T("TESTDRIVE_PROFILE"), sPath);
			}
			{	// set profile common binary path
				CString sPath;
				sPath.Format(_T("%scommon\\bin\\"), (LPCTSTR)full_path);
				full_path.SetPath(sPath);
				//GetFullPathName(sPath, 1024, fullpath, NULL);
				{
					int iLen = (int)_tcslen(full_path);
					if (full_path.Path()[iLen - 1] == _T('\\'))
						full_path.Path().SetAt(iLen - 1, _T('\0'));
				}
				ModifyGlobalEnvironmentPath(full_path);
			}
		}
		else {
			int id = CheckCommand(sProfileID, g_sCmdProfileSet, TESTDRIVE_PROFILE_SIZE);

			if (id >= TESTDRIVE_PROFILE_SIZE) return false;

			if (!g_pTestDrive->SetProfilePath((TESTDRIVE_PROFILE)id, CString(sPath))) {
				g_pTestDrive->LogError(CString(_S(INVALID_FILE_PATH)) + _T(" : %s"), (LPCTSTR)CString(sPath));
				//goto ERROR_OCCUR;
			}
		}

		return true;
	}

	void ClearProfile(void) {
		g_ProfileTree.DeleteAllItems();
	}

	bool CallProfile(const char* sFileName) {
		bool		bRet = false;
		CFullPath	cur_path;

		bRet = g_pTestDrive->Build(CString(sFileName), &g_Output[COutput::TD_OUTPUT_SYSTEM], g_pTestDrive->IsThreaded());

		SetCurrentDirectory(cur_path);

		return bRet;
	}

	bool ElevatedExecute(const char* sExecute, const char* sParam, const char* sDir) {
		UACElevate UAC;
		CFullPath	cur_path;
		BOOL bRet = FALSE;
		{
			CString work_path = g_pTestDrive->RetrieveFullPath(CString(sDir));
			if (!sParam) sParam = "";
			bRet = UAC.ShellExecWithElevation(NULL, CString(sExecute), CString(sParam), work_path);
		}
		SetCurrentDirectory(cur_path);
		return bRet;
	}

	const char* GetLocaleString(void) {
		static CStringA		sLocale;
		sLocale = g_Localization.CurrentLocale()->sName;
		return sLocale;
	}
};

BOOL SearchTreeFile(LPCTSTR sPath, LPVOID pData);
class ProfileTree {
public:
	ProfileTree(const char* sName, ProfileTree* pParent) {
		CString	sTreeName(sName);
		m_Tree = g_ProfileTree.FindChildItem(pParent ? pParent->Object() : NULL, sTreeName);
		if (!m_Tree) {
			m_Tree = g_ProfileTree.InsertTree(sTreeName, pParent ? pParent->Object() : NULL);
		}
	}

	virtual ~ProfileTree(void) {
	}

	void Expand(void) {
		g_ProfileTree.ExpandTree(m_Tree, true);
	}

	void Collapse(void) {
		g_ProfileTree.ExpandTree(m_Tree, false);
	}

	bool AddItem(const char* sType, const char* sName, const char* sCommand) {
		if (!sType || !sName || !sCommand) {
			return false;
		}
		CString		szType(sType);
		int iType	= CheckCommand(szType, g_sProfileTree, (int)TREE_ITEM_PROFILE + 1);
		if (iType < 0) {
			g_pTestDrive->LogError(_T("Can't recognize item type of 'AddItem' : %s"), (LPCTSTR)szType);
			return false;
		}

		g_ProfileTree.InsertData(m_Tree, (TD_TREE_ITEM)iType, CString(sName), CString(sCommand));

		return true;
	}

	bool Search(const char* sSearchPath, const char* sFileName, const char* sHeader) {
		if (!sSearchPath || !sFileName || !sHeader) {
			return false;
		}
		g_ProfileTree.SetRootItem(m_Tree);
		g_pTestDrive->SearchSubPathFile(CString(sSearchPath), CString(sFileName), SearchTreeFile, (LPVOID)(LPCTSTR)CString(sHeader));
		return true;
	}

	inline HTREEITEM Object(void) {
		return m_Tree;
	}

protected:
	HTREEITEM	m_Tree;
};

int			luaopen_lfs(lua_State* L);;
bool TestDriveLua::Initialize(void){
	Release();

	if (!(m_pLua = luaL_newstate()))
		return false;

	luaL_openlibs(m_pLua);	// load all standard libraries
	luaopen_lfs(m_pLua);	// lfs library

	getGlobalNamespace(m_pLua)
		.beginClass<TextFile>("TextFile")
		.addConstructor<void (*)(void)>()
		.addFunction("Open", &TextFile::Open)
		.addFunction("Create", &TextFile::Create)
		.addFunction("Close", &TextFile::Close)
		.addFunction("Put", &TextFile::Puts)
		.addFunction("Get", &TextFile::Gets)
		.addFunction("GetAll", std::function<string(TextFile* pFile, bool bUseComment)>(
			[](TextFile* pFile, bool bUseComment) -> string {
				cstring sContents;
				pFile->GetAll(sContents, bUseComment);
				return sContents.c_string();
			}))
		.addFunction("LineNumber", &TextFile::LineNumber)
		.addFunction("IsOpen", &TextFile::IsOpen)
		.addFunction("IsEOF", &TextFile::IsEOF)
		.endClass()
		.beginClass<lua_cstring>("String")
		.addConstructor<void (*)(LuaRef v)>()
		.addFunction("Replace", &lua_cstring::Replace)
		.addFunction("ReplaceVariable", &lua_cstring::ReplaceVariable)
		.addProperty("s", &lua_cstring::c_str, &lua_cstring::Set)
		.addFunction("Append", &lua_cstring::Append)
		.addFunction("Length", &lua_cstring::Length)
		.addFunction("Compare", &lua_cstring::Compare)
		.addFunction("CompareFront", &lua_cstring::CompareFront)
		.addFunction("CompareBack", &lua_cstring::CompareBack)
		.addFunction("IsEmpty", &lua_cstring::IsEmpty)
		.addFunction("CutFront", &lua_cstring::CutFront)
		.addFunction("CutBack", &lua_cstring::CutBack)
		.addFunction("ReadFile", &lua_cstring::ReadFile)
		.addFunction("Trim", &lua_cstring::Trim)
		.addFunction("TrimLeft", &lua_cstring::TrimLeft)
		.addFunction("TrimRight", &lua_cstring::TrimRight)
		.addFunction("Tokenize", &lua_cstring::TokenizeLua)
		.addFunction("TokenizeVariable", &lua_cstring::TokenizeVariable)
		.addFunction("GetVariable", &lua_cstring::GetVariable)
		.addProperty("TokenizePos", &lua_cstring::GetTokenizeLuaPos, &lua_cstring::SetTokenizeLuaPos)
		.addFunction("DeleteFront", &lua_cstring::DeleteFront)
		.addFunction("DeleteBack", &lua_cstring::DeleteBack)
		.addFunction("DeleteBlock", &lua_cstring::DeleteBlock)
		.addFunction("MakeUpper", &lua_cstring::MakeUpper)
		.addFunction("MakeLower", &lua_cstring::MakeLower)
		.addFunction("CheckFileExtension", &lua_cstring::CheckFileExtensionLua)
		.addFunction("GetEnvironment", &lua_cstring::GetEnvironment)
		.addFunction("SetEnvironment", &lua_cstring::SetEnvironment)
		.addFunction("FormatDate", &lua_cstring::FormatDate)
		.addFunction("insert", &lua_cstring::insert)
		.addFunction("clear", &lua_cstring::clear)
		.addFunction("find", &lua_cstring::find)
		.addFunction("rfind", &lua_cstring::rfind)
		.addFunction("at", &lua_cstring::get)
		.addFunction("erase", &lua_cstring::erase)
		.addFunction("insert", &lua_cstring::insert)
		.endClass()
		.beginClass<ProfileTree>("ProfileTree")
		.addConstructor<void (*)(const char* sName, ProfileTree* pParent)>()
		.addFunction("AddItem", &ProfileTree::AddItem)
		.addFunction("Expand", &ProfileTree::Expand)
		.addFunction("Collapse", &ProfileTree::Collapse)
		.addFunction("Search", &ProfileTree::Search)
		.endClass()
		.beginNamespace("System")
		.addFunction("SetTitle", &Lua_System::SetTitle)
		.addFunction("SetSubTitle", &Lua_System::SetSubTitle)
		.addFunction("ShowSplash", &Lua_System::ShowSplash)
		.addFunction("CreateMemory", &Lua_System::CreateMemory)
		.addFunction("RunProfile", &Lua_System::RunProfile)
		.addFunction("Execute", &Lua_System::Execute)
		.addFunction("SetProfilePath", &Lua_System::SetProfilePath)
		.addFunction("ClearProfile", &Lua_System::ClearProfile)
		.addFunction("CallProfile", &Lua_System::CallProfile)
		.addFunction("ElevatedExecute", &Lua_System::ElevatedExecute)
		.addProperty("Locale", &Lua_System::GetLocaleString)
		.endNamespace()
		.addFunction("print", &Lua_System::Log)
		.addFunction("LOGI", &Lua_System::LogInfo)
		.addFunction("LOGE", &Lua_System::LogError)
		.addFunction("LOGW", &Lua_System::LogWarning)
		.addFunction("exec", __exec)
		.addFunction("LOG_CLEAR", &Lua_System::ClearLog);

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