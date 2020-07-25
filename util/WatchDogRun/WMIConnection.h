#pragma once

class WMIConnection
{
public:
	WMIConnection(void);
	virtual ~WMIConnection(void);

	void Open(const BSTR bstrNetworkResource, const BSTR bstrUser = NULL, const BSTR strPassword = NULL);
	void Close(void);
	HRESULT ExecQuery(const BSTR bstrQueryLanguage, const BSTR bstrQuery, IEnumWbemClassObject** pIEnumWbemClassObject);

private:
	void _InitSecurity();
	void _InitIWbemLocator();
	void _InitSecurityLevels();

private:
	CComPtr<IWbemLocator> m_pIWbemLocator;
	CComPtr<IWbemServices> m_pIWbemServices;
	BOOL	m_bInitialized;
};