using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using BTFPVideoEncodingLib;
using NLog;

namespace BTFP_Video_Encoding
{
    public partial class ctrlEncodingJob : UserControl
    {
        #region Logger
        private static readonly Logger Log = LogManager.GetCurrentClassLogger();
        #endregion

        #region Members
        private BTFPVideoEncodingJob _encodingJob = new BTFPVideoEncodingJob();
        private DateTime _lastJobStartedAt = DateTime.Now;
        private FileForEncoding _fileForEncoding;
        #endregion

        #region Events
        public event JobFinishedHandler JobFinished;
        #endregion

        public ctrlEncodingJob()
        {
            InitializeComponent();
            _encodingJob.Finished += _encodingJob_Finished;
            JobStartedAt = DateTime.MinValue;
            JobFinishedAt = DateTime.MinValue;

            _lblEndTimeInfo.Text = string.Empty;
            _lblRemainingRenderTimeInfo.Text = string.Empty;
        }

        public void AddJob(FileForEncoding fileForEncdoing)
        {
            _fileForEncoding = fileForEncdoing;
            Log.Info("Add new job: \"{0}\"", fileForEncdoing.FileName);
            _lblTitle.Text = Path.GetFileName(fileForEncdoing.FileName);
            _encodingJob.Start(fileForEncdoing.FileName, Handle);
            _timerJob.Start();
            _lastJobStartedAt = DateTime.Now;
            JobStartedAt = DateTime.Now;
        }

        void _encodingJob_Finished(object sender, EventArgs e)
        {
            if (InvokeRequired)
            {
                BeginInvoke(new Action<object, EventArgs>(_encodingJob_Finished), new object[] { sender, e });
            }
            else
            {
                _timerJob.Stop();
                _prgPreview.Value = 0;
                _prgJudges.Value = 0;
                _prgSales.Value = 0;
                _encodingJob.Finished -= _encodingJob_Finished;
                JobFinishedAt = DateTime.Now;

                Log.Info("Encoding for file: \"{0}\" finished.", _encodingJob.InputFile);
                if (JobFinished != null)
                {
                    JobFinished(this,
                        new JobFinishedArgs(_encodingJob.SalesEncodingInfo, _encodingJob.PreviewEncodingInfo,
                            _encodingJob.JudgesEncodingInfo, _encodingJob.InputFile));
                }
            }
        }

        protected override void WndProc(ref Message m)
        {
            if (_encodingJob != null)
            {
                _encodingJob.HandleEvent(m);
            }
            base.WndProc(ref m);
        }

        private void _timerJob_Tick(object sender, EventArgs e)
        {
            if (_encodingJob != null)
            {
                try
                {
                    _encodingJob.UpdateProgress();

                    _prgPreview.Value = (int) _encodingJob.PreviewEncodingInfo.Progress;
                    _prgJudges.Value = (int) _encodingJob.JudgesEncodingInfo.Progress;
                    _prgSales.Value = (int) _encodingJob.SalesEncodingInfo.Progress;
                }
                catch (Exception ex)
                {
                    Log.Error(ex, "Encoding job update progress error.");
                }
            }
        }

        public void EndJob()
        {
            if (_encodingJob != null)
            {
                _encodingJob.Stop();
            }
        }

        public DateTime JobStartedAt { get; private set; }
        public DateTime JobFinishedAt { get; private set; }

        public double TotalJobLength
        {
            get
            {
                double length = 0;
                BTFPVideoEncodingLibConfiguration config =
                    BTFPVideoEncodingLibConfigurationManager.Instance.Configuration;
                if (!config.PreviewEncodingParams.SkipThisEncode)
                {
                    length += BTFPVideoTranscoder.PREVIEW_END_SECOND - BTFPVideoTranscoder.PREVIEW_START_SECOND;
                }
                if (!config.SalesEncodingParams.SkipThisEncode)
                {
                    length += _fileForEncoding.Length;
                }
                if (!config.JudgesEncodingParams.SkipThisEncode)
                {
                    length += _fileForEncoding.Length;
                }
                return length;
            }
        }

        /// <summary>
        /// Update information on how many time has left for the remaining jobs.
        /// </summary>
        /// <param name="nrOfJobsLeft"></param>
        /// <param name="avgJobLength"></param>
        public void UpdateRemainingJobTime(List<FileForEncoding> filesForEncoding, double avgTranscodingSpeed)
        {
            try
            {
                double remainingFilesEncodeTime = GetFilesTotalEncodingTime(filesForEncoding)*avgTranscodingSpeed;

                double remJobLength = (remainingFilesEncodeTime + TotalJobLength*avgTranscodingSpeed);
                var remTimeLeft = new TimeSpan(0, 0, 0, (int)remJobLength);
                DateTime estTimeToFinish = _lastJobStartedAt.Add(remTimeLeft);
                _lblEndTimeInfo.Text = string.Format("{0}", estTimeToFinish);

                double remainingCurrentFileEncodeTime = 0;
                remainingCurrentFileEncodeTime = TotalJobLength*avgTranscodingSpeed -
                                                 (DateTime.Now.Subtract(_lastJobStartedAt).TotalSeconds);
                remJobLength = remainingFilesEncodeTime + remainingCurrentFileEncodeTime;
                remTimeLeft = new TimeSpan(0, 0, 0, (int)remJobLength);
                _lblRemainingRenderTimeInfo.Text = string.Format("{0:dd\\.hh\\:mm\\:ss}", remTimeLeft);
            }
            catch
            {


            }
        }

        private double GetFilesTotalEncodingTime(List<FileForEncoding> filesForEncoding)
        {
            double totalLength = 0;
            foreach (var fileForEncoding in filesForEncoding)
            {
                BTFPVideoEncodingLibConfiguration config =
                   BTFPVideoEncodingLibConfigurationManager.Instance.Configuration;
                if (!config.PreviewEncodingParams.SkipThisEncode)
                {
                    totalLength += BTFPVideoTranscoder.PREVIEW_END_SECOND - BTFPVideoTranscoder.PREVIEW_START_SECOND;
                }
                if (!config.SalesEncodingParams.SkipThisEncode)
                {
                    totalLength += fileForEncoding.Length;
                }
                if (!config.JudgesEncodingParams.SkipThisEncode)
                {
                    totalLength += fileForEncoding.Length;
                }
            }

            return totalLength;
        }
    }

    #region JobFinishedHandler
    public class JobFinishedArgs : EventArgs
    {
        public JobFinishedArgs(BTFPVideoEncodingLib.EncodingInfo salesEncodingInfo,
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

    public delegate void JobFinishedHandler(object sender, JobFinishedArgs args);
    #endregion
}
