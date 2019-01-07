using System;
using System.Collections;
using System.ComponentModel;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Windows.Forms;
using System.Timers;

namespace Decklink_Capture
{
	/// <summary>
	/// ucLED class
	/// </summary>
	public class ucLED : System.Windows.Forms.UserControl
	{
		#region Members

	    /// <summary>
	    /// Members
	    /// </summary>
	    private double _value = -48;//-96;

	    private int _ledWidth = 10;
		#endregion

		#region Constructor
		/// <summary>
		/// Constructor
		/// </summary>
		public ucLED()
		{
			// This call is required by the Windows Form Designer.
			InitializeComponent();

		    int top = 4;
			// add led controls
			//for(int i = 95; i >= 0; i--)
            for (int i = 47; i >= 0; i--)
			{
				// create label
				Label lbLED = new Label();
				// set color
				lbLED.BackColor = Color.DimGray;
				// set size
                lbLED.Size = new Size(_ledWidth, 2);
				// set poistion
				lbLED.Location = new Point((Width - _ledWidth + 2)/2, (i * 4) + 2 + top);
				// add control
				Controls.Add(lbLED);
			}
		}
		#endregion

		#region Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
            this.SuspendLayout();
            // 
            // ucLED
            // 
            this.BackColor = System.Drawing.Color.Gray;
            this.ForeColor = System.Drawing.SystemColors.ControlText;
            this.Name = "ucLED";
            this.Size = new System.Drawing.Size(20, 200);
            this.ResumeLayout(false);

		}
		#endregion

		#region UpdateValue
		/// <summary>
		/// Show Leds
		/// </summary>
		/// <param name="g"></param>
		protected void UpdateValue()
		{
			// define in value
            int iValue = -((-(int)(_value)) - 48);

			// fetch all control
			for(int i = 0;i < Controls.Count;i++)
			{
				// get controls
				Control ctrLED = Controls[i];

				// check if active or not
				if( i < iValue )
				{
                    // check color
                    //if (i < 78)
                    //    ctrLED.BackColor = Color.LightGreen;
                    //else if (i < 88)
                    //    ctrLED.BackColor = Color.Peru;
                    //else
                    //    ctrLED.BackColor = Color.Red;
                    if (i < 34)
                        ctrLED.BackColor = Color.LightGreen;
                    else if (i < 40)
                        ctrLED.BackColor = Color.Peru;
                    else
                        ctrLED.BackColor = Color.Red;
				}
				else
				{
					// inactive
					ctrLED.BackColor = Color.DimGray;
				}
			}
		}
		#endregion

		#region Value
		/// <summary>
		/// Value
		/// </summary>
		public double Value
		{
			set
			{
				// check value
				if( value > 0 || value < -96 )
					return;
			    if (value < -48)
			        value = -48;
                _value = value;
				// update
				UpdateValue();
			}
		}
		#endregion

	    protected override void OnPaint(PaintEventArgs e)
	    {
	        base.OnPaint(e);
            //ControlPaint.DrawBorder(e.Graphics, this.ClientRectangle, Color.LightSlateGray, ButtonBorderStyle.Solid);
	        var pen = new Pen(Color.DarkGray, 0.5f);
            DrawRoundedRectangle(e.Graphics, ClientRectangle, 5, pen);
            pen.Dispose();
	    }
        private void DrawRoundedRectangle(Graphics gfx, Rectangle Bounds, int CornerRadius, Pen DrawPen)
        {
            int strokeOffset = Convert.ToInt32(Math.Ceiling(DrawPen.Width));
            Bounds = Rectangle.Inflate(Bounds, -strokeOffset, -strokeOffset);

            DrawPen.EndCap = DrawPen.StartCap = LineCap.Round;

            GraphicsPath gfxPath = new GraphicsPath();
            gfxPath.AddArc(Bounds.X, Bounds.Y, CornerRadius, CornerRadius, 180, 90);
            gfxPath.AddArc(Bounds.X + Bounds.Width - CornerRadius, Bounds.Y, CornerRadius, CornerRadius, 270, 90);
            gfxPath.AddArc(Bounds.X + Bounds.Width - CornerRadius, Bounds.Y + Bounds.Height - CornerRadius, CornerRadius, CornerRadius, 0, 90);
            gfxPath.AddArc(Bounds.X, Bounds.Y + Bounds.Height - CornerRadius, CornerRadius, CornerRadius, 90, 90);
            gfxPath.CloseAllFigures();

            //gfx.FillPath(new SolidBrush(FillColor), gfxPath);
            gfx.DrawPath(DrawPen, gfxPath);
        }
	}
}

