#pragma once
#include "ViewObject.h"
#include "TestDrive.h"
#include "AccelCodeDecoder.h"
#include <dcomp.h>
#include <functional>
#include <ole2.h>
#include <vector>
#include "ComponentBase.h"

class CHtmlCtrl;

#ifdef USE_WEBVIEW2_WIN10
#include <winrt/Windows.UI.Composition.h>
#include <winrt/Windows.UI.ViewManagement.h>
namespace winrtComp = winrt::Windows::UI::Composition;
#endif

class CHtml :
	public CHtmlView,
	public AccelCodeDecoder
{
	DECLARE_DYNAMIC(CHtml)
public:
	CHtml(void);
	virtual ~CHtml(void);

	void Initialize(CWnd* pParent);

	virtual void PostNcDestroy(){}
	virtual void OnDocumentComplete(LPCTSTR lpszURL);
	virtual void OnBeforeNavigate2(LPCTSTR lpszURL, DWORD nFlags,
		LPCTSTR lpszTargetFrameName, CByteArray& baPostedData,
		LPCTSTR lpszHeaders, BOOL* pbCancel);
// 	virtual BOOL OnAmbientProperty(COleControlSite* pSite, DISPID dispid,
// 		VARIANT* pvar);

	void SetManager(ITDHtmlManager* pManager, DWORD dwID);
	BOOL CallJScript(LPCTSTR lpszScript);
	void SetBlockNewWindow(BOOL bBlock = TRUE);
	BOOL PutText(CString strFormName, CString strObjectID, CString strPutText);
	BOOL GetText(CString strFormName, CString strObjectID, CString& lpszText);
	BOOL ClickButton(LPCTSTR sObjectID);
	BOOL RunScript(LPCTSTR sScript);

	HRESULT ExecFormCommand(const GUID *pGuid, long cmdID, long cmdExecOpt, VARIANT* pInVar, VARIANT* pOutVar) const;
	HRESULT ExecFormCommand(long cmdID, long cmdExecOpt, VARIANT* pInVar = NULL, VARIANT* pOutVar = NULL) const ;

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

protected:
	HRESULT OnCreateEnvironmentCompleted(HRESULT result, ICoreWebView2Environment* environment);
	HRESULT OnCreateCoreWebView2ControllerCompleted(HRESULT result, ICoreWebView2Controller* controller);

	BOOL CheckVisible(void);

	virtual void OnAccel(ACCEL_CODE code);
	virtual void OnNewWindow2(LPDISPATCH* ppDisp, BOOL* Cancel);
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	DWORD				m_dwID;
	BOOL				m_bBlockNewWindow;
	ITDHtmlManager*		m_pManager;
	CWnd*				m_pParent;

	DECLARE_MESSAGE_MAP()
	afx_msg int OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message);
	afx_msg void OnDestroy();

	// WebView2
	template <class ComponentType, class... Args> void NewComponent(Args&&... args);
	template <class ComponentType> ComponentType* GetComponent();

	DWORD m_creationModeId;
	Microsoft::WRL::ComPtr<ICoreWebView2Environment> m_webViewEnvironment;
	Microsoft::WRL::ComPtr<ICoreWebView2Controller> m_controller;
	Microsoft::WRL::ComPtr<ICoreWebView2> m_webView;
	Microsoft::WRL::ComPtr<IDCompositionDevice> m_dcompDevice;
	std::vector<std::unique_ptr<ComponentBase>> m_components;
	
	void ResizeEverything(void);
	HRESULT DCompositionCreateDevice2(IUnknown* renderingDevice, REFIID riid, void** ppv);
};

template <class ComponentType, class... Args> void CHtml::NewComponent(Args&&... args)
{
	m_components.emplace_back(new ComponentType(std::forward<Args>(args)...));
}

template <class ComponentType> ComponentType* CHtml::GetComponent()
{
	for (auto& component : m_components)
	{
		if (auto wanted = dynamic_cast<ComponentType*>(component.get()))
		{
			return wanted;
		}
	}
	return nullptr;
}
