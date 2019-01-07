using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Reflection;
using System.Text;
using System.Windows.Forms;
using DirectShowLib.BDA;
using NLog;
using System.Threading;
using System.Windows.Threading;

namespace BTFPVideoEncodingLib
{
    [Serializable()]
    public class BTFPVideoEncodingJob
    {
        #region Logger
        private static readonly Logger Log = LogManager.GetCurrentClassLogger();
        #endregion

        #region Members
        private BTFPVideoTranscoder _transcodeSales = null;
        private BTFPVideoTranscoder _transcodePreview = null;
        private BTFPVideoTranscoder _transcodeJudges = null;
        /// <summary>
        /// Unique identifier for DirectShow messages. We will increment it for each instance.
        /// </summary>
        private static int WM_GRAPHNOTIFY = 0x8000 + 1;
        private readonly object _generateIdLock = new object();
        private string _inputFile = string.Empty;
        private IntPtr _msgHandle = IntPtr.Zero;

        private Dispatcher _dispatcher = null;
        
        #endregion

        #region Events

        public event EventHandler Finished;
        #endregion

        #region Constructor

        public BTFPVideoEncodingJob()
        {            
            SalesEncodingInfo = new EncodingInfo();
            PreviewEncodingInfo = new EncodingInfo();
            JudgesEncodingInfo = new EncodingInfo();            
        }
        #endregion

        #region Methods
        /// <summary>
        /// This method starts transcoding SALES, PREVIEW and JUDGES files for the input files,
        /// according to specified BTFPEnodingParameters.
        /// </summary>
        /// <param name="inputFile"></param>
        /// <param name="msgHandle"></param>
        public void Start(string inputFile, IntPtr msgHandle)
        {
            Log.Info("Start.");
            _inputFile = inputFile;
            _msgHandle = msgHandle;

            Thread thread = new Thread(() =>
            {
                StartImpl(inputFile, msgHandle);
                _dispatcher = Dispatcher.CurrentDispatcher;
                Dispatcher.Run();            
                Log.Debug("Thread stopped");                
            });

            thread.Start();
            //_dispatcher = Dispatcher.FromThread(thread);
            
        }

        private void StartImpl(string inputFile, IntPtr msgHandle)
        {
            try
            {
                var config = BTFPVideoEncodingLibConfigurationManager.Instance.Configuration;

                // Start them on separate threads. Monogram H264 Encoder (or the underlying x264 lib) only allows 2
                // instances in the process amd on third it crashes. This way it works.
                //
                // Still 2 instances max. We do it like this: first Preview and Sales go in parallel, at the end judges.

                PreviewEncodingInfo.IsEnabled = !config.PreviewEncodingParams.SkipThisEncode;
                SalesEncodingInfo.IsEnabled = !config.SalesEncodingParams.SkipThisEncode;
                JudgesEncodingInfo.IsEnabled = !config.JudgesEncodingParams.SkipThisEncode;

                StartPreviewEncoding();

                if (!PreviewEncodingInfo.IsEnabled)
                {
                    StartSalesEncoding();
                }

                // If preview or sales are skipped, go directly to judges.
                if ((!PreviewEncodingInfo.IsEnabled && !SalesEncodingInfo.IsEnabled))
                {
                    StartJudgesEncoding();
                }
            }
            catch (Exception ex)
            {
                Log.Error(ex, "General Error.");
            }
        }

        /// <summary>
        /// Call this method to read the latest progress about the SALES, PREVIEW and JUDGES encoding.
        /// </summary>
        public void UpdateProgress()
        {
            try
            {
                try
                {
                    if (_transcodeSales != null)
                    {

                        if (_transcodeSales.IsRunning)
                        {
                            double prog = 0.0;
                            _dispatcher.Invoke((Action)(() => { prog = 100.0 * (_transcodeSales.Position / _transcodeSales.Duration); }));
                            
                            if (prog < 0) prog = 0;
                            if (prog > 100) prog = 100;

                            if (SalesEncodingInfo.Finished)
                                prog = 100;
                            SalesEncodingInfo.Progress = prog;
                        }
                        else
                        {
                            if (SalesEncodingInfo.IsEnabled && SalesEncodingInfo.Finished && !SalesEncodingInfo.Error)
                            {
                                SalesEncodingInfo.Progress = 100;
                            }
                        }
                    }
                }
                catch (Exception ex)
                {
                    Log.Error(ex, "Update progress error.");
                }
                try
                {
                    if (_transcodePreview != null)
                    {
                        if (_transcodePreview.IsRunning)
                        {
                            double prog = 0.0;
                            _dispatcher.Invoke((Action)(() => { prog = 100.0 * (_transcodePreview.Position / _transcodePreview.Duration); }));                            
                            if (prog < 0) prog = 0;
                            if (prog > 100) prog = 100;
                            if (PreviewEncodingInfo.Finished)
                                prog = 100;

                            PreviewEncodingInfo.Progress = prog;
                        }
                        else
                        {
                            if (PreviewEncodingInfo.IsEnabled && PreviewEncodingInfo.Finished && !PreviewEncodingInfo.Error)
                            {
                                PreviewEncodingInfo.Progress = 100;
                            }
                        }
                    }
                }
                catch (Exception ex)
                {
                    Log.Error(ex, "Update progress error.");
                }
                try
                {
                    if (_transcodeJudges != null)
                    {
                        if (_transcodeJudges.IsRunning)
                        {
                            double prog = 0.0;
                            _dispatcher.Invoke((Action)(() => { prog = 100.0 * (_transcodeJudges.Position / _transcodeJudges.Duration); }));                            
                            if (prog < 0) prog = 0;
                            if (prog > 100) prog = 100;
                            if (JudgesEncodingInfo.Finished)
                                prog = 100;
                            JudgesEncodingInfo.Progress = prog;
                        }
                        else
                        {
                            if (JudgesEncodingInfo.IsEnabled && JudgesEncodingInfo.Finished && !JudgesEncodingInfo.Error)
                            {
                                JudgesEncodingInfo.Progress = 100;
                            }
                        }
                    }
                }
                catch (Exception ex)
                {
                    Log.Error(ex, "Update progress error.");
                }
            }
            catch (Exception ex)
            {
                Log.Error(ex, "Update progress error.");
            }
        }
        /// <summary>
        /// Aborts all current encodings.
        /// </summary>
        public void Stop()
        {
            Log.Info("Stop.");
            _dispatcher.BeginInvoke((Action)(() =>
            {
                try
                {
                    if (_transcodeSales != null)
                    {
                        _transcodeSales.Unload();
                    }
                    if (_transcodeJudges != null)
                    {
                        _transcodeJudges.Unload();
                    }
                    if (_transcodePreview != null)
                    {
                        _transcodePreview.Unload();
                    }
                    _dispatcher.BeginInvokeShutdown(DispatcherPriority.Background);
                }
                catch (Exception ex)
                {
                    Log.Error(ex, "Stop error.");
                }
            }));
        }
        /// <summary>
        /// DirectShow events are routed here.
        /// </summary>
        /// <param name="m"></param>
        public void HandleEvent(Message m)
        {
            if (_transcodeSales != null && m.Msg == _transcodeSales.WM_GRAPHNOTIFY)
            {
                _transcodeSales.HandleEvent(m);
            }
            else if (_transcodePreview != null && m.Msg == _transcodePreview.WM_GRAPHNOTIFY)
            {
                _transcodePreview.HandleEvent(m);
            }
            else if (_transcodeJudges != null && m.Msg == _transcodeJudges.WM_GRAPHNOTIFY)
            {
                _transcodeJudges.HandleEvent(m);
            }
        }
        #endregion

        #region Properties
        /// <summary>
        /// Gets the encoding info for SALES file encoding.
        /// </summary>
        public EncodingInfo SalesEncodingInfo { get; private set; }
        /// <summary>
        /// Gets the encoding info for PREVIEW file encoding.
        /// </summary>
        public EncodingInfo PreviewEncodingInfo { get; private set; }
        /// <summary>
        /// Gets the encoding info for JUDGES file encoding.
        /// </summary>
        public EncodingInfo JudgesEncodingInfo { get; private set; }
        /// <summary>
        /// Gets the input file being encoded.
        /// </summary>
        public string InputFile{
            get { return _inputFile; }
        }
        #endregion

        #region Events
        /// <summary>
        /// Judges file encoding has finished
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        void _transcodeJudges_Finished(object sender, EventArgs e)
        {
            Log.Info("JUDGES encoding finished.");
            JudgesEncodingInfo.Finished = true;
            _transcodeJudges.Unload();
            OnFinish();
        }
        /// <summary>
        /// Preview file encoding has finsihed.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        void _transcodePreview_Finished(object sender, EventArgs e)
        {
            Log.Info("PREVIEW encoding finished.");
            PreviewEncodingInfo.Finished = true;
            _transcodePreview.Unload();
            OnFinish();
        }
        /// <summary>
        /// Sales encoding file is finished
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        void _transcodeSales_Finished(object sender, EventArgs e)
        {
            Log.Info("SALES encoding finshed.");
            SalesEncodingInfo.Finished = true;
            _transcodeSales.Unload();
            OnFinish();
        }
        /// <summary>
        /// Judges encoding file has error.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        void _transcodeJudges_ErrorAbort(object sender, EventArgs e)
        {
            Log.Error("JUDGES encoding error.");
            JudgesEncodingInfo.Error = true;
            JudgesEncodingInfo.Finished = true;
            JudgesEncodingInfo.ErrorInfo = _transcodeJudges.LastError;
            OnFinish();
        }
        /// <summary>
        /// Preview encoding file has error.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        void _transcodePreview_ErrorAbort(object sender, EventArgs e)
        {
            Log.Error("PREVIEW encoding error.");
            PreviewEncodingInfo.Error = true;
            PreviewEncodingInfo.Finished = true;
            PreviewEncodingInfo.ErrorInfo = _transcodePreview.LastError;
            OnFinish();
        }
        /// <summary>
        /// Sales encoding file has error.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        void _transcodeSales_ErrorAbort(object sender, EventArgs e)
        {
            Log.Error("SALES encoding error.");
            SalesEncodingInfo.Error = true;
            SalesEncodingInfo.Finished = true;
            SalesEncodingInfo.ErrorInfo = _transcodeSales.LastError;
            OnFinish();
        }
        
        /// <summary>
        /// Checks if a specific encoding is finished.
        /// </summary>
        /// <param name="encodingInfo"></param>
        /// <returns></returns>
        private bool IsFinished(EncodingInfo encodingInfo)
        {
            if (!encodingInfo.IsEnabled) return true;
            if (encodingInfo.Finished) return true;
            if (encodingInfo.Error) return true;
            return false;
        }
        /// <summary>
        /// Thread safe function to get unique graph notify id.
        /// </summary>
        /// <returns></returns>
        private int GetGraphNotifyId()
        {
            lock (_generateIdLock)
            {
                WM_GRAPHNOTIFY += 1;
                Log.Info("WM_GRAPH_NOTIFY: {0}", WM_GRAPHNOTIFY);
                return WM_GRAPHNOTIFY;
            }
        }
        /// <summary>
        /// Copies the source file to finish successfully folder or error folder, when
        /// all of the encoding are done.
        /// </summary>
        private void OnFinish()
        {
            _dispatcher.BeginInvoke((Action)(() =>
            {
                if (IsFinished(PreviewEncodingInfo) && (SalesEncodingInfo.IsEnabled && !SalesEncodingInfo.Started))
                {
                    StartSalesEncoding();
                    return;
                }
                if ((IsFinished(PreviewEncodingInfo) && IsFinished(SalesEncodingInfo)) &&
                    (JudgesEncodingInfo.IsEnabled && !JudgesEncodingInfo.Started))
                {
                    StartJudgesEncoding();
                    return;
                }
                if (IsFinished(JudgesEncodingInfo) && IsFinished(PreviewEncodingInfo) && IsFinished(SalesEncodingInfo))
                {
                    try
                    {
                        Log.Info("All jobs has finished.");
                        var config = BTFPVideoEncodingLibConfigurationManager.Instance.Configuration;

                        string outputDir = config.DirectoryFinished;
                        if (JudgesEncodingInfo.Error || PreviewEncodingInfo.Error || SalesEncodingInfo.Error)
                        {
                            outputDir = config.DirectoryErrored;
                        }
                        string dstFile = Path.Combine(outputDir, Path.GetFileName(_inputFile));
                        Log.Info("Move file: \"{0}\" to \"{1}\"", _inputFile, dstFile);
                        File.Move(_inputFile, dstFile);
                    }
                    catch (Exception ex)
                    {
                        Log.Error(ex, "OnFinish error.");
                    }
                    finally
                    {
                        FireFinished();
                    }
                }
            }));
        }
        /// <summary>
        /// Fire event that all of the files are transcoded.
        /// </summary>
        private void FireFinished()
        {
            if (IsFinished(JudgesEncodingInfo) && IsFinished(PreviewEncodingInfo) && IsFinished(SalesEncodingInfo))
            {
                if (Finished != null)
                {
                    Finished(this, EventArgs.Empty);
                }
                _dispatcher.BeginInvokeShutdown(DispatcherPriority.Background);
            }
        }
        #endregion

        #region Helpers
        /// <summary>
        /// Starts preview file encoding on separate thread.
        /// </summary>
        /// 
        private void StartPreviewEncoding()
        {
            var config = BTFPVideoEncodingLibConfigurationManager.Instance.Configuration;
            //var thread = new Thread(() =>
            //{
                Log.Info("Init PREVIEW encoder. MsgHandle: {0}. IsEnabled: {1}", _msgHandle.ToInt32(),
                    !config.PreviewEncodingParams.SkipThisEncode);
                if (config.PreviewEncodingParams.SkipThisEncode) return;
                try
                {
                    _transcodePreview = new BTFPVideoTranscoder
                    {
                        WM_GRAPHNOTIFY = GetGraphNotifyId(),
                        MsgHandle = _msgHandle
                    };
                    _transcodePreview.Finished += _transcodePreview_Finished;
                    _transcodePreview.ErrorAbort += _transcodePreview_ErrorAbort;

                    Log.Info("Start PREVIEW encoder.");
                    _transcodePreview.Transcode(_inputFile, config.PreviewEncodingParams);

                    PreviewEncodingInfo.Started = true;
                    PreviewEncodingInfo.Duration = _transcodePreview.Duration;
                }
                catch (Exception ex)
                {
                    Log.Error(ex, "PREVIEW encoding exception.");
                    PreviewEncodingInfo.Finished = true;
                    PreviewEncodingInfo.Error = true;
                    PreviewEncodingInfo.Started = false;
                    PreviewEncodingInfo.ErrorInfo = ex.Message;

                    OnFinish();
                }
            //});
            //thread.SetApartmentState(ApartmentState.STA);
            //thread.Start();
        }
       
        /// <summary>
        ///  Starts sales file encoding on separate thread.
        /// </summary>
        private void StartSalesEncoding()
        {
            var config = BTFPVideoEncodingLibConfigurationManager.Instance.Configuration;
            //new Thread(() =>
            //{
                Log.Info("Init SALES encoder. MsgHandle: {0}. IsEnabled: {1}", _msgHandle.ToInt32(),
                    !config.SalesEncodingParams.SkipThisEncode);
                if (config.SalesEncodingParams.SkipThisEncode) return;
                try
                {
                    _transcodeSales = new BTFPVideoTranscoder
                    {
                        WM_GRAPHNOTIFY = GetGraphNotifyId(),
                        MsgHandle = _msgHandle
                    };
                    _transcodeSales.Finished += _transcodeSales_Finished;
                    _transcodeSales.ErrorAbort += _transcodeSales_ErrorAbort;

                    Log.Info("Start SALES encoder.");
                    _transcodeSales.Transcode(_inputFile, config.SalesEncodingParams);

                    SalesEncodingInfo.Started = true;
                    SalesEncodingInfo.Duration = _transcodeSales.Duration;
                }
                catch (Exception ex)
                {
                    Log.Error(ex, "SALES encoding exception.");
                    SalesEncodingInfo.Finished = true;
                    SalesEncodingInfo.Error = true;
                    SalesEncodingInfo.Started = false;
                    SalesEncodingInfo.ErrorInfo = ex.Message;

                    OnFinish();
                }
            //});
            //thread.SetApartmentState(ApartmentState.MTA);
            //thread.Start();
        }
        /// <summary>
        ///  Starts judges file encoding on separate thread.
        /// </summary>
        private void StartJudgesEncoding()
        {
            var config = BTFPVideoEncodingLibConfigurationManager.Instance.Configuration;

            //var thread = new Thread(() =>
            //{
               // CoInitialize((IntPtr)0);
                //WM_GRAPHNOTIFY += 1;
                GetGraphNotifyId();
                Log.Info("Init JUDGES encoder. MsgHandle: {0}. IsEnabled: {1}", _msgHandle.ToInt32(),
                    !config.JudgesEncodingParams.SkipThisEncode);
                if (config.JudgesEncodingParams.SkipThisEncode) return;
                try
                {
                    _transcodeJudges = new BTFPVideoTranscoder
                    {
                        WM_GRAPHNOTIFY = GetGraphNotifyId(),
                        MsgHandle = _msgHandle
                    };
                    _transcodeJudges.Finished += _transcodeJudges_Finished;
                    _transcodeJudges.ErrorAbort += _transcodeJudges_ErrorAbort;

                    Log.Info("Start JUDGES encoder.");
                    _transcodeJudges.Transcode(_inputFile, config.JudgesEncodingParams);

                    JudgesEncodingInfo.Started = true;
                    JudgesEncodingInfo.Duration = _transcodeJudges.Duration;
                }
                catch (Exception ex)
                {
                    Log.Error(ex, "JUDGES encoding exception.");
                    JudgesEncodingInfo.Finished = true;
                    JudgesEncodingInfo.Error = true;
                    JudgesEncodingInfo.Started = false;
                    JudgesEncodingInfo.ErrorInfo = ex.Message;

                    OnFinish();
                }
           // });
            //thread.SetApartmentState(ApartmentState.MTA);
            //thread.Start();
        }
        #endregion
    }

    #region Encoding Info
    [Serializable()]
    public class EncodingInfo
    {
        public EncodingInfo()
        {
            Finished = false;
            Error = false;
            ErrorInfo = string.Empty;
            IsEnabled = false;
            Progress = 0;
            Duration = 0;
            Started = false;
        }
        public bool Finished { get; set; }
        public bool Error { get; set; }
        public string ErrorInfo { get; set; }
        public bool IsEnabled { get; set; }
        public double Progress { get; set; }
        public double Duration { get; set; }
        public bool Started { get; set; }
    }
    #endregion
}
