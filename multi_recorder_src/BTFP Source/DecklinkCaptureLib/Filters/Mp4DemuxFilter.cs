using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using DirectShowLib;

namespace DecklinkCaptureLib.Filters
{
    [ComImport, Guid("025BE2E4-1787-4DA4-A585-C5B2B9EEB57C")]
    public class Mp4DemuxFilter
    {
    }

    public class GDCLMp4DemuxFilterLoader
    {
        private static string FilterName = "mp4demux.dll";
        private static IntPtr _hDllPtr = IntPtr.Zero;

        public static IBaseFilter LoadFilter()
        {
            return VideoHelpers.LoadFilter(VideoHelpers.FiltersDirectory + FilterName, typeof(GDCLMp4DemuxFilter).GUID, ref _hDllPtr);
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

    [ComImport, Guid("025BE2E4-1787-4DA4-A585-C5B2B9EEB57C")]
    public class GDCLMp4DemuxFilter
    {
    }
}
