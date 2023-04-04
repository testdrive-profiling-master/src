#include "stdafx.h"
#include "RedirectExecute.h"
#include "Main.h"
#include "TestDriveImp.h"
#include "FullPath.h"
#include "MainFrm.h"

const int BUF_SIZE = 8192;

CRedirectExecute*	CRedirectExecute::m_pCurrent	= NULL;

CRedirectExecute::CRedirectExecute(LPCTSTR lpszCommand, COutput *pMsg, LPCTSTR lpszArgument, LPCTSTR lpszWorkDir)
{
	m_bKillDebug				= FALSE;
	m_bStopped					= FALSE;
	m_dwSleepMilliseconds		= 10;
	m_pMsg						= pMsg;
	m_pErrorTokenLink			= NULL;
	m_iErrorCode				= 0;
	m_fLogFunc					= NULL;
	m_hSTDIN					= NULL;

	ASSERT(lpszCommand);

	{
		TCHAR sWorkPath[1024];
		if(lpszWorkDir){
			CFullPath	full_path(lpszWorkDir);
			m_sWorkDirectory	= full_path.Path();
			_tcscpy(sWorkPath, m_sWorkDirectory);
		}else{
			GetCurrentDirectory(1024, sWorkPath);
		}
		m_sWorkDirectory	= sWorkPath;
		if(sWorkPath[_tcslen(sWorkPath)-1]!=_T('\\')) m_sWorkDirectory.Append(_T("\\"));

		if(_tcsstr(lpszCommand, _T(":\\")) || _tcsstr(lpszCommand, _T(":/"))){
			m_sCommand = lpszCommand;
		}else
		if(_tcsstr(lpszCommand, _T("\\")) || _tcsstr(lpszCommand, _T("/")) || _tcsstr(lpszCommand, _T("%"))){
			CFullPath full_path(lpszCommand);
			m_sCommand	= full_path.Path();
		}else{
			//m_sCommand.Format(_T("\"%s%s\""), m_sWorkDirectory, lpszCommand);
			m_sCommand = lpszCommand;
		}
	}

	if(lpszArgument) {
		m_sCommand.Append(_T(" "));
		m_sCommand.Append(lpszArgument);
	}

	m_pNext		= m_pCurrent;
	m_pCurrent	= this;
}

CRedirectExecute::~CRedirectExecute(void){
	ClearErrorToken();
	if(m_pCurrent == this){
		m_pCurrent	= m_pNext;
	}else{
		CRedirectExecute*	pNode	= m_pCurrent;
		while(pNode->m_pNext != this) pNode	= pNode->m_pNext;
		pNode->m_pNext	= m_pNext;
	}
}

BOOL CRedirectExecute::Run(void)
{
	HANDLE					hStdout_read	= NULL,
							hStdout_write	= NULL,
							hStdin_read		= NULL,
							hStdin_write	= NULL;
	PROCESS_INFORMATION		ProcessInfo;
	SECURITY_ATTRIBUTES		SecurityAttributes;
	STARTUPINFO				StartupInfo;
	BOOL					Success;
	DWORD					dwSleepCount = 0;

	// run
	m_bStopped		= TRUE;
	m_iErrorCode	= 0;

	//	Zero the structures.
	ZeroMemory( &StartupInfo,			sizeof( StartupInfo ));
	ZeroMemory( &ProcessInfo,			sizeof( ProcessInfo ));
	ZeroMemory( &SecurityAttributes,	sizeof( SecurityAttributes ));

	//	Create a pipe for the child's STDOUT.
	SecurityAttributes.nLength              = sizeof(SECURITY_ATTRIBUTES);
	SecurityAttributes.bInheritHandle       = TRUE;
	SecurityAttributes.lpSecurityDescriptor = NULL;

	Success = CreatePipe
		(
		&hStdout_read,			// address of variable for read handle
		&hStdout_write,			// address of variable for write handle
		&SecurityAttributes,	// pointer to security attributes
		0						// number of bytes reserved for pipe (use default size)
		);

	if ( !Success )
	{
		ShowLastError(_T("Error creating stdout pipe"));
		goto CREATE_PIPE_FAILED;
	}

	Success = CreatePipe
		(
		&hStdin_read,			// address of variable for read handle
		&hStdin_write,			// address of variable for write handle
		&SecurityAttributes,	// pointer to security attributes
		0						// number of bytes reserved for pipe (use default size)
		);

	if ( !Success )
	{
		ShowLastError(_T("Error creating stdin pipe"));
		goto CREATE_PIPE_FAILED;
	}

	//	Set up members of STARTUPINFO structure.
	StartupInfo.cb			= sizeof(STARTUPINFO);
	StartupInfo.dwFlags		= STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
	StartupInfo.wShowWindow = SW_HIDE;
	StartupInfo.hStdOutput	= hStdout_write;
	StartupInfo.hStdError	= hStdout_write;
	StartupInfo.hStdInput	= hStdin_read;
	m_hSTDIN				= hStdin_write;

	//	Create the child process.
	Success = CreateProcess
		( 
		NULL,					// pointer to name of executable module
		(LPTSTR)(LPCTSTR)m_sCommand,// command line 
		NULL,					// pointer to process security attributes
		NULL,					// pointer to thread security attributes (use primary thread security attributes)
		TRUE,					// inherit handles
		0,						// creation flags
		NULL,					// pointer to new environment block (use parent's)
		m_sWorkDirectory,		// pointer to current directory name
		&StartupInfo,			// pointer to STARTUPINFO
		&ProcessInfo			// pointer to PROCESS_INFORMATION
		);                 

	if ( !Success )
	{
		CString str;
		/*TCHAR line[MAX_PATH];
		GetCurrentDirectory(MAX_PATH, line);
		str.Format(_T("CurDir = \"%s\""), line);

		g_pTestDrive->LogOut(str, ITDSystem::SYSMSG_INFO);*/

		str.Format(_T("Command = \"%s\", WorkDir = \"%s\""), m_sCommand, m_sWorkDirectory);
		//ShowLastError(_T("Error creating process"));
		g_pTestDrive->LogOut(str, ITDSystem::SYSMSG_ERROR);
		goto CREATE_PROCESS_FAILED;
	}

	DWORD	BytesLeftThisMessage = 0;
	DWORD	NumBytesRead;
	TCHAR	PipeData[BUF_SIZE];
	char	chardump[BUF_SIZE];
	DWORD	TotalBytesAvailable = 0;

	m_bStopped		= FALSE;

	for ( ; ; )
	{ 
		NumBytesRead = 0;

		Success = PeekNamedPipe
			( 
			hStdout_read,				// handle to pipe to copy from 
			chardump,					// pointer to data buffer 
			1,							// size, in bytes, of data buffer 
			&NumBytesRead,				// pointer to number of bytes read 
			&TotalBytesAvailable,		// pointer to total number of bytes available
			&BytesLeftThisMessage		// pointer to unread bytes in this message 
			);

		if ( !Success )
		{
			ShowLastError(_T("PeekNamedPipe failed"));
			break;
		}

		if ( NumBytesRead )
		{
			Success = ReadFile
				(
				hStdout_read,		// handle to pipe to copy from 
				chardump,			// address of buffer that receives data
				(BUF_SIZE) - 1,		// number of bytes to read
				&NumBytesRead,		// address of number of bytes read
				NULL				// address of structure for data for overlapped I/O
				);

			if ( !Success )
			{
				ShowLastError(_T("ReadFile failed"));
				m_iErrorCode = 0x80000000;
				break;
			}


			chardump[NumBytesRead] = NULL;

			// Unicode 변환
			NumBytesRead = MultiByteToWideChar(CP_ACP, 0, chardump, NumBytesRead, NULL, NULL);
			MultiByteToWideChar(CP_ACP, 0, chardump, BUF_SIZE, PipeData, BUF_SIZE);

			//	Zero-terminate the data.
			PipeData[NumBytesRead] = _T('\0');

RETREAT_MES:
			//	If we're running a batch file that contains a pause command, 
			//	assume it is the last output from the batch file and remove it.
			TCHAR  *ptr = _tcsstr(PipeData, _T("Press any key to continue . . ."));
			if ( ptr ) *ptr = _T('\0');
			else{
				ptr = _tcsstr(PipeData, _T("$$"));
				if(ptr){
					*ptr = _T('\0');
					if(*PipeData) AppendText(PipeData);
					ptr += 2;
					if(_tcsstr(ptr,_T("CLS"))==ptr){
						m_pMsg->Clear();
						ptr+=3;
					}else
					if(_tcsstr(ptr,_T("APP"))==ptr){
						m_pMsg = &g_Output[COutput::TD_OUTPUT_APPLICATION];
						ptr+=3;
					}else
					if(_tcsstr(ptr,_T("SYS"))==ptr){
						m_pMsg = &g_Output[COutput::TD_OUTPUT_SYSTEM];
						ptr+=3;
					}else
					if(_tcsstr(ptr,_T("LINK"))==ptr){
						ptr+=4;
						TCHAR* pEnd = _tcsstr(ptr, _T(";;"));
						if(pEnd){
							LPCTSTR sDelim	= _T(";");
							COLORREF	cColor	= RGB(0,0,255);
							CString		sURL, sName;
							sURL	= _tcstok(ptr, sDelim);
							if(!sURL.IsEmpty()) sName	= _tcstok(NULL, sDelim);
							{
								CString sColor;
								if(!sName.IsEmpty()) sColor	= _tcstok(NULL, sDelim);
								if(!sColor.IsEmpty()){
									DWORD r,g,b;
									_stscanf(sColor, _T("%02X%02X%02X"), &r, &g, &b);
									cColor	= RGB(r,g,b);
								}
							}
							m_pMsg->LogLink(sURL, sName, cColor);

							ptr	= pEnd+2;
						}
					}
					_tcscpy(PipeData, ptr);
					goto RETREAT_MES;
				}
			}

			//	Append the output to the CEdit control.
			if(!m_bStopped) AppendText(PipeData);
			//	Peek and pump messages.
			PeekAndPump();
		}else{
			//	If the child process has completed, break out.
			if ( WaitForSingleObject(ProcessInfo.hProcess, 0) == WAIT_OBJECT_0 ) break;	//lint !e1924 (warning about C-style cast)

			//	Peek and pump messages.
			PeekAndPump();

			//	If the user canceled the operation, terminate the process.
			if ( m_bStopped )
			{
				Success = TerminateProcess(ProcessInfo.hProcess, 0);
				if ( Success )	AppendText(_T("\r\nProcess is terminated unnormally.\r\n"));
				else			ShowLastError(_T("Error terminating process."));
				//break;
			}
			//	Sleep.
			Sleep(m_dwSleepMilliseconds);
			if(m_bKillDebug){
				HWND hWnd = FindWindow(NULL, _T("Visual Studio Just-In-Time Debugger"));
				if(hWnd){
					::SendMessage(hWnd, WM_CLOSE, NULL, NULL);
					m_iErrorCode = 0x80000000;
					{
						CString str;
						str.Format(_T("Process is terminated unnormally. : %s\n"), m_sCommand);
						m_pMsg->LogOut(str, RGB(255, 0, 0));
					}
				}
			}
		}
	}

	//	Close handles.
	Success = CloseHandle(ProcessInfo.hThread);		if ( !Success )	ShowLastError(_T("Error closing thread handle."));
	Success = CloseHandle(ProcessInfo.hProcess);	if ( !Success )	ShowLastError(_T("Error closing process handle."));
CREATE_PROCESS_FAILED:
	if(hStdout_read)	{Success = CloseHandle(hStdout_read);	if ( !Success ) ShowLastError(_T("Error closing pipe stdout read handle."));}
	if(hStdout_write)	{Success = CloseHandle(hStdout_write);	if ( !Success )	ShowLastError(_T("Error closing pipe stdout write handle."));}
	if(hStdin_read)		{Success = CloseHandle(hStdin_read);	if ( !Success ) ShowLastError(_T("Error closing pipe stdin read handle."));}
	if(hStdin_write)	{Success = CloseHandle(hStdin_write);	if ( !Success )	ShowLastError(_T("Error closing pipe stdin write handle."));}
	m_hSTDIN	= NULL;
CREATE_PIPE_FAILED:
	
	return (!m_bStopped);
}

void CRedirectExecute::Stop(void)
{
	m_bStopped = TRUE;
}

void CRedirectExecute::AddErrorToken(LPCTSTR error_token, DWORD_PTR dwID)
{
	for(;;){
		if(error_token[0]==_T('@')){
			error_token++;
			switch(error_token[0]){
				case _T('D'):	m_bKillDebug	= TRUE;				break;	// allow debug window
			}
			error_token++;
			continue;
		}
		break;
	}

	if((*error_token)){
		TokenLink* pToken = new TokenLink(error_token, dwID);
		if(m_pErrorTokenLink) m_pErrorTokenLink->AddLink(pToken);
		else m_pErrorTokenLink = pToken;
	}
}

void CRedirectExecute::ClearErrorToken(void){
	if(m_pErrorTokenLink){
		delete m_pErrorTokenLink;
		m_pErrorTokenLink	= NULL;
	}
}

int CRedirectExecute::GetErrorCode(void)
{
	return m_iErrorCode;
}

void CRedirectExecute::ShowLastError(LPCTSTR szText)
{
	LPVOID		lpMsgBuf;
	DWORD		Success;

	//	Get the system error message.
	Success = FormatMessage
		(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL,
		GetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),	//lint !e1924 (warning about C-style cast)
		LPTSTR(&lpMsgBuf),
		0,
		NULL
		);

	CString	Msg;

	Msg = szText;
	Msg += _T("\r\n");
	if ( Success )	Msg += LPTSTR(lpMsgBuf);
	else			Msg += _T("No status because FormatMessage failed.\r\n");

	AppendText(Msg);
}

void CRedirectExecute::PeekAndPump()
{
	MSG Msg;

	while (::PeekMessage(&Msg, NULL, 0, 0, PM_NOREMOVE)) 
	{
		// MFC에서는 이걸로 사용한다.
		//(void)AfxGetApp()->PumpMessage(); //lint !e1924 (warning about C-style cast)
		
		// dummy message handler
		BOOL bRet = ::GetMessage(&Msg, NULL, 0, 0);
		if(bRet == -1)
		{
			//TRACE(atlTraceUI, 0, _T("::GetMessage returned -1 (error)\n"));
			continue;   // error, don't process
		}
		else if(!bRet)
		{
			//TRACE(atlTraceUI, 0, _T("CMessageLoop::Run - exiting\n"));
			break;	   // WM_QUIT, exit message loop
		}

		if(Msg.message == WM_KEYDOWN && !(Msg.wParam&0xFF80)){
			SendKey((char)Msg.wParam);
		}

		//if(!theApp.PreTranslateMessage(&Msg))
		if (!g_pMainFrame->PreTranslateMessage(&Msg))
		{
			::TranslateMessage(&Msg);
			::DispatchMessage(&Msg);
		}
	}
}

void CRedirectExecute::AppendText(LPCTSTR Text)
{
	if(!m_pErrorTokenLink){
		if(m_fLogFunc){
			const char* sUserCommand	= m_fLogFunc(Text, 0);
			if(sUserCommand && m_hSTDIN){	// 사용자 command 전달하기
				DWORD	dwLen	= (DWORD)strlen(sUserCommand);
				DWORD	dwLenWritten;
				while(dwLen){
					WriteFile(m_hSTDIN, sUserCommand, dwLen, &dwLenWritten, NULL);
					dwLen	-= dwLenWritten;
				}
			}
		}
		else m_pMsg->LogOut(Text);
	}else{
		TCHAR line[BUF_SIZE];
		TCHAR msg[BUF_SIZE];
		TokenLink*	pToken;
		_tcscpy(line, Text);
		TCHAR *strStart	= line;
		TCHAR *strEnd	= NULL;
		TCHAR *pStr		= NULL;
		while(strStart){
			*msg = NULL;
			strEnd	= _tcsstr(strStart, _T("\n"));
			if(strEnd) *strEnd	= NULL;
			if(*strStart) _tcscpy(msg, strStart);
			if(strEnd) _tcscat(msg, _T("\n"));

			pToken = m_pErrorTokenLink;
			while(pToken){
				pStr = _tcsstr(strStart, pToken->GetParamToken());
				if(pStr){
					if(pToken->IsParamterize()){
						void* pID = (void*)(pToken->GetID());
						_stscanf(pStr, pToken->GetToken(), pID);
						if(m_fLogFunc) m_fLogFunc(msg, (int)pID);	// void* 에서 int 로 포인트 잘림
						else{
							m_pMsg->LogOut(msg, RGB(0, 0, 255));
						}
					}else{
						int iID = (int)pToken->GetID();
						if (iID != INT_MAX) {
							// 음수(에러코드) 우선
							if (!m_iErrorCode) m_iErrorCode = iID;
							else if (iID < 0) {
								if (iID < m_iErrorCode)	m_iErrorCode = iID;
							}
							else if (iID > 0) {
								if (m_iErrorCode > 0 && iID > m_iErrorCode)	m_iErrorCode = iID;
							}
						}

						if(m_fLogFunc) m_fLogFunc(msg, iID);
						else{
							DWORD dwColor = (iID == INT_MAX) ? RGB(255,85,0) : (iID < 0 ? RGB(255, 0, 0) : RGB(0, 0, 255));
							m_pMsg->LogOut(msg, dwColor);
							g_Output[COutput::TD_OUTPUT_NOTIFY].LogOut(msg, dwColor);
						}
					}
					break;
				}
				pToken = pToken->GetNext();
			}
			if(!pToken){
				if(m_fLogFunc) m_fLogFunc(msg, 0);
				else m_pMsg->LogOut(msg);
			}

			if(!strEnd) break;
			strStart = strEnd + 1;
		}
	}
}

void CRedirectExecute::SetSleepInterval(DWORD dwMilliseconds)
{
	m_dwSleepMilliseconds = dwMilliseconds;
}

void CRedirectExecute::SetLogFunction(TD_REDIRECT_LOG func){
	m_fLogFunc	= func;
}

BOOL CRedirectExecute::SendKey(char ch){
	if(m_hSTDIN){
		DWORD	dwLenWritten	= 0;
		while(!dwLenWritten)
			WriteFile(m_hSTDIN, &ch, 1, &dwLenWritten, NULL);
		return TRUE;
	}
	return FALSE;
}
