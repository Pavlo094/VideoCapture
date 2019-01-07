using System;
using System.Collections.Concurrent;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading;
using DecklinkCaptureLib.Filters;
using DeckLinkAPI;
using DirectShowLib;
using NLog;


namespace DecklinkCaptureLib
{
    /// <summary>
    /// This class wrapps Decklink SDK API and creates 2 DirectShow source filters (which it exposes); when running
    /// it pushes video/audio samples from the DECKLINK API to source filters.
    /// It also has methods for iterating for decklink devices on the system
    /// Singleton implementation.
    /// </summary>
    public class DecklinApiWrapper : IDeckLinkInputCallback
    {
        #region Logger

        private static readonly Logger Log = LogManager.GetCurrentClassLogger();

        #endregion

        #region Members

        /// <summary>
        /// The only instance of this class.
        /// </summary>
        private static DecklinApiWrapper _instance;
        /// <summary>
        /// Decklink device interface.
        /// </summary>
        private IDeckLink _decklink = null;
        /// <summary>
        /// This interface is used for controlling decklink video/audio streams.
        /// </summary>
        private IDeckLinkInput _decklinkInput = null;
        /// <summary>
        /// Push source filter for video
        /// </summary>
        private IBaseFilter _pVideoPushSourceFilter;
        /// <summary>
        /// Push source filter for audio
        /// </summary>
        private IBaseFilter _pAudioPushSourceFilter;
        /// <summary>
        /// AVPushSource filter interface for video
        /// </summary>
        private IAVPushSource3 _piVideoPushSource;
        /// <summary>
        /// AVPushSource filter interface for audio.
        /// </summary>
        private IAVPushSource3 _piAudioPushSource;
        /// <summary>
        /// Output pin from the video push source filter. It must be kept because we wan't to use
        /// it's interface
        /// </summary>
        private IPin _pPushVideoPin = null;
        /// <summary>
        /// Output pin from the audio push source filter. It must be kept because we wan't to use
        /// it's interface
        /// </summary>
        private IPin _pPushAudioPin = null;
        private bool _isInitialized = false;
        private ManualResetEvent _processingEvent = new ManualResetEvent(false);
        private bool _abort = false;
        private ConcurrentQueue<IDeckLinkVideoInputFrame> _videoFrames = new ConcurrentQueue<IDeckLinkVideoInputFrame>();
        private ConcurrentQueue<IDeckLinkAudioInputPacket>  _audioPackets = new ConcurrentQueue<IDeckLinkAudioInputPacket>();
        private Thread _videoThread = null;
        private Thread _audioThread = null;
        private int _framesArrived = 0;
        private int _framesProcessed = 0;
        private int _audioPacketArrived = 0;
        private int _audioPacketProcessed = 0;
        private static int MaxFramesInQueue = 25;
        #endregion

        #region Constructor

        /// <summary>
        /// Private constructor for singleton implementation.
        /// </summary>
        private DecklinApiWrapper()
        {
        }

        #endregion

        #region Instance

        /// <summary>
        /// The only instance of this class.
        /// </summary>
        public static DecklinApiWrapper Instance
        {
            get { return _instance ?? (_instance = new DecklinApiWrapper()); }
        }

        #endregion

        #region Public Methods

        /// <summary>
        /// Returns a list of currently installed decklink devices.
        /// </summary>
        /// <returns></returns>
        public List<string> GetDecklinkDevices()
        {
            Log.Info("GetDecklinkDevices");
            var lst = new List<string>();
            // Create the COM instance
            IDeckLinkIterator deckLinkIterator = new CDeckLinkIterator();
            if (deckLinkIterator == null)
            {
                Log.Error("Cannot create IDecklinkIterator.");
                throw new Exception(
                    "This application requires the DeckLink drivers installed.\nPlease install the Blackmagic DeckLink drivers to use the features of this application");
            }
            try
            {
                IDeckLink decklink;
                deckLinkIterator.Next(out decklink);
                while (decklink != null)
                {
                    string displayName, modelName;
                    decklink.GetDisplayName(out displayName);
                    decklink.GetModelName(out modelName);
                    Log.Info("Decklink device found. Display name: \"{0}\", Model name: \"{1}\"", displayName, modelName);
                    lst.Add(displayName);

                    Marshal.ReleaseComObject(decklink);
                    deckLinkIterator.Next(out decklink);
                }
            }
            finally
            {
                if (deckLinkIterator != null)
                {
                    Marshal.ReleaseComObject(deckLinkIterator);
                }
            }

            return lst;
        }

        public List<long> GetDecklinkDevicesIds()
        {
            //
            Log.Info("GetDecklinkDevicesIds");
            var lst = new List<long>();
            // Create the COM instance
            IDeckLinkIterator deckLinkIterator = new CDeckLinkIterator();
            if (deckLinkIterator == null)
            {
                Log.Error("Cannot create IDecklinkIterator.");
                throw new Exception(
                    "This application requires the DeckLink drivers installed.\nPlease install the Blackmagic DeckLink drivers to use the features of this application");
            }
            try
            {
                IDeckLink decklink;
                deckLinkIterator.Next(out decklink);
                while (decklink != null)
                {
                    string displayName, modelName;
                    decklink.GetDisplayName(out displayName);
                    decklink.GetModelName(out modelName);
                    Log.Info("Decklink device found. Display name: \"{0}\", Model name: \"{1}\"", displayName, modelName);

                    IDeckLinkAttributes attribs = decklink as IDeckLinkAttributes;
                    if (attribs != null)
                    {
                        bool result = false;
                        long id = 0;
                        //BMDDeckLinkPairedDevicePersistentID
                        var attribIds = new List<_BMDDeckLinkAttributeID>() {
                            _BMDDeckLinkAttributeID.BMDDeckLinkPairedDevicePersistentID,
                            _BMDDeckLinkAttributeID.BMDDeckLinkPersistentID,
                            _BMDDeckLinkAttributeID.BMDDeckLinkDeviceGroupID,
                            _BMDDeckLinkAttributeID.BMDDeckLinkTopologicalID
                        };
                        foreach (var attrib in attribIds)
                            try
                            {
                                attribs.GetInt(attrib, out id);
                                result = true;
                                break;
                            }
                            catch
                            {
                            }
                        if (result)
                            lst.Add(id);
                    }

                    Marshal.ReleaseComObject(decklink);
                    deckLinkIterator.Next(out decklink);
                }
            }
            finally
            {
                if (deckLinkIterator != null)
                {
                    Marshal.ReleaseComObject(deckLinkIterator);
                }
            }

            return lst;
        }
        


        /// <summary>
        /// Queries for IDecklink and IDecklinkInput interfaces.
        /// </summary>
        private void GetDecklinkDeviceByName(string deviceName, out IDeckLink decklink)
        {
            // Create the COM instance
            decklink = null;
            IDeckLinkIterator deckLinkIterator = new CDeckLinkIterator();
            try
            {
                IDeckLink item;
                deckLinkIterator.Next(out item);

                while (item != null)
                {
                    string displayName, modelName;
                    item.GetDisplayName(out displayName);
                    item.GetModelName(out modelName);
                    if (deviceName == displayName)
                    {
                        decklink = item;                        
                        break;
                    }
                    else
                    {
                        Marshal.ReleaseComObject(item);
                        deckLinkIterator.Next(out item);
                    }
                }
            }
            catch (Exception ex)
            {
                Log.Error("CreateDecklinkInterfaces error. Details: {0}", ex.Message);
                throw;
            }
            finally
            {
                if (deckLinkIterator != null)
                {
                    Marshal.ReleaseComObject(deckLinkIterator);
                }
            }
        }

        public List<string> GetVideoModes(string deviceName)
        {
            var result = new List<string>();
            IDeckLink device = null;
            try
            {
                GetDecklinkDeviceByName(deviceName, out device);
                if (device == null) throw new Exception("Can not find decklink device.");

                IDeckLinkInput input = (IDeckLinkInput)device;

                IDeckLinkDisplayModeIterator displayModeIterator;
                input.GetDisplayModeIterator(out displayModeIterator);
                IDeckLinkDisplayMode displayMode = null;
                displayModeIterator.Next(out displayMode);
                while (displayMode != null)
                {
                    string name;
                    displayMode.GetName(out name);
                    result.Add(name);
                    displayModeIterator.Next(out displayMode);
                }
            }
            catch (Exception ex)
            {
                Log.Error("CreateDecklinkInterfaces error. Details: {0}", ex.Message);
                throw;
            }
            finally
            {
                if (device != null)
                {
                    Marshal.ReleaseComObject(device);
                }
            }
            return result;
        }

        public void GetVideoModeProperties(string deviceName, string videoModeName, out int width, out int height, out long avgFrameDuration, out _BMDFieldDominance fieldOrder)
        {
            width = 0;
            height = 0;
            avgFrameDuration = 0;
            fieldOrder = _BMDFieldDominance.bmdProgressiveFrame;
            IDeckLink device = null;
            try
            {
                GetDecklinkDeviceByName(deviceName, out device);
                if (device == null) throw new Exception("Can not find decklink device.");

                IDeckLinkInput input = (IDeckLinkInput)device;

                IDeckLinkDisplayModeIterator displayModeIterator;
                input.GetDisplayModeIterator(out displayModeIterator);
                IDeckLinkDisplayMode displayMode = null;
                displayModeIterator.Next(out displayMode);
                while (displayMode != null)
                {
                    string name;
                    displayMode.GetName(out name);
                    if (videoModeName == name)
                    {
                        width = displayMode.GetWidth();
                        height = displayMode.GetHeight();
                        long frameDuration;
                        long timeScale;
                        displayMode.GetFrameRate(out frameDuration, out timeScale);
                        long second = 1000 * 10000;
                        avgFrameDuration = second * frameDuration / timeScale;
                        fieldOrder = displayMode.GetFieldDominance();
                        break;
                    }
                    displayModeIterator.Next(out displayMode);
                }
            }
            catch (Exception ex)
            {
                Log.Error("CreateDecklinkInterfaces error. Details: {0}", ex.Message);
                throw;
            }
            finally
            {
                if (device != null)
                {
                    Marshal.ReleaseComObject(device);
                }
            }
        }
        public void GetVideoModeName(string deviceName, out string videoModeName, int width, int height, long avgFrameDuration, _BMDFieldDominance fieldOrder)
        {
            videoModeName = String.Empty;
            IDeckLink device = null;
            try
            {
                GetDecklinkDeviceByName(deviceName, out device);
                if (device == null) throw new Exception("Can not find decklink device.");

                IDeckLinkInput input = (IDeckLinkInput)device;

                IDeckLinkDisplayModeIterator displayModeIterator;
                input.GetDisplayModeIterator(out displayModeIterator);
                IDeckLinkDisplayMode displayMode = null;
                displayModeIterator.Next(out displayMode);
                while (displayMode != null)
                {
                    string name;
                    displayMode.GetName(out name);
                    long frameDuration;
                    long timeScale;
                    displayMode.GetFrameRate(out frameDuration, out timeScale);
                    long second = 1000 * 10000;
                    long _avgFrameDuration = second * frameDuration / timeScale;
                    if (width == displayMode.GetWidth() &&
                        height == displayMode.GetHeight() &&
                        _avgFrameDuration == avgFrameDuration &&
                        fieldOrder == displayMode.GetFieldDominance())
                    {
                        videoModeName = name;
                        break;
                    }
                    displayModeIterator.Next(out displayMode);
                }
            }
            catch (Exception ex)
            {
                Log.Error("CreateDecklinkInterfaces error. Details: {0}", ex.Message);
                throw;
            }
            finally
            {
                if (device != null)
                {
                    Marshal.ReleaseComObject(device);
                }
            }
        }

        /// <summary>
        /// Initialize component.
        /// </summary>
        public void Init()
        {
            Log.Info("Init");
            if (IsRunning)
            {
                Log.Warn("Already running. Call stop.");
                Stop();
            }

            CreateDecklinkInterfaces();

            Log.Info("Decklink input set callback.");
            _decklinkInput.SetCallback(this);

            CreatePushSourceFilters();

            ConfigureVideoPushSourceFilter();

            ConfigureAudioPushSourceFilter();

            DroppedFrames = 0;

            _isInitialized = true;
        }
        /// <summary>
        /// Starts streaming. Init must be called first.
        /// </summary>
        public void Start()
        {
            Log.Info("Start");
            if (!_isInitialized)
            {
                Log.Error("Start called but the library is not initialized. Call Init first.");
                return;
            }

            if (IsRunning)
            {
                Log.Warn("Already running. Return...");
                return;
            }

            QueryPushFilterInterfaces();

            DroppedFrames = 0;

            Log.Info("Enable Decklink Video Input");
            //_BMDDisplayMode displayMode = _BMDDisplayMode.bmdModeHD1080i5994; //_BMDDisplayMode.bmdModeHD1080p25;
            _BMDDisplayMode displayMode = _BMDDisplayMode.bmdModeHD1080p25; //_BMDDisplayMode.bmdModeHD1080p25;
            _decklinkInput.EnableVideoInput(displayMode, _BMDPixelFormat.bmdFormat8BitYUV,
                _BMDVideoInputFlags.bmdVideoInputFlagDefault);

            Log.Info("Enable Decklink Audio Input");
            _decklinkInput.EnableAudioInput(_BMDAudioSampleRate.bmdAudioSampleRate48kHz,
                _BMDAudioSampleType.bmdAudioSampleType16bitInteger, 8);

            _abort = false;
            Log.Info("Start streams.");
            _decklinkInput.StartStreams();

            _videoThread = new Thread(VideoThread);
            _videoThread.SetApartmentState(ApartmentState.MTA);
            _audioThread = new Thread(AudioThread);
            _audioThread.SetApartmentState(ApartmentState.MTA);
            _videoThread.Start();
            _audioThread.Start();

            IsRunning = true;
        }

        public void StopAPIThreads()
        {
            _abort = true;
            if (_videoThread != null)
            {
                _videoThread.Join();
            }
            if (_audioThread != null)
            {
                _audioThread.Join();
            }
            if (IsRunning && _decklinkInput != null)
            {
                //WaitHandle[] handles = new WaitHandle[1];
                //handles[0] = _processingEvent;
                //WaitHandle.WaitAll(handles);

                IDeckLinkVideoInputFrame videoFrame;
                while (_videoFrames.TryDequeue(out videoFrame))
                {
                    Marshal.ReleaseComObject(videoFrame);
                }

                IDeckLinkAudioInputPacket audioPacket;
                while (_audioPackets.TryDequeue(out audioPacket))
                {
                    Marshal.ReleaseComObject(audioPacket);
                }

                _decklinkInput.SetCallback(null);

                Log.Info("DecklinkInput stop streams.");
                _decklinkInput.StopStreams();

                _decklinkInput.DisableVideoInput();
                _decklinkInput.DisableAudioInput();

                GC.Collect();
            }
        }
        /// <summary>
        /// Stops decklink device.
        /// </summary>
        public void Stop()
        {
            Log.Info("Stop");

            
           
            _decklinkInput = null;
            if (_decklink != null)
            {
                Marshal.ReleaseComObject(_decklink);
            }

            if (_pPushVideoPin != null)
            {
                Marshal.ReleaseComObject(_pPushVideoPin);
                _pPushVideoPin = null;
            }

            if (_pPushAudioPin != null)
            {
                Marshal.ReleaseComObject(_pPushAudioPin);
                _pPushAudioPin = null;
            }

            if (_pVideoPushSourceFilter != null)
            {
                Marshal.ReleaseComObject(_pVideoPushSourceFilter);
            }
            _piVideoPushSource = null;
            if (_pAudioPushSourceFilter != null)
            {
               Marshal.ReleaseComObject(_pAudioPushSourceFilter);
            }
            _piAudioPushSource = null;

            IsRunning = false;
            _isInitialized = false;
        }
        #endregion

        #region Properties
        /// <summary>
        /// Gets opr sets if streams from this decklink device are running.
        /// </summary>
        public bool IsRunning { get; private set; }
        /// <summary>
        /// Gets the video push source IBaseFilter.
        /// </summary>
        public IBaseFilter VideoPushSourceFilter
        {
            get
            {
                return _pVideoPushSourceFilter;
            }
        }
        /// <summary>
        /// Gets the audio push source IBaseFilter.
        /// </summary>
        public IBaseFilter AudioPushSourceFilter
        {
            get
            {
                return _pAudioPushSourceFilter;
            }
        }
        public int DroppedFrames { get; set; }
        #endregion

        #region Processing Threads
        /// <summary>
        /// This is where the main video processing is done.
        /// </summary>
        private void VideoThread()
        {
            while (!_abort)
            {
                try
                {
                    IntPtr pDstBuffer = IntPtr.Zero;
                    uint dstSize = 0;
                    if (_piVideoPushSource.GetFrameBufferEx(out pDstBuffer, out dstSize) == 0)
                    {
                        IDeckLinkVideoInputFrame videoFrame = null;
                        while (!_videoFrames.TryDequeue(out videoFrame))
                        {
                            System.Threading.Thread.Sleep(2);
                            if (_abort) break;
                        }

                        try
                        {
                            if (_abort)
                            {
                                if (videoFrame != null)
                                {
                                    Marshal.ReleaseComObject(videoFrame);
                                }
                                videoFrame = null;
                                _piVideoPushSource.DeliverEx(pDstBuffer, 0);
                                break;
                            }

                            if (pDstBuffer != IntPtr.Zero && videoFrame != null)
                            {
                                IntPtr pBufferSrc;
                                videoFrame.GetBytes(out pBufferSrc);

                                CopyMemory(pDstBuffer, pBufferSrc, (uint) dstSize);

                                //Log.Info("Video deliver: {0}", dstSize);
                                _piVideoPushSource.DeliverEx(pDstBuffer, dstSize);
                            }
                        }
                        catch (Exception ex)
                        {
                            Log.Error(ex, "Video Thread Push Error.");
                        }
                        finally
                        {
                            if (videoFrame != null)
                            {
                                Marshal.ReleaseComObject(videoFrame);
                            }
                        }
                    }
                    else
                    {
                        System.Threading.Thread.Sleep(2);
                    }
                }
                catch (Exception ex)
                {
                    Log.Error(ex, "Video Thread General Error.");
                }
            }

            Log.Info("Video thread out");
        }
        /// <summary>
        /// This is where the main audio processing is done.
        /// </summary>
        private unsafe void AudioThread()
        {

            while (!_abort)
            {
                try
                {
                    IntPtr pDstBuffer = IntPtr.Zero;
                    uint dstSize = 0;
                    if (_piAudioPushSource.GetFrameBufferEx(out pDstBuffer, out dstSize) == 0)
                    {
                        IDeckLinkAudioInputPacket audioPacket = null;
                        while (!_audioPackets.TryDequeue(out audioPacket))
                        {
                            System.Threading.Thread.Sleep(2);
                            if (_abort) break;
                        }

                        try
                        {
                            if (_abort)
                            {
                                if (audioPacket != null)
                                {
                                    Marshal.ReleaseComObject(audioPacket);
                                }
                                audioPacket = null;
                                _piAudioPushSource.DeliverEx(pDstBuffer, 0);
                                break;
                            }

                            if (pDstBuffer != IntPtr.Zero && audioPacket != null)
                            {
                                IntPtr pBufferSrc;
                                audioPacket.GetBytes(out pBufferSrc);

                                int srcSampleFrameCount = audioPacket.GetSampleFrameCount();
                                int srcSize = srcSampleFrameCount*
                                              8*
                                              DecklinkCaptureLibConfigurationManager.Instance.Configuration
                                                  .AudioBitCount/8;
                                uint actSize = Math.Min((uint) srcSize, dstSize);

                                // Decklink delivers 8 channels, and we work with only 6
                                int nrSrcSamples = srcSize/2;
                                int nrDstSamples = (int) dstSize/2;
                                short* pSrcSamples = (short*) pBufferSrc;
                                short* pDstSamples = (short*) pDstBuffer;
                                int counter = 0;
                                uint actNrOfSamples = 0;
                                for (int i = 0; i < nrSrcSamples; i++)
                                {
                                    int channel = counter%8;
                                    if (channel > 5)
                                    {
                                        pSrcSamples++;
                                        counter++;
                                        continue;
                                    }

                                    *pDstSamples = *pSrcSamples;

                                    pSrcSamples++;
                                    pDstSamples++;
                                    counter++;
                                    actNrOfSamples++;
                                }
                                //Log.Info("Audio deliver: {0}", 2 * actNrOfSamples);
                                // 2 * size for 16bit short
                                _piAudioPushSource.DeliverEx(pDstBuffer, (uint) 2*actNrOfSamples);
                            }
                        }
                        catch (Exception ex)
                        {
                            Log.Error(ex, "Audio Thread Push Error.");
                        }
                        finally
                        {
                            if (audioPacket != null)
                            {
                                Marshal.ReleaseComObject(audioPacket);
                            }
                        }
                    }
                    else
                    {
                        System.Threading.Thread.Sleep(2);
                    }
                }
                catch (Exception ex)
                {
                    Log.Error(ex, "Audio Thread General Error.");
                }
            }
            Log.Info("Audio thread out");
        }
        #endregion

        #region Decklink Callbacks
        public void VideoInputFormatChanged(_BMDVideoInputFormatChangedEvents notificationEvents, IDeckLinkDisplayMode newDisplayMode, _BMDDetectedVideoInputFormatFlags detectedSignalFlags)
        {
            Log.Info("VideoInputFormatChanged");
        }
        /// <summary>
        /// This is where frames and audio packets from decklink device arrive. From here we just push it to
        /// our video and audio push source filters.
        /// </summary>
        /// <param name="videoFrame"></param>
        /// <param name="audioPacket"></param>
        public void VideoInputFrameArrived(IDeckLinkVideoInputFrame videoFrame, IDeckLinkAudioInputPacket audioPacket)
        {
            if (_abort)
            {
                if (videoFrame != null)
                {
                    Marshal.ReleaseComObject(videoFrame);
                }
                if (audioPacket != null)
                {
                    Marshal.ReleaseComObject(audioPacket);
                }
                return;
            }

            if (videoFrame != null)
            {
                if (_videoFrames.Count <= MaxFramesInQueue)
                {
                    _videoFrames.Enqueue(videoFrame);
                }
                else
                {
                    Marshal.ReleaseComObject(videoFrame);
                    DroppedFrames++;
                }
            }
            if (audioPacket != null)
            {
                _audioPackets.Enqueue(audioPacket);
            }
            #region Old
            //try
            //{
            //    if (_abort)
            //    {
            //        _processingEvent.Set();
            //        return;
            //    }
            //    Log.Info("VideoInpuitFrameArrived IN");
            //    _processingEvent.Reset();
            //    if (_piVideoPushSource != null && videoFrame != null)
            //    {
            //        IntPtr pDstBuffer = IntPtr.Zero;
            //        uint dstSize = 0;
            //        int counter = 0;
            //        while (_piVideoPushSource.GetFrameBufferEx(ref pDstBuffer, ref dstSize) != 0)
            //        {
            //            System.Threading.Thread.Sleep(5);
            //            if (_abort)
            //            {
            //                break;
            //            }
            //            counter++;
            //            if (counter > 5)
            //            {
            //                break;
            //            }
            //        }

            //        //if (_piVideoPushSource.GetFrameBufferEx(ref pDstBuffer, ref dstSize) == 0)
            //        if(pDstBuffer != IntPtr.Zero)
            //        {

            //            IntPtr pBufferSrc;
            //            videoFrame.GetBytes(out pBufferSrc);

            //            CopyMemory(pDstBuffer, pBufferSrc, (uint) dstSize);

            //            _piVideoPushSource.DeliverEx(pDstBuffer, dstSize);
            //        }
            //    }

            //    if (_piAudioPushSource != null && audioPacket != null)
            //    {
            //        IntPtr pDstBuffer = IntPtr.Zero;
            //        uint dstSize = 0;
            //        int tryCounter = 0;
            //        while (_piAudioPushSource.GetFrameBufferEx(ref pDstBuffer, ref dstSize) != 0)
            //        {
            //            System.Threading.Thread.Sleep(5);
            //            if (_abort)
            //            {
            //                break;
            //            }
            //            tryCounter++;
            //            if (tryCounter > 5)
            //            {
            //                return;
            //            }
            //        }

            //        //if (_piAudioPushSource.GetFrameBufferEx(ref pDstBuffer, ref dstSize) == 0)
            //        if(pDstBuffer != IntPtr.Zero)
            //        {
            //            if (_abort)
            //            {
            //                return;
            //            }
            //            IntPtr pBufferSrc;
            //            audioPacket.GetBytes(out pBufferSrc);

            //            int srcSampleFrameCount = audioPacket.GetSampleFrameCount();
            //            int srcSize = srcSampleFrameCount*
            //                          8*
            //                          DecklinkCaptureLibConfigurationManager.Instance.Configuration.AudioBitCount/8;
            //            uint actSize = Math.Min((uint) srcSize, dstSize);

            //            // Decklink delivers 8 channels, and we work with only 6
            //            int nrSrcSamples = srcSize/2;
            //            int nrDstSamples = (int) dstSize/2;
            //            short* pSrcSamples = (short*) pBufferSrc;
            //            short* pDstSamples = (short*) pDstBuffer;
            //            int counter = 0;
            //            uint actNrOfSamples = 0;
            //            for (int i = 0; i < nrSrcSamples; i++)
            //            {
            //                int channel = counter%8;
            //                if (channel > 5)
            //                {
            //                    pSrcSamples++;
            //                    counter++;
            //                    continue;
            //                }

            //                *pDstSamples = *pSrcSamples;

            //                pSrcSamples++;
            //                pDstSamples++;
            //                counter++;
            //                actNrOfSamples++;
            //            }
            //            // 2 * size for 16bit short
            //            _piAudioPushSource.DeliverEx(pDstBuffer, (uint) 2*actNrOfSamples);
            //        }
            //    }
            //}
            //catch (Exception ex)
            //{
            //    Log.Error("VideoInputFrameArrived error. Details: {0}", ex.Message);
            //}
            //finally
            //{
            //    _processingEvent.Set();
            //    Log.Info("VideoInpuitFrameArrived OUT");
            //}
            #endregion
        }
        #endregion

        #region Helpers
        /// <summary>
        /// Queries for IDecklink and IDecklinkInput interfaces.
        /// </summary>
        private void CreateDecklinkInterfaces()
        {
            Log.Info("Get IDecklink interface for device: \"{0}\"",
                DecklinkCaptureLibConfigurationManager.Instance.Configuration.DecklinkDevice);

            // Create the COM instance
            IDeckLinkIterator deckLinkIterator = new CDeckLinkIterator();
            try
            {
                IDeckLink decklink;
                deckLinkIterator.Next(out decklink);

                while (decklink != null)
                {
                    string displayName, modelName;
                    decklink.GetDisplayName(out displayName);
                    decklink.GetModelName(out modelName);
                    if (DecklinkCaptureLibConfigurationManager.Instance.Configuration.DecklinkDevice == displayName)
                    {
                        Log.Info("Decklink device found. Display name: \"{0}\", Model name: \"{1}\"", displayName,
                            modelName);
                        _decklink = decklink;
                        Log.Info("Query IDecklinkInput interface.");
                        _decklinkInput = decklink as IDeckLinkInput;
                        break;
                    }
                    else
                    {
                        Marshal.ReleaseComObject(decklink);
                        deckLinkIterator.Next(out decklink);
                    }
                }
            }
            catch (Exception ex)
            {
                Log.Error("CreateDecklinkInterfaces error. Details: {0}", ex.Message);
                throw;
            }
            finally
            {
                if (deckLinkIterator != null)
                {
                    Marshal.ReleaseComObject(deckLinkIterator);
                }
            }
        }
        /// <summary>
        /// Create video and audio push source filters.
        /// </summary>
        private void CreatePushSourceFilters()
        {
            Log.Info("Create push source filters.");

            Log.Info("Create video push source filter.");
            _pVideoPushSourceFilter = (IBaseFilter)new VideoPushSource();//VideoPushSourceLoader.LoadFilter();

            Log.Info("Create audio push source filter");
            _pAudioPushSourceFilter = (IBaseFilter)new AudioPushSource();//AudioPushSourceLoader.LoadFilter();
            Log.Info("Query IAVPushSource interface from audio filter.");
        }
        /// <summary>
        /// Query the push interfaces, from it's output pins, for pushing video/audio samples.
        /// </summary>
        private void QueryPushFilterInterfaces()
        {
            Log.Info("Query IAVPushSource interface from video filter.");
            _pPushVideoPin = DsFindPin.ByDirection(_pVideoPushSourceFilter, PinDirection.Output, 0);
            if (_pPushVideoPin != null)
            {
                _piVideoPushSource = _pPushVideoPin as IAVPushSource3;
            }

            _pPushAudioPin = DsFindPin.ByDirection(_pAudioPushSourceFilter, PinDirection.Output, 0);
            if (_pPushAudioPin != null)
            {
                _piAudioPushSource = _pPushAudioPin as IAVPushSource3;
            }
        }
        /// <summary>
        /// Configure video push source filter using values from configuration manager.
        /// </summary>
        private void ConfigureVideoPushSourceFilter()
        {
            Log.Info("Configure Video Push Source Filter.");
            Log.Info("Find video source output pin.");
            IPin pPin = DsFindPin.ByDirection(_pVideoPushSourceFilter, PinDirection.Output, 0);
            var config = DecklinkCaptureLibConfigurationManager.Instance.Configuration;
            AMMediaType mt = new AMMediaType();
            try
            {

                Log.Info("Query IAMStreamConfig from pin.");
                var streamConfig = (IAMStreamConfig) pPin;

                
                Log.Info("Create Video AMMediaType for configuration.");
                mt.fixedSizeSamples = true;
                mt.temporalCompression = false;
                mt.formatSize = Marshal.SizeOf(typeof (VideoInfoHeader));
                mt.formatPtr = Marshal.AllocCoTaskMem(mt.formatSize);
                mt.majorType = MediaType.Video;
                mt.subType = MediaSubType.UYVY;
                mt.sampleSize = config.VideoWidth*config.VideoHeight*16/8;
                mt.formatType = FormatType.VideoInfo;

                //VideoInfoHeader2 vfi = new VideoInfoHeader2();
                VideoInfoHeader vfi = new VideoInfoHeader();
                vfi.AvgTimePerFrame = config.VideoAvgTimePerFrame;
                vfi.BitErrorRate = 0;
                vfi.BitRate = 0;
                vfi.BmiHeader = new BitmapInfoHeader();
                vfi.BmiHeader.BitCount = 16;
                vfi.BmiHeader.ClrImportant = 0;
                vfi.BmiHeader.ClrUsed = 0;
                vfi.BmiHeader.Compression = (int)MakeFourCC('H', 'D', 'Y', 'C'); //MakeFourCC('U', 'Y', 'V', 'Y');
                vfi.BmiHeader.Height = config.VideoHeight;
                vfi.BmiHeader.ImageSize = mt.sampleSize;
                vfi.BmiHeader.Planes = 1;
                vfi.BmiHeader.Size = Marshal.SizeOf(typeof (BitmapInfoHeader));
                vfi.BmiHeader.Width = config.VideoWidth;
                vfi.BmiHeader.XPelsPerMeter = 0;
                vfi.BmiHeader.YPelsPerMeter = 0;

                //vfi.InterlaceFlags = (AMInterlace)(0x00000081);
                //vfi.PictAspectRatioX = 16;
                //vfi.PictAspectRatioY = 9;
                //vfi.ControlFlags = 0;
                //vfi.CopyProtectFlags = 0;
                //vfi.SrcRect = new DsRect(0, 0, config.VideoWidth, config.VideoHeight);
                //vfi.TargetRect = new DsRect(0, 0, config.VideoWidth, config.VideoHeight);


                Marshal.StructureToPtr(vfi, mt.formatPtr, true);

                Log.Info("Set video format.");
                int hr = streamConfig.SetFormat(mt);
                DsError.ThrowExceptionForHR(hr);
            }
            finally
            {
                if (mt != null)
                {
                    DsUtils.FreeAMMediaType(mt);
                }

                if (pPin != null)
                {
                    Marshal.ReleaseComObject(pPin);
                }
            }
        }
        /// <summary>
        /// Configure audio push source filter.
        /// </summary>
        private void ConfigureAudioPushSourceFilter()
        {
            Log.Info("Configure Audio Push Source Filter.");
            Log.Info("Find audio source output pin.");
            IPin pPin = DsFindPin.ByDirection(_pAudioPushSourceFilter, PinDirection.Output, 0);
            var config = DecklinkCaptureLibConfigurationManager.Instance.Configuration;
            AMMediaType mt = new AMMediaType();
            try
            {
                Log.Info("Query IAMStreamConfig from pin.");
                var streamConfig = (IAMStreamConfig)pPin;

                Log.Info("Create Audio AMMediaType for configuration.");
                mt.fixedSizeSamples = true;
                mt.temporalCompression = false;
                mt.formatSize = Marshal.SizeOf(typeof(WaveFormatEx));
                mt.formatPtr = Marshal.AllocCoTaskMem(mt.formatSize);
                mt.majorType = MediaType.Audio;
                mt.subType = MediaSubType.PCM;
                mt.sampleSize = config.AudioSamplerate * config.AudioChannels * config.AudioBitCount / 8;
                mt.formatType = FormatType.WaveEx;

                WaveFormatEx wfex = new WaveFormatEx();
                wfex.cbSize = 0;
                wfex.nAvgBytesPerSec = mt.sampleSize;
                wfex.nBlockAlign = (short)(config.AudioChannels*config.AudioBitCount/8);
                wfex.nChannels = (short)config.AudioChannels;
                wfex.nSamplesPerSec = config.AudioSamplerate;
                wfex.wBitsPerSample = (short)config.AudioBitCount;
                wfex.wFormatTag = 1;//WAVE_FORMAT_PCM

                Marshal.StructureToPtr(wfex, mt.formatPtr, true);

                Log.Info("Set audio format.");
                int hr = streamConfig.SetFormat(mt);
                DsError.ThrowExceptionForHR(hr);
            }
            finally
            {
                if (mt != null)
                {
                    DsUtils.FreeAMMediaType(mt);
                }

                if (pPin != null)
                {
                    Marshal.ReleaseComObject(pPin);
                }
            }
        }
        public static UInt32 MakeFourCC(char ch0, char ch1, char ch2, char ch3)
        {
            UInt32 result = ((UInt32)(byte)(ch0) | ((UInt32)(byte)(ch1) << 8) |
                    ((UInt32)(byte)(ch2) << 16) | ((UInt32)(byte)(ch3) << 24));

            return result;
        }
        [DllImport("kernel32.dll", EntryPoint = "CopyMemory", SetLastError = false)]
        public static extern void CopyMemory(IntPtr dest, IntPtr src, uint count);
        public static T AddRcwRef<T>(T t)
        {
            IntPtr ptr = Marshal.GetIUnknownForObject(t);
            try
            {
                return (T)Marshal.GetObjectForIUnknown(ptr);
            }
            finally
            {
                Marshal.Release(ptr); // done with the IntPtr
            }
        }
        #endregion
    }
}
