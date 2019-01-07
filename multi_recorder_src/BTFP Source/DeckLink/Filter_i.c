

/* this ALWAYS GENERATED file contains the IIDs and CLSIDs */

/* link this file in with the server and any clients */


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

MIDL_DEFINE_GUID(IID, LIBID_RGBFiltersLib,0xB259B55F,0xF389,0x46b7,0x8B,0x38,0x42,0xE0,0x20,0xEE,0x0B,0xA9);


MIDL_DEFINE_GUID(CLSID, CLSID_StereoTransform4DeckLink,0xEF7E081F,0x59CF,0x4732,0x89,0xBD,0x53,0x75,0xF5,0x54,0x85,0x87);


MIDL_DEFINE_GUID(CLSID, CLSID_DeckLinkHDStereoSource,0x92541934,0x95FD,0x46e4,0xA7,0xF7,0x53,0xA3,0x04,0x51,0xE8,0xDC);


MIDL_DEFINE_GUID(IID, IID_IDeckLinkSourceConfig,0x2ECEB259,0x1516,0x4957,0xA7,0x01,0xA1,0x53,0x71,0x6A,0xA2,0x21);

#undef MIDL_DEFINE_GUID

#ifdef __cplusplus
}
#endif



