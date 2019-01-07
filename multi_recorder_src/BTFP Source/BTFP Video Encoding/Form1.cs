using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Globalization;
using System.IO;
using System.Linq;
using System.Text;
using System.Text.RegularExpressions;
using System.Windows.Forms;
using BTFPVideoEncodingLib;
using Decklink_Capture;
using MediaInfoLib;
using NLog;
using System.Threading;

namespace BTFP_Video_Encoding
{
    public partial class Form1 : Form
    {
        #region Logger
        private static readonly Logger Log = LogManager.GetCurrentClassLogger();
        #endregion

        #region Members
        private bool _started = false;
        private List<TotalEncodingInfo> _encodedFilesList = new List<TotalEncodingInfo>();
        private bool _encodingPaused = false;
        #endregion

        public Form1()
        {
            InitializeComponent();

            ctrlEncodingJobList1.JobFinished += ctrlEncodingJobList1_JobFinished;
            UpdateConfiguration();
        }

        private void exitToolStripMenuItem_Click(object sender, EventArgs e)
        {
            Close();
        }

        private void configurationToolStripMenuItem_Click(object sender, EventArgs e)
        {
            try
            {
                var frm = new frmConfiguration();
                if (frm.ShowDialog() == DialogResult.OK)
                {
                    _txtMonitorFolder.Text =
                    BTFPVideoEncodingLibConfigurationManager.Instance.Configuration.DirectoryMonitor;
                }
            }
            catch (Exception ex)
            {
                UIHelpers.ShowError(ex);
            }
        }

        private void aboutToolStripMenuItem_Click(object sender, EventArgs e)
        {
            frmAbout frm = new frmAbout();
            frm.updateLogo(BTFPVideoEncodingLibConfigurationManager.Instance.Configuration.LogoPath);
            frm.ShowDialog();
            UpdateConfiguration();
        }

        private void _resourcesTimer_Tick(object sender, EventArgs e)
        {
            double appRam;
            short appCpu;
            double cpuTotal;
            double ramTotal;
            SystemResources.GetAppSystemResources(out appRam, out appCpu, out cpuTotal, out ramTotal);

            ulong driveFreeBytes = 0;
            if (System.IO.Directory.Exists(BTFPVideoEncodingLibConfigurationManager.Instance.Configuration.SalesEncodingParams.OutputDirectory))
            {
                SystemResources.DriveFreeBytes(
                    BTFPVideoEncodingLibConfigurationManager.Instance.Configuration.SalesEncodingParams.OutputDirectory, out driveFreeBytes);
            }
            _lblAppCpu.Text = string.Format("Application CPU Usage: {0}%", appCpu);
            _lblAppRam.Text = string.Format("Application RAM Usage: {0:##.##}MB", appRam);
            _lblTotalCpu.Text = string.Format("Total CPU Usage: {0}%", (int)cpuTotal);
            _lblTotalRam.Text = string.Format("Total RAM Usage: {0:##.##}GB", ramTotal / 1024.0);
            _lblDriveFreeSpace.Text = string.Format("Drive Free Space: {0:##.##}GB", driveFreeBytes / (1024.0 * 1024.0 * 1024.0));
        }

        private void Form1_Load(object sender, EventArgs e)
        {
            try
            {
                Log.Info("On form load.");

                SystemResources.Init();
                _resourcesTimer.Start();

                _txtMonitorFolder.Text =
                    BTFPVideoEncodingLibConfigurationManager.Instance.Configuration.DirectoryMonitor;
            }
            catch (Exception ex)
            {
                Log.Error(ex, "Form Load Error.");
                UIHelpers.ShowError(ex);
            }
        }

        private void _btnStart_Click(object sender, EventArgs e)
        {
            try
            {
                if (!_started)
                {
                    Log.Info("Start monitor.");

                    CheckForFolders();

                    LoadFiles();

                    fileSystemWatcher1.Path =
                        BTFPVideoEncodingLibConfigurationManager.Instance.Configuration.DirectoryMonitor;
                    fileSystemWatcher1.EnableRaisingEvents = true;

                    _encodingControlTimer.Start();

                    _started = true;
                    _btnStart.Text = "Stop Monitor";
                    _btnStart.Image = Properties.Resources.stop;
                    //tableLayoutPanel1.Enabled = true;
                    configurationToolStripMenuItem.Enabled = false;

                    _renderTimer.Start();
                }
                else
                {
                    fileSystemWatcher1.EnableRaisingEvents = false;
                    _encodingControlTimer.Stop();
                    _lstFilesForEncoding.Items.Clear();
                    _started = false;
                    _btnStart.Text = "Start Monitor";
                    _btnStart.Image = Properties.Resources.play;
                    //tableLayoutPanel1.Enabled = false;
                    configurationToolStripMenuItem.Enabled = true;
                }
            }
            catch (Exception ex)
            {
                Log.Error(ex,"Start error.");
                UIHelpers.ShowError(ex);
            }
        }
        /// <summary>
        /// On this timer tick, the program checks if new files arrived for encoding and if the
        /// number of current job is smaller then the maximum allowed job it will add new file
        /// for processing.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void _encodingControlTimer_Tick(object sender, EventArgs e)
        {
            try
            {
                if (_encodingPaused)
                {
                    return;
                }
                var config = BTFPVideoEncodingLibConfigurationManager.Instance.Configuration;
                if (config.NumberOfEncodingThreads > ctrlEncodingJobList1.CurrentJobsNr)
                {
                    ListViewItem item = null;
                    for (int i = 0; i < _lstFilesForEncoding.Items.Count; i++)
                    {
                        item = _lstFilesForEncoding.Items[i];
                        var fileForEncoding = (FileForEncoding) item.Tag;
                        //string file = (string)item.Tag;
                        if (!IsFileLocked(new FileInfo(fileForEncoding.FileName)))
                        {
                            ctrlEncodingJobList1.AddNewJob(fileForEncoding);
                           
                            break;
                        }
                        item = null;
                        Log.Info("File: {0} is locked.", fileForEncoding.FileName);
                    }

                    if (item != null)
                    {
                        _lstFilesForEncoding.Items.Remove(item);
                        ctrlEncodingJobList1.UpdateRemainingJobTime(GetFilesForEncoding());
                    }
                }
            }
            catch (Exception ex)
            {
                Log.Error(ex, "Encoding control timer error.");
            }

        }

        private List<FileForEncoding> GetFilesForEncoding()
        {
            return (from ListViewItem item in _lstFilesForEncoding.Items select item.Tag as FileForEncoding).ToList();
        }

        /// <summary>
        /// Some jobs has finished. Move it to finished listview.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="args"></param>
        void ctrlEncodingJobList1_JobFinished(object sender, JobFinishedArgs args)
        {
            try
            {
                var totalEncodingInfo = new TotalEncodingInfo(args.SalesEncodingInfo, args.PreviewEncodingInfo,
                    args.JudgesEncodingInfo, args.InputFile);

                AddEncodedFileInListView(totalEncodingInfo);
                _encodedFilesList.Add(totalEncodingInfo);
            }
            catch (Exception ex)
            {
                Log.Error(ex, "Encoding job finished error.");
            }
        }
        /// <summary>
        /// Checks for needed folders from the configuration and creates them if they are not already created.
        /// </summary>
        private void CheckForFolders()
        {
            Log.Info("Check for folders.");
            var config = BTFPVideoEncodingLibConfigurationManager.Instance.Configuration;

            if (!Directory.Exists(config.DirectoryMonitor))
            {
                throw new Exception("Monitor folder does not exist");
            }

            if (!Directory.Exists(config.DirectoryFinished))
            {
                Directory.CreateDirectory(config.DirectoryFinished);
            }

            if (!Directory.Exists(config.DirectoryErrored))
            {
                Directory.CreateDirectory(config.DirectoryErrored);
            }

            if (!Directory.Exists(config.JudgesEncodingParams.OutputDirectory))
            {
                Directory.CreateDirectory(config.JudgesEncodingParams.OutputDirectory);
            }

            if (!Directory.Exists(config.PreviewEncodingParams.OutputDirectory))
            {
                Directory.CreateDirectory(config.PreviewEncodingParams.OutputDirectory);
            }

            if (!Directory.Exists(config.SalesEncodingParams.OutputDirectory))
            {
                Directory.CreateDirectory(config.SalesEncodingParams.OutputDirectory);
            }
        }
        /// <summary>
        /// Loads mp4 files into files list view, from the monitor folder, defined in the config.
        /// </summary>
        private void LoadFiles()
        {
            var config = BTFPVideoEncodingLibConfigurationManager.Instance.Configuration;
            Log.Info("Load files.");
            string[] filePaths = Directory.GetFiles(config.DirectoryMonitor, "*.mp4");
            Array.Sort(filePaths, CompareStrings);

            foreach (var file in filePaths)
            {
                AddFileToEncodeToListView(file);
            }
        }
        /// <summary>
        /// Compares strings. Tries to sort strings like if they were numbers.
        /// </summary>
        /// <param name="s1"></param>
        /// <param name="s2"></param>
        /// <returns></returns>
        public static int CompareStrings(string s1, string s2)
        {
            Match m1 = Regex.Match(s1, @"\d+");
            Match m2 = Regex.Match(s2, @"\d+");
            if (!string.IsNullOrEmpty(m1.Value) && !string.IsNullOrEmpty(m2.Value))
            {
                int n1, n2;
                bool b1Number = int.TryParse(m1.Value, out n1);
                bool b2Number = int.TryParse(m2.Value, out n2);
                if (b1Number && b2Number)
                {
                    if (n1 == n2) return 0;
                    if (n1 < n2) return -1;
                    if (n1 > n2) return 1;
                }
            }
            // None of them are numbers
            var sc = StringComparer.Create(CultureInfo.InvariantCulture, false);
            return sc.Compare(s1, s2);
        }

        private void AddFileToEncodeToListView(string fileName)
        {
            try
            {
                var fileForEncoding = new FileForEncoding();
                fileForEncoding.FileName = fileName;
                GetMediaLength(fileForEncoding);

                var item = new ListViewItem(Path.GetFileName(fileForEncoding.FileName)) {Tag = fileForEncoding};
                _lstFilesForEncoding.Items.Add(item);
            }
            catch (Exception ex)
            {
                UIHelpers.ShowError(ex);
            }
        }

        private void GetMediaLength(FileForEncoding fileForEncoding)
        {
            // Get length.
            MediaInfo mi = new MediaInfo();
            bool fileOpened = false;
            try
            {
                if (mi.Open(fileForEncoding.FileName) > 0)
                {
                    fileOpened = true;
                    string strLength = mi.Get(StreamKind.General, 0, "Duration");
                    if (strLength.Length != 0)
                    {
                        int lengthInMs = int.Parse(strLength);
                        fileForEncoding.Length = lengthInMs / 1000.0;
                    }
                    else
                    {
                        fileForEncoding.Length = 0;
                    }
                }
            }
            finally
            {
                if (fileOpened)
                {
                    mi.Close();
                }
            }
        }

        private void _btnClearEncodedList_Click(object sender, EventArgs e)
        {
            _lstEncodedFiles.Items.Clear();
            _encodedFilesList.Clear();
        }

        private void _chkShowOnlyWithErrors_CheckedChanged(object sender, EventArgs e)
        {
            try
            {
                _lstEncodedFiles.Items.Clear();
                foreach (var encodingInfo in _encodedFilesList)
                {
                    if (_chkShowOnlyWithErrors.Checked)
                    {
                        if ((encodingInfo.JudgesEncodingInfo.Error || encodingInfo.PreviewEncodingInfo.Error ||
                             encodingInfo.SalesEncodingInfo.Error))
                        {
                            AddEncodedFileInListView(encodingInfo);
                        }
                    }
                    else
                    {
                        AddEncodedFileInListView(encodingInfo);
                    }
                }
            }
            catch (Exception ex)
            {
               UIHelpers.ShowError(ex);
            }
        }

        private void AddEncodedFileInListView(TotalEncodingInfo encodingInfo)
        {
            var item = new ListViewItem(Path.GetFileName(encodingInfo.InputFile));
            item.Tag = encodingInfo;
            item.ImageIndex = (encodingInfo.JudgesEncodingInfo.Error || encodingInfo.PreviewEncodingInfo.Error ||
                               encodingInfo.SalesEncodingInfo.Error)
                ? 1
                : 0;
            _lstEncodedFiles.Items.Add(item);
        }

        private void fileSystemWatcher1_Created(object sender, FileSystemEventArgs e)
        {
            if (e.ChangeType == WatcherChangeTypes.Created)
            {
                Log.Info("File system watcher detected new file: \"{0}\"", e.FullPath);
                AddFileToEncodeToListView(e.FullPath);
            }
        }

        private bool IsFileLocked(FileInfo file)
        {
            FileStream stream = null;

            try
            {
                stream = file.Open(FileMode.Open, FileAccess.Read, FileShare.None);
            }
            catch (IOException)
            {
                //the file is unavailable because it is:
                //still being written to
                //or being processed by another thread
                //or does not exist (has already been processed)
                return true;
            }
            finally
            {
                if (stream != null)
                    stream.Close();
            }

            //file is not locked
            return false;
        }

        private void Form1_FormClosing(object sender, FormClosingEventArgs e)
        {
            try
            {
                if (ctrlEncodingJobList1.CurrentJobsNr > 0)
                {
                    var dr = UIHelpers.ShowQuestion(
                        "There are still active encoding jobs. Are you sure you want to exit and cancel the current job?",
                        "Cancel encoding job");
                    if (dr == DialogResult.Yes)
                    {
                        fileSystemWatcher1.EnableRaisingEvents = false;
                        _resourcesTimer.Stop();
                        _encodingControlTimer.Stop();
                        ctrlEncodingJobList1.EndCurrentJob();
                    }
                    else
                    {
                        e.Cancel = true;
                    }
                }
            }
            catch (Exception ex)
            {
                UIHelpers.ShowError(ex);
            }
        }

        private void _lstEncodedFiles_MouseClick(object sender, MouseEventArgs e)
        {
            if (_lstEncodedFiles.SelectedItems.Count == 0) return;

            try
            {
                var encodingInfo = (TotalEncodingInfo) _lstEncodedFiles.SelectedItems[0].Tag;
                if (encodingInfo != null)
                {
                    frmEncodingInfo frm = new frmEncodingInfo(encodingInfo);
                    frm.ShowDialog();
                }
            }
            catch (Exception ex)
            {
                UIHelpers.ShowError(ex);
            }
        }

        private void _btnPauseEncoding_Click(object sender, EventArgs e)
        {
            try
            {
                Log.Info("Pause encoding.");
                if (_encodingPaused)
                {
                    _btnPauseEncoding.Image = Properties.Resources.pause_2;
                    _btnPauseEncoding.Text = "Pause Encoding";
                    _encodingPaused = false;
                    _renderTimer.Start();
                }
                else
                {
                    _btnPauseEncoding.Image = Properties.Resources.play;
                    _btnPauseEncoding.Text = "Resume Encoding";
                    _encodingPaused = true;
                    _renderTimer.Stop();
                }
            }
            catch (Exception ex)
            {
                UIHelpers.ShowError(ex);
            }
        }

        private void _renderTimer_Tick(object sender, EventArgs e)
        {
            foreach (ListViewItem item in _lstFilesForEncoding.Items)
            {
                var fileForEncoding = item.Tag as FileForEncoding;
                if (fileForEncoding.Length == 0)
                {
                    GetMediaLength(fileForEncoding);
                }
            }
            ctrlEncodingJobList1.UpdateRemainingJobTime(GetFilesForEncoding());
        }

        /// <summary>
        /// Set configuration values to controls
        /// </summary>
        private void UpdateConfiguration()
        {
            var config = BTFPVideoEncodingLibConfigurationManager.Instance.Configuration;
            
            ctrlEncodingJobList1.updateLogo(config.LogoPath);            
        }
    }

    #region TotalEncodingInfo
    public class TotalEncodingInfo
    {
        public TotalEncodingInfo(BTFPVideoEncodingLib.EncodingInfo salesEncodingInfo,
            BTFPVideoEncodingLib.EncodingInfo previewEncodingInfo,
            BTFPVideoEncodingLib.EncodingInfo judgesEncodingInfo,
            string inputFile)
        {
            SalesEncodingInfo = salesEncodingInfo;
            PreviewEncodingInfo = previewEncodingInfo;
            JudgesEncodingInfo = judgesEncodingInfo;
            InputFile = inputFile;
        }

        public BTFPVideoEncodingLib.EncodingInfo SalesEncodingInfo { get; private set; }
        public BTFPVideoEncodingLib.EncodingInfo PreviewEncodingInfo { get; private set; }
        public BTFPVideoEncodingLib.EncodingInfo JudgesEncodingInfo { get; private set; }
        public string InputFile { get; private set; }
    }
    #endregion

    #region FileForEncoding

    public class FileForEncoding
    {
        public FileForEncoding()
        {
            FileName = string.Empty;
            Length = 0;
        }
        public string FileName { get; set; }
        public double Length { get; set; }
    }
    #endregion
}
