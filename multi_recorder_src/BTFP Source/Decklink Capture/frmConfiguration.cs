using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using DecklinkCaptureLib;
using DeckLinkAPI;

namespace Decklink_Capture
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
                //_cmbInputVideoDevices.DataSource = VideoHelpers.GetDecklinkVideoInputDevices();
                //_cmbInputAudioDevices.DataSource = VideoHelpers.GetDecklinkAudioInputDevices();
                //_cmbInputVideoDevices.DataSource = VideoHelpers.GetDecklinkWdmDevices();
                _cmbInputVideoDevices.DataSource = DecklinApiWrapper.Instance.GetDecklinkDevices();
                _cmbAudioRenderer.DataSource = VideoHelpers.GetAudioRendereres();

                _cmbInputVideoDevices.Enabled = _cmbInputVideoDevices.Items.Count > 0;
                _cmbAudioRenderer.Enabled = _cmbAudioRenderer.Items.Count > 0;
                //_cmbInputAudioDevices.Enabled = _cmbInputAudioDevices.Items.Count > 0;

                ConifgurationToControls();
            }
            catch (Exception ex)
            {
                UIHelpers.ShowError(ex);
            }
        }

        private void _btnOutputFolder_Click(object sender, EventArgs e)
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

                _txtOutputFolder.Text = invoker.Path;
            }
            catch (Exception ex)
            {
                UIHelpers.ShowError(ex);
            }
        }
        /// <summary>
        /// Set configuration values to controls
        /// </summary>
        private void ConifgurationToControls()
        {
            if (_cmbInputVideoDevices.Items.Count > 0)
            {
                if (!string.IsNullOrEmpty(DecklinkCaptureLibConfigurationManager.Instance.Configuration.VideoSource))
                {
                    _cmbInputVideoDevices.SelectedItem =
                        DecklinkCaptureLibConfigurationManager.Instance.Configuration.DecklinkDevice;
                }
                else
                {
                    _cmbInputVideoDevices.SelectedIndex = 0;
                }
            }
            updateVideoModeList();

            if (_cmbVideoFormat.Items.Count > 0)
            {
                string modeName;
                
                DecklinApiWrapper.Instance.GetVideoModeName(
                        DecklinkCaptureLibConfigurationManager.Instance.Configuration.DecklinkDevice,
                        out modeName,
                        DecklinkCaptureLibConfigurationManager.Instance.Configuration.VideoWidth,
                        DecklinkCaptureLibConfigurationManager.Instance.Configuration.VideoHeight,
                        DecklinkCaptureLibConfigurationManager.Instance.Configuration.VideoAvgTimePerFrame,
                        DecklinkCaptureLibConfigurationManager.Instance.Configuration.FieldOrder);
                _cmbVideoFormat.SelectedItem = modeName;
            }

            //if (_cmbInputAudioDevices.Items.Count > 0)
            //{
            //    if (!string.IsNullOrEmpty(DecklinkCaptureLibConfigurationManager.Instance.Configuration.AudioSource))
            //    {
            //        _cmbInputAudioDevices.SelectedItem =
            //            DecklinkCaptureLibConfigurationManager.Instance.Configuration.AudioSource;
            //    }
            //    else
            //    {
            //        _cmbInputAudioDevices.SelectedIndex = 0;
            //    }
            //}

            if (_cmbAudioRenderer.Items.Count > 0)
            {
                if (!string.IsNullOrEmpty(DecklinkCaptureLibConfigurationManager.Instance.Configuration.AudioRenderer))
                {
                    _cmbAudioRenderer.SelectedItem =
                        DecklinkCaptureLibConfigurationManager.Instance.Configuration.AudioRenderer;
                }
                else
                {
                    _cmbAudioRenderer.SelectedIndex = 0;
                }
            }

            _numVideoBitrate.Value = DecklinkCaptureLibConfigurationManager.Instance.Configuration.VideoBitrate;
            _numAudioBitrate.Value = DecklinkCaptureLibConfigurationManager.Instance.Configuration.AudioBitrate;
            _txtOutputFolder.Text = DecklinkCaptureLibConfigurationManager.Instance.Configuration.OutputDirectory;
            _chkAudioPreview.Checked = DecklinkCaptureLibConfigurationManager.Instance.Configuration.AudioPreview;
            _chkInterlacedVideo.Checked = DecklinkCaptureLibConfigurationManager.Instance.Configuration.InterlacedVideo;
            //textLogoPath.Text = DecklinkCaptureLibConfigurationManager.Instance.Configuration.LogoPath;
            _cmbAudioRenderer.Enabled = _chkAudioPreview.Checked;
        }
        /// <summary>
        /// Read configuration values back from controls to configuration object.
        /// </summary>
        private void ControlsToConfiguration()
        {
            DecklinkCaptureLibConfigurationManager.Instance.Configuration.DecklinkDevice =
                _cmbInputVideoDevices.Items.Count > 0 ? _cmbInputVideoDevices.SelectedItem.ToString() : string.Empty;

            int width = 0;
            int height = 0;
            long avgFrameDuration = 0;
            _BMDFieldDominance fieldOrder = _BMDFieldDominance.bmdProgressiveFrame;
            DecklinApiWrapper.Instance.GetVideoModeProperties(DecklinkCaptureLibConfigurationManager.Instance.Configuration.DecklinkDevice, 
                _cmbVideoFormat.Items.Count > 0 ? _cmbVideoFormat.SelectedItem.ToString() : string.Empty,
                out width, out height, out avgFrameDuration, out fieldOrder);
            DecklinkCaptureLibConfigurationManager.Instance.Configuration.VideoWidth = width;
            DecklinkCaptureLibConfigurationManager.Instance.Configuration.VideoHeight = height;
            DecklinkCaptureLibConfigurationManager.Instance.Configuration.VideoAvgTimePerFrame = avgFrameDuration;
            DecklinkCaptureLibConfigurationManager.Instance.Configuration.FieldOrder = fieldOrder;

            //DecklinkCaptureLibConfigurationManager.Instance.Configuration.AudioSource =
            //    _cmbInputAudioDevices.Items.Count > 0 ? _cmbInputAudioDevices.SelectedItem.ToString() : string.Empty;
            DecklinkCaptureLibConfigurationManager.Instance.Configuration.VideoBitrate = (int)_numVideoBitrate.Value;
            DecklinkCaptureLibConfigurationManager.Instance.Configuration.AudioBitrate = (int) _numAudioBitrate.Value;
            DecklinkCaptureLibConfigurationManager.Instance.Configuration.OutputDirectory = _txtOutputFolder.Text;
            DecklinkCaptureLibConfigurationManager.Instance.Configuration.AudioPreview = _chkAudioPreview.Checked;
            DecklinkCaptureLibConfigurationManager.Instance.Configuration.AudioRenderer =
                _cmbAudioRenderer.Items.Count > 0 ? _cmbAudioRenderer.SelectedItem.ToString() : string.Empty;
            DecklinkCaptureLibConfigurationManager.Instance.Configuration.InterlacedVideo = _chkInterlacedVideo.Checked;
            //DecklinkCaptureLibConfigurationManager.Instance.Configuration.LogoPath = textLogoPath.Text;
        }

        private void frmConfiguration_FormClosing(object sender, FormClosingEventArgs e)
        {
            if (DialogResult == DialogResult.OK)
            {
                try
                {
                    ControlsToConfiguration();
                    DecklinkCaptureLibConfigurationManager.Instance.Save();
                }
                catch (Exception ex)
                {
                    UIHelpers.ShowError("Failed to save configuration.", ex);
                }
            }
        }

        private void _chkAudioPreview_CheckedChanged(object sender, EventArgs e)
        {
            _cmbAudioRenderer.Enabled = _chkAudioPreview.Checked;
        }

        private void _cmbInputVideoDevices_SelectedIndexChanged(object sender, EventArgs e)
        {
            updateVideoModeList();
        }

        private void updateVideoModeList()
        {
            if (_cmbInputVideoDevices.Items.Count == 0) return;
            _cmbVideoFormat.DataSource = DecklinApiWrapper.Instance.GetVideoModes(_cmbInputVideoDevices.SelectedItem.ToString());
        }

        private void _btnLogoPath_Click(object sender, EventArgs e)
        {
            try
            {
                // Create an instance of the open file dialog box.
                OpenFileDialog openFileDialog = new OpenFileDialog();

                // Set filter options and filter index.
                openFileDialog.Filter = "Image Files(*.BMP;*.JPG;*.PNG;*.GIF)|*.BMP;*.JPG;*.PNG;*.GIF|All files (*.*)|*.*";
                openFileDialog.FilterIndex = 1;
                openFileDialog.Multiselect = false;


                DialogInvoker invoker = new DialogInvoker(openFileDialog);
                var dr = invoker.Invoke();
                if (dr != DialogResult.OK) return;

                DecklinkCaptureLibConfigurationManager.Instance.Configuration.LogoPath = openFileDialog.FileName;
                //textLogoPath.Text = openFileDialog.FileName;
            }
            catch (Exception ex)
            {
                UIHelpers.ShowError(ex);
            }
        }
    }
}
