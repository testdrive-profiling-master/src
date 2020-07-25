#include "stdafx.h"
#include "WMIQuery.h"

WMIQuery::WMIQuery(WMIConnection& connection, 
	const BSTR bstrQuery,  
	const BSTR bstrQueryLanguage /*= L"WQL"*/)
	: m_connection(connection),
	m_bstrQuery(bstrQuery),
	m_bstrQueryLanguage(bstrQueryLanguage)
{
}


WMIQuery::~WMIQuery(void){
}

void WMIQuery::Open(void)
{
	Close(); // close if already open

	HRESULT hr = m_connection.ExecQuery(
		m_bstrQueryLanguage,          // Query language
		m_bstrQuery,                  // Query
		&m_pIEnumWbemClassObject);    // Enumerator

	ATLENSURE_SUCCEEDED(hr);
}

void WMIQuery::Close(void)
{
	if(NULL != m_pIEnumWbemClassObject)
		m_pIEnumWbemClassObject.Release();

	if(NULL != m_pIWbemClassObject)
		m_pIWbemClassObject.Release();
}

BOOL WMIQuery::MoveNext()
{
	BOOL bRet = FALSE;

	if(NULL != m_pIWbemClassObject)
		m_pIWbemClassObject.Release();

	if(NULL != m_pIEnumWbemClassObject)
	{
		ULONG uReturn = 0;
		HRESULT hr = m_pIEnumWbemClassObject->Next(WBEM_INFINITE, 
			1, &m_pIWbemClassObject, &uReturn);

		switch(hr)
		{
		case WBEM_S_NO_ERROR:
			bRet = TRUE;
			break;
		case WBEM_S_FALSE:
			bRet = FALSE;
			break;
		default:
			AtlThrow(hr);
		}
	}
	return bRet;
}

void WMIQuery::GetProperty(const BSTR bstrName, variant_t& vtValue, CIMTYPE& type) const
{
	ATLASSERT(NULL != m_pIWbemClassObject);
	HRESULT hr = m_pIWbemClassObject->Get(bstrName, 0, &vtValue, &type, 0);
	ATLENSURE_SUCCEEDED(hr);
}

CString WMIQuery::GetPropertyStr(const BSTR bstrName)
{
	ATLASSERT(NULL != m_pIWbemClassObject);
	CString strRet;
	variant_t vtValue;
	CIMTYPE cimType;
	HRESULT hr = m_pIWbemClassObject->Get(bstrName, 0, &vtValue, &cimType, 0);
	if(SUCCEEDED(hr) && (vtValue.vt != VT_NULL) && (vtValue.vt != VT_EMPTY))
	{
		switch(cimType)
		{
		case CIM_EMPTY:
			break;
		case CIM_SINT8:
			strRet.Format(_T("%c"), vtValue.cVal);
			break;
		case CIM_UINT8:
			strRet.Format(_T("%c"), vtValue.bVal);
			break;
		case CIM_SINT16:
			strRet.Format(_T("%d"), vtValue.iVal);
			break;
		case CIM_UINT16:
			strRet.Format(_T("%u"), vtValue.uiVal);
			break;
		case CIM_SINT32:
			strRet.Format(_T("%d"), vtValue.intVal);
			break;
		case CIM_UINT32:
			strRet.Format(_T("%u"), vtValue.uintVal);
			break;
		case CIM_SINT64:
			strRet = vtValue.bstrVal; // strRet.Format(_T("%I64d"), vtValue.intVal);
			break;
		case CIM_UINT64:
			strRet = vtValue.bstrVal; // strRet.Format(_T("%I64u"), vtValue.intVal);
			break;
		case CIM_REAL32:
			strRet.Format(_T("%.4f"), vtValue.fltVal);
			break;
		case CIM_REAL64:
			strRet.Format(_T("%f"), vtValue.dblVal);
			break;
		case CIM_BOOLEAN:
			strRet = (vtValue.boolVal) ? _T("TRUE") : _T("FALSE");
			break;
		case CIM_STRING:
			strRet = vtValue.bstrVal;
			break;
		case CIM_DATETIME:
			strRet = _WMIDatetimeToString(vtValue.bstrVal);
			break;
		default:
			ATLTRACE("Unhandled CIM type");
			break;
		}
	}
	return strRet;
}

DWORD WMIQuery::GetPropertyInt(const BSTR bstrName){
	ATLASSERT(NULL != m_pIWbemClassObject);
	variant_t vtValue;
	CIMTYPE cimType;
	HRESULT hr = m_pIWbemClassObject->Get(bstrName, 0, &vtValue, &cimType, 0);
	if(SUCCEEDED(hr) && (vtValue.vt != VT_NULL) && (vtValue.vt != VT_EMPTY))
	{
		switch(cimType)
		{
		case CIM_SINT8:
			return (DWORD)vtValue.cVal;
		case CIM_UINT8:
			return (DWORD)vtValue.bVal;
		case CIM_SINT16:
			return (DWORD)vtValue.iVal;
		case CIM_UINT16:
			return (DWORD)vtValue.uiVal;
		case CIM_SINT32:
			return (DWORD)vtValue.intVal;
		case CIM_UINT32:
			return (DWORD)vtValue.uintVal;
		case CIM_SINT64:
		case CIM_UINT64:
		case CIM_STRING:
			return (DWORD)_tstol(vtValue.bstrVal);
		case CIM_BOOLEAN:
			return (vtValue.boolVal) ? 1 : 0;
		default:
			//_tprintf(_T("type : %d"), cimType);
			//ATLTRACE("Unhandled CIM type");
			break;
		}
	}
	return (DWORD)-1;
}

ULONG64 WMIQuery::GetPropertyInt64(const BSTR bstrName){
	ATLASSERT(NULL != m_pIWbemClassObject);
	variant_t vtValue;
	CIMTYPE cimType;
	HRESULT hr = m_pIWbemClassObject->Get(bstrName, 0, &vtValue, &cimType, 0);
	if(SUCCEEDED(hr) && (vtValue.vt != VT_NULL) && (vtValue.vt != VT_EMPTY))
	{
		switch(cimType)
		{
		case CIM_SINT8:
			return (ULONG64)vtValue.cVal;
		case CIM_UINT8:
			return (ULONG64)vtValue.bVal;
		case CIM_SINT16:
			return (ULONG64)vtValue.iVal;
		case CIM_UINT16:
			return (ULONG64)vtValue.uiVal;
		case CIM_SINT32:
			return (ULONG64)vtValue.intVal;
		case CIM_UINT32:
			return (ULONG64)vtValue.uintVal;
		case CIM_SINT64:
		case CIM_UINT64:
		case CIM_STRING:
			return (ULONG64)_tstol(vtValue.bstrVal);
		case CIM_BOOLEAN:
			return (vtValue.boolVal) ? 1 : 0;
		default:
			break;
		}
	}
	return (ULONG64)-1;
}

CString WMIQuery::_WMIDatetimeToString(const BSTR bstrIn)
{
	CString strValue;

	CComPtr<ISWbemDateTime> pISWbemDateTime;
	HRESULT hr = CoCreateInstance(__uuidof(SWbemDateTime), 0,
		CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pISWbemDateTime));

	if(SUCCEEDED(hr))
	{
		hr = pISWbemDateTime->put_Value(bstrIn);
		LONG lYear = 0, lMonth = 0, lDay = 0, lHour = 0, lMinutes = 0, lSeconds = 0;
		hr = pISWbemDateTime->get_Year(&lYear);
		hr = pISWbemDateTime->get_Month(&lMonth);
		hr = pISWbemDateTime->get_Day(&lDay);
		hr = pISWbemDateTime->get_Hours(&lHour);
		hr = pISWbemDateTime->get_Minutes(&lMinutes);
		hr = pISWbemDateTime->get_Seconds(&lSeconds);

		COleDateTime odt(lYear, lMonth, lDay, lHour, lMinutes, lSeconds);
		strValue = odt.Format();
	}
	return strValue;
}