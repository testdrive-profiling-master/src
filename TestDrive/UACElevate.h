#pragma once
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <shellapi.h>
#include <shlobj.h>

class UACElevate  
{
public:
	UACElevate();
	virtual ~UACElevate();

public:
	// 특정 프로그램을 관리자 권한으로 실행 하기 위한 함수
	BOOL    ShellExecWithElevation(HWND hWnd, LPCTSTR lpPath, LPCTSTR lpParameters, LPCTSTR lpDirectory);

	// 자기 자신을 관리자 권한으로 다시 시작 하는 함수
	BOOL    SelfExecuteWithElevation();

	// 특정 Url 을 관리자 권한으로 Internet Explorer 를 실행 하는 함수
	BOOL    OpenUrlWithElevation(HWND hWnd, LPCTSTR lpUrl);

	// 권한 상승이 필요한지 여부, 1 - UAC 미지원 OS, 2 - 상태 정보 조회 불가, 3 - 권한 상승 필요, 4 - 권한 상승 되어 있음
	int     GetNeedElevate();

	// OS가 Vista 보다 상위 인지 여부
	BOOL    IsVistaOrHigher(void);

	// 권한 체크
	HRESULT IsElevated(BOOL *pElevated);

private:    
	// 관리자 권한으로 실행 하기 위한 가장 기초 함수,ShellExecWithElevation 를 사용하도록 권장
	BOOL    ShellExecWithVerb(HWND hWnd, LPCTSTR lpVerb, LPCTSTR lpPath, LPCTSTR lpParameters, LPCTSTR lpDirectory);
};
