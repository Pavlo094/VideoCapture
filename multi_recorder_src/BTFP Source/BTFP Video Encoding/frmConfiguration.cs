using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace BTFP_Video_Encoding
{
    public partial class frmConfiguration : Form
    {
        public frmConfiguration()
        {
            InitializeComponent();
        }

        private void frmConfiguration_Load(object sender, EventArgs e)
        {
            try
            {
                var config = BTFPVideoEncodingLib.BTFPVideoEncodingLibConfigurationManager.Instance.Configuration;

                _txtMonitorFolder.Text = config.DirectoryMonitor;
                _txtFinishedFolder.Text = config.DirectoryFinished;
                _txtErrorFolder.Text = config.DirectoryErrored;

                _encodingSales.SetParams(config.SalesEncodingParams);
                _encodingPreview.SetParams(config.PreviewEncodingParams);
                _encodingJudges.SetParams(config.JudgesEncodingParams);

                _numParallelJobs.Value = config.NumberOfEncodingThreads;
            }
            catch (Exception ex)
            {
                UIHelpers.ShowError(ex);
            }
        }


        private void _btnMonitorFolder_Click(object sender, EventArgs e)
        {
            var dlg = new FolderBrowserDialog();
            dlg.SelectedPath = _txtMonitorFolder.Text;
            if (dlg.ShowDialog() == DialogResult.OK)
            {
                _txtMonitorFolder.Text = dlg.SelectedPath;
            }
        }

        private void _btnFinishedFolder_Click(object sender, EventArgs e)
        {
            var dlg = new FolderBrowserDialog();
            dlg.SelectedPath = _txtFinishedFolder.Text;
            if (dlg.ShowDialog() == DialogResult.OK)
            {
                _txtFinishedFolder.Text = dlg.SelectedPath;
            }
        }

        private void _btnErrorFolder_Click(object sender, EventArgs e)
        {
            var dlg = new FolderBrowserDialog();
            dlg.SelectedPath = _txtErrorFolder.Text;
            if (dlg.ShowDialog() == DialogResult.OK)
            {
                _txtErrorFolder.Text = dlg.SelectedPath;
            }
        }

        private void frmConfiguration_FormClosing(object sender, FormClosingEventArgs e)
        {
            if (DialogResult == DialogResult.OK)
            {
                var config = BTFPVideoEncodingLib.BTFPVideoEncodingLibConfigurationManager.Instance.Configuration;

                _encodingJudges.ReadParams();
                _encodingPreview.ReadParams();
                _encodingSales.ReadParams();

                config.DirectoryErrored = _txtErrorFolder.Text;
                config.DirectoryFinished = _txtFinishedFolder.Text;
                config.DirectoryMonitor = _txtMonitorFolder.Text;

                config.NumberOfEncodingThreads = (int)_numParallelJobs.Value;

                BTFPVideoEncodingLib.BTFPVideoEncodingLibConfigurationManager.Instance.Save();
            }
        }

        
    }
}
