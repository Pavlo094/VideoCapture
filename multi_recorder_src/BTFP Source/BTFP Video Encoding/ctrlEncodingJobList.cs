using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.IO;

namespace BTFP_Video_Encoding
{
    public partial class ctrlEncodingJobList : UserControl
    {
        #region Members
        private const int CtrlStartLocationX = 3;
        private const int CtrlStartLocationY = 3;
        private const int CtrlOffsetY = 6;
        private List<double> _transcodingSpeed = new List<double>();
        private List<double> _transcodingLengths = new List<double>(); 
        #endregion

        #region Events

        public event JobFinishedHandler JobFinished;
        #endregion

        public ctrlEncodingJobList()
        {
            InitializeComponent();

            
        }

        /// <summary>
        /// Adds new encoding job for the input file. This method creates new
        /// ctrlEncodingJob and adds it to this control. ctrlEncodingJob creates new BTFPVideoEncodingJob
        /// and starts it.
        /// </summary>
        /// <param name="fileForEncoding"></param>
        public void AddNewJob(FileForEncoding fileForEncoding)
        {
            ctrlEncodingJob job = new ctrlEncodingJob();
            int x = CtrlStartLocationX;
            int y = CtrlStartLocationY;
            foreach (var ctrl in Controls)
            {
                if (ctrl is ctrlEncodingJob)
                {
                    y += ((Control)ctrl).Height + CtrlOffsetY;
                }
            }
            job.Location = new Point(x, y);
            //job.Anchor = AnchorStyles.Left | AnchorStyles.Right | AnchorStyles.Top;
            job.Dock = DockStyle.Top;
            Controls.Add(job);

            job.JobFinished += job_JobFinished;
            job.AddJob(fileForEncoding);
        }
        /// <summary>
        /// Job has finished.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="args"></param>
        void job_JobFinished(object sender, JobFinishedArgs args)
        {
            if (InvokeRequired)
            {
                BeginInvoke(new Action<object, JobFinishedArgs>(job_JobFinished), new object[] {sender, args});
            }
            else
            {
                if (Controls.Contains(sender as Control))
                {
                    ((ctrlEncodingJob)sender).JobFinished -= job_JobFinished;
                    Controls.Remove(sender as Control);

                    TimeSpan ts =
                        ((ctrlEncodingJob) sender).JobFinishedAt.Subtract(((ctrlEncodingJob) sender).JobStartedAt);
                    _transcodingSpeed.Add(ts.TotalSeconds/((ctrlEncodingJob)sender).TotalJobLength);
                    _transcodingLengths.Add(ts.TotalSeconds);
                }

                if (JobFinished != null)
                {
                    JobFinished(this, args);
                }
            }
        }
        /// <summary>
        /// Ends current running job.
        /// </summary>
        public void EndCurrentJob()
        {
            foreach (var ctrl in Controls)
            {
                if (ctrl is ctrlEncodingJob)
                {
                    ((ctrlEncodingJob) ctrl).EndJob();
                }
            }
        }
        /// <summary>
        /// Gets the number of current ongoing jobs.
        /// </summary>
        public int CurrentJobsNr
        {
            get
            {
                int count = 0;
                foreach (var ctrl in Controls)
                {
                    if (ctrl is ctrlEncodingJob)
                    {
                        count++;
                    }
                }
                return count;
            }
        }

        public double AvgTranscodingSpeed
        {
            get
            {
                if (_transcodingSpeed.Count == 0) return 0.3;

                return _transcodingSpeed.Average();
            }
        }

        public double AvgTranscodingLength
        {
            get
            {
                if (_transcodingLengths.Count == 0) return 0;
                return _transcodingLengths.Average();
            }
        }
        /// <summary>
        /// Update information on how many time has left for the remaining jobs.
        /// </summary>
        /// <param name="filesForEncoding"></param>
        public void UpdateRemainingJobTime(List<FileForEncoding> filesForEncoding)
        {
            foreach (var ctrl in Controls)
            {
                if (ctrl is ctrlEncodingJob)
                {
                    ((ctrlEncodingJob)ctrl).UpdateRemainingJobTime(filesForEncoding, AvgTranscodingSpeed);
                }
            }
        }

        internal void updateLogo(string p)
        {
            if (p.Length != 0)
            {
                //pictureBox1.Load(p);
                pictureBox1.Image = Image.FromStream(new MemoryStream(File.ReadAllBytes(p)));
            }
            else
            {
                pictureBox1.Image = BTFP_Video_Encoding.Properties.Resources.UnauthorizedUseLogo;
            }
        }
    }
}
