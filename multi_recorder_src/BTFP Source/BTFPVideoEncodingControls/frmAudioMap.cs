using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using BTFPVideoEncodingLib;

namespace BTFPVideoEncodingControls
{
    public partial class frmAudioMap : Form
    {
        private BTFPEncodingParameters _encodingParams;
        public frmAudioMap(BTFPEncodingParameters encodingParams)
        {
            InitializeComponent();

            _encodingParams = encodingParams;
            ctrlAudioChannelsMap1.Initialise(_encodingParams);
        }

        private void frmAudioMap_FormClosed(object sender, FormClosedEventArgs e)
        {
            if (DialogResult == DialogResult.OK)
            {
                ctrlAudioChannelsMap1.ApplyChange();
            }
        }
    }
}
