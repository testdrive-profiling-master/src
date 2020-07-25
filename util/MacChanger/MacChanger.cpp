// MacChanger.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include "MacChanger.h"
#include "NetworkAdapter.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 유일한 응용 프로그램 개체입니다.
CWinApp theApp;

using namespace std;


int Init(ADAPTERINFO*& pAdapters)
{
	if(pAdapters) delete[] pAdapters;
	pAdapters = NULL;
	CNetworkAdapterList nal;
	int count = 0;
	if(nal.IsValid())
	{
		count = nal.GetCount();				
		if(count>0)
		{
			pAdapters = new ADAPTERINFO[count];
			nal.GetAdapters(pAdapters);
		}
	}
	return count;
}

bool VerifyMacID(CString strMac)
{
	bool bRet = false;
	strMac.MakeUpper().Remove(_T(' '));		
	if(strMac.GetLength() == 12)
	{			
		for(int i=0; i<12; i++)
		{
			if( !( ( (strMac[i] >= _T('0')) && (strMac[i] <= _T('9')) )
				|| ( (strMac[i] >= _T('A')) && (strMac[i] <= _T('F')) ) ) )
			{				
				break;
			}
			if(i==11)
				bRet = true;
		}		
	}
	return bRet;
}

void main_program(int argc, TCHAR* argv[]){
	ADAPTERINFO*			pAdapters = NULL;
	ADAPTERINFO*			pCurAdapter;
	int count				= Init(pAdapters);
	
	printf("Mac-Address Changer (" __DATE__ ", Q&A : clonextop@gmail.com)\n\n");

	if(argc == 1){
		printf("Usage : MacChanger.exe [-i connection_name] [-d] [mac_address] [-l]\n"	\
			"  -l                  : List up all adapters\n" \
			"  -i connection_name  : Select adapter\n" \
			"  -d                  : Reset mac-address of selected network adapter\n" \
			"  mac_address         : Mac-Address you have to change\n"
			);
		goto ON_PROGRAM_END;
	}

	if(!count){
		printf("Can't find net adapter list\n");
		return;
	}
	{
		// find wireless adopter
		pCurAdapter = &pAdapters[0];	// search default adapter
		for(int i=1;i<argc;i++){
			CString desc = pAdapters[i].Description;
			desc.MakeLower();
			if(desc.Find(_T("wireless")) != -1){
				pCurAdapter = &pAdapters[i];
				break;
			}
		}
		
		for(int i=1;i<argc;i++){
			if(*argv[i] == _T('-') && argv[i][2] == 0){
				switch(argv[i][1]){
				case _T('L'):
				case _T('l'):
					printf("Adapter list :\n");
					for(int t=0;t<count;t++){
							printf("[%d] : %s\n", t+1, (const char*)CStringA(pAdapters[t].Description));
					}
					break;
				case _T('I'):
				case _T('i'):
					{
						if(i+1 == argc){
							printf("Need network connection name!\n");
							goto ON_PROGRAM_END;
						}
						i++;
						{
							CString name(argv[i]);
							name.MakeLower();
							for(int t=0;t<count;t++){
								CString desc(pAdapters[t].Description);
								desc.MakeLower();
								if(desc.Find(name) != -1){
									printf("Set current adapter : %s\n", (const char*)CStringA(pAdapters[t].Description));
									pCurAdapter = &pAdapters[t];
									goto FOUND_ADAPTER;
								}
							}
							printf("Can't find adapter : %s\n", (const char*)CStringA(argv[i]));
							goto ON_PROGRAM_END;
FOUND_ADAPTER:				;
						}
					}
					break;
				case _T('D'):
				case _T('d'):
					printf("Reset adapter : %s\n", (const char*)CStringA(pCurAdapter->Description));
					UpdateRegistry(pCurAdapter->InstanceId);
					Reset(pCurAdapter);
					break;
				}
				// options
			}else{
				// Set address
				CString mac_address(argv[i]);
				printf("Try set mac-address : %s <= %s\n", (const char*)CStringA(pCurAdapter->Description), (const char*)CStringA(mac_address));
				if(!VerifyMacID(mac_address)){
					printf("It's not a mac address : %s\n", (const char*)CStringA(argv[i]));
					goto ON_PROGRAM_END;
				}
				mac_address.Remove(_T(' '));
				if(UpdateRegistry(pCurAdapter->InstanceId, mac_address)){
					if(Reset(pCurAdapter)){
						count = Init(pAdapters);
						printf("- Ok!\n");
					}else{
						printf("Can't reset adapter : %s\n", (const char*)CStringA(pCurAdapter->Description));
						goto ON_PROGRAM_END;
					}
				}else{
					printf("Can't Update registry : %s\n", (const char*)CStringA(pCurAdapter->Description));
					goto ON_PROGRAM_END;
				}
			}
		}

ON_PROGRAM_END:
		if(pAdapters) delete [] pAdapters;
	}
}

int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	int nRetCode = 0;

	HMODULE hModule = ::GetModuleHandle(NULL);

	if (hModule != NULL)
	{
		// MFC를 초기화합니다. 초기화하지 못한 경우 오류를 인쇄합니다.
		if (!AfxWinInit(hModule, NULL, ::GetCommandLine(), 0))
		{
			// TODO: 오류 코드를 필요에 따라 수정합니다.
			_tprintf(_T("심각한 오류: MFC를 초기화하지 못했습니다.\n"));
			nRetCode = 1;
		}
		else
		{
			// TODO: 응용 프로그램의 동작은 여기에서 코딩합니다.
			main_program(argc, argv);
		}
	}
	else
	{
		// TODO: 오류 코드를 필요에 따라 수정합니다.
		_tprintf(_T("심각한 오류: GetModuleHandle 실패\n"));
		nRetCode = 1;
	}

	return nRetCode;
}
