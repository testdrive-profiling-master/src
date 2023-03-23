#include "StdAfx.h"
#include "Html.h"
#include "TestDriveImp.h"
#include <sstream>
#include <windowsx.h>
#include <WinUser.h>
#include <winstring.h>	// For wil::unique_hstring
#include <wil/common.h>
#include <wil/win32_helpers.h>

#ifdef USE_WEBVIEW2_WIN10
#include <windows.ui.composition.interop.h>
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include "TestDriveImp.h"

CHtml::CHtml(void) : m_pManager(NULL), m_dwID(0), m_bBlockNewWindow(FALSE), m_pParent(NULL), m_bInitialized(FALSE),
	m_NewWindowRequestedToken({0}),
	m_navigationStartingToken({0}),
	m_navigationCompletedToken({0}),
	m_WebMessageReceivedToken({0})
{
}

CHtml::~CHtml(void)
{
	m_pManager	= NULL;
}

void CHtml::Initialize(CWnd* pParent)
{
	m_pParent		= pParent;
	m_dcompDevice	= nullptr;
	HRESULT hr2 = DCompositionCreateDevice2(nullptr, IID_PPV_ARGS(&m_dcompDevice));
	if (!SUCCEEDED(hr2))
	{
		AfxMessageBox(L"Attempting to create WebView using DComp Visual is not supported.\r\n"
			"DComp device creation failed.\r\n"
			"Current OS may not support DComp.\r\n"
			"Create with Windowless DComp Visual Failed", MB_OK);
		return;
	}

#ifdef USE_WEBVIEW2_WIN10
	m_wincompCompositor = nullptr;
#endif
	LPCWSTR subFolder = nullptr;
	LPCWSTR sBrowserArg = L" --disable-web-security --allow-file-access-from-files --allow-file-access --allow-running-insecure-content --allow-insecure-localhost --allow-cross-origin-auth-prompt";
	auto options = Microsoft::WRL::Make<CoreWebView2EnvironmentOptions>();
	options->put_AllowSingleSignOnUsingOSPrimaryAccount(FALSE);
	options->put_ExclusiveUserDataFolderAccess(FALSE);
	options->put_AdditionalBrowserArguments(sBrowserArg);

	HRESULT hr = CreateCoreWebView2EnvironmentWithOptions(
		subFolder, nullptr, options.Get(),
		Microsoft::WRL::Callback
		<ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler>
		(this, &CHtml::OnCreateEnvironmentCompleted).Get());

	if (!SUCCEEDED(hr))
	{
		if (hr == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND))
		{
			TRACE("Couldn't find Edge installation. Do you have a version installed that is compatible with this ");
		}
		else
		{
			AfxMessageBox(L"Failed to create webview environment");
		}
	}
	else {
		// wait for creation complete
		MSG msg;
		while (!m_bInitialized && GetMessage(&msg, nullptr, WM_USER + 1, WM_USER + 1)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
}

HRESULT CHtml::DCompositionCreateDevice2(IUnknown* renderingDevice, REFIID riid, void** ppv)
{
	HRESULT hr = E_FAIL;
	static decltype(::DCompositionCreateDevice2)* fnCreateDCompDevice2 = nullptr;
	if (fnCreateDCompDevice2 == nullptr)
	{
		HMODULE hmod = ::LoadLibraryEx(L"dcomp.dll", nullptr, 0);
		if (hmod != nullptr)
		{
			fnCreateDCompDevice2 = reinterpret_cast<decltype(::DCompositionCreateDevice2)*>(
				::GetProcAddress(hmod, "DCompositionCreateDevice2"));
		}
	}
	if (fnCreateDCompDevice2 != nullptr)
	{
		hr = fnCreateDCompDevice2(renderingDevice, riid, ppv);
	}
	return hr;
}

HRESULT CHtml::OnCreateEnvironmentCompleted(HRESULT result, ICoreWebView2Environment* environment)
{
	m_webViewEnvironment = environment;
	m_webViewEnvironment->CreateCoreWebView2Controller(
		m_pParent->GetSafeHwnd(), Microsoft::WRL::Callback
		<ICoreWebView2CreateCoreWebView2ControllerCompletedHandler>
		(this, &CHtml::OnCreateCoreWebView2ControllerCompleted).Get());

	return S_OK;
}

HRESULT CHtml::OnCreateCoreWebView2ControllerCompleted(HRESULT result, ICoreWebView2Controller* controller)
{
	if (result == S_OK)
	{
		m_controller = controller;
		Microsoft::WRL::ComPtr<ICoreWebView2> coreWebView2;
		m_controller->get_CoreWebView2(&coreWebView2);
		m_webView = coreWebView2.Get();

		m_webView->add_NewWindowRequested(
			Microsoft::WRL::Callback
			<ICoreWebView2NewWindowRequestedEventHandler>(this, &CHtml::OnNewWindowRequested).Get(),
			&m_NewWindowRequestedToken);

		m_webView->add_NavigationStarting(
			Microsoft::WRL::Callback
			<ICoreWebView2NavigationStartingEventHandler>(this, &CHtml::OnNavigationStart).Get(),
			&m_navigationStartingToken);

		m_webView->add_NavigationCompleted(
			Microsoft::WRL::Callback
			<ICoreWebView2NavigationCompletedEventHandler>(this, &CHtml::OnNavigationComplete).Get(),
			& m_navigationCompletedToken);

		m_webView->add_WebMessageReceived(
			Microsoft::WRL::Callback
			<ICoreWebView2WebMessageReceivedEventHandler>(this, &CHtml::OnWebMessageReceived).Get(),
			&m_WebMessageReceivedToken);
	}
	else
	{
		TRACE("Failed to create webview");
	}
	m_bInitialized = TRUE;
	return S_OK;
}

HRESULT CHtml::OnNewWindowRequested(ICoreWebView2* sender, ICoreWebView2NewWindowRequestedEventArgs* args) {
	// no implementation
	return S_OK;
}

HRESULT CHtml::OnNavigationStart(ICoreWebView2* sender, ICoreWebView2NavigationStartingEventArgs* args) {
	if (m_pManager) {
		wil::unique_cotaskmem_string uri;
		args->get_Uri(&uri);

		CString	sURL(uri.get());
		LPCTSTR lpszURL = m_pManager->OnHtmlBeforeNavigate(m_dwID, sURL);

		if (!lpszURL) {
			args->put_Cancel(TRUE);
		}
	}
	return S_OK;
}

HRESULT CHtml::OnNavigationComplete(ICoreWebView2* sender, ICoreWebView2NavigationCompletedEventArgs* args) {
	if (m_pManager) {
		BOOL success;
		args->get_IsSuccess(&success);
		if (success) {
			wil::unique_cotaskmem_string uri;
			m_webView->get_Source(&uri);
			m_pManager->OnHtmlDocumentComplete(m_dwID, uri.get());
		}
	}

	return S_OK;
}

HRESULT CHtml::OnWebMessageReceived(ICoreWebView2* sender, ICoreWebView2WebMessageReceivedEventArgs* args) {
	if (m_pManager) {
		wil::unique_cotaskmem_string uri;
		args->get_Source(&uri);
		wil::unique_cotaskmem_string messageRaw;
		args->TryGetWebMessageAsString(&messageRaw);
		CString	sMessage = messageRaw.get();
		sMessage = m_pManager->OnHtmlWebMessageReceived(m_dwID, sMessage);
		if (!sMessage.IsEmpty()) {
			sender->PostWebMessageAsJson(sMessage);
		}
	}
	return S_OK;
}

void CHtml::ResizeEverything(void)
{
	if (m_pParent) {
		RECT availableBounds = { 0 };
		m_pParent->GetClientRect(&availableBounds);

		if (m_controller) {
			m_controller->put_Bounds(availableBounds);
		}
	}
}

void CHtml::Navigate(LPCTSTR lpszURL, LPCTSTR lpszTargetFrame)
{
	if (m_webView && m_webView->Navigate(lpszURL) == S_OK) {
		ResizeEverything();
	}
}

void CHtml::OnBeforeNavigate2(	LPCTSTR lpszURL, DWORD nFlags,
								LPCTSTR lpszTargetFrameName, CByteArray& baPostedData,
								LPCTSTR lpszHeaders, BOOL* pbCancel){
	/*
	if(m_pManager) lpszURL = m_pManager->OnHtmlBeforeNavigate(m_dwID, lpszURL);

	if(!lpszURL) *pbCancel = TRUE;
	else{
		CHtmlView::OnBeforeNavigate2(	lpszURL, nFlags,
										lpszTargetFrameName, baPostedData,
										lpszHeaders, pbCancel);
	}*/
}

void CHtml::SetManager(ITDHtmlManager* pManager, DWORD dwID){
	m_pManager	= pManager;
	m_dwID		= dwID;
}

BOOL CHtml::CallJScript(LPCTSTR lpszScript){
	if (m_webView && m_bInitialized) {
		m_webView->ExecuteScript(lpszScript,
			Microsoft::WRL::Callback
			<ICoreWebView2ExecuteScriptCompletedHandler>(
				[this](HRESULT errorCode, LPCWSTR resultObjectAsJson)->HRESULT {
					return S_OK;
				}
		).Get());
		return TRUE;
	}

	return FALSE;
}

void CHtml::SetBlockNewWindow(BOOL bBlock){
	m_bBlockNewWindow = bBlock;
}

void CHtml::OnNewWindow2(LPDISPATCH* ppDisp, BOOL* Cancel){
	/*if (m_bBlockNewWindow) {
		//*ppDisp = GetApplication();	// 새창 띄우기를 하지 않음.
		*Cancel	= TRUE;
		//Navigate2(m_sNaviURL);
	}else
	CHtmlView::OnNewWindow2(ppDisp, Cancel);*/
}

BOOL CHtml::PutText(CString strFormName, CString strObjectID, CString strPutText){

	return TRUE;
}

BOOL CHtml::GetText(CString strFormName, CString strObjectID, CString& lpszText){

	return TRUE;
}

BOOL CHtml::ClickButton(LPCTSTR sObjectID)
{

	return S_OK;
}
