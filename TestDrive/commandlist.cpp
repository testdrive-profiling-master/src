#include "stdafx.h"
#include "commandlist.h"

const TCHAR* g_sCmd[CMD_SIZE] = {
	_T("system"),
	_T("profile"),
	_T("memory"),
	_T("video"),
	_T("if"),
};

const TCHAR* g_sCmdSystem[CMD_SYSTEM_SIZE] = {
	_T("title"),
	_T("subtitle"),
	_T("splash"),
	_T("clear"),
	_T("call"),
	_T("shell"),
	_T("lua"),
	_T("document"),
	_T("msg"),
	_T("error"),
};

const TCHAR* g_sCmdSystemDocument[CMD_SYSTEM_VIEW_SIZE] = {
	_T("add"),
	_T("close"),
	_T("call"),
};

const TCHAR* g_sCmdVideo[CMD_VIDEO_SIZE] = {
	_T("create"),
	_T("address"),
	_T("stride"),
	_T("update"),
	_T("store"),
};

const TCHAR* g_sCmdIf[CMD_IF_SIZE] = {
	_T("file"),
	_T("document"),
	_T("locale"),
	_T("else"),
	_T("end"),
};

const TCHAR* g_sCmdMemory[CMD_MEMORY_SIZE] = {
	_T("create"),
	_T("load"),
	_T("store"),
};

const TCHAR* g_sCmdMemoryLoadStore[CMD_MEMORY_LOAD_SIZE] = {
	_T("image"),
	_T("sim"),
	_T("bin"),
	_T("dword"),
	_T("word"),
	_T("byte"),
	_T("float"),
	_T("text"),
};

const TCHAR* g_sCmdProfile[CMD_PROFILE_SIZE] = {
	_T("tree"),
	_T("clear"),
	_T("call"),
	_T("set"),
	_T("path"),
};

const TCHAR* g_sCmdProfileList[CMD_PROFILE_LIST_SIZE] = {
	_T("root"),
	_T("parent"),
	_T("child"),
	_T("add"),
};

const TCHAR* g_sCmdProfileSet[TESTDRIVE_PROFILE_SIZE] = {
	_T("initialize"),
	_T("check"),
	_T("cleanup"),
};

const TCHAR* g_sProfileTree[PROFILE_ITEM_SIZE] = {
	_T("screen"),
	_T("chart"),
	_T("report"),
	_T("network"),
	_T("profile"),
	_T("branch"),
	_T("search"),
	_T("expand"),
};
