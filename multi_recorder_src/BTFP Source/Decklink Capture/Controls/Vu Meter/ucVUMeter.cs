using System;
using System.Collections;
using System.ComponentModel;
using System.Drawing;
using System.Windows.Forms;
//using Pixel.Data;

namespace Decklink_Capture
{
	/// <summary>
	/// VUMEter Control
	/// </summary>
	[DefaultEvent("PreviewChanged")]
	public class ucVUMeter : ucBase
	{
		#region Members

        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Label label3;
        private ucLED vu1;
        private ucLED vu2;
        private ucLED vu3;
        private ucLED vu4;
        private ucLED vu5;
        private ucLED vu6;
        private Label label6;
        private Label label5;
        private Label label1;
        private Label label7;
        private Label label8;
        private Label label9;
        private Label label4;
        private Label label10;
        private Label label11;
        private Label label12;
        private Label label13;
		//private Pixel.Controls.ucCheckBoxEx ctrEnable;
		private int m_Channel;
		#endregion

		#region Handlers
		/// <summary>
		/// PreviewChanged
		/// </summary>
		public event EventHandler PreviewChanged;
		#endregion

		#region Constructor
		/// <summary>
		/// Constructor
		/// </summary>
		public ucVUMeter()
		{
			// This call is required by the Windows Form Designer.
			InitializeComponent();

			// set def channel
			m_Channel = 6;
		}
		#endregion

		#region Overload Caption
        ///// <summary>
        ///// Overload Caption
        ///// </summary>
        //public string Caption
        //{
        //    get{return ctrEnable.Caption;}
        //    set{ctrEnable.Caption=value;}
        //}
		#endregion

		#region Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
            this.label2 = new System.Windows.Forms.Label();
            this.label3 = new System.Windows.Forms.Label();
            this.vu1 = new ucLED();
            this.vu2 = new ucLED();
            this.vu3 = new ucLED();
            this.vu4 = new ucLED();
            this.vu5 = new ucLED();
            this.vu6 = new ucLED();
            this.label6 = new System.Windows.Forms.Label();
            this.label5 = new System.Windows.Forms.Label();
            this.label1 = new System.Windows.Forms.Label();
            this.label7 = new System.Windows.Forms.Label();
            this.label8 = new System.Windows.Forms.Label();
            this.label9 = new System.Windows.Forms.Label();
            this.label4 = new System.Windows.Forms.Label();
            this.label10 = new System.Windows.Forms.Label();
            this.label11 = new System.Windows.Forms.Label();
            this.label12 = new System.Windows.Forms.Label();
            this.label13 = new System.Windows.Forms.Label();
            this.SuspendLayout();
            // 
            // label2
            // 
            this.label2.Location = new System.Drawing.Point(23, 0);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(100, 23);
            this.label2.TabIndex = 6;
            // 
            // label3
            // 
            this.label3.Location = new System.Drawing.Point(23, 0);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(100, 23);
            this.label3.TabIndex = 5;
            // 
            // vu1
            // 
            this.vu1.BackColor = System.Drawing.Color.Gray;
            this.vu1.ForeColor = System.Drawing.SystemColors.ControlText;
            this.vu1.Location = new System.Drawing.Point(27, 3);
            this.vu1.Name = "vu1";
            this.vu1.Size = new System.Drawing.Size(20, 203);
            this.vu1.TabIndex = 7;
            // 
            // vu2
            // 
            this.vu2.BackColor = System.Drawing.Color.Gray;
            this.vu2.ForeColor = System.Drawing.SystemColors.ControlText;
            this.vu2.Location = new System.Drawing.Point(53, 3);
            this.vu2.Name = "vu2";
            this.vu2.Size = new System.Drawing.Size(20, 203);
            this.vu2.TabIndex = 8;
            // 
            // vu3
            // 
            this.vu3.BackColor = System.Drawing.Color.Gray;
            this.vu3.ForeColor = System.Drawing.SystemColors.ControlText;
            this.vu3.Location = new System.Drawing.Point(79, 3);
            this.vu3.Name = "vu3";
            this.vu3.Size = new System.Drawing.Size(20, 203);
            this.vu3.TabIndex = 9;
            // 
            // vu4
            // 
            this.vu4.BackColor = System.Drawing.Color.Gray;
            this.vu4.ForeColor = System.Drawing.SystemColors.ControlText;
            this.vu4.Location = new System.Drawing.Point(103, 3);
            this.vu4.Name = "vu4";
            this.vu4.Size = new System.Drawing.Size(20, 203);
            this.vu4.TabIndex = 10;
            // 
            // vu5
            // 
            this.vu5.BackColor = System.Drawing.Color.Gray;
            this.vu5.ForeColor = System.Drawing.SystemColors.ControlText;
            this.vu5.Location = new System.Drawing.Point(129, 3);
            this.vu5.Name = "vu5";
            this.vu5.Size = new System.Drawing.Size(20, 203);
            this.vu5.TabIndex = 11;
            // 
            // vu6
            // 
            this.vu6.BackColor = System.Drawing.Color.Gray;
            this.vu6.ForeColor = System.Drawing.SystemColors.ControlText;
            this.vu6.Location = new System.Drawing.Point(155, 3);
            this.vu6.Name = "vu6";
            this.vu6.Size = new System.Drawing.Size(20, 203);
            this.vu6.TabIndex = 12;
            // 
            // label6
            // 
            this.label6.ForeColor = System.Drawing.Color.Black;
            this.label6.Location = new System.Drawing.Point(158, 209);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(14, 16);
            this.label6.TabIndex = 18;
            this.label6.Text = "6";
            // 
            // label5
            // 
            this.label5.ForeColor = System.Drawing.Color.Black;
            this.label5.Location = new System.Drawing.Point(132, 209);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(14, 16);
            this.label5.TabIndex = 17;
            this.label5.Text = "5";
            // 
            // label1
            // 
            this.label1.ForeColor = System.Drawing.Color.Black;
            this.label1.Location = new System.Drawing.Point(106, 209);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(14, 16);
            this.label1.TabIndex = 16;
            this.label1.Text = "4";
            // 
            // label7
            // 
            this.label7.ForeColor = System.Drawing.Color.Black;
            this.label7.Location = new System.Drawing.Point(82, 209);
            this.label7.Name = "label7";
            this.label7.Size = new System.Drawing.Size(14, 16);
            this.label7.TabIndex = 15;
            this.label7.Text = "3";
            // 
            // label8
            // 
            this.label8.ForeColor = System.Drawing.Color.Black;
            this.label8.Location = new System.Drawing.Point(57, 209);
            this.label8.Name = "label8";
            this.label8.Size = new System.Drawing.Size(14, 16);
            this.label8.TabIndex = 14;
            this.label8.Text = "2";
            // 
            // label9
            // 
            this.label9.ForeColor = System.Drawing.Color.Black;
            this.label9.Location = new System.Drawing.Point(32, 209);
            this.label9.Name = "label9";
            this.label9.Size = new System.Drawing.Size(14, 16);
            this.label9.TabIndex = 13;
            this.label9.Text = "1";
            // 
            // label4
            // 
            this.label4.Font = new System.Drawing.Font("Verdana", 6F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.label4.ForeColor = System.Drawing.Color.Black;
            this.label4.Location = new System.Drawing.Point(4, 194);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(19, 16);
            this.label4.TabIndex = 19;
            this.label4.Text = "-48";
            this.label4.TextAlign = System.Drawing.ContentAlignment.TopRight;
            // 
            // label10
            // 
            this.label10.Font = new System.Drawing.Font("Verdana", 6F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.label10.ForeColor = System.Drawing.Color.Black;
            this.label10.Location = new System.Drawing.Point(4, 4);
            this.label10.Name = "label10";
            this.label10.Size = new System.Drawing.Size(19, 16);
            this.label10.TabIndex = 20;
            this.label10.Text = "-0";
            this.label10.TextAlign = System.Drawing.ContentAlignment.TopRight;
            // 
            // label11
            // 
            this.label11.Font = new System.Drawing.Font("Verdana", 6F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.label11.ForeColor = System.Drawing.Color.Black;
            this.label11.Location = new System.Drawing.Point(4, 47);
            this.label11.Name = "label11";
            this.label11.Size = new System.Drawing.Size(19, 16);
            this.label11.TabIndex = 21;
            this.label11.Text = "-12";
            this.label11.TextAlign = System.Drawing.ContentAlignment.TopRight;
            // 
            // label12
            // 
            this.label12.Font = new System.Drawing.Font("Verdana", 6F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.label12.ForeColor = System.Drawing.Color.Black;
            this.label12.Location = new System.Drawing.Point(4, 25);
            this.label12.Name = "label12";
            this.label12.Size = new System.Drawing.Size(19, 16);
            this.label12.TabIndex = 22;
            this.label12.Text = "-6";
            this.label12.TextAlign = System.Drawing.ContentAlignment.TopRight;
            // 
            // label13
            // 
            this.label13.Font = new System.Drawing.Font("Verdana", 6F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.label13.ForeColor = System.Drawing.Color.Black;
            this.label13.Location = new System.Drawing.Point(4, 79);
            this.label13.Name = "label13";
            this.label13.Size = new System.Drawing.Size(19, 16);
            this.label13.TabIndex = 23;
            this.label13.Text = "-20";
            this.label13.TextAlign = System.Drawing.ContentAlignment.TopRight;
            // 
            // ucVUMeter
            // 
            this.BackColor = System.Drawing.Color.Gray;
            this.Controls.Add(this.label13);
            this.Controls.Add(this.label12);
            this.Controls.Add(this.label11);
            this.Controls.Add(this.label10);
            this.Controls.Add(this.label4);
            this.Controls.Add(this.label6);
            this.Controls.Add(this.label5);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.label7);
            this.Controls.Add(this.label8);
            this.Controls.Add(this.label9);
            this.Controls.Add(this.vu6);
            this.Controls.Add(this.vu5);
            this.Controls.Add(this.vu4);
            this.Controls.Add(this.vu3);
            this.Controls.Add(this.vu2);
            this.Controls.Add(this.vu1);
            this.Controls.Add(this.label3);
            this.Controls.Add(this.label2);
            this.Name = "ucVUMeter";
            this.Size = new System.Drawing.Size(184, 228);
            this.ResumeLayout(false);

		}
		#endregion

		#region AudioPeaks
		/// <summary>
		/// Set peaks for 6 channels.
		/// </summary>
		public void SetPeaks(double[] peaks)
		{
		    vu1.Value = double.IsInfinity(peaks[0]) ? -96 : peaks[0];
            vu2.Value = double.IsInfinity(peaks[1]) ? -96 : peaks[1];
            vu3.Value = double.IsInfinity(peaks[2]) ? -96 : peaks[2];
            vu4.Value = double.IsInfinity(peaks[3]) ? -96 : peaks[3];
            vu5.Value = double.IsInfinity(peaks[4]) ? -96 : peaks[4];
            vu6.Value = double.IsInfinity(peaks[5]) ? -96 : peaks[5];
		}
		#endregion

		#region PreviewChannels Changed
		/// <summary>
		/// PreviewChannels Changed
		/// </summary>
		/// <param name="sender"></param>
		/// <param name="e"></param>
		private void ctrEnable_CheckedChanged(object sender, System.EventArgs e)
		{
			// check handler
			if( PreviewChanged != null )
				PreviewChanged(this,EventArgs.Empty);
		}
		#endregion

		

		#region Channel
		/// <summary>
		/// Channel
		/// </summary>
		public int Channel
		{
			get{return m_Channel;}
			set{m_Channel=value;}
		}
		#endregion
	}
}

