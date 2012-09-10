// ATLSimpleObject.cpp : Implementation of CATLSimpleObject

#include "stdafx.h"
#include "ATLSimpleObject.h"
#include <atlcomcli.h>
#include <winhttp.h>
#include <comutil.h>

#pragma comment(lib, "comsupp.lib")
#pragma comment(lib, "winhttp.lib")


// CATLSimpleObject
/*SetSite function establishes connection when the IExplorer is opened.
It also releases the connection when IExplorer is closed.
The IObjectWithSite interface has all the implementations necessary for the BHO
to perform its tasks and therefore we call the parent at the end of the function.
*/
STDMETHODIMP CATLSimpleObject::SetSite(IUnknown *pUnkSite)
{
	if(pUnkSite != NULL)
	{
		//check if connection established successfully.
		HRESULT hr = pUnkSite->QueryInterface(IID_IWebBrowser2, (void**)&m_spWebBrowser);
		if(SUCCEEDED(hr))
		{
			hr = DispEventAdvise(m_spWebBrowser);
			if(SUCCEEDED(hr))
				m_fAdvised = TRUE;
		}
	}
	else
	{
		//if connection established break it as its no longer needed
		if(m_fAdvised)
		{
			DispEventUnadvise(m_spWebBrowser);
			m_fAdvised = FALSE;
		}
		m_spWebBrowser.Release();
	}

	return IObjectWithSiteImpl<CATLSimpleObject>::SetSite(pUnkSite);
}

/*
This is where the BHO is implemented. Once the document is complete the BHO which we implement
will do the necessary tasks. 
1. create the querystring.
2. establish HTTP session.
3. if session established, create an HTTP connection.
4. if Connection established, open an HTTP request
5. If request opened successfully, send the request.
6. if response received successfully proceed with the processing.
*/
void STDMETHODCALLTYPE CATLSimpleObject::OnDocumentComplete(IDispatch *pDisp, VARIANT * pvarURL)
{
	HWND hwnd;
	HRESULT hr = m_spWebBrowser->get_HWND((LONG_PTR*)&hwnd);

	if(SUCCEEDED(hr))
	{
		//given API string
		BSTR APIStr = L"http://api.shop-o-saur.us/website/deals/info?pageUrl=";
		long len1, len2;
		DWORD dwSize = 0;
		DWORD dwDownloaded = 0;
		LPSTR pszOutBuffer;

		//Captured URL of the current document loaded.
		BSTR *URLStr = new BSTR[1000];
		m_spWebBrowser->get_LocationURL(URLStr);
		CComBSTR bstr(APIStr);
		bstr.AppendBSTR(*URLStr);

		//Given API string + captured URL
		BSTR TotalStr = bstr.Detach();

		BOOL  bResults = FALSE;
		HINTERNET	hSession = NULL, 
					hConnect = NULL,
					hRequest = NULL;

	// Use WinHttpOpen to obtain a session handle.
	hSession = WinHttpOpen( L"WinHTTP Example/1.0",  
							WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
							WINHTTP_NO_PROXY_NAME, 
							WINHTTP_NO_PROXY_BYPASS, 0);

	// Specify an HTTP server.
	if (hSession)
		hConnect = WinHttpConnect( hSession, TotalStr,
									INTERNET_DEFAULT_HTTPS_PORT, 0);

	// Create an HTTP request handle.
	if (hConnect)
		hRequest = WinHttpOpenRequest( hConnect, L"GET", NULL,
										NULL, WINHTTP_NO_REFERER, 
										WINHTTP_DEFAULT_ACCEPT_TYPES, 
										WINHTTP_FLAG_SECURE);

	//header required for the HTTP request.
	BSTR headers = L"Tenant-Id: 853e99f0affb11e1afa60800200c9a66";

	// Send a request.
	if (hRequest)
		bResults = WinHttpSendRequest( hRequest,
										headers,
										0, WINHTTP_NO_REQUEST_DATA, 0, 
										0, 0);

	//These are debug statements for verifying at what stage the HTTP session failed.
	/*if (hSession)
		MessageBox(hwnd, L"Session Successfull.", L"BHO", MB_OK);
	if(hConnect)
		MessageBox(hwnd, L"Connection Successfull.", L"BHO", MB_OK);
	if(hRequest)
		MessageBox(hwnd, L"Request Successfull.", L"BHO", MB_OK);
	if(bResults)
		MessageBox(hwnd, L"Result Successfull.", L"BHO", MB_OK);*/

	// End the request.
	if (bResults)
		bResults = WinHttpReceiveResponse( hRequest, NULL);

		// Keep checking for data until there is nothing left.
		if (bResults)
		{
			do 
			{
				// Check for available data.
				dwSize = 0;
				if (!WinHttpQueryDataAvailable( hRequest, &dwSize)) 
				{
					MessageBox(hwnd, L"Error in WinHttpQueryDataAvailable.", L"BHO", MB_OK);
					break;
				}
            
				// No more available data.
				if (!dwSize)
					break;

				// Allocate space for the buffer.
				pszOutBuffer = new char[dwSize+1];
				if (!pszOutBuffer)
				{
					MessageBox(hwnd, L"Out of Memory.", L"BHO", MB_OK);
					break;
				}
            
				// Read the Data.
				ZeroMemory(pszOutBuffer, dwSize+1);

				if (!WinHttpReadData( hRequest, (LPVOID)pszOutBuffer, 
										dwSize, &dwDownloaded))
				{    
					MessageBox(hwnd, L"Error in WinHttpReadData.", L"BHO", MB_OK);
				}
				else
				{
					//This is where the response will be available. 
					// we need to perform remaining tasks from the data availabel in the pszOutBuffer.
					MessageBox(hwnd, L"Success!!!", L"BHO", MB_OK);
					
					/*bstr_t _bstr = pszOutBuffer;
					MessageBox(hwnd, *pszOutBuffer, L"BHO", MB_OK);*/
				}
        
				// Free the memory allocated to the buffer.
				delete [] pszOutBuffer;

				// This condition should never be reached since WinHttpQueryDataAvailable
				// reported that there are bits to read.
				if (!dwDownloaded)
					break;
                
			} while (dwSize > 0);
		}
	
		else
		{
			//All generic errors captured here. These inculde any failure while creating 
			// the HTTP request like session or connection establishment or Open or send
			//HTTP request failures.
			MessageBox(hwnd, L"Unknown error Occured.", L"BHO", MB_OK);
		}
		//cleanup
		*URLStr = NULL;
		delete *URLStr;
		
		// Close open handles.
		if( hRequest ) WinHttpCloseHandle( hRequest );
		if( hConnect ) WinHttpCloseHandle( hConnect );
		if( hSession ) WinHttpCloseHandle( hSession );
		
	}
}