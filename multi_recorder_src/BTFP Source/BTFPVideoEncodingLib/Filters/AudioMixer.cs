using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using DirectShowLib;

namespace BTFPVideoEncodingLib.Filters
{
    [ComImport, Guid("77CED088-610B-4e3f-9705-035365649A41")]    
    public class AudioMixer
    {
    }

    public class AudioMixerFilterLoader
    {
        private static string FilterName = "AudioMixer.dll";
        private static IntPtr _hDllPtr = IntPtr.Zero;

        public AudioMixerFilterLoader()
        {
        }

        public static IBaseFilter LoadFilter()
        {
            return VideoHelpers.LoadFilter(VideoHelpers.FiltersDirectory + FilterName, typeof(AudioMixer).GUID, ref _hDllPtr);
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

    [ComImport, System.Security.SuppressUnmanagedCodeSecurity,
    Guid("719D1F20-A427-4D63-BA1B-36FBC0E1F85F"),
    InterfaceType(ComInterfaceType.InterfaceIsIUnknown)]
    public interface IAudioMixer
    {
        [PreserveSig]
        int SetVolume(double volume);
    }
}
