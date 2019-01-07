using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using DirectShowLib;

namespace BTFPVideoEncodingLib.Filters
{
    #region IMonogramX264
    [ComImport, System.Security.SuppressUnmanagedCodeSecurity,
    Guid("56263DF8-1782-4916-806B-56D0A2FB55F0"),//IMonogramX264_2 interface
    InterfaceType(ComInterfaceType.InterfaceIsIUnknown)]
    public interface IMonogramX264Encoder
    {
        [PreserveSig]
        int SetBitrate(int bitrate_kbps);

        [PreserveSig]
        int GetDefaults(ref MONOGRAM_X264_CONFIG2 config);

        [PreserveSig]
        int GetConfig(ref MONOGRAM_X264_CONFIG2 config);

        [PreserveSig]
        int SetConfig(ref MONOGRAM_X264_CONFIG2 config);

        [PreserveSig]
        int GetVideoProps(ref MONOGRAM_X264_VIDEOPROPS props);

    }
    #endregion

    #region MonogramX264 Filter

    public class MonogramX264EncoderFilterLoader
    {
        private static string FilterName = "mmx264.ax";
        private static IntPtr _hDllPtr = IntPtr.Zero;

        public static IBaseFilter LoadFilter()
        {
            return VideoHelpers.LoadFilter(VideoHelpers.FiltersDirectory + FilterName, typeof(MonogramX264EncoderFilter).GUID, ref _hDllPtr);
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

    [ComImport, Guid("1FB0F046-623C-40a7-B439-41E4BFCB8BAB")]
    public class MonogramX264EncoderFilter
    {
    }
    #endregion

    #region HelixYUVCodec

    //public class HelixYV12CodecLoader
    //{
    //    private static string FilterName = "yv12vfw.dll";

    //    public static IBaseFilter LoadFilter()
    //    {
    //        return VideoHelpers.LoadFilter(VideoHelpers.FiltersDirectory + FilterName, typeof(HelixYV12Codec).GUID);
    //    }
    //}

    [ComImport, Guid("D76E2820-1563-11CF-AC98-00AA004C0FA9")]
    public class HelixYV12Codec
    {
    }
    #endregion

    #region Enums
    public enum AVCLevel
    {
        MONO_AVC_LEVEL_1 = 0,
        MONO_AVC_LEVEL_1B = 1,
        MONO_AVC_LEVEL_1_1 = 2,
        MONO_AVC_LEVEL_1_2 = 3,
        MONO_AVC_LEVEL_1_3 = 4,
        MONO_AVC_LEVEL_2 = 5,
        MONO_AVC_LEVEL_2_1 = 6,
        MONO_AVC_LEVEL_2_2 = 7,
        MONO_AVC_LEVEL_3 = 8,
        MONO_AVC_LEVEL_3_1 = 9,
        MONO_AVC_LEVEL_3_2 = 10,
        MONO_AVC_LEVEL_4 = 11,
        MONO_AVC_LEVEL_4_1 = 12,
        MONO_AVC_LEVEL_4_2 = 13,
        MONO_AVC_LEVEL_5 = 14,
        MONO_AVC_LEVEL_5_1 = 15,
    }

    public enum AVCRCMode
    {
        MONO_AVC_RC_MODE_ABR = 0,
        MONO_AVC_RC_MODE_CQP = 1,
        MONO_AVC_RC_MODE_CRF = 2,
    }

    public enum AVCInter
    {
        MONO_AVC_INTER_LOW = 0,
        MONO_AVC_INTER_MID = 1,
        MONO_AVC_INTER_HIGH = 2,
    }

    public enum AVCSubPel
    {
        MONO_AVC_SUBPEL_NO = 0,
        MONO_AVC_SUBPEL_HALF_LO = 1,
        MONO_AVC_SUBPEL_HALF_HI = 2,
        MONO_AVC_SUBPEL_QUART_LO = 3,
        MONO_AVC_SUBPEL_QUART_HI = 4,
    }

    public enum AVCTrellis
    {
        MONO_AVC_TRELLIS_DISABLED = 0,
        MONO_AVC_TRELLIS_MACROBLOCK = 1,
        MONO_AVC_TRELLIS_ALLMODES = 2,
    }

    public enum AVCDirectMV
    {
        MONO_AVC_DIRECTMV_NONE = 0,
        MONO_AVC_DIRECTMV_AUTO = 1,
        MONO_AVC_DIRECTMV_SPATIAL = 2,
        MONO_AVC_DIRECTMV_TEMPORAL = 3,
    }

    public enum AVCWeighted
    {
        MONO_AVC_WEIGHTED_DISABLED = 0,
        MONO_AVC_WEIGHTED_PFRAMES = 1,
        MONO_AVC_WEIGHTED_PANDBFRAMES = 2,
    }

    public enum AVCBFramesAdaptive
    {
        MONO_AVC_BFRAMESADAPTIVE_DISABLED = 0,
        MONO_AVC_BFRAMESADAPTIVE_FAST = 1,
        MONO_AVC_BFRAMESADAPTIVE_OPTIMAL = 2,
    }

    public enum AVCBPyramid
    {
        MONO_AVC_BPYRAMID_NONE = 0,
        MONO_AVC_BPYRAMID_STRICT = 1,
        MONO_AVC_BPYRAMID_NORMAL = 2,
    }
    #endregion

    #region Structs
    [System.Runtime.InteropServices.StructLayoutAttribute(System.Runtime.InteropServices.LayoutKind.Sequential)]
    public struct MONOGRAM_X264_VIDEOPROPS
    {
        public int width;
        public int height;
        public int fps_num;
        public int fps_den;
    }

    [System.Runtime.InteropServices.StructLayoutAttribute(System.Runtime.InteropServices.LayoutKind.Sequential)]
    public struct MONOGRAM_X264_CONFIG2
    {
        public int level;

        [System.Runtime.InteropServices.MarshalAsAttribute(System.Runtime.InteropServices.UnmanagedType.I1)]
        public bool parallel;

        [System.Runtime.InteropServices.MarshalAsAttribute(System.Runtime.InteropServices.UnmanagedType.I1)]
        public bool sliced_threading;

        [System.Runtime.InteropServices.MarshalAsAttribute(System.Runtime.InteropServices.UnmanagedType.I1)]
        public bool cabac;

        [System.Runtime.InteropServices.MarshalAsAttribute(System.Runtime.InteropServices.UnmanagedType.I1)]
        public bool interlaced;

        [System.Runtime.InteropServices.MarshalAsAttribute(System.Runtime.InteropServices.UnmanagedType.I1)]
        public bool deblocking;

        public int deblock_strength;

        [System.Runtime.InteropServices.MarshalAsAttribute(System.Runtime.InteropServices.UnmanagedType.I1)]
        public bool au_delimiters;

        [System.Runtime.InteropServices.MarshalAsAttribute(System.Runtime.InteropServices.UnmanagedType.I1)]
        public bool annexb;

        public int reference_frames;

        public int key_int_min;

        public int key_int_max;

        public int bframes;

        [System.Runtime.InteropServices.MarshalAsAttribute(System.Runtime.InteropServices.UnmanagedType.I1)]
        public bool periodic_intra_refresh;

        public int adaptive_b_frames;

        public int bframes_pyramid;

        public int rcmethod;

        public int bitrate_kbps;

        public int tolerance_percent;

        public int vbv_maxrate_kbps;

        public int vbv_buffer_size;

        public int qp_min;

        public int qp_max;

        public int const_qp;

        public int partitions_intra;

        public int partitions_inter;

        public int subpixel_refine;

        [System.Runtime.InteropServices.MarshalAsAttribute(System.Runtime.InteropServices.UnmanagedType.I1)]
        public bool mixed_references;

        public int weighted_prediction;

        public int direct_mv;

        public int trellis;
    }


    #endregion
}
