using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace Decklink_Capture
{
    public partial class SingleTrackBar : UserControl
    {
        #region Members

        private double _min = 0;
        private double _max = 2;
        private double _value = 1;
        private double _valuePercent = 0.5;
        private Color _textColor = Color.Black;
        private int _tickFrequency = 10;
        private TrackBarScaleEnum _scaleType = TrackBarScaleEnum.Linear;
        private Point _oldLocation;
        private bool _isDragging = false;
        private Bitmap _scaledBitmap;
        private Color _tickColor = Color.Silver;
        private double _lastValue = 1;
        #endregion

        #region Events
        public event TrackBarHandler ValueChanged;
        #endregion

        public SingleTrackBar()
        {
            InitializeComponent();

            SetStyle(ControlStyles.SupportsTransparentBackColor, true);
            SetStyle(ControlStyles.ResizeRedraw, true);
            //SetStyle(ControlStyles.UserPaint | ControlStyles.AllPaintingInWmPaint | ControlStyles.DoubleBuffer, true);
            Width = pointerControl1.Width;
            pointerControl1.MouseDown += pointerControl1_MouseDown;
            pointerControl1.MouseMove += pointerControl1_MouseMove;
            pointerControl1.MouseUp += pointerControl1_MouseUp;
        }

        #region Properties

        /// <summary>
        /// Gets or sets minumum value of the track bar
        /// </summary>
        public double Min
        {
            get { return _min; }
            set
            {
                _min = value;
                CreateScaledBitmap();
                Invalidate();
            }
        }
        /// <summary>
        /// Gets or sets maximum value of the trackbar
        /// </summary>
        public double Max
        {
            get { return _max; }
            set
            {
                _max = value;
                CreateScaledBitmap();
                Invalidate();
            }
        }
        /// <summary>
        /// Gets or sets the current value of the trackbar.
        /// </summary>
        public double Value
        {
            get { return _scaleType == TrackBarScaleEnum.Linear ? _value : dBToDouble(_value); }
            set
            {
                _value = _scaleType == TrackBarScaleEnum.Linear ? value : DoubleTodB(value);
                SetValue();
            }
        }
        /// <summary>
        /// Gets or sets the tick frequency of the scale.
        /// </summary>
        public int TickFrequency
        {
            get { return _tickFrequency; }
            set
            {
                _tickFrequency = value;
                CreateScaledBitmap();
                Invalidate();
            }
        }
        /// <summary>
        /// Gets or sets the tick color.
        /// </summary>
        public Color TickColor
        {
            get { return _tickColor; }
            set
            {
                _tickColor = value;
                CreateScaledBitmap();
                Invalidate();
            }
        }

        /// <summary>
        /// Gets or sets the text color of the control.
        /// </summary>
        public Color TextColor
        {
            get { return _textColor; }
            set
            {
                _textColor = value;
                CreateScaledBitmap();
                Invalidate();
            }
        }
        /// <summary>
        /// Gets or sets the scale type of the control, i.e. double od dB
        /// </summary>
        public TrackBarScaleEnum ScaleType
        {
            get { return _scaleType; }
            set { _scaleType = value; }
        }
        #endregion

        #region Helpers
        /// <summary>
        /// Create the bitmap that represents the control.
        /// </summary>
        private void CreateScaledBitmap()
        {
            // Check for smalles dimensions
            if (Height < 10 || Width < 2)
            {
                return;
            }
            // Clear existing
            if (_scaledBitmap != null)
            {
                _scaledBitmap.Dispose();
                _scaledBitmap = null;
            }

            // Get graphics
            var graphics = CreateGraphics();
            _scaledBitmap = new Bitmap(Width, Height, graphics);
            graphics.Dispose();

            // Draw the control on the bitmap
            using (var grf = Graphics.FromImage(_scaledBitmap))
            {
                grf.Clear(Color.Transparent);

                Pen linePen = new Pen(_tickColor, 1.0f);

                float y = (float)pointerControl1.Height / 2;
                float tickWidth = (Height - pointerControl1.Height) / (float)TickFrequency;

                while (y < Height - pointerControl1.Height / 2)
                {
                    grf.DrawLine(linePen, new PointF(0, y), new PointF(pointerControl1.Width, y));
                    y += tickWidth;
                }


                linePen.Dispose();
                grf.Dispose();
            }
        }
       
        public static double dBToDouble(double dB)
        {
            return Math.Pow(10, dB / 20.0);
        }

        public static double DoubleTodB(double x)
        {
            return 20 * Math.Log10(x);
        }
        /// <summary>
        /// Set current value of the trackbar.
        /// </summary>
        private void SetValue()
        {
            _value = Math.Min(_value, _max);
            _value = Math.Max(_value, _min);
            _valuePercent = _value / (_max - _min);
            pointerControl1.Top = (int)((Height - pointerControl1.Height) * (1 - _valuePercent));
            pointerControl1.Invalidate();
        }
        /// <summary>
        /// Calculates the value from the pointer position and raises event.
        /// </summary>
        private void OnValueChanged()
        {
            _valuePercent = pointerControl1.Top / (double)(Height - pointerControl1.Height);
            _value = (_max - _min) * (1 - _valuePercent);
            double curentValue = _value;
            if (_scaleType == TrackBarScaleEnum.dB)
                curentValue = dBToDouble(curentValue);

            if (curentValue == _lastValue)
            {
                return;
            }
            if (ValueChanged != null)
                ValueChanged(this, new TrackBarEventArgs(curentValue));

            Debug.WriteLine("Value: " + _value.ToString());
            _lastValue = Value;
        }
        #endregion

        #region pointer control mouse events
        void pointerControl1_MouseUp(object sender, MouseEventArgs e)
        {
            _isDragging = false;
        }

        void pointerControl1_MouseMove(object sender, MouseEventArgs e)
        {
            if (e.Button == MouseButtons.Left && _isDragging)
            {
                int newY = pointerControl1.Top + (e.Y - _oldLocation.Y);

                if (newY < 0)
                {
                    newY = 0;
                    _isDragging = false;
                }
                else if (newY > (Height - pointerControl1.Height))
                {
                    newY = Height - pointerControl1.Height;
                    _isDragging = false;
                }

                pointerControl1.Top = newY;
                pointerControl1.Invalidate();
               
                OnValueChanged(); 
            }
        }

        void pointerControl1_MouseDown(object sender, MouseEventArgs e)
        {
            if (e.Button == MouseButtons.Left)
            {
                _oldLocation = new Point(e.X, e.Y);
                _isDragging = true;
            }
        }
        #endregion

        #region Paint

        protected override void OnPaint(PaintEventArgs e)
        {
            base.OnPaint(e);

            if (_scaledBitmap != null)
            {
                e.Graphics.DrawImage(_scaledBitmap, 0, 0, Width, Height);
            }
            Rectangle rec = new Rectangle(pointerControl1.Width / 2 - 1, pointerControl1.Height / 2, 5, (this.Height - pointerControl1.Height));
            ControlPaint.DrawBorder3D(e.Graphics, rec, Border3DStyle.Sunken, Border3DSide.All); 
        }

        protected override void OnResize(EventArgs e)
        {
            base.OnResize(e);
            CreateScaledBitmap();
            SetValue(); 
        }

        #endregion
    }

    public enum TrackBarScaleEnum
    {
        Linear,
        dB
    }
    public delegate void TrackBarHandler(object sender, TrackBarEventArgs e);

    public class TrackBarEventArgs : EventArgs
    {
        public TrackBarEventArgs(double curentValue)
        {
            CurentValue = curentValue;
        }

        public double CurentValue { get; private set; }
    }
}
