using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using DirectShowLib;

namespace DecklinkCaptureLib.Filters
{
    [ComImport, Guid("5F534F6B-82F0-465B-9124-15913F3AC306")]
    class ChannelDownmixer
    {
    }

    class ChannelDownmixerLoader
    {
        private const string FilterName = "ChannelDownmixer.ax";
        private static IntPtr _hDllPtr = IntPtr.Zero;

        public static IBaseFilter LoadFilter()
        {
            return VideoHelpers.LoadFilter(VideoHelpers.FiltersDirectory + FilterName, typeof(ChannelDownmixer).GUID, ref _hDllPtr);
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
