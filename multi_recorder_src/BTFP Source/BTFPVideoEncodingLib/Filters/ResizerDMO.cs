using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;

namespace BTFPVideoEncodingLib.Filters
{
    [ComImport, Guid("1EA1EA14-48F4-4054-AD1A-E8AEE10AC805")]
    public class ResizerDMOFilter
    {
    }

    /// <summary>
    /// Sets properties on the video resizer DSP.
    /// </summary>
    [ComImport, System.Security.SuppressUnmanagedCodeSecurity,
    Guid("57665d4c-0414-4faa-905b-10e546f81c33"),
    InterfaceType(ComInterfaceType.InterfaceIsIUnknown)]
    public interface IWMResizerProps
    {
        /// <summary>
        /// Specifies whether to use an algorithm that produces higher-quality video, or a faster algorithm. 
        /// </summary>
        /// <param name="lquality">Boolean value. If TRUE, the video resizer uses an algorithm that produces higher-quality video. 
        /// If FALSE, the video resizer uses a faster algorithm.</param>
        /// <returns></returns>
        [PreserveSig]
        int SetResizerQuality(
            [In] int lquality
            );

        /// <summary>
        /// The SetInterlaceMode method specifies whether the input video stream is interlaced. 
        /// </summary>
        /// <param name="lmode">Boolean value. If TRUE, the video is interlaced. If FALSE, the video is progressive. </param>
        /// <returns></returns>
        [PreserveSig]
        int SetInterlaceMode(
            [In] int lmode
            );

        /// <summary>
        /// Sets the source rectangle.
        /// </summary>
        /// <param name="lClipOriXSrc"></param>
        /// <param name="lClipOriYSrc"></param>
        /// <param name="lClipWidthSrc"></param>
        /// <param name="lClipHeightSrc"></param>
        /// <returns></returns>
        [PreserveSig]
        int SetClipRegion(
            [In] int lClipOriXSrc,
            [In] int lClipOriYSrc,
            [In] int lClipWidthSrc,
            [In] int lClipHeightSrc
            );

        /// <summary>
        /// Sets the source and destination rectangles.
        /// </summary>
        /// <param name="lClipOriXSrc"></param>
        /// <param name="lClipOriYSrc"></param>
        /// <param name="lClipWidthSrc"></param>
        /// <param name="lClipHeightSrc"></param>
        /// <param name="lClipOriXDst"></param>
        /// <param name="lClipOriYDst"></param>
        /// <param name="lClipWidthDst"></param>
        /// <param name="lClipHeightDst"></param>
        /// <returns></returns>
        [PreserveSig]
        int SetFullCropRegion(
            [In] int lClipOriXSrc,
            [In] int lClipOriYSrc,
            [In] int lClipWidthSrc,
            [In] int lClipHeightSrc,
            [In] int lClipOriXDst,
            [In] int lClipOriYDst,
            [In] int lClipWidthDst,
            [In] int lClipHeightDst
            );

        /// <summary>
        /// Retrieves the source and destination rectangles.
        /// </summary>
        /// <param name="lClipOriXSrc"></param>
        /// <param name="lClipOriYSrc"></param>
        /// <param name="lClipWidthSrc"></param>
        /// <param name="lClipHeightSrc"></param>
        /// <param name="lClipOriXDst"></param>
        /// <param name="lClipOriYDst"></param>
        /// <param name="lClipWidthDst"></param>
        /// <param name="lClipHeightDst"></param>
        /// <returns></returns>
        [PreserveSig]
        int GetFullCropRegion(
            [Out] out int lClipOriXSrc,
            [Out] out int lClipOriYSrc,
            [Out] out int lClipWidthSrc,
            [Out] out int lClipHeightSrc,
            [Out] out int lClipOriXDst,
            [Out] out int lClipOriYDst,
            [Out] out int lClipWidthDst,
            [Out] out int lClipHeightDst
            );
    }
}
