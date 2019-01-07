using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace Decklink_Capture
{
    public partial class SixChannelTrackbar : UserControl
    {
        #region Events
        public event SixChannelsTrackBarHandler ValueChanged;
        #endregion

        #region Constructor
        public SixChannelTrackbar()
        {
            InitializeComponent();
            ChangeBackColor();
            ChangeLinkBackColor();
        }
        #endregion

        private void singleTrackBar1_ValueChanged(object sender, TrackBarEventArgs e)
        {
            if (_chkLink12.Checked)
            {
                if (sender == singleTrackBar1)
                {
                    singleTrackBar2.Value = singleTrackBar1.Value;
                }
                else if (sender == singleTrackBar2)
                {
                    singleTrackBar1.Value = singleTrackBar2.Value;
                }
            }
            if (_chkLink34.Checked)
            {
                if (sender == singleTrackBar3)
                {
                    singleTrackBar4.Value = singleTrackBar3.Value;
                }
                else if (sender == singleTrackBar4)
                {
                    singleTrackBar3.Value = singleTrackBar4.Value;
                }
            }
            if (_chkLink56.Checked)
            {
                if (sender == singleTrackBar5)
                {
                    singleTrackBar6.Value = singleTrackBar5.Value;
                }
                else if (sender == singleTrackBar6)
                {
                    singleTrackBar5.Value = singleTrackBar6.Value;
                }
            }
            if (ValueChanged != null)
            {
                ValueChanged(this, new SixChannelsTrackBarEventArgs(GetVolumeValues(), GetCheckedChannels(), checkBox1.Checked));
            }
        }
        /// <summary>
        /// Get current volume track bar values.
        /// </summary>
        /// <returns></returns>
        public double[] GetVolumeValues()
        {
            var values = new double[8];
            values[0] = singleTrackBar1.Value;
            values[1] = singleTrackBar2.Value;
            values[2] = singleTrackBar3.Value;
            values[3] = singleTrackBar4.Value;
            values[4] = singleTrackBar5.Value;
            values[5] = singleTrackBar6.Value;
            values[6] = 0;
            values[7] = 0;

            return values;
        }

        private void _chkDisableChannel1_CheckedChanged(object sender, EventArgs e)
        {
            if (!_updateCheck)
                return;

            ChangeBackColor();
            if (ValueChanged != null)
            {
                ValueChanged(this, new SixChannelsTrackBarEventArgs(GetVolumeValues(), GetCheckedChannels(), checkBox1.Checked));
            }
        }

        private void ChangeBackColor()
        {
            _chkEnableChannel1.BackColor = _chkEnableChannel1.Checked ? Color.DeepSkyBlue : Color.Gray;
            _chkEnableChannel2.BackColor = _chkEnableChannel2.Checked ? Color.DeepSkyBlue : Color.Gray;
            _chkEnableChannel3.BackColor = _chkEnableChannel3.Checked ? Color.DeepSkyBlue : Color.Gray;
            _chkEnableChannel4.BackColor = _chkEnableChannel4.Checked ? Color.DeepSkyBlue : Color.Gray;
            _chkEnableChannel5.BackColor = _chkEnableChannel5.Checked ? Color.DeepSkyBlue : Color.Gray;
            _chkEnableChannel6.BackColor = _chkEnableChannel6.Checked ? Color.DeepSkyBlue : Color.Gray;
            
            
        }

        private void ChangeLinkBackColor()
        {
            _chkLink12.BackColor = _chkLink12.Checked ? Color.DeepSkyBlue : Color.Gray;
            _chkLink34.BackColor = _chkLink34.Checked ? Color.DeepSkyBlue : Color.Gray;
            _chkLink56.BackColor = _chkLink56.Checked ? Color.DeepSkyBlue : Color.Gray;
        }
        /// <summary>
        /// Get list of all the channels with info if they're checked or not.
        /// </summary>
        /// <returns></returns>
        public bool[] GetCheckedChannels()
        {
            var values = new bool[8];
            values[0] = _chkEnableChannel1.Checked;
            values[1] = _chkEnableChannel2.Checked;
            values[2] = _chkEnableChannel3.Checked;
            values[3] = _chkEnableChannel4.Checked;
            values[4] = _chkEnableChannel5.Checked;
            values[5] = _chkEnableChannel6.Checked;
            values[6] = false;
            values[7] = false;

            return values;
        }

        private void _chkLink12_CheckedChanged(object sender, EventArgs e)
        {
            ChangeLinkBackColor();
        }

        private bool _updateCheck = true;
        private void checkBox1_CheckedChanged(object sender, EventArgs e)
        {
            _updateCheck = false;
            bool check = checkBox1.Checked;
            _chkEnableChannel1.Enabled = !check;
            _chkEnableChannel2.Enabled = !check;
            _chkEnableChannel3.Enabled = !check;
            _chkEnableChannel4.Enabled = !check;
            _chkEnableChannel5.Enabled = !check;
            _chkEnableChannel6.Enabled = !check;
            _updateCheck = true;

            checkBox1.BackColor = checkBox1.Checked ? Color.DeepSkyBlue : Color.Gray;

            //ChangeBackColor();
            if (ValueChanged != null)
            {
                ValueChanged(this, new SixChannelsTrackBarEventArgs(GetVolumeValues(), GetCheckedChannels(), checkBox1.Checked));
            }
        }

        public bool MuteAll{get { return checkBox1.Checked; }}
    }

    #region EventHandlers & Args
    public delegate void SixChannelsTrackBarHandler(object sender, SixChannelsTrackBarEventArgs e);

    public class SixChannelsTrackBarEventArgs : EventArgs
    {
        public SixChannelsTrackBarEventArgs(double[] volumes, bool[] checkedChannels, bool muteAll)
        {
            Volumes = volumes;
            CheckedChannels = checkedChannels;
            MuteAll = muteAll;
        }

        public double[] Volumes { get; private set; }
        public bool[] CheckedChannels { get; private set; }
        public bool MuteAll { get; private set; }
    }
    #endregion
}
