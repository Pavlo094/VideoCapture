#define USE_EVR
#define VIDEO_ENABLED
#define AUDIO_ENABLED
using System;
using System.Collections.Generic;
using System.Diagnostics.Contracts;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices;
using System.Runtime.InteropServices.ComTypes;
using System.Text;
using System.Threading;
using System.Windows.Forms;
using NLog;
using DirectShowLib;
using DirectShowLib.Utils;
using DecklinkCaptureLib.Filters;
using MediaFoundation;
using MediaFoundation.EVR;
using MediaFoundation.Misc;
using AMMediaType = DirectShowLib.AMMediaType;
using VideoInfoHeader = DirectShowLib.VideoInfoHeader;
using WaveFormatEx = DirectShowLib.WaveFormatEx;
using DeckLinkAPI;

namespace DecklinkCaptureLib
{
    /// <summary>
    /// Singleton implementation of DecklinkCaptureLib. This class exposes method for capturing from
    /// decklink cards to MP4/H264 and AAC audio. It exposes methods for reading the installed devices
    /// on the system, starting and stoping capture to file, and VUMeters and audio volume control for
    /// up to 6 channels of audio.
    /// </summary>
    public class DecklinkCaptureLib
    {
        #region Logger
        private static readonly Logger Log = LogManager.GetCurrentClassLogger();
        #endregion

        #region Members
        /// <summary>
        /// The only instance of this class.
        /// </summary>
        private static DecklinkCaptureLib _instance;
        /// <summary>
        /// Flag for directshow windows message
        /// </summary>
        public const int WM_GRAPHNOTIFY = 0x8000 + 1;
        /// <summary>
        /// Main filtergraph object.
        /// </summary>
        private IGraphBuilder _filterGraph = null;
        /// <summary>
        /// Interface for controlling filter graph states.
        /// </summary>
        private IMediaControl _mediaControl = null;
        /// <summary>
        /// Interface for processing filter graph events.
        /// </summary>
        private IMediaEventEx _mediaEvent = null;
        /// <summary>
        /// Handle to the preview window.
        /// </summary>
        private IntPtr _previewHandle = IntPtr.Zero;
        /// <summary>
        /// Wav meter interface for controlling the VU meter and volume control filter for preview branch.
        /// </summary>
        private IWavMeterEx _iWavMeterPreview = null;
        /// <summary>
        ///  Wav meter interface for controlling the VU meter and volume control filter for recording branch.
        /// </summary>
        private IWavMeterEx _iWavMeterRecord = null;

        private IBaseFilter _pWavMeterPreviewFilter = null;
        private IBaseFilter _pWavMeterRecordFilter = null;
        private string _currentOutputFile = string.Empty;
        private IAMDroppedFrames _droppedFrames = null;
        private IBaseFilter _sourceFilter = null;
        private IVideoWindow _videoWindow;
        private IMFVideoDisplayControl _pVideoDisplayControl = null;
        private IBaseFilter _pVideoRendererFilter = null;
        #endregion

        #region Events
        /// <summary>
        /// This event is fired when DirectShow fires it's evil EC_ErrorAbort event.
        /// </summary>
        public event EventHandler ErrorAbort;
        #endregion

        #region Constructor

        /// <summary>
        /// Private constructor for singleton implementation.
        /// </summary>
        private DecklinkCaptureLib()
        {
            MsgHandle = IntPtr.Zero;
        }
        #endregion

        #region Instance
        /// <summary>
        /// The only instance of this class
        /// </summary>
        public static DecklinkCaptureLib Instance
        {
            get { return _instance ?? (_instance = new DecklinkCaptureLib()); }
        }
        #endregion

        #region Methods
        /// <summary>
        /// Starts video/audio preview from the selected decklinks card.
        /// </summary>
        /// <param name="previewHandle">Handle to the control for video preview</param>
        public void StartPreview(IntPtr previewHandle)
        {
            Log.Info("Start preview.");
            // Check if we're already previewing.
            if (IsPreviewing)
            {
                Log.Info("Already previewing.");
                return;
            }
            // Check if we're recording
            if (IsRecording)
            {
                Log.Info("Recording. Call stop.");
                Stop();
            }

            // Check if we have input video and audio decklink cards
            if (string.IsNullOrEmpty(DecklinkCaptureLibConfigurationManager.Instance.Configuration.DecklinkDevice))
            {
                throw new DecklinkCaptureLibException("Input Decklink card must be set.");
            }

            IBaseFilter pAudioRender = null;
            ICaptureGraphBuilder2 captureGraphBuilder = null;
            IBaseFilter pChannelDownmixer = null;
            _previewHandle = previewHandle;            
            int hr = 0;
            try
            {
                // Create filtergraph and interfaces for control    
                CreateFilterGraphAndInterfaces();

                //_sourceFilter = (IBaseFilter)new DeckLinkHDStereoSource();// DecklinkSourceLoader.LoadFilter();
                _sourceFilter = DecklinkSourceLoader.LoadFilter();
                _droppedFrames = _sourceFilter as IAMDroppedFrames;
                IBaseFilter pVideoSource = _sourceFilter;
                IBaseFilter pAudioSource = pVideoSource;

                DecklinkSourceLoader.ConfigureFilter(pVideoSource,
                    DecklinkCaptureLibConfigurationManager.Instance.Configuration.DecklinkDevice,
                    DecklinkCaptureLibConfigurationManager.Instance.Configuration.VideoWidth,
                    DecklinkCaptureLibConfigurationManager.Instance.Configuration.VideoHeight,
                    DecklinkCaptureLibConfigurationManager.Instance.Configuration.VideoAvgTimePerFrame,
                    (int)DecklinkCaptureLibConfigurationManager.Instance.Configuration.FieldOrder);

                // Configure capture graph builder
                Log.Info("Configure Capture Graph Builder.");
                captureGraphBuilder = (ICaptureGraphBuilder2)new CaptureGraphBuilder2();
                hr = captureGraphBuilder.SetFiltergraph(_filterGraph);
                DsError.ThrowExceptionForHR(hr);

                _filterGraph.AddFilter(pVideoSource, "Video Source");

                // Add video and audio renderer
                //AddVideoAndAudioRender(out _pVideoRendererFilter, out pAudioRender);
                DSUtils.AddVideoRenderer(_filterGraph, out _pVideoRendererFilter);
                // Render video stream
                Log.Info("Render video stream.");
                hr = captureGraphBuilder.RenderStream(PinCategory.Preview, MediaType.Video, pVideoSource, null, _pVideoRendererFilter);
                DsError.ThrowExceptionForHR(hr);

                if (DecklinkCaptureLibConfigurationManager.Instance.Configuration.AudioPreview)
                {
                    DSUtils.AddAudioRender(_filterGraph, DecklinkCaptureLibConfigurationManager.Instance.Configuration.AudioRenderer, out pAudioRender);
                    //_filterGraph.AddFilter(pAudioSource, "Audio Source");
                    // Add wav meter filter
                    DSUtils.AddWavMeterFilter(_filterGraph, out _pWavMeterPreviewFilter);
                    _iWavMeterPreview = _pWavMeterPreviewFilter as IWavMeterEx;

                    // Add channel downmixer
                    DSUtils.AddAudioChannelDownmixer(_filterGraph, out pChannelDownmixer);

                    Log.Info("Render audio stream to channel downmixer.");
                    hr = captureGraphBuilder.RenderStream(null, MediaType.Audio, pAudioSource, _pWavMeterPreviewFilter, pChannelDownmixer);
                    //DsError.ThrowExceptionForHR(hr);
                    if (hr != 0)
                    {
                        Log.Error("Audio render failed with hr={0}", hr);
                    }

                    Log.Info("Render audio stream from channel downmixer to audio render");
                    hr = captureGraphBuilder.RenderStream(null, MediaType.Audio, pChannelDownmixer, null, pAudioRender);
                    if (hr != 0)
                    {
                        Log.Error("Audio render failed with hr={0}", hr);
                    }
                }

                SetNotifyWindow();

#if USE_EVR
                InitEvr(_pVideoRendererFilter);
                SetupEvr();
#else
                InitVideoWIndow();
                SetupVideoWindow();
#endif
                _shouldStartApi = true;
                Log.Info("Run.");
                _mediaControl.Run();
                IsPreviewing = true;
            }
            finally
            {
                if (captureGraphBuilder != null)
                {
                    captureGraphBuilder.SetFiltergraph(null);
                    Marshal.ReleaseComObject(captureGraphBuilder);
                }

                if (pAudioRender != null)
                {
                    Marshal.ReleaseComObject(pAudioRender);
                }
                if (pChannelDownmixer != null)
                {
                    Marshal.ReleaseComObject(pChannelDownmixer);
                }
            }
        }

        private bool _shouldStartApi = false;
        /// <summary>
        /// Starts capture to specified file name.
        /// </summary>
        /// <param name="fileName"></param>
        /// <param name="previewHandle"></param>
        public void StartCapture(string fileName, IntPtr previewHandle)
        {
            Log.Info("Start capture to file: \"{0}\"", fileName);
            // Check if we are in preview mode.
            if (IsPreviewing || IsRecording)
            {
                Log.Info("Previewing or recording. Stop graph.");
                Stop();
            }

            if (string.IsNullOrEmpty(DecklinkCaptureLibConfigurationManager.Instance.Configuration.DecklinkDevice))
            {
                throw new DecklinkCaptureLibException("Input Decklink card must be set.");
            }

            IBaseFilter pAudioRender = null;
            IBaseFilter pChannelDownmixer = null;
            IBaseFilter pMuxerFilter = null;
            IBaseFilter pX264Filter = null;
            IBaseFilter pVideoInfTeeFilter = null;
            IBaseFilter pAudioInfTeeFilter = null;
            ICaptureGraphBuilder2 captureGraphBuilder = null;
            _previewHandle = previewHandle;
            _currentOutputFile = fileName;
            try
            {
                // Create filter graph and controlling interfaces.
                CreateFilterGraphAndInterfaces();

                //DecklinApiWrapper.Instance.Init();

                _sourceFilter = DecklinkSourceLoader.LoadFilter();
                _droppedFrames = _sourceFilter as IAMDroppedFrames;
                IBaseFilter pVideoSource = _sourceFilter;
                IBaseFilter pAudioSource = _sourceFilter;

                DecklinkSourceLoader.ConfigureFilter(pVideoSource,
                    DecklinkCaptureLibConfigurationManager.Instance.Configuration.DecklinkDevice,
                    DecklinkCaptureLibConfigurationManager.Instance.Configuration.VideoWidth,
                    DecklinkCaptureLibConfigurationManager.Instance.Configuration.VideoHeight,
                    DecklinkCaptureLibConfigurationManager.Instance.Configuration.VideoAvgTimePerFrame,
                    (int)DecklinkCaptureLibConfigurationManager.Instance.Configuration.FieldOrder);

                // Configure capture graph builder
                Log.Info("Configure Capture Graph Builder.");
                captureGraphBuilder = (ICaptureGraphBuilder2)new CaptureGraphBuilder2();
                int hr = captureGraphBuilder.SetFiltergraph(_filterGraph);
                DsError.ThrowExceptionForHR(hr);

                // Create MP4 muxer filter
                DSUtils.AddMp4SinkFilter(_filterGraph, _currentOutputFile, out pMuxerFilter); 

#if VIDEO_ENABLED

                _filterGraph.AddFilter(pVideoSource, "Video Source");

                // Create InfTee filters
                DSUtils.CreateInfTeeFilter(_filterGraph, out pVideoInfTeeFilter);                                           
                
                // Connect video source to inf tee and video renderer, i.e. create preview portion of the graph
                /*Log.Info("Connect video source to inf tee and video renderer");
                hr = captureGraphBuilder.RenderStream(null, MediaType.Video, pVideoSource, null, pVideoInfTeeFilter);
                DsError.ThrowExceptionForHR(hr);*/

                //IPin videoCapturePin = DsFindPin.ByDirection(pVideoInfTeeFilter, PinDirection.Output, 0);
                IPin videoCapturePin = DsFindPin.ByDirection(pVideoSource, PinDirection.Output, 0);
                if (videoCapturePin != null)
                {
                    // Create x264 encoder filter
                    bool interlaced = (DecklinkCaptureLibConfigurationManager.Instance.Configuration.FieldOrder == _BMDFieldDominance.bmdUpperFieldFirst) || (DecklinkCaptureLibConfigurationManager.Instance.Configuration.FieldOrder == _BMDFieldDominance.bmdLowerFieldFirst);
                    DSUtils.CreateX264Filter(_filterGraph, videoCapturePin, out pX264Filter, interlaced,  DecklinkCaptureLibConfigurationManager.Instance.Configuration.VideoBitrate);
                    Marshal.ReleaseComObject(videoCapturePin);

                    // Connect video inf tee with H264 encoder and file sink
                    Log.Info("Connect video inf tee with h264 encoder and mp4 muxer.");
                    hr = captureGraphBuilder.RenderStream(null, null, pX264Filter, null, pMuxerFilter);                    
                    /*IPin x264OutPin = DsFindPin.ByDirection(pX264Filter, PinDirection.Output, 0);
                    if (null != x264OutPin)
                    {
                        hr = DSUtils.ConnectOutputPin(_filterGraph, x264OutPin, pMuxerFilter);
                        Marshal.ReleaseComObject(x264OutPin);
                    }
                    else
                        hr = -1;*/
                    DsError.ThrowExceptionForHR(hr);
                }

                
                // Add video and audio renderer                
                DSUtils.AddVideoRenderer(_filterGraph, out _pVideoRendererFilter);

                //IPin pVideoPreviewPin = DsFindPin.ByDirection(pVideoInfTeeFilter, PinDirection.Output, 1);
                IPin pVideoPreviewPin = DsFindPin.ByDirection(pVideoSource, PinDirection.Output, 2);
                if (pVideoPreviewPin != null)
                {
                    hr = DSUtils.ConnectOutputPin(_filterGraph, pVideoPreviewPin, _pVideoRendererFilter);
                    Marshal.ReleaseComObject(pVideoPreviewPin);
                    DsError.ThrowExceptionForHR(hr);                    
                }
#else 
                _filterGraph.AddFilter(pAudioSource, "Audio Source");
#endif

#if AUDIO_ENABLED
                //Audio
                DSUtils.AddWavMeterFilter(_filterGraph, out _pWavMeterRecordFilter);
                _iWavMeterRecord = _pWavMeterRecordFilter as IWavMeterEx;                

                if (DecklinkCaptureLibConfigurationManager.Instance.Configuration.AudioPreview)
                {
                    DSUtils.AddAudioRender(_filterGraph, DecklinkCaptureLibConfigurationManager.Instance.Configuration.AudioRenderer, out pAudioRender);
                    DSUtils.CreateInfTeeFilter(_filterGraph, out pAudioInfTeeFilter);

                    // Connect audio source with wave meter and inf tee
                    Log.Info("Connect audio source to wav meter and inf tee");
                    hr = captureGraphBuilder.RenderStream(null, MediaType.Audio, pAudioSource, null,
                        pAudioInfTeeFilter);
                    DsError.ThrowExceptionForHR(hr);

                    IPin pAudioOutputPin = DsFindPin.ByDirection(pAudioInfTeeFilter, PinDirection.Output, 0);
                    // Add wav meter filter
                    DSUtils.AddWavMeterFilter(_filterGraph, out _pWavMeterPreviewFilter);
                    _iWavMeterPreview = _pWavMeterPreviewFilter as IWavMeterEx;

                    Log.Info("Connect audio inf tee with channel downmixer and audio renderer.");
                    if (pAudioOutputPin != null)
                    {
                        hr = DSUtils.ConnectOutputPin(_filterGraph, pAudioOutputPin, _pWavMeterPreviewFilter);
                        Marshal.ReleaseComObject(pAudioOutputPin);
                        pAudioOutputPin = null;
                        DsError.ThrowExceptionForHR(hr);
                    }
                    // Add channel downmixer needed for audio preview
                    DSUtils.AddAudioChannelDownmixer(_filterGraph, out pChannelDownmixer);
                    hr = captureGraphBuilder.RenderStream(null, MediaType.Audio, _pWavMeterPreviewFilter, pChannelDownmixer, pAudioRender);
                    if (hr != 0)
                    {
                        Log.Error("Audio render failed with hr={0}", hr);
                    }

                    hr = captureGraphBuilder.RenderStream(null, MediaType.Audio, pAudioInfTeeFilter, null, _pWavMeterRecordFilter);
                    DsError.ThrowExceptionForHR(hr);                    
                }
                else
                {
                    hr = captureGraphBuilder.RenderStream(null, MediaType.Audio, pAudioSource, null, _pWavMeterRecordFilter);
                    DsError.ThrowExceptionForHR(hr);
                }

                ConnectAudioMultipleChannels(captureGraphBuilder, _pWavMeterRecordFilter, pMuxerFilter);
#endif
                
                SetNotifyWindow();

#if VIDEO_ENABLED
#if USE_EVR
                if (_pVideoRendererFilter != null)
                {
                    InitEvr(_pVideoRendererFilter);
                    SetupEvr();
                }
#else
                InitVideoWIndow();
                SetupVideoWindow();
#endif
#endif
                _shouldStartApi = true;
                _mediaControl.Run();
                
                Log.Info("Run.");
                
                IsRecording = true;
            }
            finally
            {
                if (captureGraphBuilder != null)
                {
                    captureGraphBuilder.SetFiltergraph(null);
                    Marshal.ReleaseComObject(captureGraphBuilder);
                }
                if (pAudioRender != null)
                {
                    Marshal.ReleaseComObject(pAudioRender);
                }
                if (pChannelDownmixer != null)
                {
                    Marshal.ReleaseComObject(pChannelDownmixer);
                }
                if (pMuxerFilter != null)
                {
                    Marshal.ReleaseComObject(pMuxerFilter);
                }
                if (pX264Filter != null)
                {
                    Marshal.ReleaseComObject(pX264Filter);
                }
                if (pVideoInfTeeFilter != null)
                {
                    Marshal.ReleaseComObject(pVideoInfTeeFilter);
                }
                if (pAudioInfTeeFilter != null)
                {
                    Marshal.ReleaseComObject(pAudioInfTeeFilter);
                }
            }
        }
        /// <summary>
        /// Stops preview or record.
        /// </summary>
        public void Stop()
        {
            Log.Info("Stop.");
            if (!IsPreviewing && !IsRecording)
            {
                Log.Warn("Not previewing or recording.");
                return;
            }

            try
            {
                DecklinApiWrapper.Instance.StopAPIThreads();
                DecklinApiWrapper.Instance.Stop();

                if (_mediaControl != null)
                {
                    Log.Info("Stop.");
                    _mediaControl.Stop();
                }

                if (_pWavMeterPreviewFilter != null)
                {
                    Marshal.ReleaseComObject(_pWavMeterPreviewFilter);
                    _iWavMeterPreview = null;
                }
                if (_pWavMeterRecordFilter != null)
                {                    
                    Marshal.ReleaseComObject(_pWavMeterRecordFilter);
                    _iWavMeterRecord = null;
                }
                if (_mediaEvent != null)
                {
                    _mediaEvent.SetNotifyWindow(IntPtr.Zero, 0, IntPtr.Zero);
                }
                if (_videoWindow != null)
                {
                    _videoWindow.put_Visible(OABool.False);
                    _videoWindow.put_Owner(IntPtr.Zero);
                    _videoWindow.put_MessageDrain(IntPtr.Zero);
                }
                if (_pVideoRendererFilter != null)
                {
                    Marshal.ReleaseComObject(_pVideoRendererFilter);
                    _pVideoRendererFilter = null;
                }
                if (_pVideoDisplayControl != null)
                {
                    Marshal.ReleaseComObject(_pVideoDisplayControl);
                    _pVideoDisplayControl = null;
                }
                if (_filterGraph != null)
                {
                    Marshal.ReleaseComObject(_filterGraph);
                    _filterGraph = null;
                }                

                if (_sourceFilter != null)
                {
                    int count = Marshal.ReleaseComObject(_sourceFilter);
                    Log.Info("count {0}", count);
                    _sourceFilter = null;
                }                

                if (_droppedFrames != null)
                {
                    Marshal.ReleaseComObject(_droppedFrames);
                    _droppedFrames = null;
                }
                
                _currentOutputFile = string.Empty;

                if (null != _mediaControl)
                {
                    Marshal.ReleaseComObject(_mediaControl);
                }
                _mediaControl = null;

                if (null != _mediaEvent)
                {
                    Marshal.ReleaseComObject(_mediaEvent);
                }
                _mediaEvent = null;


                //ChannelDownmixerLoader.Unload();
                MonogramAACEncoderFilterLoader.Unload();
                MonogramX264EncoderFilterLoader.Unload();
                MonogramMP4MuxFilterLoader.Unload();
                GDCLMP4MuxFilterLoader.Unload();
                GDCLMp4DemuxFilterLoader.Unload();

                GC.Collect();
            }
            finally
            {
                IsPreviewing = false;
                IsRecording = false;
            }
        }
        /// <summary>
        /// This method resizes the preview video. Should be called when form is resized.
        /// </summary>
        public void OnResize()
        {
            if (!IsPreviewing && !IsRecording)
            {
                return;
            }
#if USE_EVR
            SetupEvr();
#else
            SetupVideoWindow();
#endif
        }
        /// <summary>
        /// If there is audio preview, this will mute/un-mute the output speakers.
        /// </summary>
        /// <param name="mute"></param>
        public void MuteAudio(bool mute)
        {
            Log.Info("Mute speakers: {0}", mute);
            if (_filterGraph == null)
            {
                return;
            }

            var ba = _filterGraph as IBasicAudio;
            if (ba != null)
            {
                int volume = mute ? -10000 : -500;
                Log.Info("Set volume to: {0}", volume);
                ba.put_Volume(volume);
            }
        }
        /// <summary>
        /// Gets audio peaks. By default, it reads 6 channel peaks.
        /// </summary>
        /// <returns></returns>
        public double[] GetPeaks()
        {
            // Instantiate peaks and initialize to their default, min value.
            var peaks = new double[8];
            for(int i=0; i<peaks.Length; i++)
            {
                peaks[i] = -96;
            }
            if (_iWavMeterRecord != null)
            {
                _iWavMeterRecord.GetPeaks(peaks);
            }
            else if (_iWavMeterPreview != null)
            {
                _iWavMeterPreview.GetPeaks(peaks);
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
                for(int i=0; i<selectedChannels.Length; i++)
                {
                    selectedChannels[i] = !selectedChannels[i];
                }
            }
            for (int i = 6; i < selectedChannels.Length; i++)
            {
                selectedChannels[i] = true;
            }
            if (_iWavMeterPreview != null)
            {
                _iWavMeterPreview.SetVolume(volume);

                _iWavMeterPreview.SetMute(selectedChannels);
            }
            if (_iWavMeterRecord != null)
            {
#if DEBUG
                Log.Info(
                    "Recording volume. channel1: {0}, channel2: {1}, channel3: {2}, channel4: {3}, channel5: {4}, channel6: {5}",
                    new object[] {volume[0], volume[1], volume[2], volume[3], volume[4], volume[5]});
#endif
                _iWavMeterRecord.SetVolume(volume);
            }
        }
        /// <summary>
        /// Get number of dropped frames and current file size of the file captured.
        /// </summary>
        /// <param name="numDroppedFrames"></param>
        /// <param name="fileSize">File size in bytes</param>
        public void GetInfo(out int numDroppedFrames, out long fileSize)
        {
            numDroppedFrames = 0;
            fileSize = 0;
            if (!IsRecording)
            {
                return;
            }
            if (File.Exists(_currentOutputFile))
            {
                FileInfo fi = new FileInfo(_currentOutputFile);
                fileSize = fi.Length;
            }
            
            //numDroppedFrames = DecklinApiWrapper.Instance.DroppedFrames;
            if (_droppedFrames != null)
            {
                _droppedFrames.GetNumDropped(out numDroppedFrames);
            }
        }
        /// <summary>
        /// Get number of dropped frames
        /// </summary>
        /// <param name="numDroppedFrames"></param>
        /// <param name="fileSize">File size in bytes</param>
        public int GetDroppedFrames()
        {
            if (!IsRecording)
            {
                return 0;
            }
            int result = 0;
            if (_droppedFrames != null)
            {
                _droppedFrames.GetNumDropped(out result);
            }
            return result;
        }
        /// <summary>
        /// Get number of dropped frames and current file size of the file captured.
        /// </summary>
        /// <param name="numDroppedFrames"></param>
        /// <param name="fileSize">File size in bytes</param>
        public long GetFileSize()
        {
            long fileSize = 0;
            if (!IsRecording)
            {
                return fileSize;
            }
            if (File.Exists(_currentOutputFile))
            {
                FileInfo fi = new FileInfo(_currentOutputFile);
                fileSize = fi.Length;
            }
            return fileSize;
        }
        /// <summary>
        /// Handles DirectShow messages.
        /// </summary>
        /// <param name="m"></param>
        public void HandleEvent(Message m)
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
                    
                    Log.Info("DirectShow event: {0}. HRESULT: {1}, HRESULT text: {2}", evCode, param1.ToInt32(), DsError.GetErrorText(param1.ToInt32()));

                    if (evCode == EventCode.Paused && _shouldStartApi)
                    {
                        DecklinApiWrapper.Instance.Start();
                        _shouldStartApi = false;
                    }

                    if (evCode == EventCode.ErrorAbort)
                    {
                        if (ErrorAbort != null)
                        {
                            ErrorAbort.BeginInvoke(this, EventArgs.Empty, null, null);
                        }
                    }
                }
                //Log.Info("FilterGraphHelper*****************************\n{0}\n***********************************", fgHelper.Text);
            }
        }
        #endregion

        #region Properties
        /// <summary>
        /// Gets or sets if the component is in preview mode.
        /// </summary>
        public bool IsPreviewing { get; private set; }
        /// <summary>
        /// Gets or sets if the component is in record mode.
        /// </summary>
        public bool IsRecording { get; private set; }
        /// <summary>
        /// Gets or sets the window handle where DirectShow messages are dispatched.
        /// </summary>
        public IntPtr MsgHandle { get; set; }
        /// <summary>
        /// Gets current file that the library is capturing to. This file is reset on Stop.
        /// </summary>
        public string CurrentOutputFile{get { return _currentOutputFile; }}
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
        }
        /// <summary>
        /// Add source filters to the graph.
        /// </summary>
        /// <param name="pVideoSource"></param>
        /// <param name="pAudioSource"></param>
        private void AddSourcesToGraph(out IBaseFilter pVideoSource, out IBaseFilter pAudioSource)
        {
            Log.Info("Add video source \"{0}\" to graph",
               DecklinkCaptureLibConfigurationManager.Instance.Configuration.VideoSource);
            pVideoSource = FilterGraphTools.AddFilterByName(_filterGraph, FilterCategory.VideoInputDevice,
                DecklinkCaptureLibConfigurationManager.Instance.Configuration.VideoSource);

            Log.Info("Add audio source \"{0}\" to graph",
                DecklinkCaptureLibConfigurationManager.Instance.Configuration.AudioSource);
            pAudioSource = FilterGraphTools.AddFilterByName(_filterGraph, FilterCategory.AudioInputDevice,
                DecklinkCaptureLibConfigurationManager.Instance.Configuration.AudioSource);
        }
        /// <summary>
        /// Configure video input filter using IAMStreamConfig interface.
        /// </summary>
        /// <param name="pVideoSource"></param>
        private void ConfigureVideoSource(IBaseFilter pVideoSource)
        {
            Log.Info("Configure video source.");
            Log.Info("Find video source output pin.");
            IPin pPin = DsFindPin.ByDirection(pVideoSource, PinDirection.Output, 0);
            Log.Info("Query IAMStreamConfig from pin.");
            var streamConfig = (IAMStreamConfig) pPin;

            Log.Info("Get number of video capabilities.");
            int piCount, piSize;
            int hr = streamConfig.GetNumberOfCapabilities(out piCount, out piSize);
            DsError.ThrowExceptionForHR(hr);

            var config = DecklinkCaptureLibConfigurationManager.Instance.Configuration;

            VideoInfoHeader vih = new VideoInfoHeader();
            DirectShowLib.VideoInfoHeader2 vih2 = new DirectShowLib.VideoInfoHeader2();
            VideoStreamConfigCaps vsc = new VideoStreamConfigCaps();
            IntPtr pSc = Marshal.AllocHGlobal(piSize);
            int videoFormatIndex = -1;
            AMMediaType mt = null;
            try
            {
                // Loop through all capabilities
                for (int i = 0; i < piCount; i++)
                {
                    try
                    {
                        // Get capabilities for this index.
                        hr = streamConfig.GetStreamCaps(i, out mt, pSc);
                        DsError.ThrowExceptionForHR(hr);

                        Marshal.PtrToStructure(mt.formatPtr, vih2);

                        // Copy unmanaged structure to managed in order to check it.
                        Marshal.PtrToStructure(mt.formatPtr, vih);
                        Marshal.PtrToStructure(pSc, vsc);

                        // log the video format
                        string capline = String.Format("width:{0} height:{1} fps (min-max): {2}-{3}, SubType: {4}",
                            new object[]
                            {
                                vih.BmiHeader.Width, vih.BmiHeader.Height,
                                10000000f/vsc.MaxFrameInterval,
                                10000000f/vsc.MinFrameInterval, mt.subType
                            });
                        Log.Info(capline);

                        if (vih.BmiHeader.Width == config.VideoWidth &&
                            vih.BmiHeader.Height == config.VideoHeight &&
                            (config.VideoAvgTimePerFrame >= vsc.MaxFrameInterval && config.VideoAvgTimePerFrame <= vsc.MinFrameInterval))
                        {
                            videoFormatIndex = i;
                            break;
                        }
                    }
                    finally
                    {
                        if (mt != null)
                        {
                            DsUtils.FreeAMMediaType(mt);
                        }
                    }
                }

                // If we found the format we want
                if (videoFormatIndex != -1)
                {
                    try
                    {
                        Log.Info("Get stream caps for the video format index: {0}", videoFormatIndex);
                        hr = streamConfig.GetStreamCaps(videoFormatIndex, out mt, pSc);
                        DsError.ThrowExceptionForHR(hr);

                        Log.Info("Set video format.");
                        hr = streamConfig.SetFormat(mt);
                        DsError.ThrowExceptionForHR(hr);
                    }
                    finally
                    {
                        if (mt != null)
                        {
                            DsUtils.FreeAMMediaType(mt);
                        }
                    }
                }
                else
                {
                    throw new DecklinkCaptureLibException("Failed to configure input video source.");
                }
            }
            finally
            {
                Marshal.FreeHGlobal(pSc);

                if (pPin != null)
                {
                    Marshal.ReleaseComObject(pPin);
                }
            }
        }
        /// <summary>
        /// Configure audio input filter using IAMStreamConfig interface.
        /// </summary>
        /// <param name="pAudioSource"></param>
        private void ConfigureAudioSource(IBaseFilter pAudioSource)
        {
            Log.Info("Configure audio source.");
            Log.Info("Find audio source output pin.");
            var pPin = DsFindPin.ByDirection(pAudioSource, PinDirection.Output, 0);

            Log.Info("Query IAMStreamConfig from pin.");
            var streamConfig = (IAMStreamConfig) pPin;

            Log.Info("Get number of capabilities.");
            int piCount, piSize;
            int hr = streamConfig.GetNumberOfCapabilities(out piCount, out piSize);
            DsError.ThrowExceptionForHR(hr);

            var config = DecklinkCaptureLibConfigurationManager.Instance.Configuration;

            WaveFormatEx wfeEx = new WaveFormatEx();
            AudioStreamConfigCaps asc = new AudioStreamConfigCaps();
            IntPtr pSc = Marshal.AllocHGlobal(piSize);
            int audioFormatIndex = -1;
            AMMediaType mt = null;

            try
            {
                for (int i = 0; i < piCount; i++)
                {
                    try
                    {
                        // Get capabilities for this index.
                        hr = streamConfig.GetStreamCaps(i, out mt, pSc);
                        DsError.ThrowExceptionForHR(hr);

                        // Copy unmanaged structure to managed in order to check it.
                        Marshal.PtrToStructure(mt.formatPtr, wfeEx);
                        Marshal.PtrToStructure(pSc, asc);

                        Log.Info("Audio format. SampleRate: {0}, BitCount: {1}, Channels: {2}", wfeEx.nSamplesPerSec,
                            wfeEx.wBitsPerSample, wfeEx.nChannels);

                        if (wfeEx.nSamplesPerSec == config.AudioSamplerate &&
                            wfeEx.wBitsPerSample == config.AudioBitCount &&
                            wfeEx.nChannels == config.AudioChannels)
                        {
                            audioFormatIndex = i;
                            break;
                        }
                    }
                    finally
                    {
                        if (mt != null)
                        {
                            DsUtils.FreeAMMediaType(mt);
                        }
                    }
                }

                if (audioFormatIndex != -1)
                {
                    try
                    {
                        Log.Info("Get stream caps for the audio format index: {0}", audioFormatIndex);
                        hr = streamConfig.GetStreamCaps(audioFormatIndex, out mt, pSc);
                        DsError.ThrowExceptionForHR(hr);

                        Log.Info("Set audio format.");
                        hr = streamConfig.SetFormat(mt);
                        DsError.ThrowExceptionForHR(hr);
                    }
                    finally
                    {
                        if (mt != null)
                        {
                            DsUtils.FreeAMMediaType(mt);
                        }
                    }
                }
                else
                {
                    throw new DecklinkCaptureLibException("Failed to configure input audio source.");
                }
            }
            finally
            {
                 Marshal.FreeHGlobal(pSc);
                if (pPin != null)
                {
                    Marshal.ReleaseComObject(pPin);
                }
            }
        }
        /// <summary>
        /// Adds video and audio renderers to the graph.
        /// </summary>
        /// <param name="pVideoRender"></param>
        /// <param name="pAudioRender"></param>
        private void AddVideoAndAudioRender(out IBaseFilter pVideoRender, out IBaseFilter pAudioRender)
        {
            int hr = 0;
#if USE_EVR
            Log.Info("Create EVR Filter.");
            pVideoRender = (IBaseFilter)new EnhancedVideoRenderer();
#else
            Log.Info("Create Video Renderer Filter.");
            pVideoRender = (IBaseFilter) new VideoMixingRenderer9();
#endif
            hr = _filterGraph.AddFilter(pVideoRender, "Video Renderer");
            DsError.ThrowExceptionForHR(hr);

            if (DecklinkCaptureLibConfigurationManager.Instance.Configuration.AudioPreview)
            {
                if (!string.IsNullOrEmpty(DecklinkCaptureLibConfigurationManager.Instance.Configuration.AudioRenderer))
                {
                    Log.Info("Create specified audio renderer: \"{0}\"",
                        DecklinkCaptureLibConfigurationManager.Instance.Configuration.AudioRenderer);

                    pAudioRender = FilterGraphTools.AddFilterByName(_filterGraph, FilterCategory.AudioRendererCategory,
                        DecklinkCaptureLibConfigurationManager.Instance.Configuration.AudioRenderer);
                }
                else
                {
                    Log.Info("Create DSound Renderer Filter.");
                    pAudioRender = (IBaseFilter) new DSoundRender(); //(IBaseFilter) new DefaultWaveOut();
                    hr = _filterGraph.AddFilter(pAudioRender, "Audio Renderer");
                    DsError.ThrowExceptionForHR(hr);
                }
            }
            else
            {
                Log.Info("Create Null Renderer for audio stream.");
                pAudioRender = (IBaseFilter) new NullRenderer();
                hr = _filterGraph.AddFilter(pAudioRender, "Audio Renderer");
                DsError.ThrowExceptionForHR(hr);
            }
         
        }
        /// <summary>
        /// Add our csutom wav meter and volume control filter.
        /// </summary>
        /// <param name="pWavMeterFilter"></param>
        private void AddWavMeterFilter(out IBaseFilter pWavMeterFilter)
        {
            Log.Info("Add Wav Meter Filter to graph.");

            pWavMeterFilter = WavMeterExLoader.LoadFilter();//(IBaseFilter)new WavMeterEx();
            int hr = _filterGraph.AddFilter(pWavMeterFilter, "Wav Meter");
            DsError.ThrowExceptionForHR(hr);
        }
        /// <summary>
        /// Initialize video window.
        /// </summary>
        private void InitVideoWIndow()
        {
            Log.Info("Init video window.");
            if (_previewHandle == IntPtr.Zero)
            {
                return;
            }

            if (_filterGraph == null)
            {
                return;
            }

            _videoWindow = _filterGraph as IVideoWindow;
            if (_videoWindow == null)
            {
                Log.Error("Cannot get IVideoWindow interface.");
                return;
            }

            Log.Info("Setup video window.");
            int hr = _videoWindow.put_Owner(_previewHandle);
            DsError.ThrowExceptionForHR(hr);
            hr = _videoWindow.put_WindowStyle(WindowStyle.Child | WindowStyle.ClipSiblings);
            DsError.ThrowExceptionForHR(hr);
            hr = _videoWindow.put_MessageDrain(_previewHandle);
            DsError.ThrowExceptionForHR(hr);
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
            catch(Exception ex)
            {
                Marshal.ReleaseComObject(o);
                Log.Error("Failed to create IMFVideoDisplayControl. Details: {0}", ex.Message);
            }
        }
        /// <summary>
        /// Setup video preview window.
        /// </summary>
        private void SetupVideoWindow()
        {
            Log.Info("Setup video window.");
            if (_previewHandle == IntPtr.Zero || _videoWindow == null)
            {
                Log.Error("PreviewHandle: {0}, or IVideoWindow: {1} is null.", _previewHandle, _videoWindow);
                return;
            }

            if (_filterGraph == null)
            {
                return;
            }

            Size sz = NativeHelpers.GetControlSize(_previewHandle);
            Log.Info("The size of the preview window: {0}x{1}", sz.Width, sz.Height);

            Log.Info("Query the IBasicVideo interface.");
            var basicVideo = _filterGraph as IBasicVideo;
            if (basicVideo != null)
            {
                int videoWidth, videoHeight;
                basicVideo.get_VideoWidth(out videoWidth);
                basicVideo.get_VideoHeight(out videoHeight);

                var outWidth = sz.Width;
                var outHeight = sz.Width*videoHeight/videoWidth;

                _videoWindow.SetWindowPosition(0, (sz.Height - outHeight)/2, outWidth, outHeight);
            }
            else
            {
                _videoWindow.SetWindowPosition(0, 0, sz.Width, sz.Height);
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
        /// Create x264 encoder and configure it.
        /// </summary>
        /// <param name="pX264Filter"></param>
        private void CreateX264Filter(IPin sourcePin, out IBaseFilter pX264Filter)
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
            Log.Info("Set X264 bitrate to: {0}", DecklinkCaptureLibConfigurationManager.Instance.Configuration.VideoBitrate);
            config.bitrate_kbps = DecklinkCaptureLibConfigurationManager.Instance.Configuration.VideoBitrate;
            config.level = (int)AVCLevel.MONO_AVC_LEVEL_4_1;
            config.interlaced = (DecklinkCaptureLibConfigurationManager.Instance.Configuration.FieldOrder == _BMDFieldDominance.bmdUpperFieldFirst) || (DecklinkCaptureLibConfigurationManager.Instance.Configuration.FieldOrder == _BMDFieldDominance.bmdLowerFieldFirst);

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
        /// <summary>
        /// Creates and configures aac filter.
        /// </summary>
        /// <param name="pAACFilter"></param>
        private void CreateAACFilter(out IBaseFilter pAACFilter)
        {
            Log.Info("Add AAC encoder filter");
            //pAACFilter = (IBaseFilter) new MonogramAACEncoderFilter();
            pAACFilter = MonogramAACEncoderFilterLoader.LoadFilter();
            int hr = _filterGraph.AddFilter(pAACFilter, "AAC Filter");
            DsError.ThrowExceptionForHR(hr);

            Log.Info("Configure AAC filter");
            IMonogramAACEncoder iAacEncoder = pAACFilter as IMonogramAACEncoder;
            AACConfig config = new AACConfig();
            iAacEncoder.GetConfig(ref config);
            Log.Info("Set AAC bitrate to: {0}",
                DecklinkCaptureLibConfigurationManager.Instance.Configuration.AudioBitrate);
            config.bitrate = DecklinkCaptureLibConfigurationManager.Instance.Configuration.AudioBitrate * 1000;
            config.version = (int)AACMPEGVersion.MPEG4;
            config.object_type = (int)AACObjectType.AAC_OBJECT_LOW;
            config.output_type = (int) AACOutputType.AAC_OUTPUT_RAW;
            hr = iAacEncoder.SetConfig(config);
            DsError.ThrowExceptionForHR(hr);
        }
        /// <summary>
        /// Creates inf tee filters.
        /// </summary>
        /// <param name="pInfTee"></param>
        private void CreateInfTeeFilters(out IBaseFilter pInfTee)
        {
            Log.Info("Create inf tee filter.");
            pInfTee = (IBaseFilter) new InfTee();
            int hr = _filterGraph.AddFilter(pInfTee, "Inf Tee");
            DsError.ThrowExceptionForHR(hr);
        }
        /// <summary>
        /// Add audio channel downmixer filter.
        /// </summary>
        /// <param name="pChannelDownmixer"></param>
        private void AddChannelDownmixer(out IBaseFilter pChannelDownmixer)
        {
            Log.Info("Add Audio Channel Downmixer Filter");
            pChannelDownmixer = ChannelDownmixerLoader.LoadFilter();//(IBaseFilter)new AudioDownMixer();//ChannelDownmixerLoader.LoadFilter();
            int hr = _filterGraph.AddFilter(pChannelDownmixer, "Audio Downmixer");
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
        /// Connect audio as seperate tracks to muxer. Each track will have 1 channel.
        /// </summary>
        /// <param name="captureGraphBuilder"></param>
        /// <param name="pAudioSource"></param>
        /// <param name="pMuxer"></param>
        private void ConnectAudioMultipleChannels(ICaptureGraphBuilder2 captureGraphBuilder, object pAudioSource,
            IBaseFilter pMuxer)
        {
            Log.Info("Create Audio Channel Splitter");
            IBaseFilter pAudioChannelSplitterFilter = AudioChannelSplitterLoader.LoadFilter();// (IBaseFilter)new AudioChannelSplitter();
            int hr = _filterGraph.AddFilter(pAudioChannelSplitterFilter, "Audio Channel Spliter");

            try
            {
                Log.Info("Connect audio inf tee with audio channel splitter");
                hr = captureGraphBuilder.RenderStream(null, MediaType.Audio, pAudioSource, null, pAudioChannelSplitterFilter);
                DsError.ThrowExceptionForHR(hr);

                Log.Info("Add 6 AAC encoders and connect it with muxer");
                // Add encoder for each of the 6 channels
                for (int i = 0; i < 6; i++)
                {
                    IBaseFilter pAACEncoder = null;
                    IPin pAudioOutputPin = null;
                    try
                    {
                        Log.Info("Create AAC Encoder {0}", i);

                        CreateAACFilter(out pAACEncoder);

                        pAudioOutputPin = DsFindPin.ByDirection(pAudioChannelSplitterFilter, PinDirection.Output, i);
                        hr = DSUtils.ConnectOutputPin(_filterGraph, pAudioOutputPin, pAACEncoder);
                        DsError.ThrowExceptionForHR(hr);
                        Log.Info("Connect audio channel splitter with aac encoder and muxer filter.");
                        hr = captureGraphBuilder.RenderStream(null, null, pAACEncoder, null, pMuxer);
                        DsError.ThrowExceptionForHR(hr);
                    }
                    catch (Exception ex)
                    {
                        Log.Error(ex, "Connect audio exception.");
                    }
                    finally
                    {
                        if (pAACEncoder != null)
                        {
                            Marshal.ReleaseComObject(pAACEncoder);
                        }
                        if (pAudioOutputPin != null)
                        {
                            Marshal.ReleaseComObject(pAudioOutputPin);
                        }
                    }
                }
            }
            finally
            {
                if (pAudioChannelSplitterFilter != null)
                {
                    Marshal.ReleaseComObject(pAudioChannelSplitterFilter);
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
        #endregion
    }
}
