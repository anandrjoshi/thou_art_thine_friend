// ATLSimpleObject.h : Declaration of the CATLSimpleObject

#pragma once
#include "resource.h"       // main symbols
#include <ShlGuid.h>
#include <ExDispid.h>

#include "MyFirstBHO_i.h"



#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif

using namespace ATL;


// CATLSimpleObject

class ATL_NO_VTABLE CATLSimpleObject :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CATLSimpleObject, &CLSID_ATLSimpleObject>,
	public IObjectWithSiteImpl<CATLSimpleObject>,
	public IDispatchImpl<IATLSimpleObject, &IID_IATLSimpleObject, &LIBID_MyFirstBHOLib, /*wMajor =*/ 1, /*wMinor =*/ 0>,
	public IDispEventImpl<1, CATLSimpleObject, &DIID_DWebBrowserEvents2, &LIBID_SHDocVw, 1, 1>
{
public:
	STDMETHOD(SetSite)(IUnknown *pUnkSite);

	void STDMETHODCALLTYPE OnDocumentComplete(IDispatch *pDisp, VARIANT *pvarURL);

	BEGIN_SINK_MAP(CATLSimpleObject)
		SINK_ENTRY_EX(1, DIID_DWebBrowserEvents2, DISPID_DOCUMENTCOMPLETE, OnDocumentComplete)
	END_SINK_MAP()

	CATLSimpleObject()
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_ATLSIMPLEOBJECT)

DECLARE_NOT_AGGREGATABLE(CATLSimpleObject)

BEGIN_COM_MAP(CATLSimpleObject)
	COM_INTERFACE_ENTRY(IATLSimpleObject)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(IObjectWithSite)
END_COM_MAP()



	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}

	void FinalRelease()
	{
	}

public:
private:
	CComPtr<IWebBrowser2> m_spWebBrowser;
	//track if object has established a connection
	bool m_fAdvised;


};

OBJECT_ENTRY_AUTO(__uuidof(ATLSimpleObject), CATLSimpleObject)
