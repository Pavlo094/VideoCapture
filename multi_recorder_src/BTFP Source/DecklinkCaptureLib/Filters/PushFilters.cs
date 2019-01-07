using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using DirectShowLib;

namespace DecklinkCaptureLib.Filters
{
    [ComImport, Guid("7FB052B4-E14E-43DA-B8BD-B9004CAFEE87")]
    class VideoPushSource
    {
    }
    [ComImport, Guid("521BD957-D0E4-4BF3-A7EE-1D5DBA376428")]
    class AudioPushSource
    {
        
    }

    [ComImport, System.Security.SuppressUnmanagedCodeSecurity,
    Guid("A3E69173-A6AF-448D-8770-9FB6A8143EE8"),
    InterfaceType(ComInterfaceType.InterfaceIsIUnknown)]
    public interface IAVPushSource
    {
        [PreserveSig]
        int GetFrameBuffer(ref IntPtr ppBuffer, ref uint pSize);

        [PreserveSig]
        int Deliver(IntPtr pBuffer);
    }

    [ComImport, System.Security.SuppressUnmanagedCodeSecurity,
   Guid("E5A7DBA7-B09F-4564-AC2A-E0975232091A"),
   InterfaceType(ComInterfaceType.InterfaceIsIUnknown)]
    public interface IAVPushSource2
    {
        [PreserveSig]
        int GetFrameBuffer(out IntPtr ppSample);

        [PreserveSig]
        int Deliver(IntPtr pSample);
    }

    [ComImport, System.Security.SuppressUnmanagedCodeSecurity,
   Guid("B1B6A2BF-577F-4A7C-A19F-B08A93ACE343"),
   InterfaceType(ComInterfaceType.InterfaceIsIUnknown)]
    public interface IAVPushSource3
    {
        [PreserveSig]
        int GetFrameBufferEx(out IntPtr ppBuffer, out uint pSize);

        [PreserveSig]
        int DeliverEx(IntPtr pBuffer, uint cbSize);
    }
}
