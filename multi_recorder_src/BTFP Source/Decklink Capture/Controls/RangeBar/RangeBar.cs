using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Text;
using System.Windows.Forms;
using System.Drawing.Drawing2D;

namespace Decklink_Capture
{
    public class RangeBar : UserControl
    {
        // Fields
        private int _actualWidth = 0;
        private float _myPointerPos = 0f;
        private int _scrollValue = 0;
        private ActiveMarkType ActiveMark = ActiveMarkType.none;
        private int BarHeight = 18;
        private Color colorInner = Color.LightGreen;
        private Color colorRange = Color.Black;
        private Color colorShadowDark = Color.FromKnownColor(KnownColor.ControlDarkDark);
        private Color colorShadowLight = Color.FromKnownColor(KnownColor.ControlLightLight);
        private Container components = null;
        private Point[] LMarkPnt = new Point[5];
        private bool m_DrawPointer = false;
        private bool m_MovePointer = false;
        private int m_PointerPosition = 0;
        private RectangleF m_PointerRect = new RectangleF();
        private int MarkHeight = 0x18;
        private int MarkWidth = 8;
        private double Maximum = 10.0;
        private double Minimum = 0.0;
        private bool MoveLMark = false;
        private bool MoveRMark = false;
        private int numAxisDivision = 10;
        private RangeBarOrientation orientationBar = RangeBarOrientation.horizontal;
        private TopBottomOrientation orientationScale = TopBottomOrientation.bottom;
        private int PosL = 0;
        private int PosR = 0;
        private double rangeMax = 5.0;
        private double rangeMin = 3.0;
        private Point[] RMarkPnt = new Point[5];
        private int sizeShadow = 1;
        private int TickHeight = 6;
        private int XPosMax;
        private int XPosMin;

        // Events
        public event ActualWidthChangedEventHandler ActualWidthChanged;

        public event RangeChangedEventHandler LeftRangeChanging;

        public event EventHandler PointerPositionChanged;

        public event RangeChangedEventHandler RangeChanged;

        public event RangeChangedEventHandler RangeChanging;

        public event RangeChangedEventHandler RightRangeChanging;

        public event ScrollEventHandler Scrolling;

        // Methods
        public RangeBar()
        {
            this.InitializeComponent();
            this._actualWidth = base.Width;
        }

        protected override void Dispose(bool disposing)
        {
            if (disposing && (this.components != null))
            {
                this.components.Dispose();
            }
            base.Dispose(disposing);
        }

        public void DoScroll(int scrollValue)
        {
            this._scrollValue = scrollValue;
            float pos = ((float)(((double)(this._actualWidth * this._myPointerPos)) / (this.Maximum - this.Minimum))) - this._scrollValue;
            this.m_PointerRect = new RectangleF(pos, this.m_PointerRect.Y, this.m_PointerRect.Width, this.m_PointerRect.Height);
            Range2Pos();
            base.Invalidate(true);
        }

        private void InitializeComponent()
        {
            base.SuspendLayout();
            base.Name = "RangeBar";
            base.Size = new Size(0x158, 0x40);
            base.Load += new EventHandler(this.OnLoad);
            base.Paint += new PaintEventHandler(this.OnPaint);
            base.MouseMove += new MouseEventHandler(this.OnMouseMove);
            base.Leave += new EventHandler(this.OnLeave);
            base.MouseDown += new MouseEventHandler(this.OnMouseDown);
            base.Resize += new EventHandler(this.OnResize);
            base.KeyPress += new KeyPressEventHandler(this.OnKeyPress);
            base.MouseUp += new MouseEventHandler(this.OnMouseUp);
            base.SizeChanged += new EventHandler(this.OnSizeChanged);
            base.ResumeLayout(false);
        }

        private void OnKeyPress(object sender, KeyPressEventArgs e)
        {
            if (base.Enabled)
            {
                if (this.ActiveMark == ActiveMarkType.left)
                {
                    if (e.KeyChar == '+')
                    {
                        this.rangeMin++;
                        if (this.rangeMin > this.Maximum)
                        {
                            this.rangeMin = this.Maximum;
                        }
                        if (this.rangeMax < this.rangeMin)
                        {
                            this.rangeMax = this.rangeMin;
                        }
                        this.OnLeftRangeChanging(EventArgs.Empty);
                    }
                    else if (e.KeyChar == '-')
                    {
                        this.rangeMin--;
                        if (this.rangeMin < this.Minimum)
                        {
                            this.rangeMin = this.Minimum;
                        }
                        this.OnLeftRangeChanging(EventArgs.Empty);
                    }
                }
                else if (this.ActiveMark == ActiveMarkType.right)
                {
                    if (e.KeyChar == '+')
                    {
                        this.rangeMax++;
                        if (this.rangeMax > this.Maximum)
                        {
                            this.rangeMax = this.Maximum;
                        }
                        this.OnRightRangeChanging(EventArgs.Empty);
                    }
                    else if (e.KeyChar == '-')
                    {
                        this.rangeMax--;
                        if (this.rangeMax < this.Minimum)
                        {
                            this.rangeMax = this.Minimum;
                        }
                        if (this.rangeMax < this.rangeMin)
                        {
                            this.rangeMin = this.rangeMax;
                        }
                        this.OnRightRangeChanging(EventArgs.Empty);
                    }
                }
                base.Invalidate(true);
            }
        }

        private void OnLeave(object sender, EventArgs e)
        {
            this.ActiveMark = ActiveMarkType.none;
        }

        public virtual void OnLeftRangeChanging(EventArgs e)
        {
            if (this.LeftRangeChanging != null)
            {
                this.LeftRangeChanging(this, e);
            }
        }

        private void OnLoad(object sender, EventArgs e)
        {
            base.SetStyle(ControlStyles.DoubleBuffer, true);
            base.SetStyle(ControlStyles.AllPaintingInWmPaint, true);
            base.SetStyle(ControlStyles.UserPaint, true);
        }

        private void OnMouseDown(object sender, MouseEventArgs e)
        {
            if (base.Enabled)
            {
                Rectangle LMarkRect = new Rectangle(Math.Min(this.LMarkPnt[0].X, this.LMarkPnt[1].X), Math.Min(this.LMarkPnt[0].Y, this.LMarkPnt[3].Y), Math.Abs((int)(this.LMarkPnt[2].X - this.LMarkPnt[0].X)), Math.Max(Math.Abs((int)(this.LMarkPnt[0].Y - this.LMarkPnt[3].Y)), Math.Abs((int)(this.LMarkPnt[0].Y - this.LMarkPnt[1].Y))));
                Rectangle RMarkRect = new Rectangle(Math.Min(this.RMarkPnt[0].X, this.RMarkPnt[2].X), Math.Min(this.RMarkPnt[0].Y, this.RMarkPnt[1].Y), Math.Abs((int)(this.RMarkPnt[0].X - this.RMarkPnt[2].X)), Math.Max(Math.Abs((int)(this.RMarkPnt[2].Y - this.RMarkPnt[0].Y)), Math.Abs((int)(this.RMarkPnt[1].Y - this.RMarkPnt[0].Y))));
                if (LMarkRect.Contains(e.X, e.Y))
                {
                    base.Capture = true;
                    this.MoveLMark = true;
                    this.ActiveMark = ActiveMarkType.left;
                    base.Invalidate(true);
                }
                if (RMarkRect.Contains(e.X, e.Y))
                {
                    base.Capture = true;
                    this.MoveRMark = true;
                    this.ActiveMark = ActiveMarkType.right;
                    base.Invalidate(true);
                }
                if (this.m_PointerRect.Contains((float)e.X, (float)e.Y))
                {
                    base.Capture = true;
                    this.m_MovePointer = true;
                    base.Invalidate(true);
                }
            }
        }

        private void OnMouseMove(object sender, MouseEventArgs e)
        {
            if (base.Enabled)
            {
                int h = base.Height;
                int w = this._actualWidth;
                double r1 = (this.rangeMin * w) / (this.Maximum - this.Minimum);
                double r2 = (this.rangeMax * w) / (this.Maximum - this.Minimum);
                Rectangle LMarkRect = new Rectangle(Math.Min(this.LMarkPnt[0].X, this.LMarkPnt[1].X), Math.Min(this.LMarkPnt[0].Y, this.LMarkPnt[3].Y), Math.Abs((int)(this.LMarkPnt[2].X - this.LMarkPnt[0].X)), Math.Max(Math.Abs((int)(this.LMarkPnt[0].Y - this.LMarkPnt[3].Y)), Math.Abs((int)(this.LMarkPnt[0].Y - this.LMarkPnt[1].Y))));
                Rectangle RMarkRect = new Rectangle(Math.Min(this.RMarkPnt[0].X, this.RMarkPnt[2].X), Math.Min(this.RMarkPnt[0].Y, this.RMarkPnt[1].Y), Math.Abs((int)(this.RMarkPnt[0].X - this.RMarkPnt[2].X)), Math.Max(Math.Abs((int)(this.RMarkPnt[2].Y - this.RMarkPnt[0].Y)), Math.Abs((int)(this.RMarkPnt[1].Y - this.RMarkPnt[0].Y))));
                if (LMarkRect.Contains(e.X, e.Y) || RMarkRect.Contains(e.X, e.Y))
                {
                    if (this.orientationBar == RangeBarOrientation.horizontal)
                    {
                        this.Cursor = Cursors.SizeWE;
                    }
                    else
                    {
                        this.Cursor = Cursors.SizeNS;
                    }
                }
                else if (this.m_PointerRect.Contains((float)e.X, (float)e.Y))
                {
                    this.Cursor = Cursors.Hand;
                }
                else
                {
                    this.Cursor = Cursors.Arrow;
                }
                if (this.MoveLMark)
                {
                    if (this.orientationBar == RangeBarOrientation.horizontal)
                    {
                        this.Cursor = Cursors.SizeWE;
                    }
                    else
                    {
                        this.Cursor = Cursors.SizeNS;
                    }
                    if (this.orientationBar == RangeBarOrientation.horizontal)
                    {
                        this.PosL = e.X;
                    }
                    else
                    {
                        this.PosL = e.Y;
                    }
                    if ((this.PosL + this._scrollValue) < this.XPosMin)
                    {
                        this.PosL = this.XPosMin;
                    }
                    if (this.PosL > this.XPosMax)
                    {
                        this.PosL = this.XPosMax - this._scrollValue;
                    }
                    if (this.PosR < (this.PosL + this._scrollValue))
                    {
                        this.PosR = this.PosL;
                        this.Pos2RangeRight();
                    }
                    if (this.PosL > base.Width)
                    {
                        if (this.Scrolling != null)
                        {
                            this.Scrolling(this, 50);
                        }
                    }
                    else if ((this.PosL < 0) && (this.Scrolling != null))
                    {
                        this.Scrolling(this, -50);
                    }
                    this.Pos2RangeLeft();
                    this.ActiveMark = ActiveMarkType.left;
                    base.Invalidate(true);
                    this.OnLeftRangeChanging(EventArgs.Empty);
                }
                else if (this.MoveRMark)
                {
                    if (this.orientationBar == RangeBarOrientation.horizontal)
                    {
                        this.Cursor = Cursors.SizeWE;
                    }
                    else
                    {
                        this.Cursor = Cursors.SizeNS;
                    }
                    if (this.orientationBar == RangeBarOrientation.horizontal)
                    {
                        this.PosR = e.X;
                    }
                    else
                    {
                        this.PosR = e.Y;
                    }
                    if (this.PosR > this.XPosMax)
                    {
                        this.PosR = this.XPosMax;
                    }
                    if ((this.PosR + this._scrollValue) < this.XPosMin)
                    {
                        this.PosR = this.XPosMin;
                    }
                    if (this.PosL > (this.PosR + this._scrollValue))
                    {
                        this.PosL = this.PosR;
                        this.Pos2RangeLeft();
                    }
                    if (this.PosR > base.Width)
                    {
                        if (this.Scrolling != null)
                        {
                            this.Scrolling(this, 20);
                        }
                    }
                    else if ((this.PosR < 0) && (this.Scrolling != null))
                    {
                        this.Scrolling(this, -20);
                    }
                    this.Pos2RangeRight();
                    this.ActiveMark = ActiveMarkType.right;
                    base.Invalidate(true);
                    this.OnRightRangeChanging(EventArgs.Empty);
                }
                else if (this.m_MovePointer && (this.orientationBar == RangeBarOrientation.horizontal))
                {
                    float pos = e.X;
                    if (pos < 0f)
                    {
                        pos = 0f;
                    }
                    if (pos > (this._actualWidth - this.m_PointerRect.Width))
                    {
                        pos = this._actualWidth - this.m_PointerRect.Width;
                    }
                    this.m_PointerPosition = (int)(((this.Maximum - this.Minimum) / ((double)this._actualWidth)) * (pos + this._scrollValue));
                    this._myPointerPos = (float)(((this.Maximum - this.Minimum) / ((double)this._actualWidth)) * (pos + this._scrollValue));
                    this.m_PointerRect = new RectangleF(pos, this.m_PointerRect.Y, this.m_PointerRect.Width, this.m_PointerRect.Height);
                    if (this.PointerPositionChanged != null)
                    {
                        this.PointerPositionChanged(this, EventArgs.Empty);
                    }
                    base.Invalidate(true);
                }
            }
        }

        private void OnMouseUp(object sender, MouseEventArgs e)
        {
            if (base.Enabled)
            {
                base.Capture = false;
                this.MoveLMark = false;
                this.MoveRMark = false;
                base.Invalidate();
                if (!this.m_MovePointer)
                {
                    this.OnRangeChanged(EventArgs.Empty);
                }
                this.m_MovePointer = false;
            }
        }

        private void OnPaint(object sender, PaintEventArgs e)
        {
            int baryoff;
            int markyoff;
            int tickyoff1;
            int tickyoff2;
            double dtick;
            int tickpos;
            SolidBrush brushInner;
            int i;
            Font fontMark;
            SolidBrush brushMark;
            StringFormat strformat;
            int w = this._actualWidth;
            int h = base.Height;
            Pen penRange = new Pen(this.colorRange);
            Pen penShadowLight = new Pen(this.colorShadowLight);
            Pen penShadowDark = new Pen(this.colorShadowDark);
            SolidBrush brushShadowLight = new SolidBrush(this.colorShadowLight);
            SolidBrush brushShadowDark = new SolidBrush(this.colorShadowDark);
            Brush brushRange = new SolidBrush(this.colorRange);
            if (base.Enabled)
            {
                brushInner = new SolidBrush(this.colorInner);
            }
            else
            {
                brushInner = new SolidBrush(Color.FromKnownColor(KnownColor.InactiveCaption));
            }
            this.XPosMin = this.MarkWidth + 1;
            if (this.orientationBar == RangeBarOrientation.horizontal)
            {
                this.XPosMax = (w - this.MarkWidth) - 1;
            }
            else
            {
                this.XPosMax = (h - this.MarkWidth) - 1;
            }
            if (this.PosL < this.XPosMin)
            {
                this.PosL = this.XPosMin;
            }
            if (this.PosL > this.XPosMax)
            {
                this.PosL = this.XPosMax;
            }
            if (this.PosR > this.XPosMax)
            {
                this.PosR = this.XPosMax;
            }
            if (this.PosR < this.XPosMin)
            {
                this.PosR = this.XPosMin;
            }

            this.Range2Pos();

            if (this.PosR >= this.XPosMax)
            {
                this.PosR = this.XPosMax;// -this.MarkWidth - 1;
            }


            if (this.orientationBar == RangeBarOrientation.horizontal)
            {
                baryoff = (h - this.BarHeight) / 2;
                markyoff = (baryoff + ((this.BarHeight - this.MarkHeight) / 2)) - 1;
                e.Graphics.FillRectangle(brushShadowDark, 0, baryoff, w - 1, this.sizeShadow);
                e.Graphics.FillRectangle(brushShadowDark, 0, baryoff, this.sizeShadow, this.BarHeight - 1);
                e.Graphics.FillRectangle(brushShadowLight, 0, ((baryoff + this.BarHeight) - 1) - this.sizeShadow, w - 1, this.sizeShadow);
                e.Graphics.FillRectangle(brushShadowLight, (w - 1) - this.sizeShadow, baryoff, this.sizeShadow, this.BarHeight - 1);
                e.Graphics.FillRectangle(brushInner, (int)(this.PosL - this._scrollValue), (int)(baryoff + this.sizeShadow), (int)(this.PosR - this.PosL), (int)((this.BarHeight - 1) - (2 * this.sizeShadow)));
                if (this.orientationScale == TopBottomOrientation.bottom)
                {
                    tickyoff1 = tickyoff2 = (baryoff + this.BarHeight) + 2;
                }
                else if (this.orientationScale == TopBottomOrientation.top)
                {
                    tickyoff1 = tickyoff2 = (baryoff - this.TickHeight) - 4;
                }
                else
                {
                    tickyoff1 = (baryoff + this.BarHeight) + 2;
                    tickyoff2 = (baryoff - this.TickHeight) - 4;
                }
                if (this.numAxisDivision > 1)
                {
                    dtick = ((double)(this.XPosMax - this.XPosMin)) / ((double)this.numAxisDivision);
                    for (i = 0; i < (this.numAxisDivision + 1); i++)
                    {
                        tickpos = (int)Math.Round((double)(i * dtick));
                        if ((this.orientationScale == TopBottomOrientation.bottom) || (this.orientationScale == TopBottomOrientation.both))
                        {
                            e.Graphics.DrawLine(penShadowDark, ((this.MarkWidth + 1) + tickpos) - this._scrollValue, tickyoff1, ((this.MarkWidth + 1) + tickpos) - this._scrollValue, tickyoff1 + this.TickHeight);
                        }
                        if ((this.orientationScale == TopBottomOrientation.top) || (this.orientationScale == TopBottomOrientation.both))
                        {
                            e.Graphics.DrawLine(penShadowDark, ((this.MarkWidth + 1) + tickpos) - this._scrollValue, tickyoff2, ((this.MarkWidth + 1) + tickpos) - this._scrollValue, tickyoff2 + this.TickHeight);
                        }
                    }
                }
                if (this.DrawPointer)
                {
                    LinearGradientBrush pointerBr = new LinearGradientBrush(this.m_PointerRect, Color.White, Color.Blue, LinearGradientMode.Vertical);
                    e.Graphics.FillRectangle(pointerBr, this.m_PointerRect);
                    pointerBr.Dispose();
                }
                this.LMarkPnt[0].X = (this.PosL - this.MarkWidth) - this._scrollValue;
                this.LMarkPnt[0].Y = markyoff + (this.MarkHeight / 3);
                this.LMarkPnt[1].X = this.PosL - this._scrollValue;
                this.LMarkPnt[1].Y = markyoff;
                this.LMarkPnt[2].X = this.PosL - this._scrollValue;
                this.LMarkPnt[2].Y = markyoff + this.MarkHeight;
                this.LMarkPnt[3].X = (this.PosL - this.MarkWidth) - this._scrollValue;
                this.LMarkPnt[3].Y = markyoff + ((2 * this.MarkHeight) / 3);
                this.LMarkPnt[4].X = (this.PosL - this.MarkWidth) - this._scrollValue;
                this.LMarkPnt[4].Y = markyoff;
                Rectangle rctl = new Rectangle(this.LMarkPnt[0].X, this.LMarkPnt[1].Y, this.LMarkPnt[1].X - this.LMarkPnt[0].X, this.LMarkPnt[1].Y - this.LMarkPnt[2].Y);
                if (brushRange != null)
                {
                    brushRange.Dispose();
                }
                brushRange = new LinearGradientBrush(rctl, Color.Black, Color.Silver, LinearGradientMode.Vertical);
                e.Graphics.FillPolygon(brushRange, this.LMarkPnt);
                e.Graphics.DrawLine(penShadowDark, this.LMarkPnt[3].X - 1, this.LMarkPnt[3].Y, this.LMarkPnt[1].X - 1, this.LMarkPnt[2].Y);
                e.Graphics.DrawLine(penShadowLight, this.LMarkPnt[0].X - 1, this.LMarkPnt[0].Y, this.LMarkPnt[0].X - 1, this.LMarkPnt[3].Y);
                e.Graphics.DrawLine(penShadowLight, this.LMarkPnt[0].X - 1, this.LMarkPnt[0].Y, this.LMarkPnt[1].X - 1, this.LMarkPnt[1].Y);
                if (this.PosL < this.PosR)
                {
                    e.Graphics.DrawLine(penShadowDark, this.LMarkPnt[1].X, this.LMarkPnt[1].Y + 1, this.LMarkPnt[1].X, this.LMarkPnt[2].Y);
                }
                if (this.ActiveMark == ActiveMarkType.left)
                {
                    e.Graphics.DrawLine(penShadowLight, (int)(((this.PosL - (this.MarkWidth / 2)) - 1) - this._scrollValue), (int)(markyoff + (this.MarkHeight / 3)), (int)(((this.PosL - (this.MarkWidth / 2)) - 1) - this._scrollValue), (int)(markyoff + ((2 * this.MarkHeight) / 3)));
                    e.Graphics.DrawLine(penShadowDark, (int)((this.PosL - (this.MarkWidth / 2)) - this._scrollValue), (int)(markyoff + (this.MarkHeight / 3)), (int)((this.PosL - (this.MarkWidth / 2)) - this._scrollValue), (int)(markyoff + ((2 * this.MarkHeight) / 3)));
                }
                this.RMarkPnt[0].X = (this.PosR + this.MarkWidth) - this._scrollValue;
                this.RMarkPnt[0].Y = markyoff + (this.MarkHeight / 3);
                this.RMarkPnt[1].X = this.PosR - this._scrollValue;
                this.RMarkPnt[1].Y = markyoff;
                this.RMarkPnt[2].X = this.PosR - this._scrollValue;
                this.RMarkPnt[2].Y = markyoff + this.MarkHeight;
                this.RMarkPnt[3].X = (this.PosR + this.MarkWidth) - this._scrollValue;
                this.RMarkPnt[3].Y = markyoff + ((2 * this.MarkHeight) / 3);
                this.RMarkPnt[4].X = (this.PosR + this.MarkWidth) - this._scrollValue;
                this.RMarkPnt[4].Y = markyoff;
                e.Graphics.FillPolygon(brushRange, this.RMarkPnt);
                if (this.PosL < this.PosR)
                {
                    e.Graphics.DrawLine(penShadowLight, this.RMarkPnt[1].X - 1, this.RMarkPnt[1].Y + 1, this.RMarkPnt[2].X - 1, this.RMarkPnt[2].Y);
                }
                e.Graphics.DrawLine(penShadowDark, this.RMarkPnt[2].X, this.RMarkPnt[2].Y, this.RMarkPnt[3].X, this.RMarkPnt[3].Y);
                e.Graphics.DrawLine(penShadowDark, this.RMarkPnt[0].X, this.RMarkPnt[0].Y, this.RMarkPnt[1].X, this.RMarkPnt[1].Y);
                e.Graphics.DrawLine(penShadowDark, this.RMarkPnt[0].X, this.RMarkPnt[0].Y + 1, this.RMarkPnt[3].X, this.RMarkPnt[3].Y);
                if (this.ActiveMark == ActiveMarkType.right)
                {
                    e.Graphics.DrawLine(penShadowLight, (int)(((this.PosR + (this.MarkWidth / 2)) - 1) - this._scrollValue), (int)(markyoff + (this.MarkHeight / 3)), (int)(((this.PosR + (this.MarkWidth / 2)) - 1) - this._scrollValue), (int)(markyoff + ((2 * this.MarkHeight) / 3)));
                    e.Graphics.DrawLine(penShadowDark, (int)((this.PosR + (this.MarkWidth / 2)) - this._scrollValue), (int)(markyoff + (this.MarkHeight / 3)), (int)((this.PosR + (this.MarkWidth / 2)) - this._scrollValue), (int)(markyoff + ((2 * this.MarkHeight) / 3)));
                }
                if (this.MoveLMark)
                {
                    fontMark = new Font("Arial", (float)this.MarkWidth);
                    brushMark = new SolidBrush(this.colorShadowDark);
                    strformat = new StringFormat();
                    strformat.Alignment = StringAlignment.Center;
                    strformat.LineAlignment = StringAlignment.Near;
                    e.Graphics.DrawString(this.rangeMin.ToString(), fontMark, brushMark, (float)this.PosL, (float)(tickyoff1 + this.TickHeight), strformat);
                }
                if (this.MoveRMark)
                {
                    fontMark = new Font("Arial", (float)this.MarkWidth);
                    brushMark = new SolidBrush(this.colorShadowDark);
                    strformat = new StringFormat();
                    strformat.Alignment = StringAlignment.Center;
                    strformat.LineAlignment = StringAlignment.Near;
                    e.Graphics.DrawString(this.rangeMax.ToString(), fontMark, brushMark, (float)this.PosR, (float)(tickyoff1 + this.TickHeight), strformat);
                }
            }
            else
            {
                baryoff = (w + this.BarHeight) / 2;
                markyoff = (baryoff - (this.BarHeight / 2)) - (this.MarkHeight / 2);
                if (this.orientationScale == TopBottomOrientation.bottom)
                {
                    tickyoff1 = tickyoff2 = baryoff + 2;
                }
                else if (this.orientationScale == TopBottomOrientation.top)
                {
                    tickyoff1 = tickyoff2 = ((baryoff - this.BarHeight) - 2) - this.TickHeight;
                }
                else
                {
                    tickyoff1 = baryoff + 2;
                    tickyoff2 = ((baryoff - this.BarHeight) - 2) - this.TickHeight;
                }
                e.Graphics.FillRectangle(brushShadowDark, baryoff - this.BarHeight, 0, this.BarHeight, this.sizeShadow);
                e.Graphics.FillRectangle(brushShadowDark, baryoff - this.BarHeight, 0, this.sizeShadow, h - 1);
                e.Graphics.FillRectangle(brushShadowLight, baryoff, 0, this.sizeShadow, h - 1);
                e.Graphics.FillRectangle(brushShadowLight, baryoff - this.BarHeight, h - this.sizeShadow, this.BarHeight, this.sizeShadow);
                e.Graphics.FillRectangle(brushInner, (baryoff - this.BarHeight) + this.sizeShadow, this.PosL, this.BarHeight - (2 * this.sizeShadow), this.PosR - this.PosL);
                if (this.numAxisDivision > 1)
                {
                    dtick = ((double)(this.XPosMax - this.XPosMin)) / ((double)this.numAxisDivision);
                    for (i = 0; i < (this.numAxisDivision + 1); i++)
                    {
                        tickpos = (int)Math.Round((double)(i * dtick));
                        if ((this.orientationScale == TopBottomOrientation.bottom) || (this.orientationScale == TopBottomOrientation.both))
                        {
                            e.Graphics.DrawLine(penShadowDark, tickyoff1, (this.MarkWidth + 1) + tickpos, tickyoff1 + this.TickHeight, (this.MarkWidth + 1) + tickpos);
                        }
                        if ((this.orientationScale == TopBottomOrientation.top) || (this.orientationScale == TopBottomOrientation.both))
                        {
                            e.Graphics.DrawLine(penShadowDark, tickyoff2, (this.MarkWidth + 1) + tickpos, tickyoff2 + this.TickHeight, (this.MarkWidth + 1) + tickpos);
                        }
                    }
                }
                this.LMarkPnt[0].Y = this.PosL - this.MarkWidth;
                this.LMarkPnt[0].X = markyoff + (this.MarkHeight / 3);
                this.LMarkPnt[1].Y = this.PosL;
                this.LMarkPnt[1].X = markyoff;
                this.LMarkPnt[2].Y = this.PosL;
                this.LMarkPnt[2].X = markyoff + this.MarkHeight;
                this.LMarkPnt[3].Y = this.PosL - this.MarkWidth;
                this.LMarkPnt[3].X = markyoff + ((2 * this.MarkHeight) / 3);
                this.LMarkPnt[4].Y = this.PosL - this.MarkWidth;
                this.LMarkPnt[4].X = markyoff;
                e.Graphics.FillPolygon(brushRange, this.LMarkPnt);
                e.Graphics.DrawLine(penShadowDark, this.LMarkPnt[3].X, this.LMarkPnt[3].Y, this.LMarkPnt[2].X, this.LMarkPnt[2].Y);
                e.Graphics.DrawLine(penShadowLight, this.LMarkPnt[0].X - 1, this.LMarkPnt[0].Y, this.LMarkPnt[3].X - 1, this.LMarkPnt[3].Y);
                e.Graphics.DrawLine(penShadowLight, this.LMarkPnt[0].X - 1, this.LMarkPnt[0].Y, this.LMarkPnt[1].X - 1, this.LMarkPnt[1].Y);
                if (this.PosL < this.PosR)
                {
                    e.Graphics.DrawLine(penShadowDark, this.LMarkPnt[1].X, this.LMarkPnt[1].Y, this.LMarkPnt[2].X, this.LMarkPnt[2].Y);
                }
                if (this.ActiveMark == ActiveMarkType.left)
                {
                    e.Graphics.DrawLine(penShadowLight, (int)(markyoff + (this.MarkHeight / 3)), (int)(this.PosL - (this.MarkWidth / 2)), (int)(markyoff + ((2 * this.MarkHeight) / 3)), (int)(this.PosL - (this.MarkWidth / 2)));
                    e.Graphics.DrawLine(penShadowDark, (int)(markyoff + (this.MarkHeight / 3)), (int)((this.PosL - (this.MarkWidth / 2)) + 1), (int)(markyoff + ((2 * this.MarkHeight) / 3)), (int)((this.PosL - (this.MarkWidth / 2)) + 1));
                }
                this.RMarkPnt[0].Y = this.PosR + this.MarkWidth;
                this.RMarkPnt[0].X = markyoff + (this.MarkHeight / 3);
                this.RMarkPnt[1].Y = this.PosR;
                this.RMarkPnt[1].X = markyoff;
                this.RMarkPnt[2].Y = this.PosR;
                this.RMarkPnt[2].X = markyoff + this.MarkHeight;
                this.RMarkPnt[3].Y = this.PosR + this.MarkWidth;
                this.RMarkPnt[3].X = markyoff + ((2 * this.MarkHeight) / 3);
                this.RMarkPnt[4].Y = this.PosR + this.MarkWidth;
                this.RMarkPnt[4].X = markyoff;
                e.Graphics.FillPolygon(brushRange, this.RMarkPnt);
                e.Graphics.DrawLine(penShadowDark, this.RMarkPnt[2].X, this.RMarkPnt[2].Y, this.RMarkPnt[3].X, this.RMarkPnt[3].Y);
                e.Graphics.DrawLine(penShadowDark, this.RMarkPnt[0].X, this.RMarkPnt[0].Y, this.RMarkPnt[1].X, this.RMarkPnt[1].Y);
                e.Graphics.DrawLine(penShadowDark, this.RMarkPnt[0].X, this.RMarkPnt[0].Y, this.RMarkPnt[3].X, this.RMarkPnt[3].Y);
                if (this.PosL < this.PosR)
                {
                    e.Graphics.DrawLine(penShadowLight, this.RMarkPnt[1].X, this.RMarkPnt[1].Y, this.RMarkPnt[2].X, this.RMarkPnt[2].Y);
                }
                if (this.ActiveMark == ActiveMarkType.right)
                {
                    e.Graphics.DrawLine(penShadowLight, (int)(markyoff + (this.MarkHeight / 3)), (int)((this.PosR + (this.MarkWidth / 2)) - 1), (int)(markyoff + ((2 * this.MarkHeight) / 3)), (int)((this.PosR + (this.MarkWidth / 2)) - 1));
                    e.Graphics.DrawLine(penShadowDark, (int)(markyoff + (this.MarkHeight / 3)), (int)(this.PosR + (this.MarkWidth / 2)), (int)(markyoff + ((2 * this.MarkHeight) / 3)), (int)(this.PosR + (this.MarkWidth / 2)));
                }
                if (this.MoveLMark)
                {
                    fontMark = new Font("Arial", (float)this.MarkWidth);
                    brushMark = new SolidBrush(this.colorShadowDark);
                    strformat = new StringFormat();
                    strformat.Alignment = StringAlignment.Near;
                    strformat.LineAlignment = StringAlignment.Center;
                    e.Graphics.DrawString(this.rangeMin.ToString(), fontMark, brushMark, (float)((tickyoff1 + this.TickHeight) + 2), (float)this.PosL, strformat);
                }
                if (this.MoveRMark)
                {
                    fontMark = new Font("Arial", (float)this.MarkWidth);
                    brushMark = new SolidBrush(this.colorShadowDark);
                    strformat = new StringFormat();
                    strformat.Alignment = StringAlignment.Near;
                    strformat.LineAlignment = StringAlignment.Center;
                    e.Graphics.DrawString(this.rangeMax.ToString(), fontMark, brushMark, (float)(tickyoff1 + this.TickHeight), (float)this.PosR, strformat);
                }
            }
        }

        public virtual void OnRangeChanged(EventArgs e)
        {
            if (this.RangeChanged != null)
            {
                this.RangeChanged(this, e);
            }
        }

        public virtual void OnRangeChanging(EventArgs e)
        {
            if (this.RangeChanging != null)
            {
                this.RangeChanging(this, e);
            }
        }

        private void OnResize(object sender, EventArgs e)
        {
            //if (this._actualWidth != base.Width)
            {
                this._actualWidth = base.Width;
                RecalculatePointerPosition();
            }
            base.Invalidate(true);
        }

        public virtual void OnRightRangeChanging(EventArgs e)
        {
            if (this.RightRangeChanging != null)
            {
                this.RightRangeChanging(this, e);
            }
        }

        private void OnSizeChanged(object sender, EventArgs e)
        {
            base.Invalidate(true);
            base.Update();
        }

        private void Pos2Range()
        {
            int w;
            if (this.orientationBar == RangeBarOrientation.horizontal)
            {
                w = this._actualWidth;
            }
            else
            {
                w = base.Height;
            }
            int posw = (w - (2 * this.MarkWidth)) - 2;
            this.rangeMin = this.Minimum + ((int)Math.Round((double)(((this.Maximum - this.Minimum) * ((this.PosL - this.XPosMin) + this._scrollValue)) / ((double)posw))));
            this.rangeMax = this.Minimum + ((int)Math.Round((double)(((this.Maximum - this.Minimum) * ((this.PosR - this.XPosMin) + this._scrollValue)) / ((double)posw))));
        }

        private void Pos2RangeLeft()
        {
            int w;
            if (this.orientationBar == RangeBarOrientation.horizontal)
            {
                w = this._actualWidth;
            }
            else
            {
                w = base.Height;
            }
            int posw = (w - (2 * this.MarkWidth)) - 2;
            this.rangeMin = this.Minimum + ((int)Math.Round((double)(((this.Maximum - this.Minimum) * ((this.PosL - this.XPosMin) + this._scrollValue)) / ((double)posw))));
        }

        private void Pos2RangeRight()
        {
            int w;
            if (this.orientationBar == RangeBarOrientation.horizontal)
            {
                w = this._actualWidth;
            }
            else
            {
                w = base.Height;
            }
            int posw = (w - (2 * this.MarkWidth)) - 2;
            this.rangeMax = this.Minimum + ((int)Math.Round((double)(((this.Maximum - this.Minimum) * ((this.PosR - this.XPosMin) + this._scrollValue)) / ((double)posw))));
        }

        private void Range2Pos()
        {
            int w;
            if (this.orientationBar == RangeBarOrientation.horizontal)
            {
                w = this._actualWidth;
            }
            else
            {
                w = base.Height;
            }
            int posw = (w - (2 * this.MarkWidth)) - 2;
            this.PosL = this.XPosMin + ((int)Math.Round((double)((posw * (this.rangeMin - this.Minimum)) / (this.Maximum - this.Minimum))));
            this.PosR = this.XPosMin + ((int)Math.Round((double)((posw * (this.rangeMax - this.Minimum)) / (this.Maximum - this.Minimum))));

            if (_actualWidth > Width)
            {
                if (PosR > _actualWidth - 2 * MarkWidth)
                    PosR = _actualWidth - 2 * MarkWidth;
            }
        }

        public void SelectActualSize(int width, int height)
        {
            this._actualWidth = width;
            this.Range2Pos();
            float pos = ((float)(((double)(this._actualWidth * this._myPointerPos)) / (this.Maximum - this.Minimum))) + this._scrollValue;
            this.m_PointerRect = new RectangleF(pos, this.m_PointerRect.Y, this.m_PointerRect.Width, this.m_PointerRect.Height);
            base.Invalidate(true);
            if (this.ActualWidthChanged != null)
            {
                this.ActualWidthChanged(this, this._actualWidth);
            }
        }

        public void SelectRange(int left, int right)
        {
            this.RangeMinimum = left;
            this.RangeMaximum = right;
            this.Range2Pos();
            base.Invalidate(true);
        }

        public void SetRangeLimit(double left, double right)
        {
            this.Minimum = left;
            this.Maximum = right;
            this.Range2Pos();
            base.Invalidate(true);
        }

        // Properties
        public int DivisionNum
        {
            get
            {
                return this.numAxisDivision;
            }
            set
            {
                this.numAxisDivision = value;
                this.Refresh();
            }
        }

        public bool DrawPointer
        {
            get
            {
                return this.m_DrawPointer;
            }
            set
            {
                this.m_DrawPointer = value;
            }
        }

        public int HeightOfBar
        {
            get
            {
                return this.BarHeight;
            }
            set
            {
                this.BarHeight = Math.Min(value, this.MarkHeight - 2);
                base.Invalidate();
                base.Update();
            }
        }

        public int HeightOfMark
        {
            get
            {
                return this.MarkHeight;
            }
            set
            {
                this.MarkHeight = Math.Max(this.BarHeight + 2, value);
                base.Invalidate();
                base.Update();
            }
        }

        public int HeightOfTick
        {
            get
            {
                return this.TickHeight;
            }
            set
            {
                this.TickHeight = Math.Min(Math.Max(1, value), this.BarHeight);
                base.Invalidate();
                base.Update();
            }
        }

        public Color InnerColor
        {
            get
            {
                return this.colorInner;
            }
            set
            {
                this.colorInner = value;
                this.Refresh();
            }
        }

        public RangeBarOrientation Orientation
        {
            get
            {
                return this.orientationBar;
            }
            set
            {
                this.orientationBar = value;
                base.Invalidate();
                base.Update();
            }
        }

        public int PointerPosition
        {
            get
            {
                return this.m_PointerPosition;
            }
            set
            {
                this.m_PointerPosition = value;
                RecalculatePointerPosition();
            }
        }

        private void RecalculatePointerPosition()
        {
            if (this.DrawPointer)
            {
                int w = 0;
                if (this.orientationBar == RangeBarOrientation.horizontal)
                {
                    w = _actualWidth;// base.Width;
                }
                else
                {
                    w = base.Height;
                }
                double PointerPositionInPixels = ((double)this.m_PointerPosition) / ((this.Maximum - this.Minimum) / ((double)w));
                if (PointerPositionInPixels < 0.0)
                {
                    PointerPositionInPixels = 0.0;
                }
                //if (PointerPositionInPixels > (base.Width - this.m_PointerRect.Width))
                //{
                //    PointerPositionInPixels = base.Width - this.m_PointerRect.Width;
                //}
                if (PointerPositionInPixels > (_actualWidth - this.m_PointerRect.Width))
                {
                    PointerPositionInPixels = _actualWidth - 2 * this.m_PointerRect.Width;
                }
                PointerPositionInPixels -= _scrollValue;

                int baryoff = (base.Height - this.BarHeight) / 2;
                int markyoff = (baryoff + ((this.BarHeight - this.MarkHeight) / 2)) - 1;
                this.m_PointerRect = new RectangleF((float)PointerPositionInPixels, markyoff + 3f, 4f, 16f);
                base.Invalidate(true);
            }
        }

        public int RangeMaximum
        {
            get
            {
                return (int)this.rangeMax;
            }
            set
            {
                this.rangeMax = value;
                if (this.rangeMax < this.Minimum)
                {
                    this.rangeMax = this.Minimum;
                }
                else if (this.rangeMax > this.Maximum)
                {
                    this.rangeMax = this.Maximum;
                }
                if (this.rangeMax < this.rangeMin)
                {
                    this.rangeMax = this.rangeMin;
                }
                this.Range2Pos();
                base.Invalidate(true);
            }
        }

        public int RangeMinimum
        {
            get
            {
                return (int)this.rangeMin;
            }
            set
            {
                this.rangeMin = value;
                if (this.rangeMin < this.Minimum)
                {
                    this.rangeMin = this.Minimum;
                }
                else if (this.rangeMin > this.Maximum)
                {
                    this.rangeMin = this.Maximum;
                }
                if (this.rangeMin > this.rangeMax)
                {
                    this.rangeMin = this.rangeMax;
                }
                this.Range2Pos();
                base.Invalidate(true);
            }
        }

        public TopBottomOrientation ScaleOrientation
        {
            get
            {
                return this.orientationScale;
            }
            set
            {
                this.orientationScale = value;
                base.Invalidate();
                base.Update();
            }
        }

        public int TotalMaximum
        {
            get
            {
                return (int)this.Maximum;
            }
            set
            {
                this.Maximum = value;
                if (this.rangeMax > this.Maximum)
                {
                    this.rangeMax = this.Maximum;
                }
                this.Range2Pos();
                base.Invalidate(true);
            }
        }

        public int TotalMinimum
        {
            get
            {
                return (int)this.Minimum;
            }
            set
            {
                this.Minimum = value;
                if (this.rangeMin < this.Minimum)
                {
                    this.rangeMin = this.Minimum;
                }
                this.Range2Pos();
                base.Invalidate(true);
            }
        }

        // Nested Types
        public enum ActiveMarkType
        {
            none,
            left,
            right
        }

        public delegate void ActualWidthChangedEventHandler(object sender, int actualWidth);

        public enum RangeBarOrientation
        {
            horizontal,
            vertical
        }

        public delegate void RangeChangedEventHandler(object sender, EventArgs e);

        public delegate void RangeChangingEventHandler(object sender, EventArgs e);

        public delegate void ScrollEventHandler(object sender, int scrollValue);

        public enum TopBottomOrientation
        {
            top,
            bottom,
            both
        }
    }
}
 
