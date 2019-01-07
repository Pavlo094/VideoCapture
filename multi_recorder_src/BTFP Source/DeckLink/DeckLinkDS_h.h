

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 8.00.0595 */
/* at Tue Mar 06 10:39:17 2018
 */
/* Compiler settings for DeckLinkDS.idl:
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


#ifndef __DeckLinkDS_h_h__
#define __DeckLinkDS_h_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __DecklinkVideoCaptureFilter_FWD_DEFINED__
#define __DecklinkVideoCaptureFilter_FWD_DEFINED__

#ifdef __cplusplus
typedef class DecklinkVideoCaptureFilter DecklinkVideoCaptureFilter;
#else
typedef struct DecklinkVideoCaptureFilter DecklinkVideoCaptureFilter;
#endif /* __cplusplus */

#endif 	/* __DecklinkVideoCaptureFilter_FWD_DEFINED__ */


#ifndef __DecklinkVideoCaptureProperties_FWD_DEFINED__
#define __DecklinkVideoCaptureProperties_FWD_DEFINED__

#ifdef __cplusplus
typedef class DecklinkVideoCaptureProperties DecklinkVideoCaptureProperties;
#else
typedef struct DecklinkVideoCaptureProperties DecklinkVideoCaptureProperties;
#endif /* __cplusplus */

#endif 	/* __DecklinkVideoCaptureProperties_FWD_DEFINED__ */


#ifndef __MEDIASUBTYPE_v210a_FWD_DEFINED__
#define __MEDIASUBTYPE_v210a_FWD_DEFINED__
typedef interface MEDIASUBTYPE_v210a MEDIASUBTYPE_v210a;

#endif 	/* __MEDIASUBTYPE_v210a_FWD_DEFINED__ */


#ifndef __MEDIASUBTYPE_r210_FWD_DEFINED__
#define __MEDIASUBTYPE_r210_FWD_DEFINED__
typedef interface MEDIASUBTYPE_r210 MEDIASUBTYPE_r210;

#endif 	/* __MEDIASUBTYPE_r210_FWD_DEFINED__ */


#ifndef __MEDIASUBTYPE_BGRA32_FWD_DEFINED__
#define __MEDIASUBTYPE_BGRA32_FWD_DEFINED__
typedef interface MEDIASUBTYPE_BGRA32 MEDIASUBTYPE_BGRA32;

#endif 	/* __MEDIASUBTYPE_BGRA32_FWD_DEFINED__ */


#ifndef __DecklinkAudioCaptureFilter_FWD_DEFINED__
#define __DecklinkAudioCaptureFilter_FWD_DEFINED__

#ifdef __cplusplus
typedef class DecklinkAudioCaptureFilter DecklinkAudioCaptureFilter;
#else
typedef struct DecklinkAudioCaptureFilter DecklinkAudioCaptureFilter;
#endif /* __cplusplus */

#endif 	/* __DecklinkAudioCaptureFilter_FWD_DEFINED__ */


#ifndef __DecklinkAudioCaptureProperties_FWD_DEFINED__
#define __DecklinkAudioCaptureProperties_FWD_DEFINED__

#ifdef __cplusplus
typedef class DecklinkAudioCaptureProperties DecklinkAudioCaptureProperties;
#else
typedef struct DecklinkAudioCaptureProperties DecklinkAudioCaptureProperties;
#endif /* __cplusplus */

#endif 	/* __DecklinkAudioCaptureProperties_FWD_DEFINED__ */


/* header files for imported files */
#include "unknwn.h"

#ifdef __cplusplus
extern "C"{
#endif 



#ifndef __DecklinkDSLib_LIBRARY_DEFINED__
#define __DecklinkDSLib_LIBRARY_DEFINED__

/* library DecklinkDSLib */
/* [helpstring][version][uuid] */ 


EXTERN_C const IID LIBID_DecklinkDSLib;

EXTERN_C const CLSID CLSID_DecklinkVideoCaptureFilter;

#ifdef __cplusplus

class DECLSPEC_UUID("5DA49F6D-E638-4A55-BCAE-1EDA48869B89")
DecklinkVideoCaptureFilter;
#endif

EXTERN_C const CLSID CLSID_DecklinkVideoCaptureProperties;

#ifdef __cplusplus

class DECLSPEC_UUID("0B3E91CA-E153-44C4-A891-E30BC37890CB")
DecklinkVideoCaptureProperties;
#endif

#ifndef __MEDIASUBTYPE_v210a_INTERFACE_DEFINED__
#define __MEDIASUBTYPE_v210a_INTERFACE_DEFINED__

/* interface MEDIASUBTYPE_v210a */
/* [version][uuid] */ 


EXTERN_C const IID IID_MEDIASUBTYPE_v210a;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("30313276-0000-0010-8000-00AA00389B71")
    MEDIASUBTYPE_v210a
    {
    public:
        BEGIN_INTERFACE
        END_INTERFACE
    };
    
    
#else 	/* C style interface */

    typedef struct MEDIASUBTYPE_v210aVtbl
    {
        BEGIN_INTERFACE
        
        END_INTERFACE
    } MEDIASUBTYPE_v210aVtbl;

    interface MEDIASUBTYPE_v210a
    {
        CONST_VTBL struct MEDIASUBTYPE_v210aVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __MEDIASUBTYPE_v210a_INTERFACE_DEFINED__ */


#ifndef __MEDIASUBTYPE_r210_INTERFACE_DEFINED__
#define __MEDIASUBTYPE_r210_INTERFACE_DEFINED__

/* interface MEDIASUBTYPE_r210 */
/* [version][uuid] */ 


EXTERN_C const IID IID_MEDIASUBTYPE_r210;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("30313272-0000-0010-8000-00AA00389B71")
    MEDIASUBTYPE_r210
    {
    public:
        BEGIN_INTERFACE
        END_INTERFACE
    };
    
    
#else 	/* C style interface */

    typedef struct MEDIASUBTYPE_r210Vtbl
    {
        BEGIN_INTERFACE
        
        END_INTERFACE
    } MEDIASUBTYPE_r210Vtbl;

    interface MEDIASUBTYPE_r210
    {
        CONST_VTBL struct MEDIASUBTYPE_r210Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __MEDIASUBTYPE_r210_INTERFACE_DEFINED__ */


#ifndef __MEDIASUBTYPE_BGRA32_INTERFACE_DEFINED__
#define __MEDIASUBTYPE_BGRA32_INTERFACE_DEFINED__

/* interface MEDIASUBTYPE_BGRA32 */
/* [version][uuid] */ 


EXTERN_C const IID IID_MEDIASUBTYPE_BGRA32;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("41524742-0000-0010-8000-00AA00389B71")
    MEDIASUBTYPE_BGRA32
    {
    public:
        BEGIN_INTERFACE
        END_INTERFACE
    };
    
    
#else 	/* C style interface */

    typedef struct MEDIASUBTYPE_BGRA32Vtbl
    {
        BEGIN_INTERFACE
        
        END_INTERFACE
    } MEDIASUBTYPE_BGRA32Vtbl;

    interface MEDIASUBTYPE_BGRA32
    {
        CONST_VTBL struct MEDIASUBTYPE_BGRA32Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __MEDIASUBTYPE_BGRA32_INTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_DecklinkAudioCaptureFilter;

#ifdef __cplusplus

class DECLSPEC_UUID("F1006F2E-E81C-44bf-B245-0930A1388DD6")
DecklinkAudioCaptureFilter;
#endif

EXTERN_C const CLSID CLSID_DecklinkAudioCaptureProperties;

#ifdef __cplusplus

class DECLSPEC_UUID("DF1553E9-2E09-4ce5-8FE4-CA9EE367BA26")
DecklinkAudioCaptureProperties;
#endif
#endif /* __DecklinkDSLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


