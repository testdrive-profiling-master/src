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
	// Ư�� ���α׷��� ������ �������� ���� �ϱ� ���� �Լ�
	BOOL    ShellExecWithElevation(HWND hWnd, LPCTSTR lpPath, LPCTSTR lpParameters, LPCTSTR lpDirectory);

	// �ڱ� �ڽ��� ������ �������� �ٽ� ���� �ϴ� �Լ�
	BOOL    SelfExecuteWithElevation();

	// Ư�� Url �� ������ �������� Internet Explorer �� ���� �ϴ� �Լ�
	BOOL    OpenUrlWithElevation(HWND hWnd, LPCTSTR lpUrl);

	// ���� ����� �ʿ����� ����, 1 - UAC ������ OS, 2 - ���� ���� ��ȸ �Ұ�, 3 - ���� ��� �ʿ�, 4 - ���� ��� �Ǿ� ����
	int     GetNeedElevate();

	// OS�� Vista ���� ���� ���� ����
	BOOL    IsVistaOrHigher(void);

	// ���� üũ
	HRESULT IsElevated(BOOL *pElevated);

private:    
	// ������ �������� ���� �ϱ� ���� ���� ���� �Լ�,ShellExecWithElevation �� ����ϵ��� ����
	BOOL    ShellExecWithVerb(HWND hWnd, LPCTSTR lpVerb, LPCTSTR lpPath, LPCTSTR lpParameters, LPCTSTR lpDirectory);
};
