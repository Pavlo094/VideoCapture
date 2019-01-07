using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace BTFP_Video_Encoding
{
    public partial class frmEncodingInfo : Form
    {
        private TotalEncodingInfo _encodingInfo = null;
        public frmEncodingInfo(TotalEncodingInfo encodingInfo)
        {
            InitializeComponent();

            _encodingInfo = encodingInfo;
        }

        private void frmEncodingInfo_Load(object sender, EventArgs e)
        {
            try
            {
                Text = string.Format("Encoding info for file: \"{0}\"", Path.GetFileName(_encodingInfo.InputFile));

                if (!_encodingInfo.PreviewEncodingInfo.Error)
                {
                    _lblPreviewInfo.Text = @"File has been encoded successfully.";
                    _lblPreviewError.Text = string.Empty;
                    _picPreviewImage.Image = Properties.Resources.ok_check_32;
                }
                else
                {
                    _lblPreviewInfo.Text = @"File has been encoded with errors.";
                    _lblPreviewError.Text = "Error: " + _encodingInfo.PreviewEncodingInfo.ErrorInfo;
                    _picPreviewImage.Image = Properties.Resources.error_check_32;
                }

                if (!_encodingInfo.SalesEncodingInfo.Error)
                {
                    _lblSalesInfo.Text = @"File has been encoded successfully.";
                    _lblSalesError.Text = string.Empty;
                    _picSalesImage.Image = Properties.Resources.ok_check_32;
                }
                else
                {
                    _lblSalesInfo.Text = @"File has been encoded with errors.";
                    _lblSalesError.Text = "Error: " + _encodingInfo.SalesEncodingInfo.ErrorInfo;
                    _picSalesImage.Image = Properties.Resources.error_check_32;
                }

                if (!_encodingInfo.JudgesEncodingInfo.Error)
                {
                    _lblJudgesInfo.Text = @"File has been encoded successfully.";
                    _lblJudgesError.Text = string.Empty;
                    _picJudgesImage.Image = Properties.Resources.ok_check_32;
                }
                else
                {
                    _lblJudgesInfo.Text = @"File has been encoded with errors.";
                    _lblJudgesError.Text = "Error: " + _encodingInfo.SalesEncodingInfo.ErrorInfo;
                    _picJudgesImage.Image = Properties.Resources.error_check_32;
                }
            }
            catch (Exception ex)
            {
                UIHelpers.ShowError(ex);
            }
        }
    }
}
