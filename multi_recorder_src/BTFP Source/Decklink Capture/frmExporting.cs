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
    public partial class frmExporting : Form
    {
        #region Logger
        private static readonly Logger Log = LogManager.GetCurrentClassLogger();
        #endregion

        public frmExporting()
        {
            InitializeComponent();
        }

        private void frmExporting_Load(object sender, EventArgs e)
        {
            _lblInfo.Text = string.Format("Exporting to \"{0}\"", Path.GetFileName(BTFPTrimmer.Instance.OutputFile));
            progressBar1.Minimum = (int)BTFPTrimmer.Instance.MarkIn;
            progressBar1.Maximum = (int) BTFPTrimmer.Instance.MarkOut;

            BTFPTrimmer.Instance.Finished += Instance_Finished;
            timer1.Start();
        }

        void Instance_Finished(object sender, EventArgs e)
        {
            if (InvokeRequired)
            {
                BeginInvoke(new Action<object, EventArgs>(Instance_Finished), new object[] {sender, e});
            }
            else
            {
                Log.Info("Export Finished.");
                BTFPTrimmer.Instance.Stop();
                Close();
            }
        }

        private void _btnCancel_Click(object sender, EventArgs e)
        {
            Close();
        }

        private void frmExporting_FormClosing(object sender, FormClosingEventArgs e)
        {
            timer1.Stop();
            BTFPTrimmer.Instance.Finished -= Instance_Finished;
            BTFPTrimmer.Instance.Stop();
        }

        private void timer1_Tick(object sender, EventArgs e)
        {
            try
            {
                int pos = (int) BTFPTrimmer.Instance.Position;
                if (pos <= progressBar1.Maximum)
                {
                    progressBar1.Value = pos;
                }
                else
                {
                    Log.Info("Current Position: {0} > Maximum: {1}", pos, progressBar1.Maximum);
                }
            }
            catch (Exception)
            {
                
                throw;
            }
        }
    }
}
