using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using BTFPVideoEncodingLib;

namespace BTFPVideoEncodingControls
{
    public partial class ctrlAudioChannelsMap : UserControl
    {
        #region Members

        private BTFPEncodingParameters _encodingParams = null;
        private readonly int[,] _audioChannelsMap = new int[6,6];
        private readonly Pen _linePen = new Pen(Color.White, 2f);
        private bool _mouseDown = false;
        private PointF _linePoint = new PointF();
        private int _currentInputIndex = -1;
        private readonly List<AudioChannelBox> _inputChannels = new List<AudioChannelBox>();
        private readonly List<AudioChannelBox> _outputChannels = new List<AudioChannelBox>(); 
        #endregion

        #region Constructor
        public ctrlAudioChannelsMap()
        {
            InitializeComponent();

            DoubleBuffered = true;

            for (int i = 0; i < 6; i++)
            {
                int channelNr = i + 1;

                var inputChannel = new AudioChannelBox
                {
                    Location = new Point(15, 31 + i*34),
                    Size = new Size(30, 28),
                    Title = channelNr.ToString()
                };
                _inputChannels.Add(inputChannel);

                var outputChannel = new AudioChannelBox
                {
                    Location = new Point(152, 31 + i*34),
                    Size = new Size(30, 28),
                    Title = channelNr.ToString()
                };
                _outputChannels.Add(outputChannel);
            }
        }
        #endregion

        public void Initialise(BTFPEncodingParameters encodingParams)
        {
            _encodingParams = encodingParams;
            for (int i = 0; i < 6; i++)
            {
                for (int j = 0; j < 6; j++)
                {
                    _audioChannelsMap[i, j] = _encodingParams.AudioChannelsMap[i, j];
                }
            }

            this.Invalidate();
        }

        public void ApplyChange()
        {
            for (int i = 0; i < 6; i++)
            {
                for (int j = 0; j < 6; j++)
                {
                    _encodingParams.AudioChannelsMap[i, j] = _audioChannelsMap[i, j];
                }
            }
        }

        protected override void OnPaint(PaintEventArgs e)
        {
            for (int i = 0; i < 6; i++)
            {
                for (int j = 0; j < 6; j++)
                {
                    if (_audioChannelsMap[i, j] == 1)
                    {
                        var fromPoint = _inputChannels[i].OutputConnectionLocation;
                        var toPoint = _outputChannels[j].InputConnectionLocation;

                        e.Graphics.DrawLine(_linePen, fromPoint, toPoint);
                    }
                }
            }

            _inputChannels.ForEach(c => c.Draw(e.Graphics));
            _outputChannels.ForEach(c => c.Draw(e.Graphics));

            if (_mouseDown)
            {
                var mousePoint = PointToClient(Cursor.Position);

                e.Graphics.DrawLine(_linePen, _linePoint, mousePoint);
            }

            base.OnPaint(e);
        }

        private void ctrlAudioChannelsMap_MouseDown(object sender, MouseEventArgs e)
        {
            if (!(_inputChannels.Any(c => c.HitTest(e.Location))))
            {
                return;
            }

            if (_encodingParams.ConfigurationName == BTFPEncodingParameters.PreviewParamsName ||
                _encodingParams.ConfigurationName == BTFPEncodingParameters.SalesParamsName)
            {
                if (!(_inputChannels[0].HitTest(e.Location)) && !(_inputChannels[1].HitTest(e.Location)))
                {
                    return;
                }
            }
            if (_encodingParams.ConfigurationName == BTFPEncodingParameters.JudgesParamsName)
            {
                if (_inputChannels[0].HitTest(e.Location) || _inputChannels[1].HitTest(e.Location))
                {
                    return;
                }
            }

            int inputIndex = _inputChannels.FindIndex(c => c.HitTest(e.Location));

            for (int i = 0; i < 6; i++)
            {
                _audioChannelsMap[inputIndex, i] = 0;
            }

            _mouseDown = true;
            _linePoint = _inputChannels[inputIndex].OutputConnectionLocation;
            _currentInputIndex = inputIndex;

            Invalidate();
        }
        private void ctrlAudioChannelsMap_MouseMove(object sender, MouseEventArgs e)
        {
            if (_inputChannels.Any(c => c.HitTest(e.Location)))
            {
                Cursor = Cursors.Hand;
            }
            else
            {
                Cursor = Cursors.Arrow;
            }
            if (!_mouseDown) return;
            Invalidate();
        }
        private void ctrlAudioChannelsMap_MouseUp(object sender, MouseEventArgs e)
        {
            Debug.WriteLine("ctrlAudioChannelsMap_MouseUp");
            _mouseDown = false;
            Invalidate();

            _mouseDown = false;
            _linePoint = new PointF();
            if (_outputChannels.Any(c => c.HitTest(e.Location)))
            {
                int outIndex = _outputChannels.FindIndex(c => c.HitTest(e.Location));

                if (_encodingParams.ConfigurationName == BTFPEncodingParameters.PreviewParamsName ||
                    _encodingParams.ConfigurationName == BTFPEncodingParameters.SalesParamsName)
                {
                    if (outIndex != 0 && outIndex != 1)
                    {
                        return;
                    }
                }
                if (_encodingParams.ConfigurationName == BTFPEncodingParameters.JudgesParamsName)
                {
                    if (outIndex == 0 || outIndex == 1)
                    {
                        return;
                    }
                }


                if (_currentInputIndex > -1 && outIndex > -1)
                {
                    for (int i = 0; i < 6; i++)
                    {
                        if (_audioChannelsMap[i, outIndex] == 1)
                        {
                            _audioChannelsMap[i, outIndex] = 0;
                        }
                    }
                    _audioChannelsMap[_currentInputIndex, outIndex] = 1;
                    _currentInputIndex = -1;
                }
            }

            Invalidate();
        }
    }

    #region AudioChannelBox
    public class AudioChannelBox
    {
        #region Members
        private readonly Brush _rectBrush = Brushes.Black;
        private Font _fnt = new Font(new FontFamily("Microsoft Sans Serif"), 8.25f, FontStyle.Bold);
        private StringFormat _sf = new StringFormat() { Alignment = StringAlignment.Center, LineAlignment = StringAlignment.Center };
        #endregion

        #region Constructor

        public AudioChannelBox()
        {
            Location = new Point();
            Size = new Size();
            Title = string.Empty;
        }
        #endregion

        #region Properties
        public Point Location { get; set; }
        public Size Size { get; set; }
        public string Title { get; set; }

        public Point OutputConnectionLocation
        {
            get
            {
                return new Point(Location.X + Size.Width, Location.Y + Size.Height/2);
            }
        }

        public Point InputConnectionLocation
        {
            get
            {
                return new Point(Location.X, Location.Y + Size.Height/2);
            }
        }
        #endregion

        #region Methods

        public void Draw(Graphics g)
        {
            g.FillRectangle(_rectBrush, new Rectangle(Location, Size));
            g.DrawString(Title, _fnt, Brushes.White, new RectangleF(Location.X, Location.Y, Size.Width, Size.Height),
                _sf);
        }

        public bool HitTest(Point pt)
        {
            var rct = new Rectangle(Location, Size);
            return rct.Contains(pt);
        }
        #endregion
    }
    #endregion
}
