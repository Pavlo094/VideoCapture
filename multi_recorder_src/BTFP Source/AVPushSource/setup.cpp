#include <streams.h>
#include "DecklinkPushSource.h"
#include "CustomAllocator.h"
#include "DecklinkVideoSource.h"
#include "DecklinkAudioSource.h"
#include "Utils.h"

//------------------------------------------------------------------------------
// DecklinkPushSource filter
//------------------------------------------------------------------------------
// Filter setup data
const AMOVIESETUP_MEDIATYPE sudPinTypesDPS[] =
{
	{ &MEDIATYPE_Video, &MEDIASUBTYPE_UYVY },
	{ &MEDIATYPE_Video, &IID_MEDIASUBTYPE_HDYC },
	{ &MEDIATYPE_Video, &MEDIASUBTYPE_YUY2 },
	{ &MEDIATYPE_Video, &IID_MEDIASUBTYPE_v210a },
	{ &MEDIATYPE_Video, &IID_MEDIASUBTYPE_r210 },
	{ &MEDIATYPE_Video, &MEDIASUBTYPE_RGB24 },
	{ &MEDIATYPE_Video, &MEDIASUBTYPE_RGB32 },
	{ &MEDIATYPE_Video, &MEDIASUBTYPE_ARGB32 },
};


const AMOVIESETUP_PIN sudOutputPinDPS = 
{
    L"Video",       // Obsolete, not used.
    FALSE,          // Is this pin rendered?
    TRUE,           // Is it an output pin?
    FALSE,          // Can the filter create zero instances?
    FALSE,          // Does the filter create multiple instances?
    &CLSID_NULL,    // Obsolete.
    NULL,           // Obsolete.
    SIZEOF_ARRAY(sudPinTypesDPS),// Number of media types.
    sudPinTypesDPS  // Pointer to media types.
};

const AMOVIESETUP_FILTER sudDPS =
{
    &CLSID_DecklinkPushSource,		// Filter CLSID
    L"Decklink Push Source Filter",	// String name
    MERIT_DO_NOT_USE,				// Filter merit
    1,								// Number pins
    &sudOutputPinDPS				// Pin details
};

//------------------------------------------------------------------------------
// DecklinkVideoSource filter
//------------------------------------------------------------------------------
// Filter setup data
const AMOVIESETUP_MEDIATYPE sudPinTypesDVS[] =
{
	{ &MEDIATYPE_Video, &MEDIASUBTYPE_UYVY },
	{ &MEDIATYPE_Video, &IID_MEDIASUBTYPE_HDYC },
	{ &MEDIATYPE_Video, &MEDIASUBTYPE_YUY2 },
	{ &MEDIATYPE_Video, &IID_MEDIASUBTYPE_v210a },
	{ &MEDIATYPE_Video, &IID_MEDIASUBTYPE_r210 },
	{ &MEDIATYPE_Video, &MEDIASUBTYPE_RGB24 },
	{ &MEDIATYPE_Video, &MEDIASUBTYPE_RGB32 },
	{ &MEDIATYPE_Video, &MEDIASUBTYPE_ARGB32 },
};


const AMOVIESETUP_PIN sudOutputPinDVS = 
{
    L"Video",       // Obsolete, not used.
    FALSE,          // Is this pin rendered?
    TRUE,           // Is it an output pin?
    FALSE,          // Can the filter create zero instances?
    FALSE,          // Does the filter create multiple instances?
    &CLSID_NULL,    // Obsolete.
    NULL,           // Obsolete.
    SIZEOF_ARRAY(sudPinTypesDVS),// Number of media types.
    sudPinTypesDVS  // Pointer to media types.
};

const AMOVIESETUP_FILTER sudDVS =
{
    &CLSID_DecklinkVideoSource,		// Filter CLSID
    L"Decklink Video Source Filter",// String name
    MERIT_DO_NOT_USE,				// Filter merit
    1,								// Number pins
    &sudOutputPinDVS				// Pin details
};

//------------------------------------------------------------------------------
// DecklinkAudioSource filter
//------------------------------------------------------------------------------
// Filter setup data
const AMOVIESETUP_MEDIATYPE sudPinTypesDAS =
{
    &MEDIATYPE_Audio,       // Major type
    &MEDIASUBTYPE_PCM      // Minor type
};


const AMOVIESETUP_PIN sudOutputPinDAS = 
{
    L"Audio",      // Obsolete, not used.
    FALSE,          // Is this pin rendered?
    TRUE,           // Is it an output pin?
    FALSE,          // Can the filter create zero instances?
    FALSE,          // Does the filter create multiple instances?
    &CLSID_NULL,    // Obsolete.
    NULL,           // Obsolete.
    1,              // Number of media types.
    &sudPinTypesDAS // Pointer to media types.
};

const AMOVIESETUP_FILTER sudDAS =
{
    &CLSID_DecklinkAudioSource,		// Filter CLSID
    L"Decklink Audio Source Filter",// String name
    MERIT_DO_NOT_USE,				// Filter merit
    1,								// Number pins
    &sudOutputPinDAS				// Pin details
};

CFactoryTemplate g_Templates[] = 
{
	{ 
		L"Decklink Push Source Filter",				// Name
		&CLSID_DecklinkPushSource,					// CLSID
		CDecklinkPushSource::CreateInstance,		// Method to create an instance of MyComponent
		NULL,										// Initialization function
		&sudDPS										// Set-up information (for filters)
	},
	{ 
		L"Decklink Push Source Custom Allocator",	// Name
		&CLSID_CustomMemAllocator,					// CLSID
		CCustomAllocator::CreateInstance,			// Method to create an instance of MyComponent
		NULL,										// Initialization function
		NULL										// Set-up information (for filters)
	},
	{ 
		L"Decklink Video Source Filter",			// Name
		&CLSID_DecklinkVideoSource,					// CLSID
		CDecklinkVideoSource::CreateInstance,		// Method to create an instance of MyComponent
		NULL,										// Initialization function
		&sudDVS										// Set-up information (for filters)
	},
	{ 
		L"Decklink Audio Source Filter",			// Name
		&CLSID_DecklinkAudioSource,					// CLSID
		CDecklinkAudioSource::CreateInstance,		// Method to create an instance of MyComponent
		NULL,										// Initialization function
		&sudDAS										// Set-up information (for filters)
	},
};

int g_cTemplates = sizeof(g_Templates) / sizeof(g_Templates[0]);

////////////////////////////////////////////////////////////////////////
//
// Exported entry points for registration and unregistration 
// (in this case they only call through to default implementations).
//
////////////////////////////////////////////////////////////////////////

STDAPI DllRegisterServer()
{
	return AMovieDllRegisterServer2(TRUE);
}

STDAPI DllUnregisterServer()
{
	return AMovieDllRegisterServer2(FALSE);
}

//
// DllEntryPoint
//
extern "C" BOOL WINAPI DllEntryPoint(HINSTANCE, ULONG, LPVOID);

BOOL APIENTRY DllMain(HANDLE hModule, 
                      DWORD  dwReason, 
                      LPVOID lpReserved)
{
	return DllEntryPoint((HINSTANCE)(hModule), dwReason, lpReserved);
}