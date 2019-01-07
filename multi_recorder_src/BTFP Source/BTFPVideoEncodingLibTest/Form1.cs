using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Diagnostics;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using BTFPVideoEncodingLib;
using System.IO;

namespace BTFPVideoEncodingLibTest
{
    public partial class Form1 : Form
    {

        BTFPVideoTranscoder _transcodeSales = new BTFPVideoTranscoder();
        BTFPVideoTranscoder _transcodePreview = new BTFPVideoTranscoder();
        BTFPVideoTranscoder _transcodeJudges = new BTFPVideoTranscoder();
        BTFPVideoEncodingJob _encodingJob = new BTFPVideoEncodingJob();
        public Form1()
        {
            InitializeComponent();

            _encodingJob.Finished += _encodingJob_Finished;
        }

        void _encodingJob_Finished(object sender, EventArgs e)
        {
            if (InvokeRequired)
            {
                BeginInvoke(new Action<object, EventArgs>(_encodingJob_Finished), new object[] {sender, e});
            }
            else
            {
                _prgPreview.Value = 0;
                _prgJudges.Value = 0;
                _prgSales.Value = 0;
                _timerJob.Stop();
            }
        }

        private void Form1_Load(object sender, EventArgs e)
        {
            try
            {                
                _transcodeSales.WM_GRAPHNOTIFY = 0x8000 + 1;
                _transcodeSales.MsgHandle = Handle;
                _transcodeSales.Finished += _transcodeSales_Finished;

                _transcodePreview.WM_GRAPHNOTIFY = 0x8000 + 2;
                _transcodePreview.MsgHandle = Handle;
                _transcodePreview.Finished += _transcodePreview_Finished;

                _transcodeJudges.WM_GRAPHNOTIFY = 0x8000 + 3;
                _transcodeJudges.MsgHandle = Handle;
                _transcodeJudges.Finished += _transcodeJudges_Finished;

                CheckForFolders();
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.ToString());
            }
        }

        private void _btnEncodeAll_Click(object sender, EventArgs e)
        {
            try
            {
                string inputFile = _txtInput.Text;
                if (string.IsNullOrEmpty(inputFile))
                {
                    MessageBox.Show("Please specifiy input file!");
                    return;
                }

                _encodingJob.Start(inputFile, Handle);

                _prgPreview.Minimum = 0;
                _prgPreview.Maximum = 100;
                _prgSales.Minimum = 0;
                _prgSales.Maximum = 100;
                _prgJudges.Minimum = 0;
                _prgJudges.Maximum = 100;

                _timerJob.Start();
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.ToString());
            }
        }

        void _transcodeJudges_Finished(object sender, EventArgs e)
        {
            timer1.Stop();
            _transcodeJudges.Unload();
        }

        void _transcodePreview_Finished(object sender, EventArgs e)
        {
            timer1.Stop();

            _transcodePreview.Unload();
        }

        void _transcodeSales_Finished(object sender, EventArgs e)
        {
            //timer1.Stop();

            _transcodeSales.Unload();
            updatePosition();
        }

        private void CheckForFolders()
        {
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

            if(!Directory.Exists(config.SalesEncodingParams.OutputDirectory))
            {
                Directory.CreateDirectory(config.SalesEncodingParams.OutputDirectory);
            }
        }

        private void _btnInputBrowse_Click(object sender, EventArgs e)
        {

        }

        private void _btnEncodeSales_Click(object sender, EventArgs e)
        {
            StartEncoding(BTFPVideoEncodingLibConfigurationManager.Instance.Configuration.SalesEncodingParams);
        }

        private void _btnEncodePreview_Click(object sender, EventArgs e)
        {
            StartEncoding(BTFPVideoEncodingLibConfigurationManager.Instance.Configuration.PreviewEncodingParams);
        }

        private void _btnEncodeJudges_Click(object sender, EventArgs e)
        {
            StartEncoding(BTFPVideoEncodingLibConfigurationManager.Instance.Configuration.JudgesEncodingParams);
        }

        private void StartEncoding(BTFPEncodingParameters encodingParams)
        {
            try
            {
                string inputFile = _txtInput.Text;
                if (string.IsNullOrEmpty(inputFile))
                {
                    MessageBox.Show("Please specifiy input file!");
                    return;
                }

                if (encodingParams.ConfigurationName == BTFPEncodingParameters.SalesParamsName)
                {
                    _transcodeSales.Transcode(inputFile, encodingParams);
                    _prgSales.Minimum = 0;
                    _prgSales.Maximum = (int)_transcodeSales.Duration;
                }
                else if (encodingParams.ConfigurationName == BTFPEncodingParameters.PreviewParamsName)
                {
                    _transcodePreview.Transcode(inputFile, encodingParams);
                    _prgPreview.Minimum = 0;
                    _prgPreview.Maximum = (int)_transcodePreview.Duration;
                }
                else if (encodingParams.ConfigurationName == BTFPEncodingParameters.JudgesParamsName)
                {
                    _transcodeJudges.Transcode(inputFile, encodingParams);
                    _prgJudges.Minimum = 0;
                    _prgJudges.Maximum = (int)_transcodeJudges.Duration;
                } 

                timer1.Start();
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.ToString());
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
                if (m.Msg == _transcodeSales.WM_GRAPHNOTIFY)
                {
                    _transcodeSales.HandleEvent(m);
                }
                else if (m.Msg == _transcodePreview.WM_GRAPHNOTIFY)
                {
                    _transcodePreview.HandleEvent(m);
                }
                else if (m.Msg == _transcodeJudges.WM_GRAPHNOTIFY)
                {
                    _transcodeJudges.HandleEvent(m);
                }

                _encodingJob.HandleEvent(m);

                base.WndProc(ref m);
            }
            catch (Exception ex)
            {
                MessageBox.Show("WndProc Error");
            }
        }

        private void updatePosition()
        {
            if (InvokeRequired)
            {
                BeginInvoke(new Action(updatePosition));
            }
            else
            {
                if (_transcodeSales.IsRunning)
                {
                    int pos = (int)_transcodeSales.Position;
                    _prgSales.Value = pos <= _prgSales.Maximum ? pos : _prgSales.Maximum;
                }
                else
                {
                    _prgSales.Value = _prgSales.Minimum;
                    _prgSales.Update();
                }
                if (_transcodePreview.IsRunning)
                {
                    int pos = (int)_transcodePreview.Position;
                    Debug.WriteLine("Position: {0}", pos);
                    if (pos < 0) pos = 0;
                    if (pos > _prgPreview.Maximum) pos = _prgPreview.Maximum;
                    _prgPreview.Value = pos;
                }
                if (_transcodeJudges.IsRunning)
                {
                    int pos = (int)_transcodeJudges.Position;
                    Debug.WriteLine("Position: {0}", pos);
                    if (pos < 0) pos = 0;
                    if (pos > _prgJudges.Maximum) pos = _prgJudges.Maximum;
                    _prgJudges.Value = pos;
                }
            }
        }

        private void timer1_Tick(object sender, EventArgs e)
        {
            updatePosition();
        }

        private void update_timer()
        {
            _encodingJob.UpdateProgress();

            _prgPreview.Value = (int)_encodingJob.PreviewEncodingInfo.Progress;
            _prgJudges.Value = (int)_encodingJob.JudgesEncodingInfo.Progress;
            _prgSales.Value = (int)_encodingJob.SalesEncodingInfo.Progress;
        }


        private void _timerJob_Tick(object sender, EventArgs e)
        {
            if (InvokeRequired)
            {
                BeginInvoke(new Action(update_timer));
            }
            else
            {
                update_timer();
            }
        }

      
       
    }
}
