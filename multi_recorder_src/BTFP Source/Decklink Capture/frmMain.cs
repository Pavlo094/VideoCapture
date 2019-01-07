using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using DecklinkCaptureLib;
using System.IO;
using System.Text.RegularExpressions;
using System.Threading;
using NLog;
using CaptureLib = DecklinkCaptureLib.DecklinkCaptureLib;
using DeckLinkAPI;
using System.IO.Compression;

namespace Decklink_Capture
{
    public partial class frmMain : Form
    {
        #region Logger
        private static readonly Logger Log = LogManager.GetCurrentClassLogger();
        #endregion

        #region members
        /// <summary>
        /// This variable carries the time when the last capture started
        /// </summary>
        private DateTime _captureStartTime = DateTime.Now;
        private readonly AutoResetEvent _waitForTimerEvent = new AutoResetEvent(false);
        #endregion

        #region Constructor
        /// <summary>
        /// Constructor
        /// </summary>
        public frmMain()
        {
            InitializeComponent();
        }
        #endregion

        /// <summary>
        /// Exit app.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void exitToolStripMenuItem_Click(object sender, EventArgs e)
        {
            Close();
        }
        /// <summary>
        /// Configuration opened.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void configurationToolStripMenuItem_Click(object sender, EventArgs e)
        {
            try
            {
                _timer.Stop();
                CaptureLib.Instance.Stop();

                var frm = new frmConfiguration();
                frm.ShowDialog();

                var config = DecklinkCaptureLibConfigurationManager.Instance.Configuration;
                //if (!string.IsNullOrEmpty(config.VideoSource) ||
                //    !string.IsNullOrEmpty(config.AudioSource))
                if(!string.IsNullOrEmpty(config.DecklinkDevice))
                {
                    UpdateConfiguration();
                    StartPreview();
                    SetNextFileName();
                }
            }
            catch (Exception ex)
            {
                UIHelpers.ShowError(ex);
            }
        }
        /// <summary>
        /// On load, offer the configuration (for the first time) or start the preview
        /// if the input video/audio cards are set.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void frmMain_Load(object sender, EventArgs e)
        {
            try
            {
                Log.Info("On form load.");

                SystemResources.Init();
                _resourcesTimer.Start();

                var config = DecklinkCaptureLibConfigurationManager.Instance.Configuration;
                UpdateConfiguration();

                CaptureLib.Instance.MsgHandle = Handle;
                CaptureLib.Instance.ErrorAbort += Instance_ErrorAbort;

                //if (string.IsNullOrEmpty(config.VideoSource) ||
                //    string.IsNullOrEmpty(config.AudioSource))
                //{
                //    Log.Info("Video and/or audio inputs cards are not set. Go to configuration.");
                //    configurationToolStripMenuItem_Click(this, EventArgs.Empty);
                //}
                if (string.IsNullOrEmpty(config.DecklinkDevice))
                {
                    Log.Info("Decklink inputs card is not set. Go to configuration.");
                    configurationToolStripMenuItem_Click(this, EventArgs.Empty);
                }
                else
                {
                   StartPreview();
                }
            }
            catch (Exception ex)
            {
                UIHelpers.ShowError(ex);
            }
        }

        void Instance_ErrorAbort(object sender, EventArgs e)
        {
            if (InvokeRequired)
            {
                BeginInvoke(new Action<object, EventArgs>(Instance_ErrorAbort), new object[] {sender, e});
            }
            else
            {
                Thread.Sleep(500);
                CaptureLib.Instance.Stop();
                StartPreview();
            }
        }
        /// <summary>
        /// Creates new filename by incrementing count on exisiting filenames.
        /// </summary>
        private void SetNextFileName()
        {
            Log.Info("Set next filename.");
            var config = DecklinkCaptureLibConfigurationManager.Instance.Configuration;

            int counter = 1;
            if (!string.IsNullOrEmpty(_txtFileName.Text))
            {
                string txt = _txtFileName.Text;
                // match a number out of string with mixed number and characters
                Match m = Regex.Match(_txtFileName.Text, @"\d+");
                if (!string.IsNullOrEmpty(m.Value))
                {
                    txt = m.Value;
                }
                int result;
                if (int.TryParse(txt, out result))
                {
                    counter = result;
                }
            }
            while (true)
            {
                string file = string.Format("{0}\\{1}.mp4", config.OutputDirectory, counter);
                if (!File.Exists(file))
                {
                    break;
                }
                counter++;
            }
            string newFile = string.Format("{0}", counter);

            _txtFileName.Text = newFile;
        }
        /// <summary>
        /// On panel preview resize update the video window.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void splitContainer1_Panel1_Resize(object sender, EventArgs e)
        {
            try
            {
                Log.Info("On preview panel resize.");
                DecklinkCaptureLib.DecklinkCaptureLib.Instance.OnResize();
            }
            catch (Exception ex)
            {
                Log.Error("Resize error. Details: {0}", ex.Message);
            }
        }
        /// <summary>
        /// On close, close the component.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void frmMain_FormClosing(object sender, FormClosingEventArgs e)
        {
            try
            {
                Log.Info("On form closing.");
                _timer.Stop();
                
                DecklinkCaptureLib.DecklinkCaptureLib.Instance.Stop();
            }
            catch (Exception ex)
            {
                Log.Error("Error stopping decklink. Details: {0}", ex.Message);
            }
        }
        /// <summary>
        /// Set configuration values to controls
        /// </summary>
        private void UpdateConfiguration()
        {
            SetNextFileName();

            var config = DecklinkCaptureLibConfigurationManager.Instance.Configuration;            
            bool interlaced = (config.FieldOrder == _BMDFieldDominance.bmdUpperFieldFirst) || (config.FieldOrder == _BMDFieldDominance.bmdLowerFieldFirst);
            _txtOutputDir.Text = config.OutputDirectory;
            _txtVideoFormat.Text = string.Format("{0}x{1}{2}{3:00.00}", config.VideoWidth, config.VideoHeight,
                (interlaced ? "i" : "p"),
                ( interlaced ? 2 : 1) * 10000000.0 / config.VideoAvgTimePerFrame);
            _txtAudioFormat.Text = string.Format("{0} channels, {1} Hz, {2} bit", config.AudioChannels,
                config.AudioSamplerate, config.AudioBitCount);
            _txtInputVideoCard.Text = config.DecklinkDevice;

            if (config.LogoPath.Length != 0)
            {
                //pictureBox1.Load(config.LogoPath);
                pictureBox1.Image = Image.FromStream(new MemoryStream(File.ReadAllBytes(config.LogoPath)));
            }
            else
            {
                pictureBox1.Image = Decklink_Capture.Properties.Resources.BTFP_Logo;
            }
        }
        /// <summary>
        /// Change volume
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void sixChannelTrackbar1_ValueChanged(object sender, SixChannelsTrackBarEventArgs e)
        {
            try
            {
                CaptureLib.Instance.SelectAudioChannels(e.CheckedChannels, e.Volumes, e.MuteAll);
            }
            catch (Exception ex)
            {
                Log.Error("Failed to set audio channels and volume. Details: {0}", ex.Message);
            }
        }
        /// <summary>
        /// Change output directory.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void _btnBrowseOutputDir_Click(object sender, EventArgs e)
        {
            try
            {
                var dlg = new FolderBrowserDialog
                {
                    SelectedPath = DecklinkCaptureLibConfigurationManager.Instance.Configuration.OutputDirectory
                };
                DialogInvoker invoker = new DialogInvoker(dlg);
                var dr = invoker.Invoke();
                if (dr != DialogResult.OK) return;

                DecklinkCaptureLibConfigurationManager.Instance.Configuration.OutputDirectory = invoker.Path;

                DecklinkCaptureLibConfigurationManager.Instance.Save();
                UpdateConfiguration();
                SetNextFileName();
            }
            catch (Exception ex)
            {
                UIHelpers.ShowError(ex);
            }
        }
        /// <summary>
        /// Cal preview on component.
        /// </summary>
        private void StartPreview()
        {
            Log.Info("Call start preview.");
            CaptureLib.Instance.StartPreview(splitContainer1.Panel1.Handle);
            CaptureLib.Instance.SelectAudioChannels(sixChannelTrackbar1.GetCheckedChannels(),
                sixChannelTrackbar1.GetVolumeValues(), sixChannelTrackbar1.MuteAll);
            _timer.Start();
        }
        /// <summary>
        /// Read info, peaks etc. here.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void _timer_Tick(object sender, EventArgs e)
        {
            try
            {
                _waitForTimerEvent.Reset();

                var peaks = CaptureLib.Instance.GetPeaks();
                ucVUMeter1.SetPeaks(peaks);

                if (CaptureLib.Instance.IsRecording)
                {
                    int droppedFrames = CaptureLib.Instance.GetDroppedFrames();
                    var recordingLength = DateTime.Now.Subtract(_captureStartTime);
                    _lblInfo2.Text = string.Format("Dropped frames: {0}, File duration: {1:hh\\:mm\\:ss}", droppedFrames, recordingLength);
                }
            }
            catch (Exception ex)
            {
                Log.Error(ex, "On timer error.");
            }
            finally
            {
                _waitForTimerEvent.Set();
            }
        }
        /// <summary>
        /// Start/stop capture
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void btnCapture_Click(object sender, EventArgs e)
        {
            try
            {
                _timer.Stop();

                if (!CaptureLib.Instance.IsRecording)
                {
                    var config = DecklinkCaptureLibConfigurationManager.Instance.Configuration;
                    string file = string.Format("{0}\\{1}.mp4", config.OutputDirectory, _txtFileName.Text);
                    if (File.Exists(file))
                    {
                        file = GetTempFile();
                    }
                    CaptureLib.Instance.StartCapture(file, splitContainer1.Panel1.Handle);
                    _captureStartTime = DateTime.Now;
                    CaptureLib.Instance.SelectAudioChannels(sixChannelTrackbar1.GetCheckedChannels(),
                        sixChannelTrackbar1.GetVolumeValues(), sixChannelTrackbar1.MuteAll);
                    btnCapture.Text = "Stop";
                    btnCapture.Image = Properties.Resources.stop;
                    configurationToolStripMenuItem.Enabled = false;
                    splitContainer1.Panel2.Enabled = false;
                    trimVideoToolStripMenuItem.Enabled = false;
                    _btnTrimVideo.Enabled = false;
                    _timer.Start();
                }
                else
                {
                    string currentFile = CaptureLib.Instance.CurrentOutputFile;
                    CaptureLib.Instance.Stop();
                    btnCapture.Text = "Record";
                    btnCapture.Image = Properties.Resources.record;
                    configurationToolStripMenuItem.Enabled = true;
                    splitContainer1.Panel2.Enabled = true;
                    trimVideoToolStripMenuItem.Enabled = true;
                    _btnTrimVideo.Enabled = true;
                    Thread.Sleep(500);
                   
                    StartPreview();

                    if (currentFile.Contains("temp"))
                    {
                        UIHelpers.ShowInfo("Temporary file has been created. Please choose a filename in the next dialog.", "Select Filename");
                        var dlg = new SaveFileDialog();
                        dlg.Filter = "MP4 Files | *.mp4";
                        dlg.DefaultExt = "mp4";
                        dlg.InitialDirectory = Path.GetDirectoryName(currentFile);

                        DialogInvoker invoker = new DialogInvoker(dlg);
                        var dr = invoker.Invoke();
                        if (dr == DialogResult.OK)
                        {
                            File.Move(currentFile, dlg.FileName);
                        }

                    }
                    SetNextFileName();
                }
            }
            catch (Exception ex)
            {
                UIHelpers.ShowError(ex);
            }
        }
        /// <summary>
        /// Override main windows procedure to capture DirectShow messages.
        /// </summary>
        /// <param name="m"></param>
        protected override void WndProc(ref Message m)
        {
            try
            {
                if (m.Msg == CaptureLib.WM_GRAPHNOTIFY)
                {
                    CaptureLib.Instance.HandleEvent(m);
                }
                base.WndProc(ref m);
            }
            catch (Exception ex)
            {
                Log.Error("WndProc error: {0}", ex.Message);
            }
        }

        private void _resourcesTimer_Tick(object sender, EventArgs e)
        {
            double appRam;
            short appCpu;
            double cpuTotal;
            double ramTotal;
            SystemResources.GetAppSystemResources(out appRam, out appCpu, out cpuTotal, out ramTotal);

            ulong driveFreeBytes = 0;
            if (System.IO.Directory.Exists(DecklinkCaptureLibConfigurationManager.Instance.Configuration.OutputDirectory))
            {
                SystemResources.DriveFreeBytes(
                    DecklinkCaptureLibConfigurationManager.Instance.Configuration.OutputDirectory, out driveFreeBytes);
            }
            _lblAppCpu.Text = string.Format("Application CPU Usage: {0}%", appCpu);
            _lblAppRam.Text = string.Format("Application RAM Usage: {0:##.##}MB", appRam);
            _lblTotalCpu.Text = string.Format("Total CPU Usage: {0}%", (int)cpuTotal);
            _lblTotalRam.Text = string.Format("Total RAM Usage: {0:##.##}GB", ramTotal / 1024.0);
            _lblDriveFreeSpace.Text = string.Format("Drive Free Space: {0:##.##}GB", driveFreeBytes / (1024.0 * 1024.0 * 1024.0));

            if (CaptureLib.Instance.IsRecording)
            {
                long fileSize = CaptureLib.Instance.GetFileSize();
                _lblInfo.Text = string.Format("File size: {0:0.###}MB", fileSize / (1024.0 * 1024.0));
            }

        }

        private void aboutToolStripMenuItem_Click(object sender, EventArgs e)
        {
            frmAbout frm = new frmAbout();
            var config = DecklinkCaptureLibConfigurationManager.Instance.Configuration;
            if (config.LogoPath.Length != 0)
            {
                frm.getPictureBox1().Image = Image.FromStream(new MemoryStream(File.ReadAllBytes(config.LogoPath)));
                //frm.getPictureBox1().Load(config.LogoPath);
            }
            else
            {
                frm.getPictureBox1().Image = Decklink_Capture.Properties.Resources.BTFP_Logo;
            }
            frm.ShowDialog();
            UpdateConfiguration();
        }

        private string GetTempFile()
        {
            var config = DecklinkCaptureLibConfigurationManager.Instance.Configuration;
            string file = string.Format("{0}\\{1}_temp.mp4", config.OutputDirectory, _txtFileName.Text);
            if (!File.Exists(file))
            {
                return file;
            }
            int counter = 1;
            while (true)
            {
                file = string.Format("{0}\\{1}_temp{2}.mp4", config.OutputDirectory, _txtFileName.Text, counter);
                if (!File.Exists(file))
                {
                    return file;
                }
                counter++;
            }
        }

        private void trimVideoToolStripMenuItem_Click(object sender, EventArgs e)
        {
            ShowTrimVideoDialog();
        }

        private void _btnTrimVideo_Click(object sender, EventArgs e)
        {
            ShowTrimVideoDialog();
        }

        private void ShowTrimVideoDialog()
        {
            try
            {
                _timer.Stop();
                CaptureLib.Instance.Stop();
                frmVideoTrimmer frm = new frmVideoTrimmer();
                frm.ShowDialog();
            }
            catch (Exception ex)
            {
                Log.Error(ex, "ShowTrimVideoDialog error.");
                UIHelpers.ShowError(ex);
            }
            finally
            {
                try
                {
                    StartPreview();
                }
                catch 
                {
                    //
                }
            }
        }

    }

    #region DialogInvoker
    /// <summary>
    /// Opens a specified Dialog box on an STA thread
    /// </summary>
    public class DialogInvoker
    {
        #region Members
        private string _fileName;
        private DialogResult _invokeResult;
        private CommonDialog _dialog;
        #endregion

        // Constructor is passed the dialog to use
        public DialogInvoker(CommonDialog dialog)
        {
            _invokeResult = DialogResult.None;
            _fileName = null;
            _dialog = dialog;
        }

        /// <summary>
        /// Start the thread and get the result
        /// </summary>
        /// <returns></returns>
        public DialogResult Invoke()
        {
            Thread thdInvokeThread;

            // No reason to waste a thread if we aren't MTA
            if (Thread.CurrentThread.GetApartmentState() == ApartmentState.MTA)
            {
                thdInvokeThread = new Thread(new ThreadStart(InvokeMethod));
                thdInvokeThread.SetApartmentState(ApartmentState.STA);

                thdInvokeThread.Start();
                thdInvokeThread.Join();
                thdInvokeThread = null;
            }
            else
            {
                // If we are already STA, just invoke the method
                InvokeMethod();
            }

            return _invokeResult;
        }
        /// <summary>
        /// Filename or selected path, depending if the dialog is OpenFileDialog, SaveFileDialog or FolderBrowserDialog
        /// </summary>
        public string Path
        {
            get { return _fileName; }
        }

        // The thread entry point
        private void InvokeMethod()
        {
            _invokeResult = _dialog.ShowDialog();

            if (_invokeResult == DialogResult.OK)
            {
                if (_dialog is OpenFileDialog)
                {
                    _fileName = ((OpenFileDialog) _dialog).FileName;
                }
                else if (_dialog is SaveFileDialog)
                {
                    _fileName = ((SaveFileDialog) _dialog).FileName;
                }
                else if (_dialog is FolderBrowserDialog)
                {
                    _fileName = ((FolderBrowserDialog) _dialog).SelectedPath;
                }
            }
        }
    }
    #endregion
}
