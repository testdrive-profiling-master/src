#pragma once
#include "WMIConnection.h"

class WMIQuery {
public:
	WMIQuery(WMIConnection& connection, const BSTR bstrQuery,  const BSTR bstrQueryLanguage = L"WQL");
	virtual ~WMIQuery(void);

	void Open(void);
	void Close(void);
	BOOL MoveNext(void);
	void GetProperty(const BSTR bstrName, variant_t& vtValue, CIMTYPE& type) const;
	CString GetPropertyStr(const BSTR bstrName);
	DWORD GetPropertyInt(const BSTR bstrName);
	ULONG64 GetPropertyInt64(const BSTR bstrName);

private:
	CString _WMIDatetimeToString(const BSTR bstrIn);

	WMIConnection& m_connection;
	const BSTR m_bstrQuery;
	const BSTR m_bstrQueryLanguage;
	CComPtr<IEnumWbemClassObject> m_pIEnumWbemClassObject;
	CComPtr<IWbemClassObject> m_pIWbemClassObject;
};