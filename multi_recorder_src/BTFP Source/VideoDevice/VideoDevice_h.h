

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 8.00.0595 */
/* at Tue Mar 06 10:39:12 2018
 */
/* Compiler settings for VideoDevice.idl:
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


#ifndef __VideoDevice_h_h__
#define __VideoDevice_h_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IVideoDeviceMode_FWD_DEFINED__
#define __IVideoDeviceMode_FWD_DEFINED__
typedef interface IVideoDeviceMode IVideoDeviceMode;

#endif 	/* __IVideoDeviceMode_FWD_DEFINED__ */


#ifndef __IVideoDeviceModeIterator_FWD_DEFINED__
#define __IVideoDeviceModeIterator_FWD_DEFINED__
typedef interface IVideoDeviceModeIterator IVideoDeviceModeIterator;

#endif 	/* __IVideoDeviceModeIterator_FWD_DEFINED__ */


#ifndef __IVideoDevice_FWD_DEFINED__
#define __IVideoDevice_FWD_DEFINED__
typedef interface IVideoDevice IVideoDevice;

#endif 	/* __IVideoDevice_FWD_DEFINED__ */


#ifndef __IVideoDeviceHolder_FWD_DEFINED__
#define __IVideoDeviceHolder_FWD_DEFINED__
typedef interface IVideoDeviceHolder IVideoDeviceHolder;

#endif 	/* __IVideoDeviceHolder_FWD_DEFINED__ */


/* header files for imported files */
#include "unknwn.h"

#ifdef __cplusplus
extern "C"{
#endif 



#ifndef __VideoDeviceConfig_LIBRARY_DEFINED__
#define __VideoDeviceConfig_LIBRARY_DEFINED__

/* library VideoDeviceConfig */
/* [helpstring][version][uuid] */ 

typedef /* [v1_enum] */ 
enum _EFieldOrder
    {
        foDefault	= 0,
        foProgressive	= 0,
        foUpFieldFirst	= ( 1 << 1 ) ,
        foLowFieldFirst	= ( 1 << 2 ) ,
        foProgressiveSegmented	= ( 1 << 3 ) 
    } 	EFieldOrder;

typedef /* [v1_enum] */ 
enum _EPixelFormat
    {
        pfDefault	= 0,
        pfYUV2	= 0x32767579,
        pfYUYV	= 0x76797579,
        pfUYVY	= 0x79767975,
        pfV210	= 0x76323130,
        pfARGB	= 32,
        pfBGRA	= 0x42475241,
        pfR210	= 0x72323130,
        pfYV12	= 0x32315659
    } 	EPixelFormat;

typedef /* [v1_enum] */ 
enum _EStereoMode
    {
        smDefault	= 0,
        smLeft	= 0,
        smRight	= ( 1 << 1 ) ,
        smSideBySideHalf	= ( 1 << 2 ) ,
        smTopAndBottomHalf	= ( 1 << 3 ) ,
        smSideBySide	= ( 1 << 4 ) ,
        smTopAndBottom	= ( 1 << 5 ) ,
        smLineByLine	= ( 1 << 6 ) ,
        smLineByLineLeft	= ( 1 << 6 ) ,
        smLineByLineRight	= ( 1 << 7 ) ,
        smAnaglyph	= ( 1 << 8 ) 
    } 	EStereoMode;

typedef /* [v1_enum] */ 
enum _EVideoConnectionType
    {
        vctDefault	= ( 1 << 4 ) ,
        vctSDI	= ( 1 << 0 ) ,
        vctHDMI	= ( 1 << 1 ) ,
        vctOpticalSDI	= ( 1 << 2 ) ,
        vctComponent	= ( 1 << 3 ) ,
        vctComposite	= ( 1 << 4 ) ,
        vctSVideo	= ( 1 << 5 ) 
    } 	EVideoConnectionType;






EXTERN_C const IID LIBID_VideoDeviceConfig;

#ifndef __IVideoDeviceMode_INTERFACE_DEFINED__
#define __IVideoDeviceMode_INTERFACE_DEFINED__

/* interface IVideoDeviceMode */
/* [helpstring][uuid][object] */ 


EXTERN_C const IID IID_IVideoDeviceMode;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("D22D583C-C2EA-4d39-B899-584189E8ADD1")
    IVideoDeviceMode : public IUnknown
    {
    public:
        virtual const int STDMETHODCALLTYPE GetWidth( void) = 0;
        
        virtual const int STDMETHODCALLTYPE GetHeight( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetWidth( 
            /* [in] */ const int width) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetHeight( 
            /* [in] */ const int height) = 0;
        
        virtual const int STDMETHODCALLTYPE GetBpp( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetBpp( 
            /* [in] */ const int bpp) = 0;
        
        virtual EPixelFormat STDMETHODCALLTYPE GetPixelFormat( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetPixelFormat( 
            /* [in] */ const EPixelFormat pixelFormat) = 0;
        
        virtual EFieldOrder STDMETHODCALLTYPE GetFieldOrder( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetFieldOrder( 
            /* [in] */ const EFieldOrder fieldOrder) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetTimeUnitsPerSecond( 
            /* [in] */ const DWORD unitsCount) = 0;
        
        virtual const DWORD STDMETHODCALLTYPE GetFrameDuration( void) = 0;
        
        virtual const double STDMETHODCALLTYPE GetFramesPerSecond( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetFrameDuration( 
            /* [in] */ const DWORD frameDuration) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetFramesPerSecond( 
            /* [in] */ const DWORD framesCount) = 0;
        
        virtual EStereoMode STDMETHODCALLTYPE GetStereoMode( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetStereoMode( 
            /* [in] */ const EStereoMode stereoMode) = 0;
        
        virtual EVideoConnectionType STDMETHODCALLTYPE GetConnectionType( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetConnectionType( 
            /* [in] */ const EVideoConnectionType connectionType) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Compare( 
            /* [in] */ IVideoDeviceMode *pMode) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IVideoDeviceModeVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IVideoDeviceMode * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IVideoDeviceMode * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IVideoDeviceMode * This);
        
        const int ( STDMETHODCALLTYPE *GetWidth )( 
            IVideoDeviceMode * This);
        
        const int ( STDMETHODCALLTYPE *GetHeight )( 
            IVideoDeviceMode * This);
        
        HRESULT ( STDMETHODCALLTYPE *SetWidth )( 
            IVideoDeviceMode * This,
            /* [in] */ const int width);
        
        HRESULT ( STDMETHODCALLTYPE *SetHeight )( 
            IVideoDeviceMode * This,
            /* [in] */ const int height);
        
        const int ( STDMETHODCALLTYPE *GetBpp )( 
            IVideoDeviceMode * This);
        
        HRESULT ( STDMETHODCALLTYPE *SetBpp )( 
            IVideoDeviceMode * This,
            /* [in] */ const int bpp);
        
        EPixelFormat ( STDMETHODCALLTYPE *GetPixelFormat )( 
            IVideoDeviceMode * This);
        
        HRESULT ( STDMETHODCALLTYPE *SetPixelFormat )( 
            IVideoDeviceMode * This,
            /* [in] */ const EPixelFormat pixelFormat);
        
        EFieldOrder ( STDMETHODCALLTYPE *GetFieldOrder )( 
            IVideoDeviceMode * This);
        
        HRESULT ( STDMETHODCALLTYPE *SetFieldOrder )( 
            IVideoDeviceMode * This,
            /* [in] */ const EFieldOrder fieldOrder);
        
        HRESULT ( STDMETHODCALLTYPE *SetTimeUnitsPerSecond )( 
            IVideoDeviceMode * This,
            /* [in] */ const DWORD unitsCount);
        
        const DWORD ( STDMETHODCALLTYPE *GetFrameDuration )( 
            IVideoDeviceMode * This);
        
        const double ( STDMETHODCALLTYPE *GetFramesPerSecond )( 
            IVideoDeviceMode * This);
        
        HRESULT ( STDMETHODCALLTYPE *SetFrameDuration )( 
            IVideoDeviceMode * This,
            /* [in] */ const DWORD frameDuration);
        
        HRESULT ( STDMETHODCALLTYPE *SetFramesPerSecond )( 
            IVideoDeviceMode * This,
            /* [in] */ const DWORD framesCount);
        
        EStereoMode ( STDMETHODCALLTYPE *GetStereoMode )( 
            IVideoDeviceMode * This);
        
        HRESULT ( STDMETHODCALLTYPE *SetStereoMode )( 
            IVideoDeviceMode * This,
            /* [in] */ const EStereoMode stereoMode);
        
        EVideoConnectionType ( STDMETHODCALLTYPE *GetConnectionType )( 
            IVideoDeviceMode * This);
        
        HRESULT ( STDMETHODCALLTYPE *SetConnectionType )( 
            IVideoDeviceMode * This,
            /* [in] */ const EVideoConnectionType connectionType);
        
        HRESULT ( STDMETHODCALLTYPE *Compare )( 
            IVideoDeviceMode * This,
            /* [in] */ IVideoDeviceMode *pMode);
        
        END_INTERFACE
    } IVideoDeviceModeVtbl;

    interface IVideoDeviceMode
    {
        CONST_VTBL struct IVideoDeviceModeVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IVideoDeviceMode_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IVideoDeviceMode_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IVideoDeviceMode_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IVideoDeviceMode_GetWidth(This)	\
    ( (This)->lpVtbl -> GetWidth(This) ) 

#define IVideoDeviceMode_GetHeight(This)	\
    ( (This)->lpVtbl -> GetHeight(This) ) 

#define IVideoDeviceMode_SetWidth(This,width)	\
    ( (This)->lpVtbl -> SetWidth(This,width) ) 

#define IVideoDeviceMode_SetHeight(This,height)	\
    ( (This)->lpVtbl -> SetHeight(This,height) ) 

#define IVideoDeviceMode_GetBpp(This)	\
    ( (This)->lpVtbl -> GetBpp(This) ) 

#define IVideoDeviceMode_SetBpp(This,bpp)	\
    ( (This)->lpVtbl -> SetBpp(This,bpp) ) 

#define IVideoDeviceMode_GetPixelFormat(This)	\
    ( (This)->lpVtbl -> GetPixelFormat(This) ) 

#define IVideoDeviceMode_SetPixelFormat(This,pixelFormat)	\
    ( (This)->lpVtbl -> SetPixelFormat(This,pixelFormat) ) 

#define IVideoDeviceMode_GetFieldOrder(This)	\
    ( (This)->lpVtbl -> GetFieldOrder(This) ) 

#define IVideoDeviceMode_SetFieldOrder(This,fieldOrder)	\
    ( (This)->lpVtbl -> SetFieldOrder(This,fieldOrder) ) 

#define IVideoDeviceMode_SetTimeUnitsPerSecond(This,unitsCount)	\
    ( (This)->lpVtbl -> SetTimeUnitsPerSecond(This,unitsCount) ) 

#define IVideoDeviceMode_GetFrameDuration(This)	\
    ( (This)->lpVtbl -> GetFrameDuration(This) ) 

#define IVideoDeviceMode_GetFramesPerSecond(This)	\
    ( (This)->lpVtbl -> GetFramesPerSecond(This) ) 

#define IVideoDeviceMode_SetFrameDuration(This,frameDuration)	\
    ( (This)->lpVtbl -> SetFrameDuration(This,frameDuration) ) 

#define IVideoDeviceMode_SetFramesPerSecond(This,framesCount)	\
    ( (This)->lpVtbl -> SetFramesPerSecond(This,framesCount) ) 

#define IVideoDeviceMode_GetStereoMode(This)	\
    ( (This)->lpVtbl -> GetStereoMode(This) ) 

#define IVideoDeviceMode_SetStereoMode(This,stereoMode)	\
    ( (This)->lpVtbl -> SetStereoMode(This,stereoMode) ) 

#define IVideoDeviceMode_GetConnectionType(This)	\
    ( (This)->lpVtbl -> GetConnectionType(This) ) 

#define IVideoDeviceMode_SetConnectionType(This,connectionType)	\
    ( (This)->lpVtbl -> SetConnectionType(This,connectionType) ) 

#define IVideoDeviceMode_Compare(This,pMode)	\
    ( (This)->lpVtbl -> Compare(This,pMode) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IVideoDeviceMode_INTERFACE_DEFINED__ */


#ifndef __IVideoDeviceModeIterator_INTERFACE_DEFINED__
#define __IVideoDeviceModeIterator_INTERFACE_DEFINED__

/* interface IVideoDeviceModeIterator */
/* [helpstring][uuid][object] */ 


EXTERN_C const IID IID_IVideoDeviceModeIterator;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("DCFF34D8-2C19-4e0d-9E1A-8DC8D208748B")
    IVideoDeviceModeIterator : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Next( 
            /* [out] */ IVideoDeviceMode **pVideoMode) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IVideoDeviceModeIteratorVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IVideoDeviceModeIterator * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IVideoDeviceModeIterator * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IVideoDeviceModeIterator * This);
        
        HRESULT ( STDMETHODCALLTYPE *Next )( 
            IVideoDeviceModeIterator * This,
            /* [out] */ IVideoDeviceMode **pVideoMode);
        
        END_INTERFACE
    } IVideoDeviceModeIteratorVtbl;

    interface IVideoDeviceModeIterator
    {
        CONST_VTBL struct IVideoDeviceModeIteratorVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IVideoDeviceModeIterator_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IVideoDeviceModeIterator_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IVideoDeviceModeIterator_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IVideoDeviceModeIterator_Next(This,pVideoMode)	\
    ( (This)->lpVtbl -> Next(This,pVideoMode) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IVideoDeviceModeIterator_INTERFACE_DEFINED__ */


#ifndef __IVideoDevice_INTERFACE_DEFINED__
#define __IVideoDevice_INTERFACE_DEFINED__

/* interface IVideoDevice */
/* [helpstring][uuid][object] */ 


EXTERN_C const IID IID_IVideoDevice;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("D55A1F71-CFE7-4dd7-BFF8-C6DEA3A1BCB3")
    IVideoDevice : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE IsVideoModeSupported( 
            /* [in] */ IVideoDeviceMode *videoMode) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE UpdateVideoModes( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetModesIterator( 
            /* [out] */ IVideoDeviceModeIterator **pIterator) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetVideoMode( 
            /* [in] */ IVideoDeviceMode *videoMode) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetCurrentVideoMode( 
            /* [out] */ IVideoDeviceMode **pVideoMode) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetDeviceName( 
            /* [out] */ BSTR *name) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IVideoDeviceVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IVideoDevice * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IVideoDevice * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IVideoDevice * This);
        
        HRESULT ( STDMETHODCALLTYPE *IsVideoModeSupported )( 
            IVideoDevice * This,
            /* [in] */ IVideoDeviceMode *videoMode);
        
        HRESULT ( STDMETHODCALLTYPE *UpdateVideoModes )( 
            IVideoDevice * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetModesIterator )( 
            IVideoDevice * This,
            /* [out] */ IVideoDeviceModeIterator **pIterator);
        
        HRESULT ( STDMETHODCALLTYPE *SetVideoMode )( 
            IVideoDevice * This,
            /* [in] */ IVideoDeviceMode *videoMode);
        
        HRESULT ( STDMETHODCALLTYPE *GetCurrentVideoMode )( 
            IVideoDevice * This,
            /* [out] */ IVideoDeviceMode **pVideoMode);
        
        HRESULT ( STDMETHODCALLTYPE *GetDeviceName )( 
            IVideoDevice * This,
            /* [out] */ BSTR *name);
        
        END_INTERFACE
    } IVideoDeviceVtbl;

    interface IVideoDevice
    {
        CONST_VTBL struct IVideoDeviceVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IVideoDevice_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IVideoDevice_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IVideoDevice_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IVideoDevice_IsVideoModeSupported(This,videoMode)	\
    ( (This)->lpVtbl -> IsVideoModeSupported(This,videoMode) ) 

#define IVideoDevice_UpdateVideoModes(This)	\
    ( (This)->lpVtbl -> UpdateVideoModes(This) ) 

#define IVideoDevice_GetModesIterator(This,pIterator)	\
    ( (This)->lpVtbl -> GetModesIterator(This,pIterator) ) 

#define IVideoDevice_SetVideoMode(This,videoMode)	\
    ( (This)->lpVtbl -> SetVideoMode(This,videoMode) ) 

#define IVideoDevice_GetCurrentVideoMode(This,pVideoMode)	\
    ( (This)->lpVtbl -> GetCurrentVideoMode(This,pVideoMode) ) 

#define IVideoDevice_GetDeviceName(This,name)	\
    ( (This)->lpVtbl -> GetDeviceName(This,name) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IVideoDevice_INTERFACE_DEFINED__ */


#ifndef __IVideoDeviceHolder_INTERFACE_DEFINED__
#define __IVideoDeviceHolder_INTERFACE_DEFINED__

/* interface IVideoDeviceHolder */
/* [helpstring][uuid][object] */ 


EXTERN_C const IID IID_IVideoDeviceHolder;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("99A44D89-9EC9-40ab-A8A8-F145D1D5FC57")
    IVideoDeviceHolder : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE LockDevice( 
            IVideoDevice **ppDevide) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE UnLockDevice( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Save( void) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IVideoDeviceHolderVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IVideoDeviceHolder * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IVideoDeviceHolder * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IVideoDeviceHolder * This);
        
        HRESULT ( STDMETHODCALLTYPE *LockDevice )( 
            IVideoDeviceHolder * This,
            IVideoDevice **ppDevide);
        
        HRESULT ( STDMETHODCALLTYPE *UnLockDevice )( 
            IVideoDeviceHolder * This);
        
        HRESULT ( STDMETHODCALLTYPE *Save )( 
            IVideoDeviceHolder * This);
        
        END_INTERFACE
    } IVideoDeviceHolderVtbl;

    interface IVideoDeviceHolder
    {
        CONST_VTBL struct IVideoDeviceHolderVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IVideoDeviceHolder_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IVideoDeviceHolder_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IVideoDeviceHolder_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IVideoDeviceHolder_LockDevice(This,ppDevide)	\
    ( (This)->lpVtbl -> LockDevice(This,ppDevide) ) 

#define IVideoDeviceHolder_UnLockDevice(This)	\
    ( (This)->lpVtbl -> UnLockDevice(This) ) 

#define IVideoDeviceHolder_Save(This)	\
    ( (This)->lpVtbl -> Save(This) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IVideoDeviceHolder_INTERFACE_DEFINED__ */

#endif /* __VideoDeviceConfig_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


