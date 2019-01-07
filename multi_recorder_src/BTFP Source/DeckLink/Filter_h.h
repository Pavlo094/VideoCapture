

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 8.00.0595 */
/* at Tue Mar 06 10:39:18 2018
 */
/* Compiler settings for Filter.idl:
    Oicf, W1, Zp8, env=Win32 (32b run), target_arch=X86 8.00.0595 
    protocol : dce , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
/* @@MIDL_FILE_HEADING(  ) */

#pragma warning( disable: 4049 )  /* more than 64k source lines */


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 475
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif // __RPCNDR_H_VERSION__


#ifndef __Filter_h_h__
#define __Filter_h_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __StereoTransform4DeckLink_FWD_DEFINED__
#define __StereoTransform4DeckLink_FWD_DEFINED__

#ifdef __cplusplus
typedef class StereoTransform4DeckLink StereoTransform4DeckLink;
#else
typedef struct StereoTransform4DeckLink StereoTransform4DeckLink;
#endif /* __cplusplus */

#endif 	/* __StereoTransform4DeckLink_FWD_DEFINED__ */


#ifndef __DeckLinkHDStereoSource_FWD_DEFINED__
#define __DeckLinkHDStereoSource_FWD_DEFINED__

#ifdef __cplusplus
typedef class DeckLinkHDStereoSource DeckLinkHDStereoSource;
#else
typedef struct DeckLinkHDStereoSource DeckLinkHDStereoSource;
#endif /* __cplusplus */

#endif 	/* __DeckLinkHDStereoSource_FWD_DEFINED__ */


#ifndef __IDeckLinkSourceConfig_FWD_DEFINED__
#define __IDeckLinkSourceConfig_FWD_DEFINED__
typedef interface IDeckLinkSourceConfig IDeckLinkSourceConfig;

#endif 	/* __IDeckLinkSourceConfig_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

#ifdef __cplusplus
extern "C"{
#endif 



#ifndef __RGBFiltersLib_LIBRARY_DEFINED__
#define __RGBFiltersLib_LIBRARY_DEFINED__

/* library RGBFiltersLib */
/* [version][uuid] */ 


EXTERN_C const IID LIBID_RGBFiltersLib;

EXTERN_C const CLSID CLSID_StereoTransform4DeckLink;

#ifdef __cplusplus

class DECLSPEC_UUID("EF7E081F-59CF-4732-89BD-5375F5548587")
StereoTransform4DeckLink;
#endif

EXTERN_C const CLSID CLSID_DeckLinkHDStereoSource;

#ifdef __cplusplus

class DECLSPEC_UUID("92541934-95FD-46e4-A7F7-53A30451E8DC")
DeckLinkHDStereoSource;
#endif

#ifndef __IDeckLinkSourceConfig_INTERFACE_DEFINED__
#define __IDeckLinkSourceConfig_INTERFACE_DEFINED__

/* interface IDeckLinkSourceConfig */
/* [helpstring][uuid][object] */ 


EXTERN_C const IID IID_IDeckLinkSourceConfig;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("2ECEB259-1516-4957-A701-A153716AA221")
    IDeckLinkSourceConfig : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE SetDeviceName( 
            /* [in] */ BSTR name) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetVideoMode( 
            /* [in] */ int width,
            /* [in] */ int height,
            /* [in] */ long long avgFrameDuration,
            /* [in] */ int fieldOrder) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IDeckLinkSourceConfigVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IDeckLinkSourceConfig * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IDeckLinkSourceConfig * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IDeckLinkSourceConfig * This);
        
        HRESULT ( STDMETHODCALLTYPE *SetDeviceName )( 
            IDeckLinkSourceConfig * This,
            /* [in] */ BSTR name);
        
        HRESULT ( STDMETHODCALLTYPE *SetVideoMode )( 
            IDeckLinkSourceConfig * This,
            /* [in] */ int width,
            /* [in] */ int height,
            /* [in] */ long long avgFrameDuration,
            /* [in] */ int fieldOrder);
        
        END_INTERFACE
    } IDeckLinkSourceConfigVtbl;

    interface IDeckLinkSourceConfig
    {
        CONST_VTBL struct IDeckLinkSourceConfigVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IDeckLinkSourceConfig_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IDeckLinkSourceConfig_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IDeckLinkSourceConfig_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IDeckLinkSourceConfig_SetDeviceName(This,name)	\
    ( (This)->lpVtbl -> SetDeviceName(This,name) ) 

#define IDeckLinkSourceConfig_SetVideoMode(This,width,height,avgFrameDuration,fieldOrder)	\
    ( (This)->lpVtbl -> SetVideoMode(This,width,height,avgFrameDuration,fieldOrder) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IDeckLinkSourceConfig_INTERFACE_DEFINED__ */

#endif /* __RGBFiltersLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


