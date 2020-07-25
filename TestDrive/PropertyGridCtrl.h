#pragma once
#include "DocumentWnd.h"
#include "TestDrive.h"

typedef struct{
	BOOL			bInitialized;
	BYTE			code_mul[8];
	BYTE			code_add[8];
	DWORD			code_last;
} PWCODE;

class CPropertyDataBase :
	public ITDPropertyData
{
public:
	CPropertyDataBase(DWORD dwID);
	virtual ~CPropertyDataBase(void);
	
	STDMETHOD_(void, Show)(BOOL bShow = TRUE);
	STDMETHOD_(void, Enable)(BOOL bEnable = TRUE);
	STDMETHOD_(void, AllowEdit)(BOOL bAllow = TRUE);
	STDMETHOD_(BOOL, AddOption)(LPCTSTR lpszOption);
	STDMETHOD_(int, GetOptionCount)(void);
	STDMETHOD_(LPCTSTR, GetOption)(int iIndex);
	STDMETHOD_(LPCTSTR, GetFormat)(void);
	STDMETHOD_(PROPERTY_TYPE, GetType)(void);
	STDMETHOD_(DWORD, GetID)(void);
	STDMETHOD_(LPCTSTR, GetName)(void);
	STDMETHOD_(DWORD_PTR, GetData)(void);
	STDMETHOD_(void, UpdateData)(BOOL bUpdate = TRUE);
	STDMETHOD_(void, UpdateConfigFile)(BOOL bUpdate = TRUE);

	void GetConfigString(CString *lpszStr, LPCTSTR lpszDefault);
	void SetConfigString(LPCTSTR lpszStr);
	int GetConfigInt(int default_value);
	void SetConfigInt(int set_value);

	inline CMFCPropertyGridProperty* GetObject(void)	{return m_pGridProperty;}

protected:
	DWORD						m_iID;
	CString						m_Format;
	CMFCPropertyGridProperty*	m_pGridProperty;
};
//-----------------------------------------------------------------------------------
class CPropertyData :
	public CPropertyDataBase,
	public CMFCPropertyGridProperty
{
public:
	CPropertyData(DWORD dwID, const CString& strName, const COleVariant& varValue, LPCTSTR lpszDescr = NULL, DWORD_PTR dwData = 0);
	virtual ~CPropertyData(void);
};
//-----------------------------------------------------------------------------------
class CPropertyDirectoryData :
	public CPropertyDataBase,
	public CMFCPropertyGridFileProperty
{
public:
	CPropertyDirectoryData(DWORD dwID, const CString& strName, const COleVariant& varValue, LPCTSTR lpszDescr = NULL, DWORD_PTR dwData = 0);
	virtual ~CPropertyDirectoryData(void);
};
//-----------------------------------------------------------------------------------
class CPropertyFileData :
	public CPropertyDataBase,
	public CMFCPropertyGridFileProperty
{
public:
	CPropertyFileData(DWORD dwID, const CString& strName, const COleVariant& varValue, DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, LPCTSTR lpszDefExt = NULL, LPCTSTR lpszFilter = NULL, LPCTSTR lpszDescr = NULL, DWORD_PTR dwData = 0);
	virtual ~CPropertyFileData(void);
};
//-----------------------------------------------------------------------------------
class CPropertyPasswordData :
	public CPropertyData
{
public:
	CPropertyPasswordData(DWORD dwID, const CString& strName, const COleVariant& varValue, LPCTSTR lpszDescr = NULL, DWORD_PTR dwData = 0);
	virtual ~CPropertyPasswordData(void);

	STDMETHOD_(void, UpdateConfigFile)(BOOL bUpdate = TRUE);

	// overrides
	virtual CWnd* CreateInPlaceEdit(CRect rectEdit, BOOL& bDefaultFormat);
	virtual CString FormatProperty();

	CString PasswordEncode(CString str);
	CString PasswordDecode(CString str);

protected:
	static	PWCODE		m_PWCode;
};
//-----------------------------------------------------------------------------------
// CPropertyGridCtrl
class CPropertyGridCtrl : public CMFCPropertyGridCtrl
{
	DECLARE_DYNAMIC(CPropertyGridCtrl)

public:
	CPropertyGridCtrl();
	virtual ~CPropertyGridCtrl();

	virtual void OnPropertyChanged(CMFCPropertyGridProperty* pProp) const;
	void UpdateProperties(void);

protected:
	DECLARE_MESSAGE_MAP()
};

extern CPropertyGridCtrl	g_PropertyGrid;