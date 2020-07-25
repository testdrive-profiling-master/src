#include "StdAfx.h"
#include "Html.h"
#include "TestDriveImp.h"
//#include "ExDispid.h" // IE5.5 �̻��� ���� �Ǵ� ����� �ʿ� �մϴ�.

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNAMIC(CHtml, CHtmlView)
BEGIN_MESSAGE_MAP(CHtml, CHtmlView)
	ON_WM_MOUSEACTIVATE()
	ON_WM_DESTROY()
END_MESSAGE_MAP()


CHtml::CHtml(void) : m_pManager(NULL), m_dwID(0), m_bBlockNewWindow(FALSE)
{
}

CHtml::~CHtml(void)
{
	SetCurrentCopynPasteAction(FALSE);
	m_pManager	= NULL;
}

int CHtml::OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message){
	// bypass CView doc/frame stuff
	SetCurrentCopynPasteAction(TRUE);
	return CWnd::OnMouseActivate(pDesktopWnd, nHitTest, message);
}

void CHtml::OnDestroy(){
	CWnd::OnDestroy();
}

void CHtml::OnBeforeNavigate2(	LPCTSTR lpszURL, DWORD nFlags,
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

void CHtml::OnDocumentComplete(LPCTSTR lpszURL){
	if(m_pManager) m_pManager->OnHtmlDocumentComplete(m_dwID, lpszURL);
}

// #include <mshtmdid.h>
// 
// BOOL CHtml::OnAmbientProperty(COleControlSite* pSite, DISPID dispid, VARIANT* pvar){
// 	if(pvar && dispid == DISPID_AMBIENT_DLCONTROL){
// 		// ��ũ��Ʈ ���� �޽��� �Ⱥ��̰� �ϱ�
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

void CHtml::SetManager(ITDHtmlManager* pManager, DWORD dwID){
	m_pManager	= pManager;
	m_dwID		= dwID;
}

BOOL CHtml::CallJScript(LPCTSTR lpszScript){
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

void CHtml::SetBlockNewWindow(BOOL bBlock){
	m_bBlockNewWindow = bBlock;
}

HRESULT CHtml::ExecFormCommand(const GUID *pGuid, long cmdID, long cmdExecOpt, VARIANT* pInVar, VARIANT* pOutVar) const 
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

HRESULT CHtml::ExecFormCommand(long cmdID, long cmdExecOpt, VARIANT* pInVar, VARIANT* pOutVar) const 
{ 
	return ExecFormCommand (&CGID_MSHTML, cmdID, cmdExecOpt, pInVar, pOutVar);
}

BOOL CHtml::CheckVisible(void){
	if(!IsWindowVisible()){
		SetCurrentCopynPasteAction(FALSE);
		return FALSE;
	}
	return TRUE;
}

void CHtml::OnAccel(ACCEL_CODE code){
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
		SetFullScreen(!GetFullScreen());	// ie �� �Ǵ� ��Ʈ��
		break;*/
	}
}

void CHtml::OnNewWindow2(LPDISPATCH* ppDisp, BOOL* Cancel){
	if(m_bBlockNewWindow){
		//*ppDisp = GetApplication();	// ��â ���⸦ ���� ����.
		*Cancel	= TRUE;
		//Navigate2(m_sNaviURL);
	}else
	CHtmlView::OnNewWindow2(ppDisp, Cancel);
}

BOOL CHtml::PreTranslateMessage(MSG* pMsg) {
	if (pMsg->message == WM_KEYDOWN && (pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE))
	{
		::TranslateMessage(pMsg);
	}
	return CHtmlView::PreTranslateMessage(pMsg);
}

BOOL CHtml::PutText(CString strFormName, CString strObjectID, CString strPutText){
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
		//������ ����� ���Ѵ�
		hr = pContainer->EnumObjects(OLECONTF_EMBEDDINGS, &pEnumerator);
		if (FAILED(hr) || !pEnumerator)
			return FALSE;

		IUnknown* pUnk = NULL;
		ULONG uFetched = 0;

		//�����Ӹ���� ��ȸ
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
	pInputElem->put_value( bstrPutText );//�ؽ�Ʈ �Է�

	return TRUE;
}

BOOL CHtml::GetText(CString strFormName, CString strObjectID, CString& lpszText){
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
		//������ ����� ���Ѵ�
		hr = pContainer->EnumObjects(OLECONTF_EMBEDDINGS, &pEnumerator);
		if (FAILED(hr) || !pEnumerator)
			return NULL;

		IUnknown* pUnk = NULL;
		ULONG uFetched = 0;

		//�����Ӹ���� ��ȸ
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

BOOL CHtml::ClickButton(LPCTSTR sObjectID)
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
		if (FAILED(hr) || !pContainer)//4.64���� ����
			return FALSE;

		CComPtr<IEnumUnknown> pEnumerator = NULL;
		//������ ����� ���Ѵ�
		hr = pContainer->EnumObjects(OLECONTF_EMBEDDINGS, &pEnumerator);
		if (FAILED(hr) || !pEnumerator)
			return FALSE;

		IUnknown* pUnk = NULL;//CComPtr<IUnknown> pUnk���� �����ϴ� ��� ���� �߻�...������غ��� �������� ��� ��������� �϶�� ���� ����..�Ƹ� ����� �޸� ���� ���� �ݺ����� ���Ǵٺ��� �׷��� �ƴѰ� ����...
		ULONG uFetched = 0;

		//�����Ӹ���� ��ȸ�ϸ鼭 ȭ��ũ�⸦ ���ϰ� �� �� ���� ū ����  nFrameWidth, nFrameHeight�� �����Ѵ�
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

BOOL CHtml::RunScript(LPCTSTR sScript){
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
		//������ ����� ���Ѵ�
		hr = pContainer->EnumObjects(OLECONTF_EMBEDDINGS, &pEnumerator);
		if (FAILED(hr) || !pEnumerator)
			return FALSE;

		IUnknown* pUnk = NULL;//CComPtr<IUnknown> pUnk���� �����ϴ� ��� ���� �߻�...������غ��� �������� ��� ��������� �϶�� ���� ����..�Ƹ� ����� �޸� ���� ���� �ݺ����� ���Ǵٺ��� �׷��� �ƴѰ� ����...
		ULONG uFetched = 0;

		//�����Ӹ���� ��ȸ�ϸ鼭 ȭ��ũ�⸦ ���ϰ� �� �� ���� ū ����  nFrameWidth, nFrameHeight�� �����Ѵ�
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
	//JavaScript ����
	hr = pScript->Invoke(dispid, IID_NULL, 0, DISPATCH_METHOD,&dispparams, &vaResult, &excepInfo, &nArgErr);
	delete [] dispparams.rgvarg;
	pScript->Release();

	if(FAILED(hr))
		return FALSE;

	return TRUE;
}