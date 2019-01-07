using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using DirectShowLib;

namespace BTFPVideoEncodingLib.Filters
{
    #region IMonogramAAC
    [ComImport, System.Security.SuppressUnmanagedCodeSecurity,
    Guid("4289BDC9-571F-41dc-B508-86B512B9F90C"),//IMonogramAAC interface
    InterfaceType(ComInterfaceType.InterfaceIsIUnknown)]
    public interface IMonogramAACEncoder
    {
        [PreserveSig]
        int GetConfig(ref AACConfig config);
        [PreserveSig]
        int SetConfig(ref AACConfig config);
        [PreserveSig]
        int GetInfo(ref AACInfo info);
    }
    #endregion

    #region MonogramAACEncoderFilter

    public class MonogramAACEncoderFilterLoader
    {
        private static string FilterName = "mmaace.ax";
        private static IntPtr _hDllPtr = IntPtr.Zero;

        public MonogramAACEncoderFilterLoader()
        {
        }

        public static IBaseFilter LoadFilter()
        {
            return VideoHelpers.LoadFilter(VideoHelpers.FiltersDirectory + FilterName, typeof(MonogramAACEncoderFilter).GUID, ref _hDllPtr);
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

    [ComImport, Guid("88F36DB6-D898-40b5-B409-466A0EECC26A")]
    public class MonogramAACEncoderFilter
    {
    }
    #endregion

    #region Structs
    [System.Runtime.InteropServices.StructLayoutAttribute(System.Runtime.InteropServices.LayoutKind.Sequential)]
    public struct AACConfig
    {
        public int version;
        public int object_type;
        public int output_type;
        public int bitrate;
    }

    [System.Runtime.InteropServices.StructLayoutAttribute(System.Runtime.InteropServices.LayoutKind.Sequential)]
    public struct AACInfo
    {
        public int samplerate;
        public int channels;
        public int frame_size;
        public long frames_done;
    }

    #endregion

    #region Enum
    public enum AACMPEGVersion { MPEG4 = 0, MPEG2 = 1 };

    public enum AACObjectType
    {
        AAC_OBJECT_MAIN = 1,
        AAC_OBJECT_LOW = 2,
        AAC_OBJECT_SSR = 3,
        AAC_OBJECT_LTP = 4
    };

    public enum AACOutputType
    {
        AAC_OUTPUT_RAW = 0,
        AAC_OUTPUT_ADTS = 1,
        AAC_OUTPUT_LATM = 2
    }

    #endregion
}
