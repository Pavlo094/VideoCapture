using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using NLog;
using System.IO;
using System.Runtime.InteropServices;
using System.Windows.Forms;
using BTFPVideoEncodingLib.Filters;
using DirectShowLib;
using DirectShowLib.DMO;
using Microsoft.SqlServer.Server;
using DirectShowLib.Utils;
using DecklinkCaptureLib;

namespace BTFPVideoEncodingLib
{
    public class BTFPVideoTranscoder : MarshalByRefObject
    {
        #region Logger
        private static readonly Logger Log = LogManager.GetCurrentClassLogger();
        #endregion

        #region Members
        /// <summary>
        /// Input file name for transcoding.
        /// </summary>
        private string _inputFile = string.Empty;
        /// <summary>
        /// Output, transcoded file.
        /// </summary>
        private string _outputFile = string.Empty;
        /// <summary>
        /// Graph builder for building and controlling graph process.
        /// </summary>
        private IGraphBuilder _filterGraph = null;
        /// <summary>
        /// Interface for controlling Graph state.
        /// </summary>
        private IMediaControl _mediaControl = null;
        /// <summary>
        /// Interface for processing Graph events.
        /// </summary>
        private IMediaEventEx _mediaEvent = null;
        /// <summary>
        /// Interface used for seeking the filter graph.
        /// </summary>
        private IMediaSeeking _mediaSeeking = null;

        private IPin muxerOutputPin = null;
        private IBaseFilter pMuxerFilter = null;
        /// <summary>
        /// Default frame width.
        /// </summary>
        private const int InputVideoWidth = 1920;
        /// <summary>
        /// Default frame height.
        /// </summary>
        private const int InputVideoHeight = 1080;

        private const int InputAvgTimePerFrame = 333667;
        /// <summary>
        /// Number of NanoSeconds in a second.
        /// </summary>
        /// <remarks>
        /// This constant may be useful for calculations
        /// </remarks>
        public const long NANOSECONDS = (1000000000);       // 10 ^ 9
        /// <summary>
        /// Number of 100NS in a second.
        /// </summary>
        /// <remarks>
        /// To convert from seconds to 100NS 
        /// units (used by most DES function), multiply the seconds by UNITS.
        /// </remarks>
        public const long UNITS = (NANOSECONDS / 100);      // 10 ^ 7
        /// <summary>
        /// Preview video should start at this second.
        /// </summary>
        public static double PREVIEW_START_SECOND = 30;
        /// <summary>
        /// Preview video should end at this second.
        /// </summary>
        public static double PREVIEW_END_SECOND = 60;
        /// <summary>
        /// Current encoding parameters.
        /// </summary>
        private BTFPEncodingParameters _encodingParams = null;
        private object _fgLock = new object();
        #endregion

        #region Events
        /// <summary>
        /// This event is fired when in playback, the file has come to an end, or when exporting
        /// when exporting has finished.
        /// </summary>
        public event EventHandler Finished;
        /// <summary>
        /// This event is fired when DirectShow fires it's evil EC_ErrorAbort event.
        /// </summary>
        public event EventHandler ErrorAbort;
        #endregion

        #region Constructor
        /// <summary>
        /// Constructor.
        /// </summary>
        public BTFPVideoTranscoder()
        {
            MsgHandle = IntPtr.Zero;
            IsRunning = false;
            Duration = 0;

            //WM_GRAPHNOTIFY += 1;
        }
        #endregion

        #region Methods
        /// <summary>
        /// Starts transcoding input file according to encoding parameters specified.
        /// </summary>
        /// <param name="inputFile"></param>
        /// <param name="encodingParams"></param>
        public void Transcode(string inputFile, BTFPEncodingParameters encodingParams)
        {
            _encodingParams = encodingParams;
            _inputFile = inputFile;
            _outputFile = encodingParams.OutputDirectory + "\\" + Path.GetFileNameWithoutExtension(_inputFile) +
                          encodingParams.FileNameAppendix + Path.GetExtension(_inputFile);

            Log.Info("Transcode input file: \"{0}\" to \"{1}\"", _inputFile, _outputFile);

            ICaptureGraphBuilder2 captureGraphBuilder = null;
            int hr = 0;
            IBaseFilter pSourceFilter = null;
            IBaseFilter pMp4DemuxFilter = null;            
            IBaseFilter pLavVideoDecoder = null;
            IBaseFilter pVideoResizerDmoFilter = null;
            IBaseFilter pX264EncoderFilter = null;
            try
            {
                CreateFilterGraphAndInterfaces();

                // Create and configure ICaptureGraphBuilder
                Log.Info("Create and configure ICaptureGraphBuilder.");
                captureGraphBuilder = (ICaptureGraphBuilder2)new CaptureGraphBuilder2();
                hr = captureGraphBuilder.SetFiltergraph(_filterGraph);
                DsError.ThrowExceptionForHR(hr);

                // Add source filter
                DSUtils.AddFileSource(_filterGraph, _inputFile, out pSourceFilter);

                // Add Mp4 Demux filter
                DSUtils.AddMp4DemuxFilter(_filterGraph, out pMp4DemuxFilter);

                /////////////////////////////////////////////////////////////////////////////////
                // VIDEO STREAM
                if (!encodingParams.SkipVideoTranscode)
                {

                    // Add VideoDEcoder
                    DSUtils.AddLavVideoDecoderFilter(_filterGraph, out pLavVideoDecoder);

                    Log.Info("Connect Source Filter with MP4 Demux Filter and Video Decoder.");
                    hr = captureGraphBuilder.RenderStream(null, null, pSourceFilter, null, pMp4DemuxFilter);
                    DsError.ThrowExceptionForHR(hr);

                    Log.Info("Connect Video Decoder with Video Resizer Filter.");
                    hr = captureGraphBuilder.RenderStream(null, MediaType.Video, pMp4DemuxFilter, null, pLavVideoDecoder);
                    DsError.ThrowExceptionForHR(hr);

                    // Add video resizer filter.
                    AddVideoResizerDSP(out pVideoResizerDmoFilter);

                    IPin pResizerInputPin = null;
                    IPin pLavVideoDecoderOutputPin = null;
                    try
                    { 
                        pLavVideoDecoderOutputPin = DsFindPin.ByDirection(pLavVideoDecoder, PinDirection.Output, 0);
                        pResizerInputPin = DsFindPin.ByDirection(pVideoResizerDmoFilter, PinDirection.Input, 0);
                        DSUtils.ConnectPinSubtype(_filterGraph, pLavVideoDecoderOutputPin, pResizerInputPin, 0x32315659);
                    }
                    finally
                    {
                        if (pResizerInputPin != null)
                        {
                            Marshal.ReleaseComObject(pResizerInputPin);
                        }
                        if (pLavVideoDecoderOutputPin != null)
                        {
                            Marshal.ReleaseComObject(pLavVideoDecoderOutputPin);
                        }
                    }
                    ConfigureVideoResizer(pVideoResizerDmoFilter, InputVideoWidth, InputVideoHeight, encodingParams);

                    IPin resizerPin = DsFindPin.ByDirection(pVideoResizerDmoFilter, PinDirection.Output, 0);
                    if (resizerPin == null)
                        throw new COMException("Could not find DMO output pin", hr);

                    DSUtils.CreateX264Filter(_filterGraph, null/*resizerPin*/, out pX264EncoderFilter, encodingParams.InterlacedVideo, encodingParams.VideoBitrate);
                    //CreateX264Filter(out pX264EncoderFilter, encodingParams);
                    /*hr = captureGraphBuilder.RenderStream(null, null, pVideoResizerDmoFilter, null, pX264EncoderFilter);
                    DsError.ThrowExceptionForHR(hr);*/
                    ConnectDirectVideoResizer(pVideoResizerDmoFilter, pX264EncoderFilter, encodingParams);

                    // Create MP4 muxer filter
                    //DSUtils.AddMp4SinkFilter(_filterGraph, _outputFile, out pMuxerFilter);
                    DSUtils.AddMonogramMp4SinkFilter(_filterGraph, _outputFile, out pMuxerFilter);

                    Log.Info("Connect X264 Encoder with Muxer Filter");
                    hr = captureGraphBuilder.RenderStream(null, null, pX264EncoderFilter, null, pMuxerFilter);
                    DsError.ThrowExceptionForHR(hr);
                }
                else
                {
                    Log.Info("Skip video encode option.");

                    Log.Info("Connect Source Filter with MP4 Demux Filter and Video Decoder.");
                    hr = captureGraphBuilder.RenderStream(null, null, pSourceFilter, null, pMp4DemuxFilter);
                    DsError.ThrowExceptionForHR(hr);

                    DSUtils.AddMonogramMp4SinkFilter(_filterGraph, _outputFile, out pMuxerFilter);
                    //CreateSinkFilter(_outputFile, out pMuxerFilter);
                    
                
                    Log.Info("Connect MP4 Demuxer with MP4 Muxer");
                    hr = captureGraphBuilder.RenderStream(null, MediaType.Video, pMp4DemuxFilter, null, pMuxerFilter);
                    DsError.ThrowExceptionForHR(hr);
                }

                /////////////////////////////////////////////////////////////////////////////////
                // AUDIO STREAM
                ConnectAudioMultipleChannels(pMp4DemuxFilter, pMuxerFilter, captureGraphBuilder, encodingParams);

                SetNotifyWindow();

                muxerOutputPin = DsFindPin.ByDirection(pMuxerFilter, PinDirection.Output, 0);
                if (muxerOutputPin != null)
                {
                    _mediaSeeking = muxerOutputPin as IMediaSeeking;
                }
                

                if (encodingParams.ConfigurationName == BTFPEncodingParameters.PreviewParamsName)
                {
                    long duration;
                    _mediaSeeking.GetDuration(out duration);
                    double start = PREVIEW_START_SECOND;
                    double end = PREVIEW_END_SECOND;
                    if (end * UNITS < duration)
                    {
                        Log.Info("Set mark in and mark out positions for Preview video at: {0} to {1}", PREVIEW_START_SECOND,
                            PREVIEW_END_SECOND);
                        AMSeekingSeekingCapabilities caps;
                        _mediaSeeking.GetCapabilities(out caps);
                        hr = _mediaSeeking.SetPositions(new DsLong((long)(PREVIEW_START_SECOND * UNITS)),
                            AMSeekingSeekingFlags.AbsolutePositioning, new DsLong((long)(PREVIEW_END_SECOND * UNITS)),
                            AMSeekingSeekingFlags.AbsolutePositioning);
                        DsError.ThrowExceptionForHR(hr);

                        Duration = PREVIEW_END_SECOND - PREVIEW_START_SECOND;
                        Log.Info("Get duration: {0}", Duration);
                    }
                    else
                    {
                        Duration = duration / (double)UNITS;
                    }
                }
                else
                {
                    long duration;
                    _mediaSeeking.GetDuration(out duration);
                    Duration = duration / (double)UNITS;
                    Log.Info("Get duration: {0}", Duration);
                }

                SetSyncSource(null);

                hr = _mediaControl.Run();
                DsError.ThrowExceptionForHR(hr);
                IsRunning = true;
            }
                catch (Exception ex)
            {
                Log.Error("Handle Event error: {0}", ex.Message);
                if (ErrorAbort != null)
                {
                    ErrorAbort.BeginInvoke(this, EventArgs.Empty, null, null);
                }
            }
            finally
            {
                //
                // Release everything
                Log.Info("Release instantiated filters");
                //
                if (captureGraphBuilder != null)
                {
                    captureGraphBuilder.SetFiltergraph(null);
                    Marshal.ReleaseComObject(captureGraphBuilder);
                }
                if (pSourceFilter != null)
                {
                    Marshal.ReleaseComObject(pSourceFilter);
                }
                if (pMp4DemuxFilter != null)
                {
                    Marshal.ReleaseComObject(pMp4DemuxFilter);
                }
                if (pMuxerFilter != null)
                {
                    //Marshal.ReleaseComObject(pMuxerFilter);
                }
                if (pLavVideoDecoder != null)
                {
                    Marshal.ReleaseComObject(pLavVideoDecoder);
                }
                if (pVideoResizerDmoFilter != null)
                {
                    Marshal.ReleaseComObject(pVideoResizerDmoFilter);
                }
                if (pX264EncoderFilter != null)
                {
                    Marshal.ReleaseComObject(pX264EncoderFilter);
                }
            }
        }
        /// <summary>
        /// Call this method to stop transcoding or unload the resources after transcoding has finished.
        /// </summary>
        public void Unload()
        {
            Log.Info("Unload.");
            lock (_fgLock)
            {
                try
                {

                    if (_mediaControl != null)
                    {
                        _mediaControl.Stop();
                    }

                    if (_mediaEvent != null)
                    {
                        _mediaEvent.SetNotifyWindow(IntPtr.Zero, 0, IntPtr.Zero);
                    }

                    if (_filterGraph != null)
                    {
                        Marshal.ReleaseComObject(_filterGraph);
                    }

                    _mediaControl = null;
                    _mediaEvent = null;
                    _mediaSeeking = null;

                    /*if (muxerOutputPin != null)
                    {
                        Marshal.ReleaseComObject(muxerOutputPin);
                    }
                    if (pMuxerFilter != null)
                    {
                        Marshal.ReleaseComObject(pMuxerFilter);
                    }*/

                    //MonogramX264EncoderFilterLoader.Unload();
                }
                finally
                {
                    IsRunning = false;
                }
            }
        }
        /// <summary>
        /// Handles DirectShow messages.
        /// </summary>
        /// <param name="m"></param>
        public void HandleEvent(Message m)
        {
            try
            {
                if (m.Msg == WM_GRAPHNOTIFY && _mediaEvent != null)
                {
                    EventCode evCode;
                    IntPtr param1, param2;

                    int hr = _mediaEvent.GetEvent(out evCode,
                        out param1,
                        out param2,
                        0);

                    if (hr == 0)
                    {
                        _mediaEvent.FreeEventParams(evCode, param1, param2);

                        Log.Info("DirectShow event: {0}. HRESULT: {1}, HRESULT text: {2}", evCode, param1.ToInt32(),
                            DsError.GetErrorText(param1.ToInt32()));

                        if (evCode == EventCode.Complete)
                        {
                            if (Finished != null)
                            {
                                Finished.BeginInvoke(this, EventArgs.Empty, null, null);
                            }
                        }
                        else if (evCode == EventCode.ErrorAbort)
                        {
                            LastError = DsError.GetErrorText(param1.ToInt32());
                            if (ErrorAbort != null)
                            {
                                ErrorAbort.BeginInvoke(this, EventArgs.Empty, null, null);
                            }
                        }
                    }
                }
            }
            catch (Exception ex)
            {
                Log.Error("Handle Event error: {0}", ex.Message);
            }
        }

        public void Run()
        {
            int hr = _mediaControl.Run();
            DsError.ThrowExceptionForHR(hr);
            IsRunning = true;
        }
        #endregion

        #region Properties
        /// <summary>
        /// Gets or sets the window handle where DirectShow messages are dispatched.
        /// </summary>
        public IntPtr MsgHandle { get; set; }
        /// <summary>
        /// Gets or sets the current position of the stream. Setting this value effectively means
        /// seeking the stream.
        /// </summary>
        public double Position
        {
            get
            {
                lock (_fgLock)
                {
                    if (_mediaSeeking != null && IsRunning)
                    {
                        long current;
                        FilterState state = FilterState.Stopped;
                        _mediaControl.GetState(0, out state);
                        if (state != FilterState.Running)
                        {
                            return 0;
                        }
                        long stopPosition = 0;
                        int hr = _mediaSeeking.GetPositions(out current, out stopPosition);
                        if (hr != 0)
                        {
                            hr = _mediaSeeking.GetCurrentPosition(out current);
                            if (hr == 0)
                                hr = _mediaSeeking.GetDuration(out stopPosition);
                        }

                        if (hr != 0)
                            return 0;
                        //DsError.ThrowExceptionForHR(hr);
                        if (current == stopPosition)
                        {
                            return 0;
                        }

                        long duration = 0;
                        hr = _mediaSeeking.GetDuration(out duration);
                        if (_encodingParams.ConfigurationName != BTFPEncodingParameters.PreviewParamsName || 
                            duration < UNITS * 30)
                        {
                            return (current/(double) UNITS);
                        }
                        else
                        {
                            return (current/(double) UNITS) - PREVIEW_START_SECOND;
                        }
                    }

                    return 0;
                }
            }
            set {}
        }
        /// <summary>
        /// Flag to see if the encoder is running or not.
        /// </summary>
        public bool IsRunning { get; set; }
        /// <summary>
        /// Gets the duration of the current loaded clip.
        /// </summary>
        public double Duration { get; set; }
        /// <summary>
        /// DirectShow message identifier, gets or sets.
        /// </summary>
        public int WM_GRAPHNOTIFY { get; set; }
        /// <summary>
        /// Gets the error string if you get ErrorAbort event.
        /// </summary>
        public string LastError { get; set; }
        #endregion

        #region Helper Methods
        /// <summary>
        /// Create filter graph and interfaces for control
        /// </summary>
        private void CreateFilterGraphAndInterfaces()
        {
            Log.Info("Create FilterGraph.");
            _filterGraph = (IGraphBuilder)new FilterGraph();
            Log.Info("Query IMediaControl");
            _mediaControl = _filterGraph as IMediaControl;
            Log.Info("Query for the IMediaEvent interface.");
            _mediaEvent = _filterGraph as IMediaEventEx;
            Log.Info("Query for the IMediaSeeking interface.");
            //_mediaSeeking = _filterGraph as IMediaSeeking;
        }
        /// <summary>
        /// Adds file source filter in filter graph, for the specified file name.
        /// </summary>
        /// <param name="fileName"></param>
        /// <param name="pFileSource"></param>
        private void AddFileSource(string fileName, out IBaseFilter pFileSource)
        {
            Log.Info("Add source filter for: \"{0}\"", fileName);
            int hr = _filterGraph.AddSourceFilter(fileName, Path.GetFileName(fileName), out pFileSource);
            DsError.ThrowExceptionForHR(hr);

        }
        /// <summary>
        /// Adds mp4 demuxer filter to the graph.
        /// </summary>
        /// <param name="pMp4Demux"></param>
        private void AddMp4DemuxFilter(out IBaseFilter pMp4Demux)
        {
            Log.Info("Add Mp4 Demux filter.");
            pMp4Demux = (IBaseFilter)new Mp4DemuxFilter();
            int hr = _filterGraph.AddFilter(pMp4Demux, "Mp4 Demux");
            DsError.ThrowExceptionForHR(hr);
        }
        /// <summary>
        /// Add our own video decoder filter.
        /// </summary>
        /// <param name="pLavVideoDevcoder"></param>
        private void AddLavVideoDecoderFilter(out IBaseFilter pLavVideoDevcoder)
        {
            Log.Info("Add Lav Video Decoder Filter to graph.");

            pLavVideoDevcoder = (IBaseFilter)new LavVideoDecoderFilter();
            int hr = _filterGraph.AddFilter(pLavVideoDevcoder, string.Format("Lav Video Decoder"));
            DsError.ThrowExceptionForHR(hr);
        }
        /// <summary>
        /// Add LAV Audio decoder filter.
        /// </summary>
        /// <param name="pLavAudioDecoder"></param>
        private void AddLavAudioDecoder(out IBaseFilter pLavAudioDecoder, int index = 0)
        {
            Log.Info("Add Lav Audio Decoder Filter to graph.");

            pLavAudioDecoder = (IBaseFilter)new LavAudioDecoderFilter();
            int hr = _filterGraph.AddFilter(pLavAudioDecoder, string.Format("Lav Audio Decoder {0}", index));
            DsError.ThrowExceptionForHR(hr);
        }
        /// <summary>
        /// Adds video resizer DSP filter.
        /// </summary>
        /// <param name="pVideoResizer"></param>
        private void AddVideoResizerDSP(out IBaseFilter pVideoResizer)
        {
            Log.Info("Create Video Resizer DSP");
            Log.Info("Create DMO Wrapper Filter");
            pVideoResizer = (IBaseFilter) new DMOWrapperFilter();
            Log.Info("Query the IDMOWrapperFilter interface.");
            IDMOWrapperFilter dmoWrapper = (IDMOWrapperFilter) pVideoResizer;
            Log.Info("Initializer IDMOWrapperFilter.");
            dmoWrapper.Init(typeof (ResizerDMOFilter).GUID, DMOCategory.VideoEffect);

            int hr = _filterGraph.AddFilter(pVideoResizer, string.Format("Video Resizer DMO"));
            DsError.ThrowExceptionForHR(hr);
        }
        /// <summary>
        /// Configure video resizer for input and output frame size.
        /// </summary>
        /// <param name="pVideoResizer"></param>
        /// <param name="inputWidth"></param>
        /// <param name="inputHeight"></param>
        /// <param name="encodingParameters"></param>
        private void ConfigureVideoResizer(IBaseFilter pVideoResizer, int inputWidth, int inputHeight, BTFPEncodingParameters encodingParameters)
        {
            Log.Info("Setup Resizer DMO");

            Log.Info("Query IWMResizerProps interface");
            IWMResizerProps resizerProps = (IWMResizerProps) pVideoResizer;

            int lClipOriXSrc = 0;
            int lClipOriYSrc = 0;
            int lClipWidthSrc = inputWidth;
            int lClipHeightSrc = inputHeight;
            int lClipOriXDst = 0;
            int lClipOriYDst = 0;
            int lClipWidthDst = encodingParameters.VideoWidth;
            int lClipHeightDst = encodingParameters.VideoHeight;

            Log.Info("Configure the resizer DMO with input ({0}x{1}) and output ({2}x{3})", lClipWidthSrc,
                lClipHeightSrc, lClipWidthDst, lClipHeightDst);
            var hr = resizerProps.SetFullCropRegion(lClipOriXSrc, lClipOriYSrc, lClipWidthSrc, lClipHeightSrc,
                lClipOriXDst, lClipOriYDst, lClipWidthDst, lClipHeightDst);
            DMOError.ThrowExceptionForHR(hr);

            int interlace = encodingParameters.InterlacedVideo == true ? 1 : 0;
            resizerProps.SetInterlaceMode(interlace);
        }
        /// <summary>
        /// Does ConnectDirect for DMO Video Resizer and Video Encoder.
        /// This is because of bug in Resizer DMO, where the output media type won't be set
        /// no matter if you set SetFullCropRegion apropriately.
        /// </summary>
        /// <param name="pVideoResizer"></param>
        /// <param name="pEncoder"></param>
        /// <param name="encodingParameters"></param>
        private void ConnectDirectVideoResizer(IBaseFilter pVideoResizer, IBaseFilter pEncoder,
            BTFPEncodingParameters encodingParameters)
        {
            Log.Info("Query for the Video Resizer Output Pin.");
            IPin pResizerInputPin = null;
            IPin pResizerOutPin = null;
            IPin pX264InputPin = null;
            int hr = 0;
            try
            {
                pResizerOutPin = DsFindPin.ByDirection(pVideoResizer, PinDirection.Output, 0);
                if (pResizerOutPin != null)
                {
                    Log.Info("Query the X264 Input Pin.");
                    pX264InputPin = DsFindPin.ByDirection(pEncoder, PinDirection.Input, 0);
                    if (pX264InputPin != null)
                    {
                        AMMediaType mt = new AMMediaType();
                        pResizerInputPin = DsFindPin.ByDirection(pVideoResizer, PinDirection.Input, 0);
                        if (pResizerInputPin != null)
                        {
                            pResizerInputPin.ConnectionMediaType(mt);
                            int bitCount = GetBitCountFromMediaType(mt);
                            mt.sampleSize = (int)(encodingParameters.VideoWidth * encodingParameters.VideoHeight * (bitCount / 8.0));

                            ConfigureVideoInfoHeader(mt, encodingParameters);

                            Log.Info("Connect direct Resizer Filter with X264 Filter.");
                            pResizerOutPin.Disconnect();
                            hr = _filterGraph.ConnectDirect(pResizerOutPin, pX264InputPin, mt);
                            DsError.ThrowExceptionForHR(hr);

                            // release mt here
                            DsUtils.FreeAMMediaType(mt);
                        }
                    }
                }
            }
            finally
            {
                if (pResizerOutPin != null)
                {
                    Marshal.ReleaseComObject(pResizerOutPin);
                }
                if (pX264InputPin != null)
                {
                    Marshal.ReleaseComObject(pX264InputPin);
                }
                if (pResizerInputPin != null)
                {
                    Marshal.ReleaseComObject(pResizerInputPin);
                }
            }
        }
        /// <summary>
        /// Returns Bitmapinfoheader.BitCount, wether the format is VideoInfo or VideoInfo2
        /// </summary>
        /// <param name="mt"></param>
        /// <returns></returns>
        private int GetBitCountFromMediaType(AMMediaType mt)
        {
            if (mt.formatType == FormatType.VideoInfo)
            {
                VideoInfoHeader vfi = new VideoInfoHeader();
                Marshal.PtrToStructure(mt.formatPtr, vfi);

                return vfi.BmiHeader.BitCount;
            }
            if (mt.formatType == FormatType.VideoInfo2)
            {
                VideoInfoHeader2 vfi = new VideoInfoHeader2();
                Marshal.PtrToStructure(mt.formatPtr, vfi);

                return vfi.BmiHeader.BitCount;
            }

            throw new Exception("Unsupported media type in GetBitcountFromMediaType.");
        }
        /// <summary>
        /// Configures the output media type video info, depending on the encodingParams, wether the format is VideoInfo or VideoInfo2
        /// </summary>
        /// <param name="mt"></param>
        /// <param name="encodingParams"></param>
        private void ConfigureVideoInfoHeader(AMMediaType mt, BTFPEncodingParameters encodingParams)
        {
            if (mt.formatType == FormatType.VideoInfo)
            {
                VideoInfoHeader vfi = new VideoInfoHeader();
                Marshal.PtrToStructure(mt.formatPtr, vfi);

                vfi.SrcRect = new DsRect(0, 0, encodingParams.VideoWidth, encodingParams.VideoHeight);
                vfi.TargetRect = new DsRect(0, 0, encodingParams.VideoWidth, encodingParams.VideoHeight);

                vfi.BmiHeader.ImageSize = mt.sampleSize;
                vfi.BmiHeader.Width = encodingParams.VideoWidth;
                vfi.BmiHeader.Height = encodingParams.VideoHeight;

                Marshal.StructureToPtr(vfi, mt.formatPtr, true);
            }
            else if (mt.formatType == FormatType.VideoInfo2)
            {
                VideoInfoHeader2 vfi = new VideoInfoHeader2();
                Marshal.PtrToStructure(mt.formatPtr, vfi);

                vfi.SrcRect = new DsRect(0, 0, encodingParams.VideoWidth, encodingParams.VideoHeight);
                vfi.TargetRect = new DsRect(0, 0, encodingParams.VideoWidth, encodingParams.VideoHeight);

                vfi.BmiHeader.ImageSize = mt.sampleSize;
                vfi.BmiHeader.Width = encodingParams.VideoWidth;
                vfi.BmiHeader.Height = encodingParams.VideoHeight;

                Marshal.StructureToPtr(vfi, mt.formatPtr, true);
            }
            else
            {
                throw new Exception("Unsupported media type in ConfigureVideoInfoHeader.");
            }
        }
        /// <summary>
        /// Creates mp4 muxer filter and returns it as an out parameter.
        /// </summary>
        /// <param name="outputFilename"></param>
        /// <param name="pSinkFilter"></param>
        private void CreateSinkFilter(string outputFilename, out IBaseFilter pSinkFilter)
        {
            Log.Info("Add MP4 Muxer filter.");

            pSinkFilter = (IBaseFilter)new GDCLMp4MuxFilter();
            int hr = _filterGraph.AddFilter(pSinkFilter, "GDCL MP4 Muxer");
            DsError.ThrowExceptionForHR(hr);

            Log.Info("Add file writer filter");
            IBaseFilter pFileWriterFilter = (IBaseFilter)new FileWriter();
            hr = _filterGraph.AddFilter(pFileWriterFilter, "File Writer");
            DsError.ThrowExceptionForHR(hr);

            Log.Info("Get IFileSinkFilter interface");
            IFileSinkFilter fileSinkFilter = pFileWriterFilter as IFileSinkFilter;

            // Set output filename
            hr = fileSinkFilter.SetFileName(outputFilename, null);
            DsError.ThrowExceptionForHR(hr);

            try
            {
                Log.Info("Connect sink filter with file writer");
                IPin pSinkOutPin = DsFindPin.ByDirection(pSinkFilter, PinDirection.Output, 0);
                if (pSinkOutPin != null)
                {
                    hr = ConnectFilters(_filterGraph, pSinkOutPin, pFileWriterFilter);
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
        /// Connects 2 directshow filters.
        /// </summary>
        /// <param name="pGraph"></param>
        /// <param name="pOutPin"></param>
        /// <param name="pDstFilter"></param>
        /// <returns></returns>
        private int ConnectFilters(IGraphBuilder pGraph, IPin pOutPin, IBaseFilter pDstFilter)
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
        /// <summary>
        /// Create x264 encoder and configure it.
        /// </summary>
        /// <param name="pX264Filter"></param>
        /// <param name="encodingParameters"></param>
        private void CreateX264Filter(out IBaseFilter pX264Filter, BTFPEncodingParameters encodingParameters)
        {
            Log.Info("Add X264 encoder filter.");
            //pX264Filter = (IBaseFilter)new MonogramX264EncoderFilter();
            pX264Filter = MonogramX264EncoderFilterLoader.LoadFilter();
            int hr = _filterGraph.AddFilter(pX264Filter, "X264 Encoder");
            DsError.ThrowExceptionForHR(hr);

            Log.Info("Configure X264 encoder.");
            IMonogramX264Encoder iX264Encoder = pX264Filter as IMonogramX264Encoder;
            MONOGRAM_X264_CONFIG2 config = new MONOGRAM_X264_CONFIG2();
            //iX264Encoder.GetConfig(ref config);
            //MONOGRAM_X264_CONFIG2 defConfig = new MONOGRAM_X264_CONFIG2();
            iX264Encoder.GetDefaults(ref config);
            Log.Info("Set X264 bitrate to: {0}", encodingParameters.VideoBitrate);
            config.bitrate_kbps = encodingParameters.VideoBitrate;
            config.level = (int)AVCLevel.MONO_AVC_LEVEL_4_1;
            config.interlaced = encodingParameters.InterlacedVideo;
            ///////////////////////////
            config.parallel = true;
            config.sliced_threading = false;
            config.cabac = false;
            config.reference_frames = 1;
            config.key_int_min = 10;
            config.key_int_max = 100;
            config.periodic_intra_refresh = false;
            config.bframes = 0;
            config.adaptive_b_frames = (int)AVCBFramesAdaptive.MONO_AVC_BFRAMESADAPTIVE_DISABLED;
            config.bframes_pyramid = (int)AVCBPyramid.MONO_AVC_BPYRAMID_NONE;
            config.rcmethod = (int)AVCRCMode.MONO_AVC_RC_MODE_ABR;
            config.tolerance_percent = 20;
            config.partitions_intra = (int)AVCInter.MONO_AVC_INTER_LOW;
            config.partitions_inter = (int)AVCInter.MONO_AVC_INTER_LOW;
            config.subpixel_refine = (int)AVCSubPel.MONO_AVC_SUBPEL_QUART_LO;
            ///////////////////////////
            hr = iX264Encoder.SetConfig(config);
            DsError.ThrowExceptionForHR(hr);
        }
        /// <summary>
        /// Sets notify window for DirectShow messages.
        /// </summary>
        private void SetNotifyWindow()
        {
            if (MsgHandle != IntPtr.Zero && _mediaEvent != null)
            {
                Log.Info("Set notify window: {0}", MsgHandle.ToInt32());
                _mediaEvent.SetNotifyWindow(MsgHandle, WM_GRAPHNOTIFY, IntPtr.Zero);
            }
        }
        /// <summary>
        /// Set sync source to filter graph.
        /// </summary>
        /// <param name="refClock"></param>
        private void SetSyncSource(IReferenceClock refClock)
        {
            Log.Info("Set sync source to: {0}", refClock);
            IMediaFilter pMediaFilter = _filterGraph as IMediaFilter;
            pMediaFilter.SetSyncSource(refClock);
        }

        private void ConfigureWavMeter(IBaseFilter pWavMeter, int boost)
        {
            Log.Info("Query IWavmeterEx interface.");
            var wavMeterEx = (IWavMeterEx)pWavMeter;
            var amplitude = Math.Pow(10, boost / 20.0);
            var tempVolumes = new double[6];
            for (int i = 0; i < tempVolumes.Length; i++)
            {
                // Only the first channel. There should be 6 separate audio tracks, each with 1 channel.
                tempVolumes[i] = amplitude;
            }
            wavMeterEx.SetVolume(tempVolumes);
        }
        /// <summary>
        /// Connects multiple tracks from demuxer to muxer.
        /// </summary>
        /// <param name="pMp4DemuxFilter"></param>
        /// <param name="pMp4MuxerFilter"></param>
        /// <param name="captureGraphBuilder2"></param>
        private void ConnectAudioMultipleChannels(IBaseFilter pMp4DemuxFilter, IBaseFilter pMp4MuxerFilter,
            ICaptureGraphBuilder2 captureGraphBuilder2, BTFPEncodingParameters encodingParams)
        {
            Log.Info("Render multiple audio tracks.");
            // Find all unconnected pins. In this app setup, there should be 6
            // separate audio tracks, each with single channel of audio.

            IBaseFilter pAudioMixer = null;
            IBaseFilter pAACFilter = null;
            IBaseFilter pInfTee = null;
            IBaseFilter pAudioRender = null;
            int hr = 0;
            int validAudioStreamsCount = 0;
            try
            {
                CreateAudioMixer(out pAudioMixer, encodingParams);

                if (encodingParams.ConfigurationName == BTFPEncodingParameters.PreviewParamsName ||
                    encodingParams.ConfigurationName == BTFPEncodingParameters.SalesParamsName)
                {
                    // Pin 0 should be video. By default here, we just want channel 1 and 2 as single track.
                    for (int i = 0; i < 2; i++)
                    {
                        // We maybe need to remap audio.
                        int outputPinIndex = GetConnectedInputChannelForOutputChannel(encodingParams, i);
                        if (outputPinIndex == -1)
                        {
                            Log.Error("There is no audio channel set for output channel: {0}", i);
                            continue;
                        }
                        // First one is video.
                        outputPinIndex += 1;

                        // Find next pin
                        Log.Info("Find audio pin {0}", outputPinIndex);
                        IPin pOutPin = DsFindPin.ByDirection(pMp4DemuxFilter, PinDirection.Output, outputPinIndex);
                        IPin decoderPin = null;
                        IBaseFilter pLavAudioDecoder = null;
                        try
                        {
                            if (pOutPin != null)
                            {
                                DSUtils.AddLavAudioDecoder(_filterGraph, out pLavAudioDecoder, i);

                                Log.Info("Render audio stream. pMp4DemuxFilter - pLavAudioDecoder - pAudioMixer.");
                                DSUtils.ConnectOutputPin(_filterGraph, pOutPin, pLavAudioDecoder);
                                decoderPin = DsFindPin.ByDirection(pLavAudioDecoder, PinDirection.Output, i);
                                DSUtils.ConnectOutputPin(_filterGraph, decoderPin, pAudioMixer);
                                /*hr = captureGraphBuilder2.RenderStream(null, null, pOutPin, pLavAudioDecoder,
                                    pAudioMixer);*/
                                DsError.ThrowExceptionForHR(hr);
                                validAudioStreamsCount++;
                            }
                            else
                            {
                                Log.Info("No pin for index: {0}. Exiting...", i);
                                break;
                            }
                        }
                        finally
                        {
                            if (pOutPin != null)
                            {
                                Marshal.ReleaseComObject(pOutPin);
                            }
                            if (decoderPin != null)
                            {
                                Marshal.ReleaseComObject(decoderPin);
                            }
                            if (pLavAudioDecoder != null)
                            {
                                Marshal.ReleaseComObject(pLavAudioDecoder);
                            }
                        }
                    }

                    if (validAudioStreamsCount == 0)
                        return;
                    CreateAACFilter(out pAACFilter, encodingParams);

                    // Seek won't be enabled if we don't add audio renderer, and we need seek because we encode
                    // just a range for PREVIEW video/audio.
                    /*if (encodingParams.ConfigurationName == BTFPEncodingParameters.PreviewParamsName)
                    {
                        IBaseFilter nullRenderer = (IBaseFilter)new NullRenderer();
                        _filterGraph.AddFilter(nullRenderer, "Dummy renderer");

                        
                        hr = captureGraphBuilder2.RenderStream(null, null, pMp4DemuxFilter, null, nullRenderer);
                        //DsError.ThrowExceptionForHR(hr);
                    }*/

                    Log.Info("Connect. pAACFilter - pMp4MuxerFilter.");
                    hr = captureGraphBuilder2.RenderStream(null, null, pAudioMixer, pAACFilter, pMp4MuxerFilter);
                    DsError.ThrowExceptionForHR(hr);

                } // PREVIEW and SALES
                else// JUDGES
                {
                    // Pin 0 should be video. By default here, we just want channel 3, 4, 5 and 6 as separate tracks.
                    for (int i = 0; i < 4; i++)
                    {
                        // We maybe need to remap audio.
                        int outputPinIndex = GetConnectedInputChannelForOutputChannel(encodingParams, i + 2);
                        if (outputPinIndex == -1)
                        {
                            Log.Error("There is no audio channel set for output channel: {0}", i + 2);
                            continue;
                        }
                        // First one is video.
                        outputPinIndex += 1;

                        // Find next pin
                        Log.Info("Find audio pin {0}", outputPinIndex);
                        IPin pOutPin = DsFindPin.ByDirection(pMp4DemuxFilter, PinDirection.Output, outputPinIndex);
                        IBaseFilter pLavAudioDecoder = null;
                        IBaseFilter pAudioTrackEncoder = null;
                        IBaseFilter pWavMeter = null;
                        try
                        {
                            if (pOutPin != null)
                            {
                                DSUtils.AddLavAudioDecoder(_filterGraph, out pLavAudioDecoder, i);

                                DSUtils.AddWavMeterFilter(_filterGraph, out pWavMeter);
                                ConfigureWavMeter(pWavMeter, encodingParams.AudioVolumeBoost);
                               
                                Log.Info("Render audio stream. pMp4DemuxFilter - pLavAudioDecoder - pAudioTrackEncoder.");
                                hr = captureGraphBuilder2.RenderStream(null, null, pOutPin, pLavAudioDecoder, pWavMeter);
                                DsError.ThrowExceptionForHR(hr);

                                CreateAACFilter(out pAudioTrackEncoder, encodingParams);

                                Log.Info("Render audio stream. pWavMeter - pAudioTrackEncoder - pMp4MuxeFilter.");
                                hr = captureGraphBuilder2.RenderStream(null, null, pWavMeter, pAudioTrackEncoder, pMp4MuxerFilter);
                                DsError.ThrowExceptionForHR(hr);
                            }
                            else
                            {
                                Log.Info("No pin for index: {0}. Exiting...", i);
                                break;
                            }
                        }
                        finally
                        {
                            if (pOutPin != null)
                            {
                                Marshal.ReleaseComObject(pOutPin);
                            }
                            if (pLavAudioDecoder != null)
                            {
                                Marshal.ReleaseComObject(pLavAudioDecoder);
                            }
                            if (pAudioTrackEncoder != null)
                            {
                                Marshal.ReleaseComObject(pAudioTrackEncoder);
                            }
                            if (pWavMeter != null)
                            {
                                Marshal.ReleaseComObject(pWavMeter);
                            }
                        }
                    }
                }
            }
            finally
            {
                if (pAudioMixer != null)
                {
                    Marshal.ReleaseComObject(pAudioMixer);
                }
                if (pAACFilter != null)
                {
                    Marshal.ReleaseComObject(pAACFilter);
                }
                if (pInfTee != null)
                {
                    Marshal.ReleaseComObject(pInfTee);
                }
                if (pAudioRender != null)
                {
                    Marshal.ReleaseComObject(pAudioRender);
                }
            }
        }

        private int GetConnectedInputChannelForOutputChannel(BTFPEncodingParameters encodingParams, int outputChannel)
        {
            for (int i = 0; i < 6; i++)
            {
                if (encodingParams.AudioChannelsMap[i, outputChannel] == 1)
                    return i;
            }

            return -1;
        }
        /// <summary>
        /// Create Audio Mixer Filter, for mixing multiple input mono streams to stereo.Also, sets the volume
        /// depending on the input encoding parameters.
        /// </summary>
        /// <param name="pAudioMixer"></param>
        private void CreateAudioMixer(out IBaseFilter pAudioMixer, BTFPEncodingParameters encodingParameters)
        {
            Log.Info("Add Audio Mixer Filter to graph.");

            pAudioMixer = AudioMixerFilterLoader.LoadFilter();
            int hr = _filterGraph.AddFilter(pAudioMixer, string.Format("Audio Mixer"));
            DsError.ThrowExceptionForHR(hr);

            Log.Info("Query IAudioMixerInterface");
            var audioMixer = (IAudioMixer) pAudioMixer;
            var amplitude = Math.Pow(10, encodingParameters.AudioVolumeBoost / 20.0);
            Log.Info("Set audio volume amplitude to: {0}[db] or {1}", encodingParameters.AudioVolumeBoost, amplitude);
            audioMixer.SetVolume(amplitude);
        }
        /// <summary>
        /// Creates and configures aac filter.
        /// </summary>
        /// <param name="pAACFilter"></param>
        private void CreateAACFilter(out IBaseFilter pAACFilter, BTFPEncodingParameters encodingParams)
        {
            Log.Info("Add AAC encoder filter");
            //pAACFilter = (IBaseFilter)new MonogramAACEncoderFilter();
            pAACFilter = MonogramAACEncoderFilterLoader.LoadFilter();
            int hr = _filterGraph.AddFilter(pAACFilter, "AAC Filter");
            DsError.ThrowExceptionForHR(hr);

            Log.Info("Configure AAC filter");
            IMonogramAACEncoder iAacEncoder = pAACFilter as IMonogramAACEncoder;
            AACConfig config = new AACConfig();
            iAacEncoder.GetConfig(ref config);
            Log.Info("Set AAC bitrate to: {0}", encodingParams.AudioBitrate);
            config.bitrate = encodingParams.AudioBitrate * 1024;
            config.version = (int)AACMPEGVersion.MPEG4;
            config.object_type = (int)AACObjectType.AAC_OBJECT_LOW;
            config.output_type = (int)AACOutputType.AAC_OUTPUT_RAW;
            hr = iAacEncoder.SetConfig(config);
            DsError.ThrowExceptionForHR(hr);
        }

        /// <summary>
        /// Add our csutom wav meter and volume control filter.
        /// </summary>
        /// <param name="pWavMeterFilter"></param>
        /// <param name="encodingParams"></param>
        private void AddWavMeterFilter(out IBaseFilter pWavMeterFilter, BTFPEncodingParameters encodingParams)
        {
            Log.Info("Add Wav Meter Filter to graph.");

            pWavMeterFilter = (IBaseFilter)new WavMeterEx();
            int hr = _filterGraph.AddFilter(pWavMeterFilter, "Wav Meter");
            DsError.ThrowExceptionForHR(hr);

            Log.Info("Query IWavmeterEx interface.");
            var wavMeterEx = (IWavMeterEx) pWavMeterFilter;
            var amplitude = Math.Pow(10, encodingParams.AudioVolumeBoost / 20.0);
            var tempVolumes = new double[6];
            for (int i = 0; i < tempVolumes.Length; i++)
            {
                // Only the first channel. There should be 6 separate audio tracks, each with 1 channel.
                tempVolumes[i] = amplitude;
            }
            wavMeterEx.SetVolume(tempVolumes);
        }
        /// <summary>
        /// Creates inf tee filter.
        /// </summary>
        /// <param name="pInfTee"></param>
        private void CreateInfTeeFilter(out IBaseFilter pInfTee)
        {
            Log.Info("Create inf tee filter.");
            pInfTee = (IBaseFilter)new InfTee();
            int hr = _filterGraph.AddFilter(pInfTee, "Inf Tee");
            DsError.ThrowExceptionForHR(hr);
        }
        /// <summary>
        /// Creates audio renderer - usually Default DSound Device - and adds it to the graph.
        /// </summary>
        /// <param name="pAudioRender"></param>
        private void AddAudioRenderer(out IBaseFilter pAudioRender)
        {
            int hr = 0;
            Log.Info("Create DSound Renderer Filter.");
            pAudioRender = (IBaseFilter)new DSoundRender();
            hr = _filterGraph.AddFilter(pAudioRender, "Audio Renderer");
            DsError.ThrowExceptionForHR(hr);
        }
        /// <summary>
        /// If there is audio preview, this will mute/un-mute the output speakers.
        /// </summary>
        /// <param name="mute"></param>
        private void MuteAudio(bool mute)
        {
            Log.Info("Mute speakers: {0}", mute);

            var ba = _filterGraph as IBasicAudio;
            if (ba != null)
            {
                int volume = mute ? -10000 : -500;
                Log.Info("Set volume to: {0}", volume);
                ba.put_Volume(volume);
            }
        }
        #endregion

    }
}
