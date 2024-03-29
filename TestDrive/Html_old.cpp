#include "StdAfx.h"
#include "Html_old.h"
#include "TestDriveImp.h"
//#include "ExDispid.h" // IE5.5 이상이 지원 되는 헤더가 필요 합니다.

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNAMIC(CHtml_old, CHtmlView)
BEGIN_MESSAGE_MAP(CHtml_old, CHtmlView)
	ON_WM_MOUSEACTIVATE()
	ON_WM_DESTROY()
END_MESSAGE_MAP()


CHtml_old::CHtml_old(void) : m_pManager(NULL), m_dwID(0), m_bBlockNewWindow(FALSE)
{
}

CHtml_old::~CHtml_old(void)
{
	SetCurrentCopynPasteAction(FALSE);
	m_pManager	= NULL;
}

int CHtml_old::OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message){
	// bypass CView doc/frame stuff
	SetCurrentCopynPasteAction(TRUE);
	return CWnd::OnMouseActivate(pDesktopWnd, nHitTest, message);
}

void CHtml_old::OnDestroy(){
	CWnd::OnDestroy();
}

void CHtml_old::OnBeforeNavigate2(	LPCTSTR lpszURL, DWORD nFlags,
								LPCTSTR lpszTargetFrameName, CByteArray& baPostedData,
								LPCTSTR lpszHeaders, BOOL* pbCancel){
	if(m_pManager) lpszURL = m_pManager->OnHtmlBeforeNavigate(m_dwID, lpszURL);

	if(!lpszURL) *pbCancel = TRUE;
	else{
		CHtmlView::OnBeforeNavigate2(	lpszURL, nFlags,
										lpszTargetFrameName, baPostedData,
										lpszHeaders, pbCancel);
	}
}

void CHtml_old::OnDocumentComplete(LPCTSTR lpszURL){
	if(m_pManager) m_pManager->OnHtmlDocumentComplete(m_dwID, lpszURL);
}

// #include <mshtmdid.h>
// 
// BOOL CHtml_old::OnAmbientProperty(COleControlSite* pSite, DISPID dispid, VARIANT* pvar){
// 	if(pvar && dispid == DISPID_AMBIENT_DLCONTROL){
// 		// 스크립트 오류 메시지 안보이게 하기
// 		V_VT(pvar) = VT_I4;
// 
// 		V_I4(pvar) =
// 			DLCTL_DLIMAGES |
// 			DLCTL_VIDEOS |
// 			DLCTL_BGSOUNDS |
// 			DLCTL_NO_SCRIPTS | 0;
// 		return TRUE;
// 	}else{
// 		return CHtmlView::OnAmbientProperty(pSite, dispid, pvar);
// 	}
// }

void CHtml_old::SetManager(ITDHtmlManager* pManager, DWORD dwID){
	m_pManager	= pManager;
	m_dwID		= dwID;
}

BOOL CHtml_old::CallJScript(LPCTSTR lpszScript){
	HRESULT			hr;
	IDispatch*		pDispatch;
	IHTMLDocument2* pDocument;
	IHTMLWindow2*	pWindow;
	CComBSTR		lang = L"JScript";
	CComBSTR		code = lpszScript;

	if(pDispatch = GetHtmlDocument())
	{
		hr = pDispatch->QueryInterface(IID_IHTMLDocument2, (void**)&pDocument);

		if(SUCCEEDED(hr))
		{
			hr = pDocument->get_parentWindow(&pWindow);

			if(SUCCEEDED(hr))
			{
				VARIANT var;
				hr = pWindow->execScript(code, lang, &var);
				pWindow->Release();
			}

			pDocument->Release();
		}

		pDispatch->Release();
	}
	return SUCCEEDED(hr);
}

void CHtml_old::SetBlockNewWindow(BOOL bBlock){
	m_bBlockNewWindow = bBlock;
}

HRESULT CHtml_old::ExecFormCommand(const GUID *pGuid, long cmdID, long cmdExecOpt, VARIANT* pInVar, VARIANT* pOutVar) const
{ 
	CComQIPtr <IHTMLDocument2> spDoc (GetHtmlDocument ()); 
	HRESULT hr = E_FAIL; 

	if (spDoc) 
	{ 
		CComQIPtr <IOleCommandTarget> spCmdTarg = spDoc; 
		if (spCmdTarg) 
			hr = spCmdTarg-> Exec (pGuid, cmdID, cmdExecOpt, pInVar, pOutVar); 
		else 
			hr = E_NOINTERFACE; 
	}
	return hr; 
}

HRESULT CHtml_old::ExecFormCommand(long cmdID, long cmdExecOpt, VARIANT* pInVar, VARIANT* pOutVar) const
{ 
	return ExecFormCommand (&CGID_MSHTML, cmdID, cmdExecOpt, pInVar, pOutVar);
}

BOOL CHtml_old::CheckVisible(void){
	if(!IsWindowVisible()){
		SetCurrentCopynPasteAction(FALSE);
		return FALSE;
	}
	return TRUE;
}

void CHtml_old::OnAccel(ACCEL_CODE code){
	if(!CheckVisible()) return;
	switch(code){
	case ACCEL_CODE_COPY:		ExecFormCommand (IDM_COPY, OLECMDEXECOPT_DODEFAULT);		break;
	case ACCEL_CODE_PASTE:		ExecFormCommand (IDM_PASTE, OLECMDEXECOPT_DODEFAULT);		break;
	case ACCEL_CODE_CUT:		ExecFormCommand (IDM_CUT, OLECMDEXECOPT_DODEFAULT);			break;
	case ACCEL_CODE_UNDO:		ExecFormCommand (IDM_UNDO, OLECMDEXECOPT_DODEFAULT);		break;
	case ACCEL_CODE_SELECTALL:	ExecFormCommand (IDM_SELECTALL, OLECMDEXECOPT_DODEFAULT);	break;
	//case ACCEL_CODE_PRINT:		ExecFormCommand (IDM_PRINTPREVIEW, OLECMDEXECOPT_PROMPTUSER);	break;
	//case ACCEL_CODE_SAVE:		ExecFormCommand (IDM_SAVEAS, OLECMDEXECOPT_DODEFAULT);		break;
	/*case ACCEL_CODE_TOGGLE_FULLSCREEN:
		SetFullScreen(!GetFullScreen());	// ie 만 되는 컨트롤
		break;*/
	}
}

void CHtml_old::OnNewWindow2(LPDISPATCH* ppDisp, BOOL* Cancel){
	if(m_bBlockNewWindow){
		//*ppDisp = GetApplication();	// 새창 띄우기를 하지 않음.
		*Cancel	= TRUE;
		//Navigate2(m_sNaviURL);
	}else
	CHtmlView::OnNewWindow2(ppDisp, Cancel);
}

BOOL CHtml_old::PreTranslateMessage(MSG* pMsg) {
	if (pMsg->message == WM_KEYDOWN && (pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE))
	{
		::TranslateMessage(pMsg);
	}
	return CHtmlView::PreTranslateMessage(pMsg);
}

BOOL CHtml_old::PutText(CString strFormName, CString strObjectID, CString strPutText){
	CComPtr<IDispatch> pDisp			= NULL;
	CComPtr<IDispatch> pActiveDisp		= NULL;
	CComPtr<IHTMLDocument2> pDispDoc2	= NULL;
	CComPtr<IHTMLFormElement> pForm		= NULL;
	HRESULT hr;

	if(!(pDisp = GetHtmlDocument())){
		return FALSE;
	}
// 	hr = m_pWebBrowser2->get_Document(&pDisp);
// 	if(FAILED(hr) || !pDisp)
// 		return FALSE;

	hr = pDisp->QueryInterface(IID_IHTMLDocument2, (void**)&pDispDoc2);

	long nForms = 0;
	CComPtr<IHTMLElementCollection> pFormCollection=NULL;
	hr = pDispDoc2->get_forms(&pFormCollection);
	if(FAILED(hr) || !pFormCollection)
		return FALSE;

	hr = pFormCollection->get_length(&nForms);
	if(FAILED(hr))
		return FALSE;

	CComPtr<IDispatch> pDispForm = NULL;

	hr = pFormCollection->item(COleVariant(strFormName.AllocSysString(), VT_BSTR), COleVariant((long)0), &pDispForm);


	if(FAILED(hr) || !pDispForm)
	{
		BOOL bSuccess = FALSE;
		CComPtr<IOleContainer> pContainer = NULL;

		//Get the container
		hr = pDisp->QueryInterface(IID_IOleContainer, (void**)&pContainer);
		if (FAILED(hr) || !pContainer)
			return FALSE;

		CComPtr<IEnumUnknown> pEnumerator = NULL;
		//프레임 목록을 구한다
		hr = pContainer->EnumObjects(OLECONTF_EMBEDDINGS, &pEnumerator);
		if (FAILED(hr) || !pEnumerator)
			return FALSE;

		IUnknown* pUnk = NULL;
		ULONG uFetched = 0;

		//프레임목록을 순회
		for (UINT i = 0; S_OK == pEnumerator->Next(1, &pUnk, &uFetched); i++)
		{
			CComPtr<IWebBrowser2> pBrowser = NULL;

			hr = pUnk->QueryInterface(IID_IWebBrowser2, (void**)&pBrowser);

			if (SUCCEEDED(hr) && pBrowser)
			{

				CComPtr<IDispatch> pFrameDisp = NULL;
				CComPtr<IHTMLDocument2> pFrameDoc2 = NULL;
				CComPtr<IHTMLElementCollection> pFrameFormCollection=NULL;
				CComPtr<IDispatch> pFrameDispForm = NULL;

				hr = pBrowser->get_Document(&pFrameDisp);
				if(FAILED(hr) && !pFrameDisp)
					continue;
				hr = pFrameDisp->QueryInterface(IID_IHTMLDocument2, (void**)&pFrameDoc2);
				if(FAILED(hr) || !pFrameDoc2)
					continue;

				hr = pFrameDoc2->get_forms(&pFrameFormCollection);
				if(FAILED(hr) || !pFrameFormCollection)
					continue;

				hr = pFrameFormCollection->get_length(&nForms);
				if(FAILED(hr))
					continue;

				hr = pFrameFormCollection->item(COleVariant(strFormName.AllocSysString(), VT_BSTR), COleVariant((long)0), &pFrameDispForm);
				if(FAILED(hr) || !pFrameDispForm)
					continue;

				hr = pFrameDispForm->QueryInterface(IID_IHTMLFormElement, (void**)&pForm);
				if(FAILED(hr) || !pForm)
					continue;

				bSuccess = TRUE;
				break;
			}

			pUnk->Release();
			pUnk = NULL;
		}

		if(!bSuccess)
			return FALSE;
	}
	else
	{
		hr = pDispForm->QueryInterface(IID_IHTMLFormElement, (void**)&pForm);
		if(FAILED(hr) || !pForm)
			return FALSE;
	}

	if(!pForm)
		return FALSE;

	CComPtr<IHTMLInputTextElement> pInputElem = NULL;
	CComPtr<IDispatch> pDispInputForm = NULL;

	hr = pForm->item(COleVariant(strObjectID.AllocSysString()),COleVariant(long(0)),&pDispInputForm);
	if(FAILED(hr) || !pDispInputForm)
		return FALSE;
	
	hr = pDispInputForm->QueryInterface(IID_IHTMLInputTextElement, (void**)&pInputElem);
	if(FAILED(hr) || !pInputElem)
		return FALSE;

	CComBSTR bstrPutText(strPutText);
	pInputElem->select();
	pInputElem->put_value( bstrPutText );//텍스트 입력

	return TRUE;
}

BOOL CHtml_old::GetText(CString strFormName, CString strObjectID, CString& lpszText){
	CComPtr<IDispatch> pDisp			= NULL;
	CComPtr<IDispatch> pActiveDisp		= NULL;
	CComPtr<IHTMLDocument2> pDispDoc2	= NULL;
	CComPtr<IHTMLFormElement> pForm		= NULL;
	HRESULT hr;

	if(!(pDisp = GetHtmlDocument())){
		return FALSE;
	}
	// 	hr = m_pWebBrowser2->get_Document(&pDisp);
	// 	if(FAILED(hr) || !pDisp)
	// 		return FALSE;

	hr = pDisp->QueryInterface(IID_IHTMLDocument2, (void**)&pDispDoc2);

	long nForms = 0;
	CComPtr<IHTMLElementCollection> pFormCollection=NULL;
	hr = pDispDoc2->get_forms(&pFormCollection);
	if(FAILED(hr) || !pFormCollection)
		return FALSE;

	hr = pFormCollection->get_length(&nForms);
	if(FAILED(hr))
		return FALSE;

	CComPtr<IDispatch> pDispForm = NULL;

	hr = pFormCollection->item(COleVariant(strFormName.AllocSysString(), VT_BSTR), COleVariant((long)0), &pDispForm);


	if(FAILED(hr) || !pDispForm)
	{
		BOOL bSuccess = FALSE;
		CComPtr<IOleContainer> pContainer = NULL;

		//Get the container
		hr = pDisp->QueryInterface(IID_IOleContainer, (void**)&pContainer);
		if (FAILED(hr) || !pContainer)
			return FALSE;

		CComPtr<IEnumUnknown> pEnumerator = NULL;
		//프레임 목록을 구한다
		hr = pContainer->EnumObjects(OLECONTF_EMBEDDINGS, &pEnumerator);
		if (FAILED(hr) || !pEnumerator)
			return NULL;

		IUnknown* pUnk = NULL;
		ULONG uFetched = 0;

		//프레임목록을 순회
		for (UINT i = 0; S_OK == pEnumerator->Next(1, &pUnk, &uFetched); i++)
		{
			CComPtr<IWebBrowser2> pBrowser = NULL;

			hr = pUnk->QueryInterface(IID_IWebBrowser2, (void**)&pBrowser);

			if (SUCCEEDED(hr) && pBrowser)
			{

				CComPtr<IDispatch> pFrameDisp = NULL;
				CComPtr<IHTMLDocument2> pFrameDoc2 = NULL;
				CComPtr<IHTMLElementCollection> pFrameFormCollection=NULL;
				CComPtr<IDispatch> pFrameDispForm = NULL;

				hr = pBrowser->get_Document(&pFrameDisp);
				if(FAILED(hr) && !pFrameDisp)
					continue;
				hr = pFrameDisp->QueryInterface(IID_IHTMLDocument2, (void**)&pFrameDoc2);
				if(FAILED(hr) || !pFrameDoc2)
					continue;

				hr = pFrameDoc2->get_forms(&pFrameFormCollection);
				if(FAILED(hr) || !pFrameFormCollection)
					continue;

				hr = pFrameFormCollection->get_length(&nForms);
				if(FAILED(hr))
					continue;

				hr = pFrameFormCollection->item(COleVariant(strFormName.AllocSysString(), VT_BSTR), COleVariant((long)0), &pFrameDispForm);
				if(FAILED(hr) || !pFrameDispForm)
					continue;

				hr = pFrameDispForm->QueryInterface(IID_IHTMLFormElement, (void**)&pForm);
				if(FAILED(hr) || !pForm)
					continue;

				bSuccess = TRUE;
				break;
			}

			pUnk->Release();
			pUnk = NULL;
		}

		if(!bSuccess)
			return NULL;
	}
	else
	{
		hr = pDispForm->QueryInterface(IID_IHTMLFormElement, (void**)&pForm);
		if(FAILED(hr) || !pForm)
			return FALSE;
	}

	if(!pForm)
		return FALSE;

	CComPtr<IHTMLInputTextElement> pInputElem = NULL;
	CComPtr<IDispatch> pDispInputForm = NULL;

	hr = pForm->item(COleVariant(strObjectID.AllocSysString()),COleVariant(long(0)),&pDispInputForm);
	if(FAILED(hr) || !pDispInputForm)
		return FALSE;

	hr = pDispInputForm->QueryInterface(IID_IHTMLInputTextElement, (void**)&pInputElem);
	if(FAILED(hr) || !pInputElem)
		return FALSE;

	pInputElem->select();
	{
		CComBSTR bstrPutText;
		pInputElem->get_value(&bstrPutText);
		lpszText	= bstrPutText;
	}
	return TRUE;
}

BOOL CHtml_old::ClickButton(LPCTSTR sObjectID)
{
	CComPtr<IHTMLElement> pElement = NULL;
	CComPtr<IDispatch> pDisp = NULL;
	CComPtr<IHTMLDocument3> pDoc3 = NULL;
	BOOL bSuccess = FALSE;

	HRESULT hr;

	//hr = m_pWebBrowser2->get_Document(&pDisp);
	if(!(pDisp = GetHtmlDocument()))
		return FALSE;


	hr = pDisp->QueryInterface(IID_IHTMLDocument3, (void **)&pDoc3);
	if(FAILED(hr) || !pDoc3)
		return FALSE;

	CComBSTR bstrClickObjectID(sObjectID);
	hr = pDoc3->getElementById(bstrClickObjectID, &pElement);
	if(SUCCEEDED(hr) && pElement)
	{
		pElement->click();
		bSuccess = TRUE;
	}
	else
	{
		CComPtr<IOleContainer> pContainer = NULL;

		//Get the container
		hr = pDisp->QueryInterface(IID_IOleContainer, (void**)&pContainer);
		if (FAILED(hr) || !pContainer)//4.64버전 수정
			return FALSE;

		CComPtr<IEnumUnknown> pEnumerator = NULL;
		//프레임 목록을 구한다
		hr = pContainer->EnumObjects(OLECONTF_EMBEDDINGS, &pEnumerator);
		if (FAILED(hr) || !pEnumerator)
			return FALSE;

		IUnknown* pUnk = NULL;//CComPtr<IUnknown> pUnk으로 선언하는 경우 에러 발생...디버깅해보니 에러나는 경우 명시적으로 하라고 나와 있음..아마 명시적 메모리 해제 없이 반복문에 사용되다보니 그런게 아닌가 싶음...
		ULONG uFetched = 0;

		//프레임목록을 순회하면서 화면크기를 구하고 그 중 가장 큰 값을  nFrameWidth, nFrameHeight에 저장한다
		for (UINT i = 0; S_OK == pEnumerator->Next(1, &pUnk, &uFetched); i++)
		{
			CComPtr<IWebBrowser2> pBrowser = NULL;

			hr = pUnk->QueryInterface(IID_IWebBrowser2, (void**)&pBrowser);

			if (SUCCEEDED(hr) && pBrowser)
			{

				CComPtr<IDispatch> pFrameDisp = NULL;
				CComPtr<IHTMLDocument3> pFrameDoc = NULL;
				CComPtr<IHTMLElement> pFrameElem = NULL;

				hr = pBrowser->get_Document(&pFrameDisp);
				if(FAILED(hr) && !pFrameDisp)
					continue;
				hr = pFrameDisp->QueryInterface(IID_IHTMLDocument3, (void**)&pFrameDoc);
				if(FAILED(hr) || !pFrameDoc)
					continue;

				hr = pFrameDoc->getElementById(bstrClickObjectID, &pFrameElem);
				if(FAILED(hr) || !pFrameElem)
					continue;

				pFrameElem->click();
				bSuccess = TRUE;
				break;
			}

			pUnk->Release();
			pUnk = NULL;
		}
	}

	return bSuccess;
}

BOOL CHtml_old::RunScript(LPCTSTR sScript){
	CComPtr<IHTMLElement> pElement = NULL;
	CComPtr<IDispatch> pDisp = NULL;
	DISPID dispid = NULL;
	CComPtr<IHTMLDocument2> pDoc2 = NULL;
	LPDISPATCH pScript = NULL;
	CComBSTR bstrScriptName(sScript);
	CStringArray paramArray;


	BOOL bSuccess = FALSE;

	HRESULT hr;

	//hr = m_pWebBrowser2->get_Document(&pDisp);
	if(!(pDisp = GetHtmlDocument()))
		return FALSE;


	hr = pDisp->QueryInterface(IID_IHTMLDocument2, (void **)&pDoc2);
	if(FAILED(hr) || !pDoc2)
		return FALSE;

	hr = pDoc2->get_Script(&pScript);
	if(FAILED(hr) || !pScript)
		return FALSE;

	hr = pScript->GetIDsOfNames(IID_NULL, &bstrScriptName, 1, LOCALE_SYSTEM_DEFAULT, &dispid);

	if(FAILED(hr) || !pScript)
	{
		if(pScript)
		{
			pScript->Release();
			pScript = NULL;
		}

		CComPtr<IOleContainer> pContainer = NULL;

		//Get the container
		hr = pDisp->QueryInterface(IID_IOleContainer, (void**)&pContainer);
		if (FAILED(hr) || !pContainer)
			return FALSE;

		CComPtr<IEnumUnknown> pEnumerator = NULL;
		//프레임 목록을 구한다
		hr = pContainer->EnumObjects(OLECONTF_EMBEDDINGS, &pEnumerator);
		if (FAILED(hr) || !pEnumerator)
			return FALSE;

		IUnknown* pUnk = NULL;//CComPtr<IUnknown> pUnk으로 선언하는 경우 에러 발생...디버깅해보니 에러나는 경우 명시적으로 하라고 나와 있음..아마 명시적 메모리 해제 없이 반복문에 사용되다보니 그런게 아닌가 싶음...
		ULONG uFetched = 0;

		//프레임목록을 순회하면서 화면크기를 구하고 그 중 가장 큰 값을  nFrameWidth, nFrameHeight에 저장한다
		for (UINT i = 0; S_OK == pEnumerator->Next(1, &pUnk, &uFetched); i++)
		{
			CComPtr<IWebBrowser2> pBrowser = NULL;

			hr = pUnk->QueryInterface(IID_IWebBrowser2, (void**)&pBrowser);

			if (SUCCEEDED(hr) && pBrowser)
			{

				CComPtr<IDispatch> pFrameDisp = NULL;
				CComPtr<IHTMLDocument2> pFrameDoc2 = NULL;
				CComPtr<IHTMLElement> pFrameElem = NULL;

				hr = pBrowser->get_Document(&pFrameDisp);
				if(FAILED(hr) && !pFrameDisp)
					continue;
				hr = pFrameDisp->QueryInterface(IID_IHTMLDocument2, (void**)&pFrameDoc2);
				if(FAILED(hr) || !pFrameDoc2)
					continue;

				hr = pFrameDoc2->get_Script(&pScript);
				if(FAILED(hr) || !pScript)
					continue;

				hr = pScript->GetIDsOfNames(IID_NULL, &bstrScriptName, 1, LOCALE_SYSTEM_DEFAULT, &dispid);
				if(FAILED(hr) || !pDisp)
				{
					pScript->Release();
					pScript = NULL;
					continue;
				}

				bSuccess = TRUE;
				break;
			}

			pUnk->Release();
			pUnk = NULL;
		}

	}

	if(!pScript || !dispid)
		return FALSE;

	const int arraySize = (const int)paramArray.GetSize();

	//Putting parameters
	DISPPARAMS dispparams;
	memset(&dispparams, 0, sizeof dispparams);
	dispparams.cArgs      = arraySize;
	dispparams.rgvarg     = new VARIANT[dispparams.cArgs];

	for( int i = 0; i < arraySize; i++)
	{
		CComBSTR bstr = paramArray.GetAt(arraySize - 1 - i); // back reading
		bstr.CopyTo(&dispparams.rgvarg[i].bstrVal);
		dispparams.rgvarg[i].vt = VT_BSTR;
	}

	dispparams.cNamedArgs = 0;


	EXCEPINFO excepInfo;
	memset(&excepInfo, 0, sizeof excepInfo);
	CComVariant vaResult;
	UINT nArgErr = (UINT)-1; // initialize to invalid arg
	//JavaScript 실행
	hr = pScript->Invoke(dispid, IID_NULL, 0, DISPATCH_METHOD,&dispparams, &vaResult, &excepInfo, &nArgErr);
	delete [] dispparams.rgvarg;
	pScript->Release();

	if(FAILED(hr))
		return FALSE;

	return TRUE;
}