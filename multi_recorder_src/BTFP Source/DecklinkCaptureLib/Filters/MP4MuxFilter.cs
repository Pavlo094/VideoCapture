using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using DirectShowLib;

namespace DecklinkCaptureLib.Filters
{
    public class MonogramMP4MuxFilterLoader
    {
        private static string FilterName = "mmmp4mux.dll";
        private static IntPtr _hDllPtr = IntPtr.Zero;

        public static IBaseFilter LoadFilter()
        {
            return VideoHelpers.LoadFilter(VideoHelpers.FiltersDirectory + FilterName, typeof(MonogramMP4MuxFilter).GUID, ref _hDllPtr);
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

    [ComImport, Guid("5D33564D-873C-47FB-90AD-C6B2657ECE1A")]
    public class MonogramMP4MuxFilter
    {
    }

    public class GDCLMP4MuxFilterLoader
    {
        private static string FilterName = "mp4mux.dll";
        private static IntPtr _hDllPtr = IntPtr.Zero;

        public static IBaseFilter LoadFilter()
        {
            return VideoHelpers.LoadFilter(VideoHelpers.FiltersDirectory + FilterName, typeof(GDCLMP4MuxFilter).GUID, ref _hDllPtr);
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

    [ComImport, Guid("5FD85181-E542-4E52-8D9D-5D613C30131B")]
    public class GDCLMP4MuxFilter
    {
    }
}
