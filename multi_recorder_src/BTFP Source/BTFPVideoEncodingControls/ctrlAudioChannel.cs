using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace BTFPVideoEncodingControls
{
    public partial class ctrlAudioChannel : UserControl
    {
        #region Members
        private string _title = "1";
        private Font _fnt = new Font(new FontFamily("Microsoft Sans Serif"), 8.25f, FontStyle.Bold );
        private StringFormat _sf = new StringFormat(){Alignment = StringAlignment.Center,LineAlignment = StringAlignment.Center};
        #endregion

        public ctrlAudioChannel()
        {
            InitializeComponent();
        }

        public string Title
        {
            get { return _title; }
            set
            {
                _title = value;
                Invalidate();
            }
        }

        protected override void OnPaint(PaintEventArgs e)
        {
            Debug.WriteLine(string.Format("Draw string: {0}", Title));
            e.Graphics.DrawString(Title, _fnt, Brushes.White, new RectangleF(0, 0, Size.Width, Size.Height), _sf);
            base.OnPaint(e);
        }
    }
}
