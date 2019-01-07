using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using DecklinkCaptureLib;
using NLog;

namespace Decklink_Capture
{
    public partial class frmVideoTrimmer : Form
    {
        #region Logger
        private static readonly Logger Log = LogManager.GetCurrentClassLogger();
        #endregion

        public frmVideoTrimmer()
        {
            InitializeComponent();
        }

        private void frmVideoTrimmer_Load(object sender, EventArgs e)
        {
            try
            {
                BTFPTrimmer.Instance.MsgHandle = Handle;
                BTFPTrimmer.Instance.Finished += Instance_Finished;
                toolTipButton.SetToolTip(btnCapture, "Start/Pause Preview.");
                toolTipButton.SetToolTip(_chkLoop, "Loop.");
                toolTipButton.SetToolTip(_btnBrowseInputClip, "Browse for an input clip.");
            }
            catch (Exception ex)
            {
                UIHelpers.ShowError(ex);
            }
        }

        private void splitContainer1_Panel1_Resize(object sender, EventArgs e)
        {

        }

        private void _btnBrowseInputClip_Click(object sender, EventArgs e)
        {
            try
            {
                var dlg = new OpenFileDialog
                {
                    InitialDirectory = DecklinkCaptureLibConfigurationManager.Instance.Configuration.OutputDirectory,
                    Filter = "Mp4 Files (*.mp4)|*.mp4|All Files (*.*)|*.*",
                    DefaultExt = "mp4"
                };
                DialogInvoker invoker = new DialogInvoker(dlg);
                var dr = invoker.Invoke();
                if (dr != DialogResult.OK) return;

                _txtInputClip.Text = dlg.FileName;
                StartPreview();
              
            }
            catch (Exception ex)
            {
                UIHelpers.ShowError(ex);
            }
        }

        private void StartPreview()
        {
            try
            {
                if (string.IsNullOrEmpty(_txtInputClip.Text) || !File.Exists(_txtInputClip.Text))
                {
                    UIHelpers.ShowInfo("Please select input file.", "Select Input File");
                    return;
                }
                string fileName = _txtInputClip.Text;

                BTFPTrimmer.Instance.Stop();
                btnCapture.Image = Properties.Resources.play;


                BTFPTrimmer.Instance.StartPreview(fileName, _pnlPreview.Handle);
                rangeBar1.TotalMinimum = 0;
                rangeBar1.TotalMaximum = (int) (10*BTFPTrimmer.Instance.Duration);
                rangeBar1.RangeMinimum = 0;
                rangeBar1.RangeMaximum = (int)(10 * BTFPTrimmer.Instance.Duration);
                BTFPTrimmer.Instance.Pause();
                btnCapture.Enabled = true;
                rangeBar1.Enabled = true;
                _chkLoop.Enabled = true;
                _lblExport.Enabled = true;
                _txtExportClip.Enabled = true;
                _btnExport.Enabled = true;

                BTFPTrimmer.Instance.SelectAudioChannels(sixChannelTrackbar1.GetCheckedChannels(),
                    sixChannelTrackbar1.GetVolumeValues(), sixChannelTrackbar1.MuteAll);
            }
            catch (Exception ex)
            {
                UIHelpers.ShowError("Failed to start preview.", ex);
                Log.Error(ex, "Failed to start preview.");
            }
        }
        private void btnCapture_Click(object sender, EventArgs e)
        {
            try
            {
                if (BTFPTrimmer.Instance.IsRunning)
                {
                    _timer.Stop();
                    BTFPTrimmer.Instance.Pause();
                    btnCapture.Image = Properties.Resources.play;
                }
                else
                {
                    BTFPTrimmer.Instance.Play();
                    _timer.Start();
                    btnCapture.Image = Properties.Resources.pause;
                }
            }
            catch (Exception ex)
            {
                UIHelpers.ShowError(ex);
            }
        }

        void Instance_Finished(object sender, EventArgs e)
        {
            if (InvokeRequired)
            {
                BeginInvoke(new Action<object, EventArgs>(Instance_Finished), new object[] {sender, e});
            }
            else
            {
                Log.Info("Playback Finished.");
                try
                {
                    if (_chkLoop.Checked)
                    {
                        BTFPTrimmer.Instance.Position = 0;
                    }
                    else
                    {
                        BTFPTrimmer.Instance.Pause();
                        btnCapture.Image = Properties.Resources.play;
                    }

                }
                catch (Exception ex)
                {
                    Log.Error(ex, "Instance finished error.");
                }
            }
        }

        private void _pnlPreview_Resize(object sender, EventArgs e)
        {
            try
            {
                BTFPTrimmer.Instance.OnResize();
            }
            catch (Exception ex)
            {
                Log.Error(ex, "On Resize Error.");
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
                if (m.Msg == BTFPTrimmer.WM_GRAPHNOTIFY)
                {
                    BTFPTrimmer.Instance.HandleEvent(m);
                }
                base.WndProc(ref m);
            }
            catch (Exception ex)
            {
                Log.Error("WndProc error: {0}", ex.Message);
            }
        }
        /// <summary>
        /// On timer update position, audio peaks and info.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void _timer_Tick(object sender, EventArgs e)
        {
            try
            {
                rangeBar1.PointerPosition = (int) (BTFPTrimmer.Instance.Position * 10);
                ucVUMeter1.SetPeaks(BTFPTrimmer.Instance.GetPeaks());
                UpdateInfo();
            }
            catch (Exception ex)
            {
                Log.Error(ex, "Timer error");
            }
        }
        /// <summary>
        /// Range bar position has changed
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void rangeBar1_PointerPositionChanged(object sender, EventArgs e)
        {
            try
            {
                BTFPTrimmer.Instance.Position = rangeBar1.PointerPosition / 10.0;
                UpdateInfo();
            }
            catch (Exception ex)
            {
                Log.Error(ex, "Failed to set position.");
            }
        }
        /// <summary>
        /// Forms is closing
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void frmVideoTrimmer_FormClosing(object sender, FormClosingEventArgs e)
        {
            try
            {
                _timer.Stop();
                BTFPTrimmer.Instance.Stop();
                BTFPTrimmer.Instance.Finished -= Instance_Finished;
            }
            catch (Exception ex)
            {
                Log.Error(ex, "Failed to stop trimmer.");
            }
        }
        /// <summary>
        /// Selected channesl for preview has changed or audio volume.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void sixChannelTrackbar1_ValueChanged(object sender, SixChannelsTrackBarEventArgs e)
        {
            try
            {
                BTFPTrimmer.Instance.SelectAudioChannels(e.CheckedChannels, e.Volumes, sixChannelTrackbar1.MuteAll);
            }
            catch (Exception ex)
            {
                Log.Error("Failed to set audio channels and volume. Details: {0}", ex.Message);
            }
        }
        /// <summary>
        /// Range bar minimum has changed
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void rangeBar1_LeftRangeChanging(object sender, EventArgs e)
        {
            if (BTFPTrimmer.Instance.Status == BTFPTrimmer.BTFPTrimmerStatus.Idle ||
                BTFPTrimmer.Instance.Status == BTFPTrimmer.BTFPTrimmerStatus.InExport)
            {
                return;
            }
            try
            {
                BTFPTrimmer.Instance.Position = rangeBar1.RangeMinimum / 10.0;
                UpdateInfo();
            }
            catch (Exception ex)
            {
                Log.Error("Failed to set left range. Details: {0}", ex.Message);
            }
        }
        /// <summary>
        /// Range bar maximum has changed.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void rangeBar1_RightRangeChanging(object sender, EventArgs e)
        {
            if (BTFPTrimmer.Instance.Status == BTFPTrimmer.BTFPTrimmerStatus.Idle ||
                 BTFPTrimmer.Instance.Status == BTFPTrimmer.BTFPTrimmerStatus.InExport)
            {
                return;
            }
            try
            {
                BTFPTrimmer.Instance.Position = rangeBar1.RangeMaximum / 10.0;
                UpdateInfo();
            }
            catch (Exception ex)
            {
                Log.Error("Failed to set right range. Details: {0}", ex.Message);
            }
        }
        /// <summary>
        /// Update info about current process.
        /// </summary>
        private void UpdateInfo()
        {
            if (InvokeRequired)
            {
                BeginInvoke(new Action(UpdateInfo));
            }
            else
            {
                try
                {
                    if (BTFPTrimmer.Instance.Status == BTFPTrimmer.BTFPTrimmerStatus.Idle)
                    {
                        _lblInfo.Text = string.Empty;
                        return;
                    }
                    TimeSpan tsPosition = TimeSpan.FromSeconds(BTFPTrimmer.Instance.Position);
                    TimeSpan tsDuration = TimeSpan.FromSeconds(BTFPTrimmer.Instance.Duration);
                    TimeSpan tsRangeMin = TimeSpan.FromSeconds(rangeBar1.RangeMinimum/10.0);
                    TimeSpan tsRangeMax = TimeSpan.FromSeconds(rangeBar1.RangeMaximum/10.0);
                    string str =
                        string.Format(
                            "(Position: {0:hh\\:mm\\:ss}, Duration: {1:hh\\:mm\\:ss}, Range: {2:hh\\:mm\\:ss} - {3:hh\\:mm\\:ss})",
                            new object[] {tsPosition, tsDuration, tsRangeMin, tsRangeMax});
                    _lblInfo.Text = str;
                }
                catch (Exception ex)
                {
                    Log.Error(ex, "Update Info Error.");
                }
            }
        }
        /// <summary>
        /// Start exporting.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void _btnExport_Click(object sender, EventArgs e)
        {
            try
            {
                BTFPTrimmer.Instance.Finished -= Instance_Finished;

                if (string.IsNullOrEmpty(_txtInputClip.Text))
                {
                    UIHelpers.ShowInfo("Please enter input clip.", "Input Clip");
                    return;
                }

                if (string.IsNullOrEmpty(_txtExportClip.Text))
                {
                    UIHelpers.ShowInfo("Please enter the name of the export clip.", "Export Clip");
                    return;
                }

                _timer.Stop();
                
                string inputFile = _txtInputClip.Text;
                string outputFile = _txtExportClip.Text;
                double markIn = rangeBar1.RangeMinimum/10.0;
                double markOut = rangeBar1.RangeMaximum/10.0;

                BTFPTrimmer.Instance.StartExport(inputFile, outputFile, markIn, markOut, _pnlPreview.Handle);

                
                frmExporting frm = new frmExporting();
                frm.ShowDialog();

                UIHelpers.ShowInfo("Render Completed Successfully.", "Render Completed");

                StartPreview();
            }
            catch (Exception ex)
            {
                UIHelpers.ShowError("Failed to start export.", ex);
            }
            finally
            {
                BTFPTrimmer.Instance.Finished += Instance_Finished;
            }
        }

        private void EnableControlsDuringCapture(bool enable)
        {
            groupBox1.Enabled = enable;
            _btnBrowseInputClip.Enabled = enable;
            label1.Enabled = enable;
            _lblExport.Enabled = enable;
            _txtExportClip.Enabled = enable;
            btnCapture.Enabled = enable;
            _chkLoop.Enabled = enable;
            rangeBar1.Enabled = enable;
            _btnExport.Text = enable ? "Export" : "Stop Export";
        }

        private void _btnBrowseOutputClip_Click(object sender, EventArgs e)
        {
            var dlg = new SaveFileDialog()
            {
                InitialDirectory = DecklinkCaptureLibConfigurationManager.Instance.Configuration.OutputDirectory,
                Filter = "Mp4 Files (*.mp4)|*.mp4|All Files (*.*)|*.*",
                DefaultExt = "mp4"
            };
            DialogInvoker invoker = new DialogInvoker(dlg);
            var dr = invoker.Invoke();
            if (dr != DialogResult.OK) return;

            _txtExportClip.Text = dlg.FileName;
        }
    }
}
