using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using DirectShowLib;

namespace DecklinkCaptureLib.Filters
{
    //[ComImport, Guid("5DA49F6D-E638-4A55-BCAE-1EDA48869B89")]
    public class DecklinkSourceLoader
    {
        private static string FilterName = "DSDeckLinkSource.dll";
        private static IntPtr _hDllPtr = IntPtr.Zero;

        public static IBaseFilter LoadFilter()
        {
            IBaseFilter fltr = VideoHelpers.LoadFilter(VideoHelpers.FiltersDirectory + FilterName, typeof(DeckLinkHDStereoSource).GUID, ref _hDllPtr);
            return fltr;
        }

        public static void Unload()
        {
            if (_hDllPtr != IntPtr.Zero)
            {
                NativeHelpers.CoFreeLibrary(_hDllPtr);
                _hDllPtr = IntPtr.Zero;
            }
        }

        public static void ConfigureFilter(IBaseFilter filter, string deviceName, int width, int height, long avgFrameDuration, int fieldOrder)
        {
            RGBFiltersLib.IDeckLinkSourceConfig config = (RGBFiltersLib.IDeckLinkSourceConfig)filter;
            if (config == null) return;
            config.SetDeviceName(deviceName);
            config.SetVideoMode(width, height, avgFrameDuration, fieldOrder);
        }
    }

    [ComImport, Guid("92541934-95FD-46e4-A7F7-53A30451E8DC")]
    public class DeckLinkHDStereoSource
    {
    }
}
