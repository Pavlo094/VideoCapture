

/* this ALWAYS GENERATED file contains the IIDs and CLSIDs */

/* link this file in with the server and any clients */


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


#ifdef __cplusplus
extern "C"{
#endif 


#include <rpc.h>
#include <rpcndr.h>

#ifdef _MIDL_USE_GUIDDEF_

#ifndef INITGUID
#define INITGUID
#include <guiddef.h>
#undef INITGUID
#else
#include <guiddef.h>
#endif

#define MIDL_DEFINE_GUID(type,name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8) \
        DEFINE_GUID(name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8)

#else // !_MIDL_USE_GUIDDEF_

#ifndef __IID_DEFINED__
#define __IID_DEFINED__

typedef struct _IID
{
    unsigned long x;
    unsigned short s1;
    unsigned short s2;
    unsigned char  c[8];
} IID;

#endif // __IID_DEFINED__

#ifndef CLSID_DEFINED
#define CLSID_DEFINED
typedef IID CLSID;
#endif // CLSID_DEFINED

#define MIDL_DEFINE_GUID(type,name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8) \
        const type name = {l,w1,w2,{b1,b2,b3,b4,b5,b6,b7,b8}}

#endif !_MIDL_USE_GUIDDEF_

MIDL_DEFINE_GUID(IID, LIBID_DecklinkDSLib,0x88715AD6,0x85BC,0x43ca,0xAD,0xD7,0x88,0xF2,0x66,0x25,0x15,0xDD);


MIDL_DEFINE_GUID(CLSID, CLSID_DecklinkVideoCaptureFilter,0x5DA49F6D,0xE638,0x4A55,0xBC,0xAE,0x1E,0xDA,0x48,0x86,0x9B,0x89);


MIDL_DEFINE_GUID(CLSID, CLSID_DecklinkVideoCaptureProperties,0x0B3E91CA,0xE153,0x44C4,0xA8,0x91,0xE3,0x0B,0xC3,0x78,0x90,0xCB);


MIDL_DEFINE_GUID(IID, IID_MEDIASUBTYPE_v210a,0x30313276,0x0000,0x0010,0x80,0x00,0x00,0xAA,0x00,0x38,0x9B,0x71);


MIDL_DEFINE_GUID(IID, IID_MEDIASUBTYPE_r210,0x30313272,0x0000,0x0010,0x80,0x00,0x00,0xAA,0x00,0x38,0x9B,0x71);


MIDL_DEFINE_GUID(IID, IID_MEDIASUBTYPE_BGRA32,0x41524742,0x0000,0x0010,0x80,0x00,0x00,0xAA,0x00,0x38,0x9B,0x71);


MIDL_DEFINE_GUID(CLSID, CLSID_DecklinkAudioCaptureFilter,0xF1006F2E,0xE81C,0x44bf,0xB2,0x45,0x09,0x30,0xA1,0x38,0x8D,0xD6);


MIDL_DEFINE_GUID(CLSID, CLSID_DecklinkAudioCaptureProperties,0xDF1553E9,0x2E09,0x4ce5,0x8F,0xE4,0xCA,0x9E,0xE3,0x67,0xBA,0x26);

#undef MIDL_DEFINE_GUID

#ifdef __cplusplus
}
#endif



