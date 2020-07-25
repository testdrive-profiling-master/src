#include "stdafx.h"
#include "WMIConnection.h"

WMIConnection::WMIConnection(void){
	m_bInitialized	= FALSE;	
}

WMIConnection::~WMIConnection(void){
	Close();
}

void WMIConnection::Open(const BSTR bstrNetworkResource,
	const BSTR bstrUser /*= NULL*/, 
	const BSTR strPassword /*= NULL*/)
{    
	Close();

	_InitSecurity();
	_InitIWbemLocator();

	// Create a connection to WMI namespace
	// http://msdn.microsoft.com/en-us/library/windows/desktop/aa389749(v=vs.85).aspx

	HRESULT hr = m_pIWbemLocator->ConnectServer(bstrNetworkResource,
		bstrUser, strPassword, 0, NULL, 0, 0, &m_pIWbemServices);

	ATLENSURE_SUCCEEDED(hr);

	_InitSecurityLevels();
}

void WMIConnection::Close(void)
{
	if(m_bInitialized){
		if(NULL != m_pIWbemServices)
			m_pIWbemServices.Release();

		if(NULL != m_pIWbemLocator)
			m_pIWbemLocator.Release();
		
		CoUninitialize();
		m_bInitialized	= FALSE;
	}
}

HRESULT WMIConnection::ExecQuery(const BSTR bstrQueryLanguage, const BSTR bstrQuery, 
	IEnumWbemClassObject** pIEnumWbemClassObject)
{
	return m_pIWbemServices->ExecQuery(bstrQueryLanguage,
		bstrQuery,
		WBEM_FLAG_FORWARD_ONLY|WBEM_FLAG_RETURN_IMMEDIATELY,
		NULL,
		pIEnumWbemClassObject);
}

void WMIConnection::_InitSecurity()
{
	CoInitializeEx(0, COINIT_MULTITHREADED);
	// Set the default process security level 
	// http://msdn.microsoft.com/en-us/library/windows/desktop/aa393617(v=vs.85).aspx
	HRESULT hr =  ::CoInitializeSecurity(
		NULL,                        // Security descriptor    
		-1,                          // COM negotiates authentication service
		NULL,                        // Authentication services
		NULL,                        // Reserved
		RPC_C_AUTHN_LEVEL_DEFAULT,   // Default authentication level for proxies
		RPC_C_IMP_LEVEL_IMPERSONATE, // Default Impersonation level for proxies
		NULL,                        // Authentication info
		EOAC_NONE,                   // Additional capabilities of the client or server
		NULL);                       // Reserved
	m_bInitialized	= TRUE;

	// ATLENSURE_SUCCEEDED(hr);
}

void WMIConnection::_InitIWbemLocator()
{
	// Initialize the IWbemLocator interface
	// http://msdn.microsoft.com/en-us/library/windows/desktop/aa389749(v=vs.85).aspx

	HRESULT hr = ::CoCreateInstance(CLSID_WbemLocator, 0, 
		CLSCTX_INPROC_SERVER, IID_IWbemLocator, (LPVOID*)&m_pIWbemLocator);

	ATLENSURE_SUCCEEDED(hr);
}

void WMIConnection::_InitSecurityLevels()
{
	// Set the security levels on WMI connection
	// http://msdn.microsoft.com/en-us/library/windows/desktop/aa393619(v=vs.85).aspx
	HRESULT hr = ::CoSetProxyBlanket(
		m_pIWbemServices, 
		RPC_C_AUTHN_WINNT,
		RPC_C_AUTHZ_NONE, 
		NULL, 
		RPC_C_AUTHN_LEVEL_CALL,
		RPC_C_IMP_LEVEL_IMPERSONATE,
		NULL,
		EOAC_NONE);

	ATLENSURE_SUCCEEDED(hr);
}
