

/* this ALWAYS GENERATED file contains the IIDs and CLSIDs */

/* link this file in with the server and any clients */


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

MIDL_DEFINE_GUID(IID, LIBID_VideoDeviceConfig,0x31E1C420,0x5E88,0x4667,0x8A,0x36,0x76,0x17,0x33,0xA3,0xB3,0x48);


MIDL_DEFINE_GUID(IID, IID_IVideoDeviceMode,0xD22D583C,0xC2EA,0x4d39,0xB8,0x99,0x58,0x41,0x89,0xE8,0xAD,0xD1);


MIDL_DEFINE_GUID(IID, IID_IVideoDeviceModeIterator,0xDCFF34D8,0x2C19,0x4e0d,0x9E,0x1A,0x8D,0xC8,0xD2,0x08,0x74,0x8B);


MIDL_DEFINE_GUID(IID, IID_IVideoDevice,0xD55A1F71,0xCFE7,0x4dd7,0xBF,0xF8,0xC6,0xDE,0xA3,0xA1,0xBC,0xB3);


MIDL_DEFINE_GUID(IID, IID_IVideoDeviceHolder,0x99A44D89,0x9EC9,0x40ab,0xA8,0xA8,0xF1,0x45,0xD1,0xD5,0xFC,0x57);

#undef MIDL_DEFINE_GUID

#ifdef __cplusplus
}
#endif



