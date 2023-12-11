#pragma once
#include "ViewObject.h"
#include "TestDrive.h"
#include "AccelCodeDecoder.h"
#include <dcomp.h>
#include <functional>
#include <ole2.h>
#include <vector>

class CHtmlCtrl;

#ifdef USE_WEBVIEW2_WIN10
#include <winrt/Windows.UI.Composition.h>
#include <winrt/Windows.UI.ViewManagement.h>
namespace winrtComp = winrt::Windows::UI::Composition;
#endif

class CHtml
{
public:
	CHtml(void);
	virtual ~CHtml(void);

	void Initialize(CWnd* pParent);
	void Navigate(LPCTSTR lpszURL, LPCTSTR lpszTargetFrame);

	virtual void OnBeforeNavigate2(LPCTSTR lpszURL, DWORD nFlags,
		LPCTSTR lpszTargetFrameName, CByteArray& baPostedData,
		LPCTSTR lpszHeaders, BOOL* pbCancel);

	void SetManager(ITDHtmlManager* pManager, DWORD dwID);
	BOOL CallJScript(LPCTSTR lpszScript);
	void SetBlockNewWindow(BOOL bBlock = TRUE);
	BOOL PutText(CString strFormName, CString strObjectID, CString strPutText);
	BOOL GetText(CString strFormName, CString strObjectID, CString& lpszText);
	BOOL ClickButton(LPCTSTR sObjectID);

	ICoreWebView2Controller* GetWebViewController()
	{
		return m_controller.Get();
	}
	ICoreWebView2* GetWebView()
	{
		return m_webView.Get();
	}
	ICoreWebView2Environment* GetWebViewEnvironment()
	{
		return m_webViewEnvironment.Get();
	}
	HWND GetMainWindow()
	{
		return m_pParent  ? m_pParent->m_hWnd : NULL;
	}
	void ResizeEverything(void);

protected:
	// creation
	HRESULT OnCreateEnvironmentCompleted(HRESULT result, ICoreWebView2Environment* environment);
	HRESULT OnCreateCoreWebView2ControllerCompleted(HRESULT result, ICoreWebView2Controller* controller);

	// event
	EventRegistrationToken	m_NewWindowRequestedToken;
	EventRegistrationToken	m_navigationStartingToken;
	EventRegistrationToken	m_navigationCompletedToken;
	EventRegistrationToken	m_WebMessageReceivedToken;
	EventRegistrationToken	m_HistoryChangedToken;
	HRESULT STDMETHODCALLTYPE OnNewWindowRequested(ICoreWebView2* sender, ICoreWebView2NewWindowRequestedEventArgs* args);
	HRESULT STDMETHODCALLTYPE OnNavigationStart(ICoreWebView2* sender, ICoreWebView2NavigationStartingEventArgs* args);
	HRESULT STDMETHODCALLTYPE OnNavigationComplete(ICoreWebView2* sender, ICoreWebView2NavigationCompletedEventArgs* args);
	HRESULT STDMETHODCALLTYPE OnWebMessageReceived(ICoreWebView2* sender, ICoreWebView2WebMessageReceivedEventArgs* args);
	HRESULT STDMETHODCALLTYPE OnHistoryChanged(ICoreWebView2* sender, IUnknown* args);


	virtual void OnNewWindow2(LPDISPATCH* ppDisp, BOOL* Cancel);

	DWORD				m_dwID;
	BOOL				m_bBlockNewWindow;
	ITDHtmlManager*		m_pManager;
	CWnd*				m_pParent;
	BOOL				m_bInitialized;

	Microsoft::WRL::ComPtr<ICoreWebView2Environment> m_webViewEnvironment;
	Microsoft::WRL::ComPtr<ICoreWebView2Controller> m_controller;
	Microsoft::WRL::ComPtr<ICoreWebView2> m_webView;
	Microsoft::WRL::ComPtr<IDCompositionDevice> m_dcompDevice;

private:
	HRESULT DCompositionCreateDevice2(IUnknown* renderingDevice, REFIID riid, void** ppv);
};
