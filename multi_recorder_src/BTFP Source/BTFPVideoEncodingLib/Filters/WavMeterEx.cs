using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;

namespace BTFPVideoEncodingLib.Filters
{
    [ComImport, Guid("BCA04069-E36C-40E4-9CD2-D08E7528F743")]
    class WavMeterEx
    {
    }


    [ComImport, System.Security.SuppressUnmanagedCodeSecurity,
    Guid("EF67001A-EA9B-4305-8F81-6D8A800C417E"),
    InterfaceType(ComInterfaceType.InterfaceIsIUnknown)]
    public interface IWavMeterEx
    {
        [PreserveSig]
        int GetPeaks([In, Out, MarshalAs(UnmanagedType.LPArray, SizeConst = 6)]double[] peaks);

        [PreserveSig]
        int SetVolume([In, MarshalAs(UnmanagedType.LPArray, SizeConst = 6)]double[] volumes);

        [PreserveSig]
        int SetMute([In, MarshalAs(UnmanagedType.LPArray, ArraySubType = UnmanagedType.Bool, SizeConst = 6)]bool[] mutes);
    }
}
