using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using System.Text;
using DirectShowLib;

namespace DecklinkCaptureLib.Filters
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
        int GetPeaks([In, Out, MarshalAs(UnmanagedType.LPArray, SizeConst = 8)]double[] peaks);

        [PreserveSig]
        int SetVolume([In, MarshalAs(UnmanagedType.LPArray, SizeConst = 8)]double[] volumes);

        [PreserveSig]
        int SetMute([In, MarshalAs(UnmanagedType.LPArray, ArraySubType = UnmanagedType.Bool, SizeConst = 6)]bool[] mutes);
    }
    class WavMeterExLoader
    {
        private const string FilterName = "WavMeterEx.ax";
        private static IntPtr _hDllPtr = IntPtr.Zero;

        public static IBaseFilter LoadFilter()
        {
            return VideoHelpers.LoadFilter(VideoHelpers.FiltersDirectory + FilterName, typeof(WavMeterEx).GUID, ref _hDllPtr);
        }

        public static void Unload()
        {
            if (_hDllPtr != IntPtr.Zero)
            {
                NativeHelpers.CoFreeLibrary(_hDllPtr);
                _hDllPtr = IntPtr.Zero;
            }
        }
    }
}
