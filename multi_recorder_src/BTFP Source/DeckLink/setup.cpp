//------------------------------------------------------------------------------
// File: Setup.cpp
//
// Desc: DirectShow sample code - implementation of RGBFilters sample filters
//
// Copyright (c) 2000-2002  Microsoft Corporation.  All rights reserved.
//------------------------------------------------------------------------------

#include "stdafx.h"

#include "Filter_h.h"
#include "Filter_i.c"

#include <combase.h>



//#include "StereoTransformFilter.h"
#include "DecklLinkSource.h"
#include "DeckLinkFilterConfigPage.h"

// List of class IDs and creator functions for the class factory. This
// provides the link between the OLE entry point in the DLL and an object
// being created. The class factory will call the static CreateInstance.
// We provide a set of filters in this one DLL.

#define  FILTER_NAME TEXT("BTFP ")
//L"DeckLink HD Extreme 3D"

CFactoryTemplate g_Templates[] = 
{
//	{ L"StereoTransform4DeckLink", &CLSID_StereoTransform4DeckLink, CStereoTransformFilter::CreateInstance, NULL, &sudStereoTransformFilter },
	{ FILTER_NAME, &CLSID_DeckLinkHDStereoSource, CDeckLinkSource::CreateInstance, NULL, &sudDeckLinkHDStereoSource},
	{ L"DeckLink Config Property Page",	&CLSID_CDeckLinkFilterConfigPage,CDeckLinkFilterConfigPage::CreateInstance},
};

int g_cTemplates = sizeof(g_Templates) / sizeof(g_Templates[0]);


STDAPI unregisterFilters() 
{
	IFilterMapper2* mapper2 = 0;
	HRESULT hr = CoCreateInstance(CLSID_FilterMapper2, 0,
		CLSCTX_INPROC_SERVER, IID_IFilterMapper2, (void**)&mapper2);

	if (!mapper2)
		return hr;

	CComPtr<ICreateDevEnum> pSysDevEnum;
	CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC_SERVER,
		IID_ICreateDevEnum, (void **)&pSysDevEnum);

	// Obtain a class enumerator for the video compressor category.
	CComPtr<IEnumMoniker> pEnumCat;
	pSysDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory, &pEnumCat, 0);

	pEnumCat == NULL ? E_FAIL : S_OK;

	// Enumerate the monikers.
	CComPtr<IMoniker> pMoniker;
	ULONG cFetched;

	while(pEnumCat->Next(1, &pMoniker, &cFetched) == S_OK && pMoniker)
	{
		CComPtr<IPropertyBag> pPropBag;
		pMoniker->BindToStorage(0, 0, IID_IPropertyBag, (void **)&pPropBag);

		// To retrieve the filter's friendly name, do the following:
		_variant_t VarName;
		if (pPropBag->Read(L"FriendlyName", &VarName, 0) != S_OK)
		{					
			pMoniker = NULL;
			continue;
		}

		CComBSTR bstrName = VarName.bstrVal;
		if (pPropBag->Read(L"CLSID", &VarName, 0) != S_OK)
		{					
			pMoniker = NULL;
			continue;
		}

		CLSID clsid;
		if (SUCCEEDED(CLSIDFromString(VarName.bstrVal, &clsid)) &&
			(clsid != CLSID_DeckLinkHDStereoSource))
		{
			pMoniker = NULL;
			continue;
		}

		mapper2->UnregisterFilter(&CLSID_VideoInputDeviceCategory, bstrName, CLSID_DeckLinkHDStereoSource);
		pMoniker = NULL;
	}

	mapper2->Release();
	return S_OK;
}

STDAPI registerFilters()
{
	// Put filter to video capture category
	// ------------------------------------
	IFilterMapper2* mapper2 = 0;
	HRESULT hr = CoCreateInstance(CLSID_FilterMapper2, 0,
		CLSCTX_INPROC_SERVER, IID_IFilterMapper2, (void**)&mapper2);

	if (!mapper2)
		return hr;

	REGFILTER2 rf2;
	rf2.cPins = 0;
	rf2.cPins2 = 0;
	rf2.rgPins2 = 0;
	rf2.dwVersion = 2;
	rf2.dwMerit = MERIT_DO_NOT_USE;

	CComPtr<IDeckLinkIterator> deckLinkIterator = NULL;
	if (CoCreateInstance(CLSID_CDeckLinkIterator, NULL, CLSCTX_ALL, IID_IDeckLinkIterator, (void**)&deckLinkIterator) != S_OK || 
		deckLinkIterator == NULL) 
	{
		return E_FAIL;
	}

	IDeckLink* pDeckLink;
	int n = 0;
	hr = E_FAIL;

	while (deckLinkIterator->Next(&pDeckLink) == S_OK ) 
	{
		CComBSTR cardNameBSTR;			
		hr = pDeckLink->GetDisplayName(&cardNameBSTR);
		CStringW sNameW = FILTER_NAME;
		sNameW += cardNameBSTR;

		CComPtr<IMoniker> pMoniker = NULL;
		hr = mapper2->RegisterFilter(
			CLSID_DeckLinkHDStereoSource,
			sNameW, // name shown to the user
			&pMoniker, // device moniker
			&CLSID_VideoInputDeviceCategory,
			sNameW, // unique instance name
			&rf2);

		if (SUCCEEDED(hr) && pMoniker)
		{
			CComPtr<IPropertyBag> pPropBag;
			hr = pMoniker->BindToStorage( 0, 0, IID_IPropertyBag, (void **)&pPropBag);

			VARIANT var;
			var.vt = VT_BSTR;
			var.bstrVal = cardNameBSTR;
			hr = pPropBag->Write(DEVICENAME, &var);
		}
	}

	mapper2->Release();
	return S_OK;
}

//
// DllRegisterServer
//
STDAPI DllRegisterServer()
{
	unregisterFilters();
	registerFilters();
	
	return AMovieDllRegisterServer2( TRUE );

} // DllRegisterServer


//
// DllUnregisterServer
//
STDAPI DllUnregisterServer()
{
	unregisterFilters();
	return AMovieDllRegisterServer2( FALSE );

} // DllUnregisterServer


