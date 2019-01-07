using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using DirectShowLib;
using DirectShowLib.Utils;
using DecklinkCaptureLib.Filters;
using System.Runtime.InteropServices;
using System.Runtime.InteropServices.ComTypes;
using MediaFoundation;
using MediaFoundation.EVR;
using MediaFoundation.Misc;
using NLog;

namespace DecklinkCaptureLib
{
    public class DSUtils
    {
        #region Logger
        private static readonly Logger Log = LogManager.GetCurrentClassLogger();
        #endregion

        /// <summary>
        /// Connects 2 directshow filters.
        /// </summary>
        /// <param name="pGraph"></param>
        /// <param name="pOutPin"></param>
        /// <param name="pDstFilter"></param>
        /// <returns></returns>
        public static int ConnectOutputPin(IGraphBuilder pGraph, IPin pOutPin, IBaseFilter pDstFilter)
        {
            // Try for all input pins
            int hr = -2147467259;// E_FAIL
            int index = 0;
            do
            {
                IPin pInPin = DsFindPin.ByDirection(pDstFilter, PinDirection.Input, index);
                if (pInPin != null)
                {
                    hr = pGraph.Connect(pOutPin, pInPin);
                    Marshal.ReleaseComObject(pInPin);
                    if (hr == 0)
                        break;
                    index++;
                }
                else
                {
                    break;
                }
            } while (true);

            return hr;
        }

        private static void AddSinkFilterToGraph(IGraphBuilder _filterGraph, string outputFilename, IBaseFilter pSinkFilter)
        {
            int hr = _filterGraph.AddFilter(pSinkFilter, "GDCL MP4 Muxer");
            DsError.ThrowExceptionForHR(hr);

            Log.Info("Add file writer filter");
            IBaseFilter pFileWriterFilter = (IBaseFilter)new FileWriter();
            hr = _filterGraph.AddFilter(pFileWriterFilter, "File Writer");
            DsError.ThrowExceptionForHR(hr);

            Log.Info("Get IFileSinkFilter interface");
            IFileSinkFilter fileSinkFilter = pFileWriterFilter as IFileSinkFilter;

            IFileSinkFilter2 fileSinkFilter2 = pFileWriterFilter as IFileSinkFilter2;
            if (fileSinkFilter2 != null)
            {
                fileSinkFilter2.SetMode(DirectShowLib.AMFileSinkFlags.OverWrite);
            }
            // Set output filename
            fileSinkFilter.SetFileName(outputFilename, null);

            try
            {
                Log.Info("Connect sink filter with file writer");
                IPin pSinkOutPin = DsFindPin.ByDirection(pSinkFilter, PinDirection.Output, 0);
                if (pSinkOutPin != null)
                {
                    hr = ConnectOutputPin(_filterGraph, pSinkOutPin, pFileWriterFilter);
                    DsError.ThrowExceptionForHR(hr);
                    Marshal.ReleaseComObject(pSinkOutPin);
                }
            }
            finally
            {
                if (pFileWriterFilter != null)
                {
                    Marshal.ReleaseComObject(pFileWriterFilter);
                }
            }
        }

        /// <summary>
        /// Creates mp4 muxer filter and returns it as an out parameter.
        /// </summary>
        /// <param name="outputFilename"></param>
        /// <param name="pSinkFilter"></param>
        public static void AddMp4SinkFilter(IGraphBuilder _filterGraph, string outputFilename, out IBaseFilter pSinkFilter)
        {
            Log.Info("Add MP4 Muxer filter.");

            pSinkFilter = GDCLMP4MuxFilterLoader.LoadFilter();
            AddSinkFilterToGraph(_filterGraph, outputFilename, pSinkFilter);
        }
        
        public static void AddMonogramMp4SinkFilter(IGraphBuilder _filterGraph, string outputFilename, out IBaseFilter pSinkFilter)
        {
            pSinkFilter = MonogramMP4MuxFilterLoader.LoadFilter();
            AddSinkFilterToGraph(_filterGraph, outputFilename, pSinkFilter);
        }

        /// <summary>
        /// Creates and adds video renderer filter to the graph.
        /// </summary>
        /// <param name="pVideoRenderer"></param>
        public static void AddVideoRenderer(IGraphBuilder _filterGraph, out IBaseFilter pVideoRenderer)
        {
            Log.Info("Create EVR Filter.");
            pVideoRenderer = (IBaseFilter)new EnhancedVideoRenderer();
            int hr = _filterGraph.AddFilter(pVideoRenderer, "EVR");
            DsError.ThrowExceptionForHR(hr);
        }

        /// <summary>
        /// Adds video and audio renderers to the graph.
        /// </summary>
        /// <param name="pVideoRender"></param>
        /// <param name="pAudioRender"></param>
        public static void AddAudioRender(IGraphBuilder _filterGraph, string deviceName, out IBaseFilter pAudioRender, string name = "")
        {
            int hr = 0;
            if (!string.IsNullOrEmpty(deviceName))
            {
                Log.Info("Create specified audio renderer: \"{0}\"",
                    DecklinkCaptureLibConfigurationManager.Instance.Configuration.AudioRenderer);

                pAudioRender = FilterGraphTools.AddFilterByName(_filterGraph, FilterCategory.AudioRendererCategory,
                    DecklinkCaptureLibConfigurationManager.Instance.Configuration.AudioRenderer, name);
            }
            else
            {
                Log.Info("Create DSound Renderer Filter.");
                pAudioRender = (IBaseFilter)new DSoundRender();
                hr = _filterGraph.AddFilter(pAudioRender, "Audio Renderer");
                DsError.ThrowExceptionForHR(hr);
            }           
        }
        /// <summary>
        /// Creates inf tee filters.
        /// </summary>
        /// <param name="pInfTee"></param>
        public static void CreateInfTeeFilter(IGraphBuilder _filterGraph, out IBaseFilter pInfTee)
        {
            Log.Info("Create inf tee filter.");
            pInfTee = (IBaseFilter)new InfTee();
            int hr = _filterGraph.AddFilter(pInfTee, "Inf Tee");
            DsError.ThrowExceptionForHR(hr);
        }
        /// <summary>
        /// Add audio channel downmixer filter.
        /// </summary>
        /// <param name="pChannelDownmixer"></param>
        public static void AddAudioChannelDownmixer(IGraphBuilder _filterGraph, out IBaseFilter pChannelDownmixer)
        {
            Log.Info("Add Audio Channel Downmixer Filter");
            //pChannelDownmixer = (IBaseFilter)new AudioDownMixer();//ChannelDownmixerLoader.LoadFilter();
            pChannelDownmixer = ChannelDownmixerLoader.LoadFilter();
            int hr = _filterGraph.AddFilter(pChannelDownmixer, "Audio Downmixer");
            DsError.ThrowExceptionForHR(hr);
        }
        /// <summary>
        /// Add our csutom wav meter and volume control filter.
        /// </summary>
        /// <param name="pWavMeterFilter"></param>
        public static void AddWavMeterFilter(IGraphBuilder _filterGraph, out IBaseFilter pWavMeterFilter, int index = 0)
        {
            Log.Info("Add Wav Meter Filter to graph.");
            
            pWavMeterFilter = WavMeterExLoader.LoadFilter();// (IBaseFilter)new WavMeterEx();
            int hr = _filterGraph.AddFilter(pWavMeterFilter, string.Format("Wav Meter {0}", index));
            DsError.ThrowExceptionForHR(hr);
        }
        /// <summary>
        /// Adds mp4 demuxer filter to the graph.
        /// </summary>
        /// <param name="pMp4Demux"></param>
        public static void AddMp4DemuxFilter(IGraphBuilder _filterGraph, out IBaseFilter pMp4Demux)
        {
            Log.Info("Add Mp4 Demux filter.");
            pMp4Demux = GDCLMp4DemuxFilterLoader.LoadFilter();
            int hr = _filterGraph.AddFilter(pMp4Demux, "Mp4 Demux");
            DsError.ThrowExceptionForHR(hr);
        }
        /// <summary>
        /// Add our own video decoder filter.
        /// </summary>
        /// <param name="pLavVideoDevcoder"></param>
        public static void AddLavVideoDecoderFilter(IGraphBuilder _filterGraph, out IBaseFilter pLavVideoDevcoder)
        {
            Log.Info("Add Lav Video Decoder Filter to graph.");

            pLavVideoDevcoder = LavVideoDecoderFilterLoader.LoadFilter();
            int hr = _filterGraph.AddFilter(pLavVideoDevcoder, string.Format("Lav Video Decoder"));
            DsError.ThrowExceptionForHR(hr);
        }
        /// <summary>
        /// Adds file source filter in filter graph, for the specified file name.
        /// </summary>
        /// <param name="fileName"></param>
        /// <param name="pFileSource"></param>
        public static void AddFileSource(IGraphBuilder _filterGraph, string fileName, out IBaseFilter pFileSource)
        {
            Log.Info("Add source filter for: \"{0}\"", fileName);
            pFileSource = (IBaseFilter)new AsyncReader();
            IFileSourceFilter sourceFilter = pFileSource as IFileSourceFilter;
            if (sourceFilter == null)
            {
                DsError.ThrowExceptionForHR(-1);
            }
            int hr = sourceFilter.Load(fileName, null);
            DsError.ThrowExceptionForHR(hr);
            _filterGraph.AddFilter(pFileSource, "Source");
        }

        /// <summary>
        /// Create x264 encoder and configure it.
        /// </summary>
        /// <param name="pX264Filter"></param>
        public static void CreateX264Filter(IGraphBuilder _filterGraph, IPin sourcePin, out IBaseFilter pX264Filter, bool interlaced, int videoBitrate)
        {
            Log.Info("Add X264 encoder filter.");
            //pX264Filter = (IBaseFilter) new MonogramX264EncoderFilter();
            pX264Filter = MonogramX264EncoderFilterLoader.LoadFilter();
            int hr = _filterGraph.AddFilter(pX264Filter, "X264 Encoder");
            DsError.ThrowExceptionForHR(hr);

            Log.Info("Configure X264 encoder.");
            IMonogramX264Encoder iX264Encoder = pX264Filter as IMonogramX264Encoder;
            MONOGRAM_X264_CONFIG2 config = new MONOGRAM_X264_CONFIG2();
            iX264Encoder.GetConfig(ref config);
            Log.Info("Set X264 bitrate to: {0}", videoBitrate);
            config.bitrate_kbps = videoBitrate;
            config.level = (int)AVCLevel.MONO_AVC_LEVEL_4_1;
            config.interlaced = interlaced;

            ///////////////////////////
            config.parallel = true;
            config.sliced_threading = false;
            config.cabac = false;
            config.deblocking = true;
            config.deblock_strength = 0;
            config.annexb = false;
            config.reference_frames = 1;
            config.key_int_min = 1;
            config.key_int_max = 30;
            config.bframes = 0;
            config.periodic_intra_refresh = false;           
            config.adaptive_b_frames = (int)AVCBFramesAdaptive.MONO_AVC_BFRAMESADAPTIVE_DISABLED;
            config.bframes_pyramid = (int)AVCBPyramid.MONO_AVC_BPYRAMID_NONE;
            config.rcmethod = (int)AVCRCMode.MONO_AVC_RC_MODE_ABR;
            config.tolerance_percent = 20;
            config.partitions_intra = (int)AVCInter.MONO_AVC_INTER_LOW;
            config.partitions_inter = (int)AVCInter.MONO_AVC_INTER_LOW;
            config.subpixel_refine = (int)AVCSubPel.MONO_AVC_SUBPEL_HALF_LO;

            ///////////////////////////

            hr = iX264Encoder.SetConfig(config);
            DsError.ThrowExceptionForHR(hr);

            hr = DSUtils.ConnectOutputPin(_filterGraph, sourcePin, pX264Filter);
            if (hr < 0)
            {
                // Add this filter for YUV conversion
                IBaseFilter pHelixYV12 = FilterGraphTools.AddFilterByName(_filterGraph,
                    FilterCategory.VideoCompressorCategory, "Helix YV12 YUV Codec");
                if (pHelixYV12 != null)
                {
                    DSUtils.ConnectOutputPin(_filterGraph, sourcePin, pHelixYV12);
                    IPin helixPin = DsFindPin.ByDirection(pHelixYV12, PinDirection.Output, 0);
                    DSUtils.ConnectOutputPin(_filterGraph, helixPin, pX264Filter);
                    if (helixPin != null) Marshal.ReleaseComObject(helixPin);
                    Marshal.ReleaseComObject(pHelixYV12);
                }
                else
                {
                    Log.Error("Failed to add Helix YV12 YUV Codec.");
                }
            }
        }

        public static void ConnectPinSubtype(IGraphBuilder _filterGraph, IPin output, IPin input, int compression)
        {
            IEnumMediaTypes enumMt = null;
            try
            {
                output.EnumMediaTypes(out enumMt);
                if (enumMt != null)
                {
                    DirectShowLib.AMMediaType[] mediaTypes = new DirectShowLib.AMMediaType[1];
                    IntPtr t = new IntPtr();
                    while (0 == enumMt.Next(1, mediaTypes, t))
                    {
                        if (mediaTypes[0].formatType == FormatType.VideoInfo)
                        {
                            DirectShowLib.VideoInfoHeader vfi = new DirectShowLib.VideoInfoHeader();
                            Marshal.PtrToStructure(mediaTypes[0].formatPtr, vfi);

                            if (vfi.BmiHeader.Compression == compression)
                            {
                                Log.Info("Connect direct Resizer Filter with X264 Filter.");
                                int hr = _filterGraph.ConnectDirect(output, input, mediaTypes[0]);
                                DsError.ThrowExceptionForHR(hr);
                                break;
                            }
                        }
                        if (mediaTypes[0].formatType == FormatType.VideoInfo2)
                        {
                            DirectShowLib.VideoInfoHeader2 vfi = new DirectShowLib.VideoInfoHeader2();
                            Marshal.PtrToStructure(mediaTypes[0].formatPtr, vfi);

                            if (vfi.BmiHeader.Compression == compression)
                            {
                                Log.Info("Connect direct Resizer Filter with X264 Filter.");
                                int hr = _filterGraph.ConnectDirect(output, input, mediaTypes[0]);
                                DsError.ThrowExceptionForHR(hr);
                                break;
                            }
                        }
                        DsUtils.FreeAMMediaType(mediaTypes[0]);
                    }
                }
            }
            finally
            {
                if (enumMt != null)
                {
                    Marshal.ReleaseComObject(enumMt);
                }
            }
        }

        /// <summary>
        /// Add LAV Audio decoder filter.
        /// </summary>
        /// <param name="pLavAudioDecoder"></param>
        public static void AddLavAudioDecoder(IGraphBuilder _filterGraph, out IBaseFilter pLavAudioDecoder, int index = 0)
        {
            Log.Info("Add Lav Audio Decoder Filter to graph.");

            pLavAudioDecoder = LavAudioDecoderFilterLoader.LoadFilter();
            int hr = _filterGraph.AddFilter(pLavAudioDecoder, string.Format("Lav Audio Decoder {0}", index));
            DsError.ThrowExceptionForHR(hr);
        }
    }
}
