using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using BTFPVideoEncodingControls;
using BTFPVideoEncodingLib;

namespace BTFP_Video_Encoding
{
    public partial class ctrlEncodingParams : UserControl
    {
        private BTFPEncodingParameters _params = null;
        private bool _skipVideoTranscodingOption = false;

        public ctrlEncodingParams()
        {
            InitializeComponent();

            _cmbVolumeBoost.Items.Add(-12);
            _cmbVolumeBoost.Items.Add(-9);
            _cmbVolumeBoost.Items.Add(-6);
            _cmbVolumeBoost.Items.Add(-3);
            _cmbVolumeBoost.Items.Add(0);
            _cmbVolumeBoost.Items.Add(3);
            _cmbVolumeBoost.Items.Add(6);
            _cmbVolumeBoost.Items.Add(9);
            _cmbVolumeBoost.Items.Add(12);
        }

        private void _chkBypassEncoding_CheckedChanged(object sender, EventArgs e)
        {
            _grpParameters.Enabled = !_chkBypassEncoding.Checked;
        }

        public string Title
        {
            get { return _grpParameters.Text; }
            set { _grpParameters.Text = value; }
        }

        public bool SkipVideoTranscodingOption
        {
            get { return _skipVideoTranscodingOption; }
            set
            {
                _chkSkipVideoTranscode.Visible = value;
                _skipVideoTranscodingOption = value;

            }
        }
        /// <summary>
        /// Set params, from configuration class to controls.
        /// </summary>
        /// <param name="parameters"></param>
        public void SetParams(BTFPEncodingParameters parameters)
        {
            _params = parameters;

            if (_params != null)
            {
                _numVideoBitrate.Value = _params.VideoBitrate;
                _numAudioBitrate.Value = _params.AudioBitrate;
                _numResolutionWidth.Value = _params.VideoWidth;
                _numResolutionHeight.Value = _params.VideoHeight;
                _cmbVolumeBoost.SelectedItem = _params.AudioVolumeBoost;
                _txtOutputFolder.Text = _params.OutputDirectory;
                _txtFilenameAppendix.Text = _params.FileNameAppendix;
                _chkInterlacedVideo.Checked = _params.InterlacedVideo;

                _chkBypassEncoding.Checked = _params.SkipThisEncode;
                if (SkipVideoTranscodingOption)
                {
                    _chkSkipVideoTranscode.Checked = _params.SkipVideoTranscode;
                }
            }
        }
        /// <summary>
        /// Read parameters from controls to BTFPEncodingParameters instance passed in SetParams function.
        /// </summary>
        public void ReadParams()
        {
            if (_params == null) return;

            _params.VideoBitrate = (int)_numVideoBitrate.Value;
            _params.AudioBitrate = (int) _numAudioBitrate.Value;
            _params.VideoWidth = (int) _numResolutionWidth.Value;
            _params.VideoHeight = (int) _numResolutionHeight.Value;
            _params.AudioVolumeBoost = (int)_cmbVolumeBoost.SelectedItem;
            _params.OutputDirectory = _txtOutputFolder.Text;
            _params.FileNameAppendix = _txtFilenameAppendix.Text;
            _params.SkipThisEncode = _chkBypassEncoding.Checked;
            _params.InterlacedVideo = _chkInterlacedVideo.Checked;
            if (SkipVideoTranscodingOption)
            {
                _params.SkipVideoTranscode = _chkSkipVideoTranscode.Checked;
            }
        }

        private void _btnOutputFolder_Click(object sender, EventArgs e)
        {
            var dlg = new FolderBrowserDialog();
            if (dlg.ShowDialog() == DialogResult.OK)
            {
                _txtOutputFolder.Text = dlg.SelectedPath;
            }
        }

        private void _chkSkipVideoTranscode_CheckedChanged(object sender, EventArgs e)
        {
            _lblVideoBitrate.Enabled = !_chkSkipVideoTranscode.Checked;
            _numVideoBitrate.Enabled = !_chkSkipVideoTranscode.Checked;
            _lblVideoBitrateKbps.Enabled = !_chkSkipVideoTranscode.Checked;
            _lblVideoResolution.Enabled = !_chkSkipVideoTranscode.Checked;
            _numResolutionWidth.Enabled = !_chkSkipVideoTranscode.Checked;
            _lblVideoResolutionX.Enabled = !_chkSkipVideoTranscode.Checked;
            _numResolutionHeight.Enabled = !_chkSkipVideoTranscode.Checked;
            _chkInterlacedVideo.Enabled = !_chkSkipVideoTranscode.Checked;
        }

        private void _btnAudioMap_Click(object sender, EventArgs e)
        {
            frmAudioMap frm = new frmAudioMap(_params);
            if (frm.ShowDialog() == DialogResult.OK)
            {
                
            }
        }
    }
}
