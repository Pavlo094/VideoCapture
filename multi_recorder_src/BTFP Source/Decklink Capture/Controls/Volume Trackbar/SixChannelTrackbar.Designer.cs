namespace Decklink_Capture
{
    partial class SixChannelTrackbar
    {
        /// <summary> 
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary> 
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Component Designer generated code

        /// <summary> 
        /// Required method for Designer support - do not modify 
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.label11 = new System.Windows.Forms.Label();
            this.label7 = new System.Windows.Forms.Label();
            this.label8 = new System.Windows.Forms.Label();
            this.label9 = new System.Windows.Forms.Label();
            this.label10 = new System.Windows.Forms.Label();
            this.singleTrackBar6 = new Decklink_Capture.SingleTrackBar();
            this.singleTrackBar5 = new Decklink_Capture.SingleTrackBar();
            this.singleTrackBar4 = new Decklink_Capture.SingleTrackBar();
            this.singleTrackBar3 = new Decklink_Capture.SingleTrackBar();
            this.singleTrackBar2 = new Decklink_Capture.SingleTrackBar();
            this.singleTrackBar1 = new Decklink_Capture.SingleTrackBar();
            this.label1 = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.checkBox1 = new System.Windows.Forms.CheckBox();
            this._chkLink56 = new System.Windows.Forms.CheckBox();
            this._chkLink34 = new System.Windows.Forms.CheckBox();
            this._chkLink12 = new System.Windows.Forms.CheckBox();
            this._chkEnableChannel6 = new System.Windows.Forms.CheckBox();
            this._chkEnableChannel5 = new System.Windows.Forms.CheckBox();
            this._chkEnableChannel4 = new System.Windows.Forms.CheckBox();
            this._chkEnableChannel3 = new System.Windows.Forms.CheckBox();
            this._chkEnableChannel2 = new System.Windows.Forms.CheckBox();
            this._chkEnableChannel1 = new System.Windows.Forms.CheckBox();
            this.SuspendLayout();
            // 
            // label11
            // 
            this.label11.Font = new System.Drawing.Font("Verdana", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.label11.ForeColor = System.Drawing.Color.Black;
            this.label11.Location = new System.Drawing.Point(3, 189);
            this.label11.Name = "label11";
            this.label11.Size = new System.Drawing.Size(25, 16);
            this.label11.TabIndex = 22;
            this.label11.Text = "0";
            this.label11.TextAlign = System.Drawing.ContentAlignment.TopRight;
            // 
            // label7
            // 
            this.label7.Font = new System.Drawing.Font("Verdana", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.label7.ForeColor = System.Drawing.Color.Black;
            this.label7.Location = new System.Drawing.Point(3, 44);
            this.label7.Name = "label7";
            this.label7.Size = new System.Drawing.Size(25, 16);
            this.label7.TabIndex = 23;
            this.label7.Text = "5";
            this.label7.TextAlign = System.Drawing.ContentAlignment.TopRight;
            // 
            // label8
            // 
            this.label8.Font = new System.Drawing.Font("Verdana", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.label8.ForeColor = System.Drawing.Color.Black;
            this.label8.Location = new System.Drawing.Point(3, 159);
            this.label8.Name = "label8";
            this.label8.Size = new System.Drawing.Size(25, 16);
            this.label8.TabIndex = 24;
            this.label8.Text = "1";
            this.label8.TextAlign = System.Drawing.ContentAlignment.TopRight;
            // 
            // label9
            // 
            this.label9.Font = new System.Drawing.Font("Verdana", 6.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.label9.ForeColor = System.Drawing.Color.Black;
            this.label9.Location = new System.Drawing.Point(3, 131);
            this.label9.Name = "label9";
            this.label9.Size = new System.Drawing.Size(25, 16);
            this.label9.TabIndex = 25;
            this.label9.Text = "2";
            this.label9.TextAlign = System.Drawing.ContentAlignment.TopRight;
            // 
            // label10
            // 
            this.label10.Font = new System.Drawing.Font("Verdana", 6.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.label10.ForeColor = System.Drawing.Color.Black;
            this.label10.Location = new System.Drawing.Point(3, 72);
            this.label10.Name = "label10";
            this.label10.Size = new System.Drawing.Size(25, 16);
            this.label10.TabIndex = 26;
            this.label10.Text = "4";
            this.label10.TextAlign = System.Drawing.ContentAlignment.TopRight;
            // 
            // singleTrackBar6
            // 
            this.singleTrackBar6.Location = new System.Drawing.Point(216, 3);
            this.singleTrackBar6.Max = 6D;
            this.singleTrackBar6.Min = 0D;
            this.singleTrackBar6.Name = "singleTrackBar6";
            this.singleTrackBar6.ScaleType = Decklink_Capture.TrackBarScaleEnum.Linear;
            this.singleTrackBar6.Size = new System.Drawing.Size(30, 210);
            this.singleTrackBar6.TabIndex = 5;
            this.singleTrackBar6.TextColor = System.Drawing.Color.Black;
            this.singleTrackBar6.TickColor = System.Drawing.Color.Silver;
            this.singleTrackBar6.TickFrequency = 12;
            this.singleTrackBar6.Value = 1D;
            this.singleTrackBar6.ValueChanged += new Decklink_Capture.TrackBarHandler(this.singleTrackBar1_ValueChanged);
            // 
            // singleTrackBar5
            // 
            this.singleTrackBar5.Location = new System.Drawing.Point(179, 3);
            this.singleTrackBar5.Max = 6D;
            this.singleTrackBar5.Min = 0D;
            this.singleTrackBar5.Name = "singleTrackBar5";
            this.singleTrackBar5.ScaleType = Decklink_Capture.TrackBarScaleEnum.Linear;
            this.singleTrackBar5.Size = new System.Drawing.Size(30, 210);
            this.singleTrackBar5.TabIndex = 4;
            this.singleTrackBar5.TextColor = System.Drawing.Color.Black;
            this.singleTrackBar5.TickColor = System.Drawing.Color.Silver;
            this.singleTrackBar5.TickFrequency = 12;
            this.singleTrackBar5.Value = 1D;
            this.singleTrackBar5.ValueChanged += new Decklink_Capture.TrackBarHandler(this.singleTrackBar1_ValueChanged);
            // 
            // singleTrackBar4
            // 
            this.singleTrackBar4.Location = new System.Drawing.Point(144, 3);
            this.singleTrackBar4.Max = 6D;
            this.singleTrackBar4.Min = 0D;
            this.singleTrackBar4.Name = "singleTrackBar4";
            this.singleTrackBar4.ScaleType = Decklink_Capture.TrackBarScaleEnum.Linear;
            this.singleTrackBar4.Size = new System.Drawing.Size(30, 210);
            this.singleTrackBar4.TabIndex = 3;
            this.singleTrackBar4.TextColor = System.Drawing.Color.Black;
            this.singleTrackBar4.TickColor = System.Drawing.Color.Silver;
            this.singleTrackBar4.TickFrequency = 12;
            this.singleTrackBar4.Value = 1D;
            this.singleTrackBar4.ValueChanged += new Decklink_Capture.TrackBarHandler(this.singleTrackBar1_ValueChanged);
            // 
            // singleTrackBar3
            // 
            this.singleTrackBar3.Location = new System.Drawing.Point(108, 3);
            this.singleTrackBar3.Max = 6D;
            this.singleTrackBar3.Min = 0D;
            this.singleTrackBar3.Name = "singleTrackBar3";
            this.singleTrackBar3.ScaleType = Decklink_Capture.TrackBarScaleEnum.Linear;
            this.singleTrackBar3.Size = new System.Drawing.Size(30, 210);
            this.singleTrackBar3.TabIndex = 2;
            this.singleTrackBar3.TextColor = System.Drawing.Color.Black;
            this.singleTrackBar3.TickColor = System.Drawing.Color.Silver;
            this.singleTrackBar3.TickFrequency = 12;
            this.singleTrackBar3.Value = 1D;
            this.singleTrackBar3.ValueChanged += new Decklink_Capture.TrackBarHandler(this.singleTrackBar1_ValueChanged);
            // 
            // singleTrackBar2
            // 
            this.singleTrackBar2.Location = new System.Drawing.Point(70, 3);
            this.singleTrackBar2.Max = 6D;
            this.singleTrackBar2.Min = 0D;
            this.singleTrackBar2.Name = "singleTrackBar2";
            this.singleTrackBar2.ScaleType = Decklink_Capture.TrackBarScaleEnum.Linear;
            this.singleTrackBar2.Size = new System.Drawing.Size(30, 210);
            this.singleTrackBar2.TabIndex = 1;
            this.singleTrackBar2.TextColor = System.Drawing.Color.Black;
            this.singleTrackBar2.TickColor = System.Drawing.Color.Silver;
            this.singleTrackBar2.TickFrequency = 12;
            this.singleTrackBar2.Value = 1D;
            this.singleTrackBar2.ValueChanged += new Decklink_Capture.TrackBarHandler(this.singleTrackBar1_ValueChanged);
            // 
            // singleTrackBar1
            // 
            this.singleTrackBar1.Location = new System.Drawing.Point(34, 3);
            this.singleTrackBar1.Max = 6D;
            this.singleTrackBar1.Min = 0D;
            this.singleTrackBar1.Name = "singleTrackBar1";
            this.singleTrackBar1.ScaleType = Decklink_Capture.TrackBarScaleEnum.Linear;
            this.singleTrackBar1.Size = new System.Drawing.Size(30, 210);
            this.singleTrackBar1.TabIndex = 0;
            this.singleTrackBar1.TextColor = System.Drawing.Color.Black;
            this.singleTrackBar1.TickColor = System.Drawing.Color.Silver;
            this.singleTrackBar1.TickFrequency = 12;
            this.singleTrackBar1.Value = 1D;
            this.singleTrackBar1.ValueChanged += new Decklink_Capture.TrackBarHandler(this.singleTrackBar1_ValueChanged);
            // 
            // label1
            // 
            this.label1.Font = new System.Drawing.Font("Verdana", 6.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.label1.ForeColor = System.Drawing.Color.Black;
            this.label1.Location = new System.Drawing.Point(3, 102);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(25, 16);
            this.label1.TabIndex = 36;
            this.label1.Text = "3";
            this.label1.TextAlign = System.Drawing.ContentAlignment.TopRight;
            // 
            // label2
            // 
            this.label2.Font = new System.Drawing.Font("Verdana", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.label2.ForeColor = System.Drawing.Color.Black;
            this.label2.Location = new System.Drawing.Point(4, 15);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(25, 16);
            this.label2.TabIndex = 37;
            this.label2.Text = "6";
            this.label2.TextAlign = System.Drawing.ContentAlignment.TopRight;
            // 
            // checkBox1
            // 
            this.checkBox1.Appearance = System.Windows.Forms.Appearance.Button;
            this.checkBox1.AutoSize = true;
            this.checkBox1.BackColor = System.Drawing.Color.Gray;
            this.checkBox1.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.checkBox1.Image = global::Decklink_Capture.Properties.Resources.mute;
            this.checkBox1.Location = new System.Drawing.Point(3, 217);
            this.checkBox1.Name = "checkBox1";
            this.checkBox1.Size = new System.Drawing.Size(22, 22);
            this.checkBox1.TabIndex = 38;
            this.checkBox1.TabStop = false;
            this.checkBox1.UseVisualStyleBackColor = false;
            this.checkBox1.CheckedChanged += new System.EventHandler(this.checkBox1_CheckedChanged);
            // 
            // _chkLink56
            // 
            this._chkLink56.Appearance = System.Windows.Forms.Appearance.Button;
            this._chkLink56.AutoSize = true;
            this._chkLink56.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this._chkLink56.Image = global::Decklink_Capture.Properties.Resources.link;
            this._chkLink56.Location = new System.Drawing.Point(200, 245);
            this._chkLink56.Name = "_chkLink56";
            this._chkLink56.Size = new System.Drawing.Size(22, 22);
            this._chkLink56.TabIndex = 35;
            this._chkLink56.TabStop = false;
            this._chkLink56.UseVisualStyleBackColor = true;
            this._chkLink56.CheckedChanged += new System.EventHandler(this._chkLink12_CheckedChanged);
            // 
            // _chkLink34
            // 
            this._chkLink34.Appearance = System.Windows.Forms.Appearance.Button;
            this._chkLink34.AutoSize = true;
            this._chkLink34.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this._chkLink34.Image = global::Decklink_Capture.Properties.Resources.link;
            this._chkLink34.Location = new System.Drawing.Point(130, 245);
            this._chkLink34.Name = "_chkLink34";
            this._chkLink34.Size = new System.Drawing.Size(22, 22);
            this._chkLink34.TabIndex = 34;
            this._chkLink34.TabStop = false;
            this._chkLink34.UseVisualStyleBackColor = true;
            this._chkLink34.CheckedChanged += new System.EventHandler(this._chkLink12_CheckedChanged);
            // 
            // _chkLink12
            // 
            this._chkLink12.Appearance = System.Windows.Forms.Appearance.Button;
            this._chkLink12.AutoSize = true;
            this._chkLink12.Checked = true;
            this._chkLink12.CheckState = System.Windows.Forms.CheckState.Checked;
            this._chkLink12.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this._chkLink12.Image = global::Decklink_Capture.Properties.Resources.link;
            this._chkLink12.Location = new System.Drawing.Point(56, 245);
            this._chkLink12.Name = "_chkLink12";
            this._chkLink12.Size = new System.Drawing.Size(22, 22);
            this._chkLink12.TabIndex = 33;
            this._chkLink12.TabStop = false;
            this._chkLink12.UseVisualStyleBackColor = true;
            this._chkLink12.CheckedChanged += new System.EventHandler(this._chkLink12_CheckedChanged);
            // 
            // _chkEnableChannel6
            // 
            this._chkEnableChannel6.Appearance = System.Windows.Forms.Appearance.Button;
            this._chkEnableChannel6.AutoSize = true;
            this._chkEnableChannel6.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this._chkEnableChannel6.Image = global::Decklink_Capture.Properties.Resources.headphones;
            this._chkEnableChannel6.Location = new System.Drawing.Point(219, 217);
            this._chkEnableChannel6.Name = "_chkEnableChannel6";
            this._chkEnableChannel6.Size = new System.Drawing.Size(22, 22);
            this._chkEnableChannel6.TabIndex = 32;
            this._chkEnableChannel6.TabStop = false;
            this._chkEnableChannel6.UseVisualStyleBackColor = true;
            this._chkEnableChannel6.CheckedChanged += new System.EventHandler(this._chkDisableChannel1_CheckedChanged);
            // 
            // _chkEnableChannel5
            // 
            this._chkEnableChannel5.Appearance = System.Windows.Forms.Appearance.Button;
            this._chkEnableChannel5.AutoSize = true;
            this._chkEnableChannel5.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this._chkEnableChannel5.Image = global::Decklink_Capture.Properties.Resources.headphones;
            this._chkEnableChannel5.Location = new System.Drawing.Point(183, 217);
            this._chkEnableChannel5.Name = "_chkEnableChannel5";
            this._chkEnableChannel5.Size = new System.Drawing.Size(22, 22);
            this._chkEnableChannel5.TabIndex = 31;
            this._chkEnableChannel5.TabStop = false;
            this._chkEnableChannel5.UseVisualStyleBackColor = true;
            this._chkEnableChannel5.CheckedChanged += new System.EventHandler(this._chkDisableChannel1_CheckedChanged);
            // 
            // _chkEnableChannel4
            // 
            this._chkEnableChannel4.Appearance = System.Windows.Forms.Appearance.Button;
            this._chkEnableChannel4.AutoSize = true;
            this._chkEnableChannel4.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this._chkEnableChannel4.Image = global::Decklink_Capture.Properties.Resources.headphones;
            this._chkEnableChannel4.Location = new System.Drawing.Point(148, 217);
            this._chkEnableChannel4.Name = "_chkEnableChannel4";
            this._chkEnableChannel4.Size = new System.Drawing.Size(22, 22);
            this._chkEnableChannel4.TabIndex = 30;
            this._chkEnableChannel4.TabStop = false;
            this._chkEnableChannel4.UseVisualStyleBackColor = true;
            this._chkEnableChannel4.CheckedChanged += new System.EventHandler(this._chkDisableChannel1_CheckedChanged);
            // 
            // _chkEnableChannel3
            // 
            this._chkEnableChannel3.Appearance = System.Windows.Forms.Appearance.Button;
            this._chkEnableChannel3.AutoSize = true;
            this._chkEnableChannel3.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this._chkEnableChannel3.Image = global::Decklink_Capture.Properties.Resources.headphones;
            this._chkEnableChannel3.Location = new System.Drawing.Point(112, 217);
            this._chkEnableChannel3.Name = "_chkEnableChannel3";
            this._chkEnableChannel3.Size = new System.Drawing.Size(22, 22);
            this._chkEnableChannel3.TabIndex = 29;
            this._chkEnableChannel3.TabStop = false;
            this._chkEnableChannel3.UseVisualStyleBackColor = true;
            this._chkEnableChannel3.CheckedChanged += new System.EventHandler(this._chkDisableChannel1_CheckedChanged);
            // 
            // _chkEnableChannel2
            // 
            this._chkEnableChannel2.Appearance = System.Windows.Forms.Appearance.Button;
            this._chkEnableChannel2.AutoSize = true;
            this._chkEnableChannel2.Checked = true;
            this._chkEnableChannel2.CheckState = System.Windows.Forms.CheckState.Checked;
            this._chkEnableChannel2.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this._chkEnableChannel2.Image = global::Decklink_Capture.Properties.Resources.headphones;
            this._chkEnableChannel2.Location = new System.Drawing.Point(74, 217);
            this._chkEnableChannel2.Name = "_chkEnableChannel2";
            this._chkEnableChannel2.Size = new System.Drawing.Size(22, 22);
            this._chkEnableChannel2.TabIndex = 28;
            this._chkEnableChannel2.TabStop = false;
            this._chkEnableChannel2.UseVisualStyleBackColor = true;
            this._chkEnableChannel2.CheckedChanged += new System.EventHandler(this._chkDisableChannel1_CheckedChanged);
            // 
            // _chkEnableChannel1
            // 
            this._chkEnableChannel1.Appearance = System.Windows.Forms.Appearance.Button;
            this._chkEnableChannel1.AutoSize = true;
            this._chkEnableChannel1.BackColor = System.Drawing.Color.Gray;
            this._chkEnableChannel1.Checked = true;
            this._chkEnableChannel1.CheckState = System.Windows.Forms.CheckState.Checked;
            this._chkEnableChannel1.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this._chkEnableChannel1.Image = global::Decklink_Capture.Properties.Resources.headphones;
            this._chkEnableChannel1.Location = new System.Drawing.Point(38, 217);
            this._chkEnableChannel1.Name = "_chkEnableChannel1";
            this._chkEnableChannel1.Size = new System.Drawing.Size(22, 22);
            this._chkEnableChannel1.TabIndex = 27;
            this._chkEnableChannel1.TabStop = false;
            this._chkEnableChannel1.UseVisualStyleBackColor = false;
            this._chkEnableChannel1.CheckedChanged += new System.EventHandler(this._chkDisableChannel1_CheckedChanged);
            // 
            // SixChannelTrackbar
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.Color.Gray;
            this.Controls.Add(this.checkBox1);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.label1);
            this.Controls.Add(this._chkLink56);
            this.Controls.Add(this._chkLink34);
            this.Controls.Add(this._chkLink12);
            this.Controls.Add(this._chkEnableChannel6);
            this.Controls.Add(this._chkEnableChannel5);
            this.Controls.Add(this._chkEnableChannel4);
            this.Controls.Add(this._chkEnableChannel3);
            this.Controls.Add(this._chkEnableChannel2);
            this.Controls.Add(this._chkEnableChannel1);
            this.Controls.Add(this.label10);
            this.Controls.Add(this.label9);
            this.Controls.Add(this.label8);
            this.Controls.Add(this.label7);
            this.Controls.Add(this.label11);
            this.Controls.Add(this.singleTrackBar6);
            this.Controls.Add(this.singleTrackBar5);
            this.Controls.Add(this.singleTrackBar4);
            this.Controls.Add(this.singleTrackBar3);
            this.Controls.Add(this.singleTrackBar2);
            this.Controls.Add(this.singleTrackBar1);
            this.Name = "SixChannelTrackbar";
            this.Size = new System.Drawing.Size(253, 270);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private SingleTrackBar singleTrackBar1;
        private SingleTrackBar singleTrackBar2;
        private SingleTrackBar singleTrackBar3;
        private SingleTrackBar singleTrackBar4;
        private SingleTrackBar singleTrackBar5;
        private SingleTrackBar singleTrackBar6;
        private System.Windows.Forms.Label label11;
        private System.Windows.Forms.Label label7;
        private System.Windows.Forms.Label label8;
        private System.Windows.Forms.Label label9;
        private System.Windows.Forms.Label label10;
        private System.Windows.Forms.CheckBox _chkEnableChannel1;
        private System.Windows.Forms.CheckBox _chkEnableChannel2;
        private System.Windows.Forms.CheckBox _chkEnableChannel3;
        private System.Windows.Forms.CheckBox _chkEnableChannel4;
        private System.Windows.Forms.CheckBox _chkEnableChannel5;
        private System.Windows.Forms.CheckBox _chkEnableChannel6;
        private System.Windows.Forms.CheckBox _chkLink12;
        private System.Windows.Forms.CheckBox _chkLink34;
        private System.Windows.Forms.CheckBox _chkLink56;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.CheckBox checkBox1;

    }
}
