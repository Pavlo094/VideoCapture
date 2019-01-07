namespace Decklink_Capture
{
    partial class frmVideoTrimmer
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

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.components = new System.ComponentModel.Container();
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(frmVideoTrimmer));
            this.splitContainer1 = new System.Windows.Forms.SplitContainer();
            this._lblInfo = new System.Windows.Forms.Label();
            this._chkLoop = new System.Windows.Forms.CheckBox();
            this.rangeBar1 = new Decklink_Capture.RangeBar();
            this.btnCapture = new System.Windows.Forms.Button();
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.ucVUMeter1 = new Decklink_Capture.ucVUMeter();
            this.sixChannelTrackbar1 = new Decklink_Capture.SixChannelTrackbar();
            this._pnlPreview = new System.Windows.Forms.Panel();
            this._btnBrowseOutputClip = new System.Windows.Forms.Button();
            this._btnExport = new System.Windows.Forms.Button();
            this._txtExportClip = new System.Windows.Forms.TextBox();
            this._lblExport = new System.Windows.Forms.Label();
            this._btnBrowseInputClip = new System.Windows.Forms.Button();
            this._txtInputClip = new System.Windows.Forms.TextBox();
            this.label1 = new System.Windows.Forms.Label();
            this.pictureBox1 = new System.Windows.Forms.PictureBox();
            this._timer = new System.Windows.Forms.Timer(this.components);
            this.toolTipButton = new System.Windows.Forms.ToolTip(this.components);
            ((System.ComponentModel.ISupportInitialize)(this.splitContainer1)).BeginInit();
            this.splitContainer1.Panel1.SuspendLayout();
            this.splitContainer1.Panel2.SuspendLayout();
            this.splitContainer1.SuspendLayout();
            this.groupBox1.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox1)).BeginInit();
            this.SuspendLayout();
            // 
            // splitContainer1
            // 
            this.splitContainer1.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.splitContainer1.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
            this.splitContainer1.Location = new System.Drawing.Point(12, 12);
            this.splitContainer1.Name = "splitContainer1";
            // 
            // splitContainer1.Panel1
            // 
            this.splitContainer1.Panel1.BackColor = System.Drawing.Color.Gray;
            this.splitContainer1.Panel1.Controls.Add(this._lblInfo);
            this.splitContainer1.Panel1.Controls.Add(this._chkLoop);
            this.splitContainer1.Panel1.Controls.Add(this.rangeBar1);
            this.splitContainer1.Panel1.Controls.Add(this.btnCapture);
            this.splitContainer1.Panel1.Controls.Add(this.groupBox1);
            this.splitContainer1.Panel1.Controls.Add(this._pnlPreview);
            this.splitContainer1.Panel1.Resize += new System.EventHandler(this.splitContainer1_Panel1_Resize);
            // 
            // splitContainer1.Panel2
            // 
            this.splitContainer1.Panel2.BackColor = System.Drawing.Color.Gray;
            this.splitContainer1.Panel2.Controls.Add(this._btnBrowseOutputClip);
            this.splitContainer1.Panel2.Controls.Add(this._btnExport);
            this.splitContainer1.Panel2.Controls.Add(this._txtExportClip);
            this.splitContainer1.Panel2.Controls.Add(this._lblExport);
            this.splitContainer1.Panel2.Controls.Add(this._btnBrowseInputClip);
            this.splitContainer1.Panel2.Controls.Add(this._txtInputClip);
            this.splitContainer1.Panel2.Controls.Add(this.label1);
            this.splitContainer1.Panel2.Controls.Add(this.pictureBox1);
            this.splitContainer1.Size = new System.Drawing.Size(827, 680);
            this.splitContainer1.SplitterDistance = 496;
            this.splitContainer1.TabIndex = 9;
            // 
            // _lblInfo
            // 
            this._lblInfo.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this._lblInfo.Location = new System.Drawing.Point(83, 310);
            this._lblInfo.Name = "_lblInfo";
            this._lblInfo.Size = new System.Drawing.Size(406, 15);
            this._lblInfo.TabIndex = 10;
            // 
            // _chkLoop
            // 
            this._chkLoop.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this._chkLoop.Appearance = System.Windows.Forms.Appearance.Button;
            this._chkLoop.AutoSize = true;
            this._chkLoop.Checked = true;
            this._chkLoop.CheckState = System.Windows.Forms.CheckState.Checked;
            this._chkLoop.Enabled = false;
            this._chkLoop.Image = ((System.Drawing.Image)(resources.GetObject("_chkLoop.Image")));
            this._chkLoop.Location = new System.Drawing.Point(55, 305);
            this._chkLoop.Name = "_chkLoop";
            this._chkLoop.Size = new System.Drawing.Size(22, 22);
            this._chkLoop.TabIndex = 9;
            this._chkLoop.UseVisualStyleBackColor = true;
            // 
            // rangeBar1
            // 
            this.rangeBar1.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.rangeBar1.DivisionNum = 10;
            this.rangeBar1.DrawPointer = true;
            this.rangeBar1.Enabled = false;
            this.rangeBar1.HeightOfBar = 8;
            this.rangeBar1.HeightOfMark = 24;
            this.rangeBar1.HeightOfTick = 6;
            this.rangeBar1.InnerColor = System.Drawing.Color.LightGreen;
            this.rangeBar1.Location = new System.Drawing.Point(3, 328);
            this.rangeBar1.Name = "rangeBar1";
            this.rangeBar1.Orientation = Decklink_Capture.RangeBar.RangeBarOrientation.horizontal;
            this.rangeBar1.PointerPosition = 0;
            this.rangeBar1.RangeMaximum = 5;
            this.rangeBar1.RangeMinimum = 3;
            this.rangeBar1.ScaleOrientation = Decklink_Capture.RangeBar.TopBottomOrientation.bottom;
            this.rangeBar1.Size = new System.Drawing.Size(486, 51);
            this.rangeBar1.TabIndex = 8;
            this.rangeBar1.TotalMaximum = 10;
            this.rangeBar1.TotalMinimum = 0;
            this.rangeBar1.LeftRangeChanging += new Decklink_Capture.RangeBar.RangeChangedEventHandler(this.rangeBar1_LeftRangeChanging);
            this.rangeBar1.PointerPositionChanged += new System.EventHandler(this.rangeBar1_PointerPositionChanged);
            this.rangeBar1.RightRangeChanging += new Decklink_Capture.RangeBar.RangeChangedEventHandler(this.rangeBar1_RightRangeChanging);
            // 
            // btnCapture
            // 
            this.btnCapture.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.btnCapture.Enabled = false;
            this.btnCapture.Image = global::Decklink_Capture.Properties.Resources.play;
            this.btnCapture.Location = new System.Drawing.Point(3, 304);
            this.btnCapture.Name = "btnCapture";
            this.btnCapture.Size = new System.Drawing.Size(46, 23);
            this.btnCapture.TabIndex = 7;
            this.btnCapture.TextImageRelation = System.Windows.Forms.TextImageRelation.ImageBeforeText;
            this.btnCapture.UseVisualStyleBackColor = true;
            this.btnCapture.Click += new System.EventHandler(this.btnCapture_Click);
            // 
            // groupBox1
            // 
            this.groupBox1.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.groupBox1.BackColor = System.Drawing.Color.Gray;
            this.groupBox1.Controls.Add(this.ucVUMeter1);
            this.groupBox1.Controls.Add(this.sixChannelTrackbar1);
            this.groupBox1.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.groupBox1.Location = new System.Drawing.Point(3, 385);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(469, 288);
            this.groupBox1.TabIndex = 6;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "Audio";
            // 
            // ucVUMeter1
            // 
            this.ucVUMeter1.BackColor = System.Drawing.Color.Gray;
            this.ucVUMeter1.BorderColorLeftTop = System.Drawing.Color.LightGray;
            this.ucVUMeter1.BorderColorRightBottom = System.Drawing.Color.DarkGray;
            this.ucVUMeter1.Channel = 6;
            this.ucVUMeter1.FillColorFirst = System.Drawing.Color.FromArgb(((int)(((byte)(64)))), ((int)(((byte)(64)))), ((int)(((byte)(64)))));
            this.ucVUMeter1.FillColorSecond = System.Drawing.Color.Gray;
            this.ucVUMeter1.Font = new System.Drawing.Font("Verdana", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.ucVUMeter1.ForeColor = System.Drawing.Color.Gainsboro;
            this.ucVUMeter1.Location = new System.Drawing.Point(269, 14);
            this.ucVUMeter1.Name = "ucVUMeter1";
            this.ucVUMeter1.Size = new System.Drawing.Size(183, 228);
            this.ucVUMeter1.TabIndex = 1;
            // 
            // sixChannelTrackbar1
            // 
            this.sixChannelTrackbar1.BackColor = System.Drawing.Color.Gray;
            this.sixChannelTrackbar1.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.sixChannelTrackbar1.Location = new System.Drawing.Point(6, 14);
            this.sixChannelTrackbar1.Name = "sixChannelTrackbar1";
            this.sixChannelTrackbar1.Size = new System.Drawing.Size(257, 268);
            this.sixChannelTrackbar1.TabIndex = 0;
            this.sixChannelTrackbar1.ValueChanged += new Decklink_Capture.SixChannelsTrackBarHandler(this.sixChannelTrackbar1_ValueChanged);
            // 
            // _pnlPreview
            // 
            this._pnlPreview.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this._pnlPreview.BackColor = System.Drawing.Color.Black;
            this._pnlPreview.Location = new System.Drawing.Point(3, 3);
            this._pnlPreview.Name = "_pnlPreview";
            this._pnlPreview.Size = new System.Drawing.Size(486, 295);
            this._pnlPreview.TabIndex = 0;
            this._pnlPreview.Resize += new System.EventHandler(this._pnlPreview_Resize);
            // 
            // _btnBrowseOutputClip
            // 
            this._btnBrowseOutputClip.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this._btnBrowseOutputClip.Location = new System.Drawing.Point(295, 73);
            this._btnBrowseOutputClip.Name = "_btnBrowseOutputClip";
            this._btnBrowseOutputClip.Size = new System.Drawing.Size(25, 23);
            this._btnBrowseOutputClip.TabIndex = 26;
            this._btnBrowseOutputClip.Text = "...";
            this._btnBrowseOutputClip.UseVisualStyleBackColor = true;
            this._btnBrowseOutputClip.Click += new System.EventHandler(this._btnBrowseOutputClip_Click);
            // 
            // _btnExport
            // 
            this._btnExport.Enabled = false;
            this._btnExport.Location = new System.Drawing.Point(3, 101);
            this._btnExport.Name = "_btnExport";
            this._btnExport.Size = new System.Drawing.Size(75, 23);
            this._btnExport.TabIndex = 25;
            this._btnExport.Text = "Export";
            this._btnExport.UseVisualStyleBackColor = true;
            this._btnExport.Click += new System.EventHandler(this._btnExport_Click);
            // 
            // _txtExportClip
            // 
            this._txtExportClip.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this._txtExportClip.Enabled = false;
            this._txtExportClip.Location = new System.Drawing.Point(3, 75);
            this._txtExportClip.Name = "_txtExportClip";
            this._txtExportClip.ReadOnly = true;
            this._txtExportClip.Size = new System.Drawing.Size(286, 20);
            this._txtExportClip.TabIndex = 23;
            // 
            // _lblExport
            // 
            this._lblExport.AutoSize = true;
            this._lblExport.Enabled = false;
            this._lblExport.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this._lblExport.Location = new System.Drawing.Point(3, 59);
            this._lblExport.Name = "_lblExport";
            this._lblExport.Size = new System.Drawing.Size(68, 13);
            this._lblExport.TabIndex = 24;
            this._lblExport.Text = "Export Clip";
            // 
            // _btnBrowseInputClip
            // 
            this._btnBrowseInputClip.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this._btnBrowseInputClip.Location = new System.Drawing.Point(295, 28);
            this._btnBrowseInputClip.Name = "_btnBrowseInputClip";
            this._btnBrowseInputClip.Size = new System.Drawing.Size(25, 23);
            this._btnBrowseInputClip.TabIndex = 22;
            this._btnBrowseInputClip.Text = "...";
            this._btnBrowseInputClip.UseVisualStyleBackColor = true;
            this._btnBrowseInputClip.Click += new System.EventHandler(this._btnBrowseInputClip_Click);
            // 
            // _txtInputClip
            // 
            this._txtInputClip.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this._txtInputClip.Location = new System.Drawing.Point(3, 30);
            this._txtInputClip.Name = "_txtInputClip";
            this._txtInputClip.ReadOnly = true;
            this._txtInputClip.Size = new System.Drawing.Size(286, 20);
            this._txtInputClip.TabIndex = 20;
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.label1.Location = new System.Drawing.Point(3, 14);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(61, 13);
            this.label1.TabIndex = 21;
            this.label1.Text = "Input Clip";
            // 
            // pictureBox1
            // 
            this.pictureBox1.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.pictureBox1.Image = global::Decklink_Capture.Properties.Resources.BTFP_Logo;
            this.pictureBox1.Location = new System.Drawing.Point(3, 280);
            this.pictureBox1.Name = "pictureBox1";
            this.pictureBox1.Size = new System.Drawing.Size(317, 187);
            this.pictureBox1.SizeMode = System.Windows.Forms.PictureBoxSizeMode.Zoom;
            this.pictureBox1.TabIndex = 19;
            this.pictureBox1.TabStop = false;
            // 
            // _timer
            // 
            this._timer.Interval = 200;
            this._timer.Tick += new System.EventHandler(this._timer_Tick);
            // 
            // frmVideoTrimmer
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.Color.DarkGray;
            this.ClientSize = new System.Drawing.Size(851, 704);
            this.Controls.Add(this.splitContainer1);
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.Name = "frmVideoTrimmer";
            this.Text = "Trimmer";
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.frmVideoTrimmer_FormClosing);
            this.Load += new System.EventHandler(this.frmVideoTrimmer_Load);
            this.splitContainer1.Panel1.ResumeLayout(false);
            this.splitContainer1.Panel1.PerformLayout();
            this.splitContainer1.Panel2.ResumeLayout(false);
            this.splitContainer1.Panel2.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.splitContainer1)).EndInit();
            this.splitContainer1.ResumeLayout(false);
            this.groupBox1.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox1)).EndInit();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.SplitContainer splitContainer1;
        private System.Windows.Forms.PictureBox pictureBox1;
        private System.Windows.Forms.Panel _pnlPreview;
        private System.Windows.Forms.GroupBox groupBox1;
        private ucVUMeter ucVUMeter1;
        private SixChannelTrackbar sixChannelTrackbar1;
        private System.Windows.Forms.Button btnCapture;
        private RangeBar rangeBar1;
        private System.Windows.Forms.TextBox _txtInputClip;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Button _btnBrowseInputClip;
        private System.Windows.Forms.Timer _timer;
        private System.Windows.Forms.CheckBox _chkLoop;
        private System.Windows.Forms.Label _lblInfo;
        private System.Windows.Forms.ToolTip toolTipButton;
        private System.Windows.Forms.TextBox _txtExportClip;
        private System.Windows.Forms.Label _lblExport;
        private System.Windows.Forms.Button _btnExport;
        private System.Windows.Forms.Button _btnBrowseOutputClip;
    }
}