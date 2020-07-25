// PropertyGridCtrl.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "TestDriveImp.h"
#include "PropertyGridCtrl.h"
#include "DocumentCtrl.h"
#include "NetworkAdapter.h"

CPropertyGridCtrl	g_PropertyGrid;

CPropertyDataBase::CPropertyDataBase(DWORD dwID){
	m_iID				= dwID;
	m_pGridProperty		= NULL;
}
CPropertyDataBase::~CPropertyDataBase(void){
}

void CPropertyDataBase::Show(BOOL bShow){
	m_pGridProperty->Show(bShow);
}

void CPropertyDataBase::Enable(BOOL bEnable){
	m_pGridProperty->Enable(bEnable);
}

void CPropertyDataBase::AllowEdit(BOOL bAllow){
	m_pGridProperty->AllowEdit(bAllow);
}

BOOL CPropertyDataBase::AddOption(LPCTSTR lpszOption){
	if(!lpszOption)	m_pGridProperty->RemoveAllOptions();
	else return m_pGridProperty->AddOption(lpszOption);
	return TRUE;
}

int CPropertyDataBase::GetOptionCount(void){
	return m_pGridProperty->GetOptionCount();
}

LPCTSTR CPropertyDataBase::GetOption(int iIndex){
	return m_pGridProperty->GetOption(iIndex);
}

LPCTSTR CPropertyDataBase::GetFormat(void){
	m_Format	= m_pGridProperty->FormatProperty();
	return (LPCTSTR)m_Format;
}

PROPERTY_TYPE CPropertyDataBase::GetType(void){
	COleVariant	rVariant	= m_pGridProperty->GetValue();
	VARIANT var				= rVariant.Detach();
	switch(var.vt){
	case VT_BSTR:	return PROPERTY_TYPE_STRING;
	case VT_I4:
	case VT_INT:	return PROPERTY_TYPE_INT;
	case VT_R4:		return PROPERTY_TYPE_FLOAT;
	case VT_R8:		return PROPERTY_TYPE_DOUBLE;
	case VT_BOOL:	return PROPERTY_TYPE_BOOL;
	}
	return PROPERTY_TYPE_NONE;
}

DWORD CPropertyDataBase::GetID(void){
	return m_iID;
}

LPCTSTR CPropertyDataBase::GetName(void){
	return m_pGridProperty->GetName();
}

DWORD_PTR CPropertyDataBase::GetData(void){
	UpdateData(TRUE);
	COleVariant	rVariant	= m_pGridProperty->GetValue();
	VARIANT var				= rVariant.Detach();
	if(var.vt == VT_BSTR) return (DWORD_PTR)var.bstrVal;

	return m_pGridProperty->GetData();
}

void CPropertyDataBase::UpdateData(BOOL bUpdate){
	COleVariant	rVariant	= m_pGridProperty->GetValue();
	VARIANT		var			= rVariant.Detach();
	DWORD_PTR	pData		= m_pGridProperty->GetData();
	switch(var.vt){
	case VT_BSTR:
		if(bUpdate)		_tcscpy((LPTSTR)pData, var.bstrVal);
		else rVariant.SetString((LPTSTR)pData, VT_BSTR);
		break;
	case VT_I4:
	case VT_INT:
		if(bUpdate) *((int*)pData)	= (int)var.lVal;
		else var.lVal	= (long)*((int*)pData);
		break;
	case VT_R4:
		if(bUpdate) *((float*)pData)	= var.fltVal;
		else var.fltVal	= *((float*)pData);
		break;
	case VT_R8:
		if(bUpdate) *((double*)pData)	= var.dblVal;
		else var.dblVal	= *((double*)pData);
		break;
	case VT_BOOL:

		if(bUpdate) *((BOOL*)pData)	= (var.boolVal == VARIANT_TRUE);
		else var.boolVal = *((BOOL*)pData) ? VARIANT_TRUE : VARIANT_FALSE;
		break;
	}

	if(!bUpdate){
		if(var.vt != VT_BSTR) rVariant.Attach(var);
		m_pGridProperty->SetValue(rVariant);
	}
}

void CPropertyDataBase::UpdateConfigFile(BOOL bUpdate){
	COleVariant	rVariant	= m_pGridProperty->GetValue();
	VARIANT		var			= rVariant.Detach();
	DWORD_PTR	pData		= m_pGridProperty->GetData();
	CDocumentView*	pDoc	= (CDocumentView*)(m_pGridProperty->GetParent()->GetData());

	switch(var.vt){
	case VT_BSTR:
		if(bUpdate){
			// read
			{
				CString str;
				GetConfigString(&str, (LPCTSTR)pData);
				_tcscpy((LPTSTR)pData, str);
			}
		}else{
			// write
			SetConfigString((LPCTSTR)pData);
		}
		break;
	case VT_I4:
	case VT_INT:
		if(bUpdate)		(*(int*)pData) = GetConfigInt((*(int*)pData));
		else			SetConfigInt((*(int*)pData));
		break;
	case VT_R4:
		{
			CString str;
			if(bUpdate){
				CString sDefault;
				sDefault.Format(_T("0x%X"), (*(DWORD*)pData));
				GetConfigString(&str, sDefault);
				_stscanf(str, _T("0x%X"), (DWORD*)pData);
			}else{
				str.Format(_T("0x%X"), (*(DWORD*)pData));
				SetConfigString(str);
			}
		}
		break;
	case VT_R8:
		{
			CString str;
			if(bUpdate){
				CString sDefault;
				sDefault.Format(_T("0x%llX"), (*(LONGLONG*)pData));
				GetConfigString(&str, sDefault);
				_stscanf(str, _T("0x%llX"), (LONGLONG*)pData);
			}else{
				str.Format(_T("0x%llX"), (*(LONGLONG*)pData));
				SetConfigString(str);
			}
		}
		break;
	case VT_BOOL:
		if(bUpdate)		(*(BOOL*)pData) = (BOOL)GetConfigInt((int)(*(BOOL*)pData));
		else			SetConfigInt((int)(*(BOOL*)pData));
		break;
	}

	if(bUpdate) UpdateData(FALSE);
}

void CPropertyDataBase::GetConfigString(CString *lpszStr, LPCTSTR lpszDefault){
	if(!lpszStr) return;
	TCHAR sStr[2048];
	((CDocumentView*)(m_pGridProperty->GetParent()->GetData()))->GetConfigString(m_pGridProperty->GetName(), sStr, 2048, lpszDefault);
	*lpszStr	= sStr;
}

void CPropertyDataBase::SetConfigString(LPCTSTR lpszStr){
	((CDocumentView*)(m_pGridProperty->GetParent()->GetData()))->SetConfigString(m_pGridProperty->GetName(), lpszStr);
}

int CPropertyDataBase::GetConfigInt(int default_value){
	return ((CDocumentView*)(m_pGridProperty->GetParent()->GetData()))->GetConfigInt(m_pGridProperty->GetName(), default_value);
}

void CPropertyDataBase::SetConfigInt(int set_value){
	((CDocumentView*)(m_pGridProperty->GetParent()->GetData()))->SetConfigInt(m_pGridProperty->GetName(), set_value);
}

//-----------------------------------------------------------------------------------
CPropertyData::CPropertyData(DWORD dwID, const CString& strName, const COleVariant& varValue, LPCTSTR lpszDescr, DWORD_PTR dwData) :
	CPropertyDataBase(dwID),
	CMFCPropertyGridProperty(strName, varValue, lpszDescr, dwData)
{
	m_pGridProperty	= this;
}

CPropertyData::~CPropertyData(void){
}

//-----------------------------------------------------------------------------------
CPropertyDirectoryData::CPropertyDirectoryData(DWORD dwID, const CString& strName, const COleVariant& varValue, LPCTSTR lpszDescr, DWORD_PTR dwData) :
	CPropertyDataBase(dwID),
	CMFCPropertyGridFileProperty(strName, _T(""), dwData, lpszDescr)
{
	m_pGridProperty	= this;
	m_pGridProperty->SetValue(varValue);
}

CPropertyDirectoryData::~CPropertyDirectoryData(void){
}

//-----------------------------------------------------------------------------------
CPropertyFileData::CPropertyFileData(DWORD dwID, const CString& strName, const COleVariant& varValue, DWORD dwFlags, LPCTSTR lpszDefExt, LPCTSTR lpszFilter, LPCTSTR lpszDescr, DWORD_PTR dwData) :
CPropertyDataBase(dwID),
	CMFCPropertyGridFileProperty(strName, TRUE, _T(""), lpszDefExt, dwFlags, lpszFilter, lpszDescr, dwData)
{
	m_pGridProperty	= this;
	m_pGridProperty->SetValue(varValue);
}

CPropertyFileData::~CPropertyFileData(void){
}

//-----------------------------------------------------------------------------------
PWCODE	CPropertyPasswordData::m_PWCode = { FALSE, {0,0,0,0,0,0,0,0}, {0,0,0,0,0,0,0,0}, 0x1f1f};

CPropertyPasswordData::CPropertyPasswordData(DWORD dwID, const CString& strName, const COleVariant& varValue, LPCTSTR lpszDescr, DWORD_PTR dwData) :
	CPropertyData(dwID, strName, varValue, lpszDescr, dwData)
{
	if(!m_PWCode.bInitialized){
		CNetworkAdapterList	netAdapter;
		{	// 컴퓨터 이름으로 갱신
			TCHAR sComputerName[256];
			DWORD dwNameSize = 256;
			GetComputerName(sComputerName, &dwNameSize);
			for(DWORD i = 0;i<dwNameSize;i++){
				DWORD seed = (DWORD)sComputerName[i];
				srand(seed);
				for(int t=0;t<8;t++){
					m_PWCode.code_mul[t] += (BYTE)rand();
				}
			}
		}
		{	// 사용자 이름
			TCHAR sUserName[256];
			DWORD dwNameSize = 256;
			GetUserName(sUserName, &dwNameSize);
			for(DWORD i = 0;i<dwNameSize;i++){
				DWORD seed = (DWORD)sUserName[i];
				srand(seed);
				for(int t=0;t<8;t++){
					m_PWCode.code_mul[t] += (BYTE)rand();
				}
			}
		}
		for(int t=0;t<8;t++){
			if(!m_PWCode.code_mul[t]) m_PWCode.code_mul[t] = 0xf1;
			m_PWCode.code_last *= m_PWCode.code_mul[t];
		}
		{	// mac 주소
			if(netAdapter.IsValid()){
				DWORD dwCount = netAdapter.GetCount();
				if(dwCount>0){
					ADAPTERINFO* pAdapters = new ADAPTERINFO[dwCount];
					LPCTSTR sMac		= (LPCTSTR)(pAdapters->MAC);
					DWORD dwNameSize	= pAdapters->MAC.GetLength();
					for(DWORD i = 0;i<dwNameSize;i++){
						DWORD seed = (DWORD)sMac[i];
						srand(seed);
						for(int t=0;t<8;t++){
							m_PWCode.code_add[t] += (BYTE)rand();
						}
					}
				}else goto NO_MAC_ADDRESS;
			}else{
NO_MAC_ADDRESS:
				for(int t=0;t<8;t++){
					m_PWCode.code_add[t] += (BYTE)rand();
				}
			}
			for(int t=0;t<8;t++){
				m_PWCode.code_last += m_PWCode.code_add[t];
			}
		}
		m_PWCode.bInitialized	= TRUE;
	}
}

CPropertyPasswordData::~CPropertyPasswordData(void){
}

CString CPropertyPasswordData::PasswordEncode(CString str){
	LPCTSTR	pStr	= (LPCTSTR)str;
	int		iLen	= str.GetLength();
	CString	pw;
	DWORD	code_check = 0;

	for(int i=0;i<iLen;i++){
		DWORD	code = (DWORD)(pStr[i])*m_PWCode.code_mul[i%8] + m_PWCode.code_add[i%8] - m_PWCode.code_last;
		code_check	+= (code - (DWORD)(pStr[i]*(i+1)));
		pw.AppendFormat(_T("%08X"), code);
	}
	pw.AppendFormat(_T("%08X"), code_check);

	return pw;
}

CString CPropertyPasswordData::PasswordDecode(CString str){
	LPCTSTR	pStr	= (LPCTSTR)str;
	int		iLen	= str.GetLength();
	CString	pw;
	if(!(iLen%8) && iLen > 8){
		CString		tok;
		DWORD		code_check;
		iLen	-= 8;
		_stscanf((pStr+iLen), _T("%08X"), &code_check);

		iLen /= 8;
		for(int i=0;i<iLen;i++){
			DWORD		code;
			_stscanf(pStr, _T("%08X"), &code);
			pStr += 8;
			code_check	-= code;
			code	= (code + m_PWCode.code_last - m_PWCode.code_add[i%8]) / m_PWCode.code_mul[i%8];
			code_check	+= (code*(i+1));
			pw.AppendChar((TCHAR)code);
		}
		if(code_check){
			// g_pTestDrive->LogWarning(_T("컴퓨터의 고유 식별 정보가 달라 password 를 해석할 수 없습니다."));
			pw.Empty();
		}
	}
	return pw;
}

void CPropertyPasswordData::UpdateConfigFile(BOOL bUpdate){
	COleVariant	rVariant	= GetValue();
	VARIANT		var			= rVariant.Detach();
	DWORD_PTR	pData		= CMFCPropertyGridProperty::GetData();
	CDocumentView*	pDoc	= (CDocumentView*)(GetParent()->GetData());

	switch(var.vt){
	case VT_BSTR:
		if(bUpdate){
			// read
			CString str;
			GetConfigString(&str, (LPCTSTR)pData);
			if(str.GetLength())
				_tcscpy((LPTSTR)pData, PasswordDecode(str));
		}else{
			// write
			CString str((LPCTSTR)pData);
			if(str.GetLength()){
				CString pw = PasswordEncode(str);
				SetConfigString(pw);
			}else{
				SetConfigString(_T(""));
			}
		}
		break;
	default:
		ASSERT(0);
		break;
	}

	if(bUpdate) UpdateData(FALSE);
}



CWnd* CPropertyPasswordData::CreateInPlaceEdit(CRect rectEdit, BOOL& bDefaultFormat){
	{
		CEdit* pWndEdit = new CEdit;

		DWORD dwStyle = WS_VISIBLE | WS_CHILD | ES_AUTOHSCROLL | ES_PASSWORD;

		if (!m_bEnabled || !m_bAllowEdit)
			dwStyle |= ES_READONLY;

		pWndEdit->Create(dwStyle, rectEdit, m_pWndList, AFX_PROPLIST_ID_INPLACE);
		pWndEdit->SetPasswordChar(_T('●'));

		bDefaultFormat = TRUE;
		return pWndEdit;
	}
}

CString CPropertyPasswordData::FormatProperty()
{
	CString strVal = (LPCTSTR)(_bstr_t)m_varValue;

	for (int i=0;i<strVal.GetLength();i++) strVal.SetAt(i, _T('●'));

	return strVal;
}

//---------------------------------------------------------------------------
// CPropertyGridCtrl
IMPLEMENT_DYNAMIC(CPropertyGridCtrl, CMFCPropertyGridCtrl)

CPropertyGridCtrl::CPropertyGridCtrl()
{

}

CPropertyGridCtrl::~CPropertyGridCtrl()
{
}


BEGIN_MESSAGE_MAP(CPropertyGridCtrl, CMFCPropertyGridCtrl)
END_MESSAGE_MAP()



// CPropertyGridCtrl 메시지 처리기입니다.
void CPropertyGridCtrl::OnPropertyChanged(CMFCPropertyGridProperty* pProp) const{
	ITDImplDocument* pImp	= ((CDocumentView*)(pProp->GetParent()->GetData()))->GetImplementation();
	CPropertyData* pPD		= (CPropertyData*)pProp;

	if (pImp && !pImp->OnPropertyUpdate(pPD)) pPD->UpdateData();

	CMFCPropertyGridCtrl::OnPropertyChanged(pProp);
}

void CPropertyGridCtrl::UpdateProperties(void){
	int count		= GetPropertyCount();

	for(int i=0;i<count;i++){
		CMFCPropertyGridProperty* pProp = GetProperty(i);
		CDocumentView* pView	= (CDocumentView*)pProp->GetData();
		CDocumentWnd* pWnd		= (CDocumentWnd*)pView->GetParent();
		pProp->Show(pProp->GetSubItemsCount() != 0);
	}
}
