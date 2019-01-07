using System;
using System.Collections.Generic;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Windows.Forms;
using DecklinkCaptureLib.Filters;
using DirectShowLib;
using DirectShowLib.Utils;
using MediaFoundation;
using MediaFoundation.EVR;
using MediaFoundation.Misc;
using DecklinkCaptureLib;
using NLog;

namespace DecklinkCaptureLib
{
    /// <summary>
    /// This class is a singleton implementation. It has methods for importing and video/audio previewing of MP4 file.
    /// Also, it has methods for trimming the input mp4 video/audio.
    /// </summary>
    public class BTFPTrimmer
    {
        #region Logger
        private static readonly Logger Log = LogManager.GetCurrentClassLogger();
        #endregion

        #region Members
        /// <summary>
        /// The only instance of this class
        /// </summary>
        private static BTFPTrimmer _instance;
        /// <summary>
        /// Flag for directshow windows message
        /// </summary>
        public const int WM_GRAPHNOTIFY = 0x8000 + 1;
        /// <summary>
        /// Handle to the preview window.
        /// </summary>
        private IntPtr _previewHandle = IntPtr.Zero;
        /// <summary>
        /// Current Input File for processing
        /// </summary>
        private string _inputFile = string.Empty;
        /// <summary>
        /// Current output file for processing.
        /// </summary>
        private string _outputFile = string.Empty;
        /// <summary>
        /// Mark in position, within the file, for export, in seconds.
        /// </summary>
        private double _markIn = 0;
        /// <summary>
        /// Mark out position, within the file, for export, in seconds.
        /// </summary>
        private double _markOut = 0;
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
        /// <summary>
        /// Vieo renderer filter.
        /// </summary>
        private IBaseFilter _pVideoRendererFilter = null;
        /// <summary>
        /// List of wev meters filters, one for each audio track.
        /// </summary>
        private List<IBaseFilter> _pWavMetersFilters = new List<IBaseFilter>();
        /// <summary>
        /// Interfaces for controllin Wav Meter Filters.
        /// </summary>
        private List<IWavMeterEx> _piWavMeters = new List<IWavMeterEx>();
        /// <summary>
        /// Video display control interface for EVR video preview.
        /// </summary>
        private IMFVideoDisplayControl _pVideoDisplayControl = null;
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
        #endregion

        #region Events
        /// <summary>
        /// This event is fired when in playback, the file has come to an end, or when exporting
        /// when exporting has finished.
        /// </summary>
        public event EventHandler Finished;
        #endregion

        #region Enums

        public enum BTFPTrimmerStatus
        {
            Idle,
            InPreview,
            InExport
        };
        #endregion

        #region Constructor
        /// <summary>
        /// Private constructor for singleton implementation.
        /// </summary>
        private BTFPTrimmer()
        {
            IsRunning = false;
            Status = BTFPTrimmerStatus.Idle;
            MsgHandle = IntPtr.Zero;
        }
        #endregion

        #region Instance
        /// <summary>
        /// The only instance of this object.
        /// </summary>
        public static BTFPTrimmer Instance
        {
            get { return _instance ?? (_instance = new BTFPTrimmer()); }
        }
        #endregion

        #region Properties
        /// <summary>
        /// Current input file for processing.
        /// </summary>
        public string InputFile
        {
            get
            {
                return _inputFile;
            }
        }
        /// <summary>
        /// Gets or sets if the object is running.
        /// </summary>
        public bool IsRunning { get; private set; }
        /// <summary>
        /// Gets or sets the window handle where DirectShow messages are dispatched.
        /// </summary>
        public IntPtr MsgHandle { get; set; }
        /// <summary>
        /// Gets or sets the current status.
        /// </summary>
        public BTFPTrimmerStatus Status { get; private set; }
        /// <summary>
        /// Gets or sets the current position of the stream. Setting this value effectively means
        /// seeking the stream.
        /// </summary>
        public double Position
        {
            get
            {
                if (_mediaSeeking != null && Status != BTFPTrimmerStatus.Idle)
                {
                    long current;
                    _mediaSeeking.GetCurrentPosition(out current);
                    return (current / (double)UNITS);
                }

                return 0;
            }
            set
            {
                if (_mediaSeeking != null && Status == BTFPTrimmerStatus.InPreview)
                {
                    _mediaSeeking.SetPositions((long) (value*UNITS), AMSeekingSeekingFlags.AbsolutePositioning, 0,
                        AMSeekingSeekingFlags.NoPositioning);
                }
            }
        }
        /// <summary>
        /// Gets the duration of the current loaded clip.
        /// </summary>
        public double Duration { get; private set; }
        public string OutputFile{get{return _outputFile;}}
        public double MarkIn { get{return _markIn;} }
        public double MarkOut{get{return _markOut;}}
        #endregion

        #region Public Methods
        /// <summary>
        /// Starts video/audio preview for the specified input file on the specified window.
        /// </summary>
        /// <param name="inputFile">Input file to preview.</param>
        /// <param name="previewHandle">Handle to a window for video preview.</param>
        public void StartPreview(string inputFile, IntPtr previewHandle)
        {
            _inputFile = inputFile;
            _previewHandle = previewHandle;
            Log.Info("Start Preview for file: {0}", _inputFile);

            // Check if already previewing
            if (Status == BTFPTrimmerStatus.InPreview)
            {
                Log.Info("Already previewing.");
                return;
            }

            // Check if we're in trimming state
            if (Status == BTFPTrimmerStatus.InExport)
            {
                Log.Info("In export. Call stop.");
                Stop();
            }

            ICaptureGraphBuilder2 captureGraphBuilder = null;
            int hr = 0;
            IBaseFilter pSourceFilter = null;
            IBaseFilter pMp4DemuxFilter = null;
            IBaseFilter pLavVideoDecoder = null;
            try
            {
                // Create filter graph and interfaces for control.
                CreateFilterGraphAndInterfaces();

                // Create and configure ICaptureGraphBuilder
                Log.Info("Create and configure ICaptureGraphBuilder.");
                captureGraphBuilder = (ICaptureGraphBuilder2) new CaptureGraphBuilder2();
                hr = captureGraphBuilder.SetFiltergraph(_filterGraph);
                DsError.ThrowExceptionForHR(hr);

                // Add source filter
                AddFileSource(_inputFile, out pSourceFilter);

                // Add Mp4 Demux filter
                DSUtils.AddMp4DemuxFilter(_filterGraph, out pMp4DemuxFilter);

                // Add video renderer
                DSUtils.AddVideoRenderer(_filterGraph, out _pVideoRendererFilter);

                // Add VideoDEcoder
                DSUtils.AddLavVideoDecoderFilter(_filterGraph, out pLavVideoDecoder);

                Log.Info("Render video stream.");
                hr = captureGraphBuilder.RenderStream(null, null, pSourceFilter, pLavVideoDecoder,
                    _pVideoRendererFilter);
                DsError.ThrowExceptionForHR(hr);

                RenderMultipleAudioTracks(pMp4DemuxFilter, captureGraphBuilder, out _pWavMetersFilters);

                Log.Info("Query the IWavMeter interfaces");
                _piWavMeters.Clear();
                foreach (var pWavMetersFilter in _pWavMetersFilters)
                {
                    var iWavMeter = pWavMetersFilter as IWavMeterEx;
                    _piWavMeters.Add(iWavMeter);
                }

                // Set notify window for DirectShow Messages.
                SetNotifyWindow();

                // Setup EVR for video preview
                InitEvr(_pVideoRendererFilter);
                SetupEvr();

                Status = BTFPTrimmerStatus.InPreview;

                long duration;
                _mediaSeeking.GetDuration(out duration);
                Duration = duration/(double) UNITS;
                Log.Info("Get duration: {0}", Duration);
            }
            catch
            {
                Status = BTFPTrimmerStatus.Idle;
                throw;
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
                if (pLavVideoDecoder != null)
                {
                    Marshal.ReleaseComObject(pLavVideoDecoder);
                }
            }
        }
        /// <summary>
        /// Starts export of the input file to the output file, for the specified time range
        /// </summary>
        /// <param name="inputFile">Input file</param>
        /// <param name="outputFile">Exported file</param>
        /// <param name="markIn">Mark in position for export</param>
        /// <param name="markOut">Mark out position for export.</param>
        /// <param name="previewHandle">Handle to preview video.</param>
        public void StartExport(string inputFile, string outputFile, double markIn, double markOut, IntPtr previewHandle)
        {
            _inputFile = inputFile;
            _outputFile = outputFile;
            _previewHandle = previewHandle;
            _markIn = markIn;
            _markOut = markOut;

            Log.Info("Start Export for input file: \"{0}\" and output file: \"{1}\" for range: {2} - {3}", _inputFile,
                _outputFile, _markIn, markOut);

            // Check if already previewing
            if (Status == BTFPTrimmerStatus.InPreview)
            {
                Log.Info("Already previewing. Call stop");
                Stop();
            }

            // Check if we're in trimming state
            if (Status == BTFPTrimmerStatus.InExport)
            {
                Log.Info("In export. Call stop.");
                Stop();
            }

            ICaptureGraphBuilder2 captureGraphBuilder = null;
            int hr = 0;
            IBaseFilter pSourceFilter = null;
            IBaseFilter pMp4DemuxFilter = null;
            IBaseFilter pMuxerFilter = null;
            try
            {
                // Create filter graph and interfaces for control.
                CreateFilterGraphAndInterfaces();

                // Create and configure ICaptureGraphBuilder
                Log.Info("Create and configure ICaptureGraphBuilder.");
                captureGraphBuilder = (ICaptureGraphBuilder2) new CaptureGraphBuilder2();
                hr = captureGraphBuilder.SetFiltergraph(_filterGraph);
                DsError.ThrowExceptionForHR(hr);

                // Add source filter
                DSUtils.AddFileSource(_filterGraph, _inputFile, out pSourceFilter);

                // Add Mp4 Demux filter
                DSUtils.AddMp4DemuxFilter(_filterGraph, out pMp4DemuxFilter);

                //
                // Now add and connect file sink portion of the graph

                // Create MP4 muxer filter
                DSUtils.AddMp4SinkFilter(_filterGraph, _outputFile, out pMuxerFilter);

                // Connect video inf tee with H264 encoder and file sink
                Log.Info("Connect video inf tee with h264 encoder and mp4 muxer.");
                //hr = captureGraphBuilder.RenderStream(null, null, pVideoInfTee, null, pMuxerFilter);
                /*hr = captureGraphBuilder.RenderStream(null, null, pSourceFilter, pMp4DemuxFilter, pMuxerFilter);
                DsError.ThrowExceptionForHR(hr);*/
                hr = captureGraphBuilder.RenderStream(null, null, pSourceFilter, null, pMp4DemuxFilter);
                DsError.ThrowExceptionForHR(hr);
                hr = captureGraphBuilder.RenderStream(null, MediaType.Video, pMp4DemuxFilter, null, pMuxerFilter);
                DsError.ThrowExceptionForHR(hr);

                ConnectAudioMultipleChannels(pMp4DemuxFilter, pMuxerFilter, captureGraphBuilder);

                // Set notify window for DirectShow Messages.
                SetNotifyWindow();

                long duration;
                _mediaSeeking.GetDuration(out duration);
                Duration = duration/(double) UNITS;
                Log.Info("Get duration: {0}", Duration);

                Log.Info("Set mark in and mark out positions");
                hr = _mediaSeeking.SetPositions(new DsLong((long) (_markIn*UNITS)),
                    AMSeekingSeekingFlags.AbsolutePositioning, new DsLong((long) (_markOut*UNITS)),
                    AMSeekingSeekingFlags.AbsolutePositioning);
                DsError.ThrowExceptionForHR(hr);

                SetSyncSource(null);

                Status = BTFPTrimmerStatus.InExport;
                hr = _mediaControl.Run();
                DsError.ThrowExceptionForHR(hr);
                IsRunning = true;
            }
            catch
            {
                Status = BTFPTrimmerStatus.Idle;
                throw;
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
                    Marshal.ReleaseComObject(pMuxerFilter);
                }
            }
        }
        /// <summary>
        /// Starts running the graph.
        /// </summary>
        public void Play()
        {
            Log.Info("Play.");
            if (Status != BTFPTrimmerStatus.InPreview)
            {
                Log.Info("Status is not preview. Return...");
                return;
            }
            if (IsRunning)
            {
                Log.Info("Already running. Return...");
                return;
            }

            _mediaControl.Run();
            IsRunning = true;
        }
        /// <summary>
        /// Pauses the running graph.
        /// </summary>
        public void Pause()
        {
            Log.Info("Pause.");
            if (Status != BTFPTrimmerStatus.InPreview)
            {
                Log.Info("Status is not preview. Return...");
                return;
            }

            _mediaControl.Pause();
            IsRunning = false;
        }
        /// <summary>
        /// Stops the running graph and destroys it.
        /// </summary>
        public void Stop()
        {
            Log.Info("Stop.");
            if (Status == BTFPTrimmerStatus.Idle)
            {
                Log.Info("Already Idle status. Return...");
                return;
            }

            try
            {
                if (_mediaControl != null)
                {
                    _mediaControl.Stop();
                }
                _mediaControl = null;

                if (_pWavMetersFilters != null)
                {
                    foreach (var pWavMeterFilter in _pWavMetersFilters)
                    {
                        Marshal.ReleaseComObject(pWavMeterFilter);
                    }
                    _pWavMetersFilters.Clear();
                }
                if (_piWavMeters != null)
                {
                    _piWavMeters.Clear();
                }
                if (_mediaEvent != null)
                {
                    _mediaEvent.SetNotifyWindow(IntPtr.Zero, 0, IntPtr.Zero);
                }
                if (_pVideoRendererFilter != null)
                {
                    Marshal.ReleaseComObject(_pVideoRendererFilter);
                    _pVideoRendererFilter = null;
                }
                if (_pVideoDisplayControl != null)
                {
                    Marshal.ReleaseComObject(_pVideoDisplayControl);
                }
                if (_filterGraph != null)
                {
                    Marshal.ReleaseComObject(_filterGraph);
                }

                _mediaControl = null;
                _mediaEvent = null;
                _mediaSeeking = null;
                _pVideoDisplayControl = null;

                LavAudioDecoderFilterLoader.Unload();
                LavVideoDecoderFilterLoader.Unload();
            }
            finally
            {
                Status = BTFPTrimmerStatus.Idle;
                IsRunning = false;
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
                            //if (ErrorAbort != null)
                            //{
                            //    ErrorAbort.BeginInvoke(this, EventArgs.Empty, null, null);
                            //}
                        }
                    }
                }
            }
            catch (Exception ex)
            {
                Log.Error("Handle Event errpr: {0}", ex.Message);
            }
        }
        /// <summary>
        /// This method resizes the preview video. Should be called when form is resized.
        /// </summary>
        public void OnResize()
        {
            Log.Info("OnResize.");
            if (Status == BTFPTrimmerStatus.Idle)
            {
                Log.Info("We're idle. Return...");
                return;
            }
            SetupEvr();
        }
        /// <summary>
        /// Gets audio peaks. By default, it reads 6 channel peaks.
        /// </summary>
        /// <returns></returns>
        public double[] GetPeaks()
        {
            // Instantiate peaks and initialize to their default, min value.
            var peaks = new double[6];
            for (int i = 0; i < peaks.Length; i++)
            {
                peaks[i] = -96;
            }
            if (_piWavMeters != null)
            {
                for (int i = 0; i < _piWavMeters.Count; i++)
                {
                    if (i < peaks.Length)
                    {
                        var tempPeaks = new double[8];
                        _piWavMeters[i].GetPeaks(tempPeaks);
                        // Only the first channel. There should be 6 tracks with one audio channel.
                        peaks[i] = tempPeaks[0];
                    }
                }
            }
            return peaks;
        }
        /// <summary>
        /// Select audio channels that should only be audible for preview and set volume for each.
        /// </summary>
        /// <param name="selectedChannels">List of 6 channels. True will enable channel, False will disable it.</param>
        /// <param name="volume">Volumes that will be applied to enabled channels. Volume for disabled channels will be ignored.</param>
        public void SelectAudioChannels(bool[] selectedChannels, double[] volume, bool muteAll)
        {
            // If any channel is selected, this will mean we want this channel ON and others mute.
            // If no channel is selected, we want all the channels on.
            // If mute all is on, mute them all, no matter what
            if (muteAll)
            {
                for (int i = 0; i < selectedChannels.Length; i++)
                {
                    selectedChannels[i] = true;
                }
            }
            else if (selectedChannels.Any(t => t))
            {
                for (int i = 0; i < selectedChannels.Length; i++)
                {
                    selectedChannels[i] = !selectedChannels[i];
                }
            }

            if (_piWavMeters != null)
            {
                for (int i = 0; i < _piWavMeters.Count; i++)
                {
                    var tempVolumes = new double[8];

                    // Only the first channel. There should be 6 separate audio tracks, each with 1 channel.
                    tempVolumes[0] = volume[i];

                    _piWavMeters[i].SetVolume(tempVolumes);

                    var tempMutes = new bool[8];
                    tempMutes[0] = selectedChannels[i];
                    _piWavMeters[i].SetMute(tempMutes);
                }
            }
        }
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
            _mediaSeeking = _filterGraph as IMediaSeeking;
        }
        /// <summary>
        /// Adds file source filter in filter graph, for the specified file name.
        /// </summary>
        /// <param name="fileName"></param>
        /// <param name="pFileSource"></param>
        private void AddFileSource(string fileName, out IBaseFilter pFileSource)
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
            //int hr = _filterGraph.AddSourceFilter(fileName, Path.GetFileName(fileName), out pFileSource);
            //DsError.ThrowExceptionForHR(hr);

        }
        /// <summary>
        /// Adds mp4 demuxer filter to the graph.
        /// </summary>
        /// <param name="pMp4Demux"></param>
        private void AddMp4DemuxFilter(out IBaseFilter pMp4Demux)
        {
            Log.Info("Add Mp4 Demux filter.");
            pMp4Demux = GDCLMp4DemuxFilterLoader.LoadFilter();
            int hr = _filterGraph.AddFilter(pMp4Demux, "Mp4 Demux");
            DsError.ThrowExceptionForHR(hr);
        }
        /// <summary>
        /// Creates and adds video renderer filter to the graph.
        /// </summary>
        /// <param name="pVideoRenderer"></param>
        private void AddVideoRenderer(out IBaseFilter pVideoRenderer)
        {
            Log.Info("Create EVR Filter.");
            pVideoRenderer = (IBaseFilter)new EnhancedVideoRenderer();
            int hr = _filterGraph.AddFilter(pVideoRenderer, "EVR");
            DsError.ThrowExceptionForHR(hr);
        }
        /// <summary>
        /// Add our own video decoder filter.
        /// </summary>
        /// <param name="pLavVideoDevcoder"></param>
        private void AddLavVideoDecoderFilter(out IBaseFilter pLavVideoDevcoder)
        {
            Log.Info("Add Lav Video Decoder Filter to graph.");

            pLavVideoDevcoder = LavVideoDecoderFilterLoader.LoadFilter();
            int hr = _filterGraph.AddFilter(pLavVideoDevcoder, string.Format("Lav Video Decoder"));
            DsError.ThrowExceptionForHR(hr);
        }
        /// <summary>
        /// Renders multiple audio tracks from pMuxer. It renders streams to separate DSound devices and adds WavMeter filters
        /// just before Direct Sound Renderers.
        /// </summary>
        /// <param name="pMuxer"></param>
        /// <param name="captureGraphBuilder2"></param>
        /// <param name="pWavMeters"></param>
        private void RenderMultipleAudioTracks(IBaseFilter pMuxer, ICaptureGraphBuilder2 captureGraphBuilder2, out List<IBaseFilter> pWavMeters)
        {
            Log.Info("Render multiple audio tracks.");
            // Find all unconnected pins. In this app setup, there should be 6
            // separate audio tracks, each with single channel of audio.
            int index = 1;// Pin 0 should be video.
            pWavMeters = new List<IBaseFilter>();
            List<IBaseFilter> renderers = new List<IBaseFilter>();
            int hr = 0;
            while (true)
            {
                IBaseFilter pAudioRenderFilter = null;
                IBaseFilter pLavAudioDecoder = null;
                IPin pOutPin = null;
                try
                {
                    // Find next pin
                    Log.Info("Find audio pin {0}", index);
                    //IPin pOutPin = DsFindPin.ByDirection(pMuxer, PinDirection.Output, index);
                    pOutPin = DsFindPin.ByConnectionStatus(pMuxer, PinConnectedStatus.Unconnected, 0);
                    if (pOutPin != null)
                    {
                        Log.Info("Create Wav Meter for audio output: {0}", index);
                        IBaseFilter pWavMeterFilter = null;
                        DSUtils.AddWavMeterFilter(_filterGraph, out pWavMeterFilter, index);
                        pWavMeters.Add(pWavMeterFilter);

                        Log.Info("Add DSound Renderer for audio output: {0}", index);
                        
                        // Add audio renderer.
                        DSUtils.AddAudioRender(_filterGraph, DecklinkCaptureLibConfigurationManager.Instance.Configuration.AudioRenderer, out pAudioRenderFilter, string.Format("Audio render {0}", index));
                        //AddAudioRenderer(out pAudioRenderFilter);

                        // Add our own aac decoder.
                        DSUtils.AddLavAudioDecoder(_filterGraph, out pLavAudioDecoder, index);

                        IPin wavOutputPin = DsFindPin.ByDirection(pWavMeterFilter, PinDirection.Output, 0);
                        /*hr = DSUtils.ConnectOutputPin(_filterGraph, pOutPin, pWavMeterFilter);
                        DsError.ThrowExceptionForHR(hr);
                        
                        hr = DSUtils.ConnectOutputPin(_filterGraph, wavOutputPin, pAudioRenderFilter);
                        DsError.ThrowExceptionForHR(hr);*/
                        Log.Info("Render audio stream. Muxer - Wav Meter - DSound.");
                        hr = captureGraphBuilder2.RenderStream(null, MediaType.Audio, pMuxer, pWavMeterFilter, pAudioRenderFilter);
                        DsError.ThrowExceptionForHR(hr);
                        renderers.Add(pAudioRenderFilter);
                        /*Log.Info("Render audio stream. Muxer - Wav Meter - DSound.");
                        hr = captureGraphBuilder2.RenderStream(null, MediaType.Audio, pWavMeterFilter, null, pAudioRenderFilter);
                        DsError.ThrowExceptionForHR(hr);*/

                        // Release pin
                        
                        //Marshal.ReleaseComObject(pWavMeterFilter);
                        Marshal.ReleaseComObject(pLavAudioDecoder);
                        Marshal.ReleaseComObject(wavOutputPin);
                    }
                    else
                    {
                        Log.Info("No pin for index: {0}. Exiting...", index);
                        break;
                    } 
                }
                catch (Exception ex)
                {
                    Log.Error("RenderMultipleAudioTracks error. Details: {0}", ex.Message);
                    break;
                }
                finally
                {
                    index++;
                    if (pAudioRenderFilter != null)
                    {
                        Marshal.ReleaseComObject(pAudioRenderFilter);
                    }
                    if (pLavAudioDecoder != null)
                    {
                        Marshal.ReleaseComObject(pLavAudioDecoder);
                    }
                    if (pOutPin != null)
                    {
                        Marshal.ReleaseComObject(pOutPin);
                        pOutPin = null;
                    }
                }
            }
        }
        /// <summary>
        /// Add our csutom wav meter and volume control filter.
        /// </summary>
        /// <param name="pWavMeterFilter"></param>
        /*private void AddWavMeterFilter(out IBaseFilter pWavMeterFilter, int index)
        {
            Log.Info("Add Wav Meter Filter to graph.");

            pWavMeterFilter = (IBaseFilter)new WavMeterEx();
            int hr = _filterGraph.AddFilter(pWavMeterFilter, string.Format("Wav Meter {0}", index));
            DsError.ThrowExceptionForHR(hr);
        }*/
        /// <summary>
        /// Creates audio renderer - usually Default DSound Device - and adds it to the graph.
        /// </summary>
        /// <param name="pAudioRender"></param>
        private void AddAudioRenderer(out IBaseFilter pAudioRender)
        {
            int hr = 0;
            Log.Info("Create DSound Renderer Filter.");
            pAudioRender = (IBaseFilter)new DSoundRender(); //(IBaseFilter) new DefaultWaveOut();
            hr = _filterGraph.AddFilter(pAudioRender, "Audio Renderer");
            DsError.ThrowExceptionForHR(hr);
            //if (DecklinkCaptureLibConfigurationManager.Instance.Configuration.AudioPreview)
            //{
            //    if (!string.IsNullOrEmpty(DecklinkCaptureLibConfigurationManager.Instance.Configuration.AudioRenderer))
            //    {
            //        Log.Info("Create specified audio renderer: \"{0}\"",
            //            DecklinkCaptureLibConfigurationManager.Instance.Configuration.AudioRenderer);

            //        pAudioRender = FilterGraphTools.AddFilterByName(_filterGraph, FilterCategory.AudioRendererCategory,
            //            DecklinkCaptureLibConfigurationManager.Instance.Configuration.AudioRenderer);
            //    }
            //    else
            //    {
            //        Log.Info("Create DSound Renderer Filter.");
            //        pAudioRender = (IBaseFilter)new DSoundRender(); //(IBaseFilter) new DefaultWaveOut();
            //        hr = _filterGraph.AddFilter(pAudioRender, "Audio Renderer");
            //        DsError.ThrowExceptionForHR(hr);
            //    }
            //}
            //else
            //{
            //    Log.Info("Create Null Renderer for audio stream.");
            //    pAudioRender = (IBaseFilter)new NullRenderer();
            //    hr = _filterGraph.AddFilter(pAudioRender, "Audio Renderer");
            //    DsError.ThrowExceptionForHR(hr);
            //}
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
        /// Initialize EVR
        /// </summary>
        /// <param name="pEVR"></param>
        private void InitEvr(IBaseFilter pEVR)
        {
            // Set the video window.
            object o;
            IMFGetService pGetService = null;
            pGetService = (IMFGetService)pEVR;
            pGetService.GetService(MFServices.MR_VIDEO_RENDER_SERVICE, typeof(IMFVideoDisplayControl).GUID, out o);

            try
            {
                _pVideoDisplayControl = (IMFVideoDisplayControl)o;
                _pVideoDisplayControl.SetVideoWindow(_previewHandle);
            }
            catch (Exception ex)
            {
                Marshal.ReleaseComObject(o);
                Log.Error("Failed to create IMFVideoDisplayControl. Details: {0}", ex.Message);
            }
        }
        /// <summary>
        /// Setup EVR video window.
        /// </summary>
        private void SetupEvr()
        {
            Log.Info("Setup EVR.");
            if (_previewHandle == IntPtr.Zero || _pVideoDisplayControl == null)
            {
                Log.Error("PreviewHandle: {0}, or IVideoDisplayControl: {1} is null.", _previewHandle, _pVideoDisplayControl);
                return;
            }

            if (_filterGraph == null)
            {
                return;
            }

            Size sz = NativeHelpers.GetControlSize(_previewHandle);
            Log.Info("The size of the preview window: {0}x{1}", sz.Width, sz.Height);

            int width = sz.Width;
            int height = sz.Height;
            MFRect rc = new MFRect(0, 0, width, height);
            Log.Info("Set width: {0}, height: {1}", width, height);
            _pVideoDisplayControl.SetAspectRatioMode(MFVideoAspectRatioMode.PreservePicture);
            _pVideoDisplayControl.SetVideoPosition(null, rc);
        }
        /// <summary>
        /// Creates inf tee filters.
        /// </summary>
        /// <param name="pInfTee"></param>
        private void CreateInfTeeFilters(out IBaseFilter pInfTee)
        {
            Log.Info("Create inf tee filter.");
            pInfTee = (IBaseFilter)new SmartTee();//InfTee();
            int hr = _filterGraph.AddFilter(pInfTee, "Inf Tee");
            DsError.ThrowExceptionForHR(hr);
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
        /// Connects multiple tracks from demuxer to muxer.
        /// </summary>
        /// <param name="pMp4DemuxFilter"></param>
        /// <param name="pMp4MuxerFilter"></param>
        /// <param name="captureGraphBuilder2"></param>
        private void ConnectAudioMultipleChannels(IBaseFilter pMp4DemuxFilter, IBaseFilter pMp4MuxerFilter, ICaptureGraphBuilder2 captureGraphBuilder2)
        {
            Log.Info("Render multiple audio tracks.");
            // Find all unconnected pins. In this app setup, there should be 6
            // separate audio tracks, each with single channel of audio.
            int index = 0;// Pin 0 should be video.         

            int hr = 0;
            while (true)
            {
                try
                {
                    // Find next pin
                    Log.Info("Find audio pin {0}", index);
                    
                    //IPin pOutPin = DsFindPin.ByDirection(pMp4DemuxFilter, PinDirection.Output, index);
                    IPin pOutPin = DsFindPin.ByConnectionStatus(pMp4DemuxFilter, PinConnectedStatus.Unconnected, 0);
                    
                    if (pOutPin != null)
                    {
                        {
                            Log.Info("Render audio stream. Demuxer - Muxer.");
                            hr = captureGraphBuilder2.RenderStream(null, null, pOutPin, null, pMp4MuxerFilter);
                            // Release pin
                            Marshal.ReleaseComObject(pOutPin);
                            DsError.ThrowExceptionForHR(hr);                            
                        } 
                    }
                    else
                    {
                        Log.Info("No pin for index: {0}. Exiting...", index);
                        break;
                    }

                    MuteAudio(true);
                }
                catch (Exception ex)
                {
                    Log.Error("RenderMultipleAudioTracks error. Details: {0}", ex.Message);
                    break;
                }
                finally
                {
                    index++;
                }
            }
        }
        /// <summary>
        /// Set sync source toi filter graph.
        /// </summary>
        /// <param name="refClock"></param>
        private void SetSyncSource(IReferenceClock refClock)
        {
            Log.Info("Set sync source to: {0}", refClock);
            IMediaFilter pMediaFilter = _filterGraph as IMediaFilter;
            pMediaFilter.SetSyncSource(refClock);
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
