using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using DirectShowLib;

namespace DecklinkCaptureLib.Filters
{
    public class LavAudioDecoderFilterLoader
    {
        private static string FilterName = "LAVAudio.ax";
        private static IntPtr _hDllPtr = IntPtr.Zero;

        public static IBaseFilter LoadFilter()
        {
            return VideoHelpers.LoadFilter(VideoHelpers.FiltersDirectory + FilterName, typeof(LavAudioDecoderFilter).GUID, ref _hDllPtr);
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

    [ComImport, Guid("E8E73B6B-4CB3-44A4-BE99-4F7BCB96E491")]
    public class LavAudioDecoderFilter
    {
    }
}
