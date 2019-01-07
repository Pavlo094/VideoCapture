namespace Decklink_Capture
{
    partial class frmMain
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(frmMain));
            this.menuStrip1 = new System.Windows.Forms.MenuStrip();
            this.fileToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.exitToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.toolsToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.configurationToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.trimVideoToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.helpToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.aboutToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.ucVUMeter1 = new Decklink_Capture.ucVUMeter();
            this.sixChannelTrackbar1 = new Decklink_Capture.SixChannelTrackbar();
            this._txtFileName = new System.Windows.Forms.TextBox();
            this.label1 = new System.Windows.Forms.Label();
            this.splitContainer1 = new System.Windows.Forms.SplitContainer();
            this.pictureBox1 = new System.Windows.Forms.PictureBox();
            this._txtAudioFormat = new System.Windows.Forms.TextBox();
            this.label6 = new System.Windows.Forms.Label();
            this._txtInputVideoCard = new System.Windows.Forms.TextBox();
            this.label5 = new System.Windows.Forms.Label();
            this._btnBrowseOutputDir = new System.Windows.Forms.Button();
            this._txtVideoFormat = new System.Windows.Forms.TextBox();
            this.label3 = new System.Windows.Forms.Label();
            this._txtOutputDir = new System.Windows.Forms.TextBox();
            this.label2 = new System.Windows.Forms.Label();
            this._timer = new System.Windows.Forms.Timer(this.components);
            this._lblInfo = new System.Windows.Forms.Label();
            this._resourcesTimer = new System.Windows.Forms.Timer(this.components);
            this.groupBox2 = new System.Windows.Forms.GroupBox();
            this._btnTrimVideo = new System.Windows.Forms.Button();
            this._lblDriveFreeSpace = new System.Windows.Forms.Label();
            this._lblTotalRam = new System.Windows.Forms.Label();
            this._lblTotalCpu = new System.Windows.Forms.Label();
            this._lblAppRam = new System.Windows.Forms.Label();
            this._lblAppCpu = new System.Windows.Forms.Label();
            this.btnCapture = new System.Windows.Forms.Button();
            this._lblInfo2 = new System.Windows.Forms.Label();
            this.menuStrip1.SuspendLayout();
            this.groupBox1.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.splitContainer1)).BeginInit();
            this.splitContainer1.Panel2.SuspendLayout();
            this.splitContainer1.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox1)).BeginInit();
            this.groupBox2.SuspendLayout();
            this.SuspendLayout();
            // 
            // menuStrip1
            // 
            this.menuStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.fileToolStripMenuItem,
            this.toolsToolStripMenuItem,
            this.helpToolStripMenuItem});
            this.menuStrip1.Location = new System.Drawing.Point(0, 0);
            this.menuStrip1.Name = "menuStrip1";
            this.menuStrip1.Size = new System.Drawing.Size(788, 24);
            this.menuStrip1.TabIndex = 1;
            this.menuStrip1.Text = "menuStrip1";
            // 
            // fileToolStripMenuItem
            // 
            this.fileToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.exitToolStripMenuItem});
            this.fileToolStripMenuItem.Name = "fileToolStripMenuItem";
            this.fileToolStripMenuItem.Size = new System.Drawing.Size(37, 20);
            this.fileToolStripMenuItem.Text = "&File";
            // 
            // exitToolStripMenuItem
            // 
            this.exitToolStripMenuItem.Image = global::Decklink_Capture.Properties.Resources.exit;
            this.exitToolStripMenuItem.Name = "exitToolStripMenuItem";
            this.exitToolStripMenuItem.Size = new System.Drawing.Size(92, 22);
            this.exitToolStripMenuItem.Text = "&Exit";
            this.exitToolStripMenuItem.Click += new System.EventHandler(this.exitToolStripMenuItem_Click);
            // 
            // toolsToolStripMenuItem
            // 
            this.toolsToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.configurationToolStripMenuItem,
            this.trimVideoToolStripMenuItem});
            this.toolsToolStripMenuItem.Name = "toolsToolStripMenuItem";
            this.toolsToolStripMenuItem.Size = new System.Drawing.Size(47, 20);
            this.toolsToolStripMenuItem.Text = "&Tools";
            // 
            // configurationToolStripMenuItem
            // 
            this.configurationToolStripMenuItem.Image = global::Decklink_Capture.Properties.Resources.configuration;
            this.configurationToolStripMenuItem.Name = "configurationToolStripMenuItem";
            this.configurationToolStripMenuItem.Size = new System.Drawing.Size(157, 22);
            this.configurationToolStripMenuItem.Text = "&Configuration...";
            this.configurationToolStripMenuItem.Click += new System.EventHandler(this.configurationToolStripMenuItem_Click);
            // 
            // trimVideoToolStripMenuItem
            // 
            this.trimVideoToolStripMenuItem.Image = ((System.Drawing.Image)(resources.GetObject("trimVideoToolStripMenuItem.Image")));
            this.trimVideoToolStripMenuItem.Name = "trimVideoToolStripMenuItem";
            this.trimVideoToolStripMenuItem.Size = new System.Drawing.Size(157, 22);
            this.trimVideoToolStripMenuItem.Text = "Trim video...";
            this.trimVideoToolStripMenuItem.Click += new System.EventHandler(this.trimVideoToolStripMenuItem_Click);
            // 
            // helpToolStripMenuItem
            // 
            this.helpToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.aboutToolStripMenuItem});
            this.helpToolStripMenuItem.Name = "helpToolStripMenuItem";
            this.helpToolStripMenuItem.Size = new System.Drawing.Size(44, 20);
            this.helpToolStripMenuItem.Text = "&Help";
            // 
            // aboutToolStripMenuItem
            // 
            this.aboutToolStripMenuItem.Name = "aboutToolStripMenuItem";
            this.aboutToolStripMenuItem.Size = new System.Drawing.Size(116, 22);
            this.aboutToolStripMenuItem.Text = "&About...";
            this.aboutToolStripMenuItem.Click += new System.EventHandler(this.aboutToolStripMenuItem_Click);
            // 
            // groupBox1
            // 
            this.groupBox1.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.groupBox1.BackColor = System.Drawing.Color.Gray;
            this.groupBox1.Controls.Add(this.ucVUMeter1);
            this.groupBox1.Controls.Add(this.sixChannelTrackbar1);
            this.groupBox1.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.groupBox1.Location = new System.Drawing.Point(12, 375);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(469, 288);
            this.groupBox1.TabIndex = 5;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "AUDIO";
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
            // _txtFileName
            // 
            this._txtFileName.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this._txtFileName.Location = new System.Drawing.Point(111, 11);
            this._txtFileName.Name = "_txtFileName";
            this._txtFileName.Size = new System.Drawing.Size(163, 20);
            this._txtFileName.TabIndex = 6;
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.label1.Location = new System.Drawing.Point(3, 14);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(103, 13);
            this.label1.TabIndex = 7;
            this.label1.Text = "Output Filename:";
            // 
            // splitContainer1
            // 
            this.splitContainer1.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.splitContainer1.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
            this.splitContainer1.Location = new System.Drawing.Point(12, 27);
            this.splitContainer1.Name = "splitContainer1";
            // 
            // splitContainer1.Panel1
            // 
            this.splitContainer1.Panel1.BackColor = System.Drawing.Color.Black;
            this.splitContainer1.Panel1.Resize += new System.EventHandler(this.splitContainer1_Panel1_Resize);
            // 
            // splitContainer1.Panel2
            // 
            this.splitContainer1.Panel2.BackColor = System.Drawing.Color.Gray;
            this.splitContainer1.Panel2.Controls.Add(this.pictureBox1);
            this.splitContainer1.Panel2.Controls.Add(this._txtAudioFormat);
            this.splitContainer1.Panel2.Controls.Add(this.label6);
            this.splitContainer1.Panel2.Controls.Add(this._txtInputVideoCard);
            this.splitContainer1.Panel2.Controls.Add(this.label5);
            this.splitContainer1.Panel2.Controls.Add(this._btnBrowseOutputDir);
            this.splitContainer1.Panel2.Controls.Add(this._txtVideoFormat);
            this.splitContainer1.Panel2.Controls.Add(this.label3);
            this.splitContainer1.Panel2.Controls.Add(this._txtOutputDir);
            this.splitContainer1.Panel2.Controls.Add(this.label2);
            this.splitContainer1.Panel2.Controls.Add(this._txtFileName);
            this.splitContainer1.Panel2.Controls.Add(this.label1);
            this.splitContainer1.Size = new System.Drawing.Size(765, 313);
            this.splitContainer1.SplitterDistance = 470;
            this.splitContainer1.TabIndex = 8;
            // 
            // pictureBox1
            // 
            this.pictureBox1.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.pictureBox1.Image = global::Decklink_Capture.Properties.Resources.BTFP_Logo;
            this.pictureBox1.Location = new System.Drawing.Point(25, 183);
            this.pictureBox1.Name = "pictureBox1";
            this.pictureBox1.Size = new System.Drawing.Size(237, 108);
            this.pictureBox1.SizeMode = System.Windows.Forms.PictureBoxSizeMode.Zoom;
            this.pictureBox1.TabIndex = 19;
            this.pictureBox1.TabStop = false;
            // 
            // _txtAudioFormat
            // 
            this._txtAudioFormat.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this._txtAudioFormat.Location = new System.Drawing.Point(111, 116);
            this._txtAudioFormat.Name = "_txtAudioFormat";
            this._txtAudioFormat.ReadOnly = true;
            this._txtAudioFormat.Size = new System.Drawing.Size(163, 20);
            this._txtAudioFormat.TabIndex = 17;
            // 
            // label6
            // 
            this.label6.AutoSize = true;
            this.label6.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.label6.Location = new System.Drawing.Point(3, 119);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(85, 13);
            this.label6.TabIndex = 18;
            this.label6.Text = "Audio Format:";
            // 
            // _txtInputVideoCard
            // 
            this._txtInputVideoCard.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this._txtInputVideoCard.Location = new System.Drawing.Point(111, 64);
            this._txtInputVideoCard.Name = "_txtInputVideoCard";
            this._txtInputVideoCard.ReadOnly = true;
            this._txtInputVideoCard.Size = new System.Drawing.Size(163, 20);
            this._txtInputVideoCard.TabIndex = 15;
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.label5.Location = new System.Drawing.Point(3, 67);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(70, 13);
            this.label5.TabIndex = 16;
            this.label5.Text = "Input Card:";
            // 
            // _btnBrowseOutputDir
            // 
            this._btnBrowseOutputDir.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this._btnBrowseOutputDir.Location = new System.Drawing.Point(249, 36);
            this._btnBrowseOutputDir.Name = "_btnBrowseOutputDir";
            this._btnBrowseOutputDir.Size = new System.Drawing.Size(25, 23);
            this._btnBrowseOutputDir.TabIndex = 14;
            this._btnBrowseOutputDir.Text = "...";
            this._btnBrowseOutputDir.UseVisualStyleBackColor = true;
            this._btnBrowseOutputDir.Click += new System.EventHandler(this._btnBrowseOutputDir_Click);
            // 
            // _txtVideoFormat
            // 
            this._txtVideoFormat.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this._txtVideoFormat.Location = new System.Drawing.Point(111, 90);
            this._txtVideoFormat.Name = "_txtVideoFormat";
            this._txtVideoFormat.ReadOnly = true;
            this._txtVideoFormat.Size = new System.Drawing.Size(163, 20);
            this._txtVideoFormat.TabIndex = 10;
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.label3.Location = new System.Drawing.Point(3, 93);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(85, 13);
            this.label3.TabIndex = 11;
            this.label3.Text = "Video Format:";
            // 
            // _txtOutputDir
            // 
            this._txtOutputDir.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this._txtOutputDir.Location = new System.Drawing.Point(111, 37);
            this._txtOutputDir.Name = "_txtOutputDir";
            this._txtOutputDir.ReadOnly = true;
            this._txtOutputDir.Size = new System.Drawing.Size(132, 20);
            this._txtOutputDir.TabIndex = 8;
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.label2.Location = new System.Drawing.Point(3, 40);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(104, 13);
            this.label2.TabIndex = 9;
            this.label2.Text = "Output Directory:";
            // 
            // _timer
            // 
            this._timer.Interval = 200;
            this._timer.Tick += new System.EventHandler(this._timer_Tick);
            // 
            // _lblInfo
            // 
            this._lblInfo.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this._lblInfo.Location = new System.Drawing.Point(93, 346);
            this._lblInfo.Name = "_lblInfo";
            this._lblInfo.Size = new System.Drawing.Size(160, 18);
            this._lblInfo.TabIndex = 9;
            // 
            // _resourcesTimer
            // 
            this._resourcesTimer.Interval = 1000;
            this._resourcesTimer.Tick += new System.EventHandler(this._resourcesTimer_Tick);
            // 
            // groupBox2
            // 
            this.groupBox2.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.groupBox2.BackColor = System.Drawing.Color.Gray;
            this.groupBox2.Controls.Add(this._btnTrimVideo);
            this.groupBox2.Controls.Add(this._lblDriveFreeSpace);
            this.groupBox2.Controls.Add(this._lblTotalRam);
            this.groupBox2.Controls.Add(this._lblTotalCpu);
            this.groupBox2.Controls.Add(this._lblAppRam);
            this.groupBox2.Controls.Add(this._lblAppCpu);
            this.groupBox2.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.groupBox2.Location = new System.Drawing.Point(487, 375);
            this.groupBox2.Name = "groupBox2";
            this.groupBox2.Size = new System.Drawing.Size(290, 288);
            this.groupBox2.TabIndex = 10;
            this.groupBox2.TabStop = false;
            this.groupBox2.Text = "SYSTEM INFO";
            // 
            // _btnTrimVideo
            // 
            this._btnTrimVideo.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this._btnTrimVideo.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this._btnTrimVideo.Image = global::Decklink_Capture.Properties.Resources.trim_video1;
            this._btnTrimVideo.Location = new System.Drawing.Point(169, 259);
            this._btnTrimVideo.Name = "_btnTrimVideo";
            this._btnTrimVideo.Size = new System.Drawing.Size(115, 23);
            this._btnTrimVideo.TabIndex = 5;
            this._btnTrimVideo.Text = "Trim Video...";
            this._btnTrimVideo.TextImageRelation = System.Windows.Forms.TextImageRelation.ImageBeforeText;
            this._btnTrimVideo.UseVisualStyleBackColor = true;
            this._btnTrimVideo.Click += new System.EventHandler(this._btnTrimVideo_Click);
            // 
            // _lblDriveFreeSpace
            // 
            this._lblDriveFreeSpace.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this._lblDriveFreeSpace.Location = new System.Drawing.Point(6, 94);
            this._lblDriveFreeSpace.Name = "_lblDriveFreeSpace";
            this._lblDriveFreeSpace.Size = new System.Drawing.Size(247, 17);
            this._lblDriveFreeSpace.TabIndex = 4;
            this._lblDriveFreeSpace.Text = "a";
            // 
            // _lblTotalRam
            // 
            this._lblTotalRam.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this._lblTotalRam.Location = new System.Drawing.Point(6, 77);
            this._lblTotalRam.Name = "_lblTotalRam";
            this._lblTotalRam.Size = new System.Drawing.Size(247, 17);
            this._lblTotalRam.TabIndex = 3;
            this._lblTotalRam.Text = "a";
            // 
            // _lblTotalCpu
            // 
            this._lblTotalCpu.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this._lblTotalCpu.Location = new System.Drawing.Point(6, 60);
            this._lblTotalCpu.Name = "_lblTotalCpu";
            this._lblTotalCpu.Size = new System.Drawing.Size(247, 17);
            this._lblTotalCpu.TabIndex = 2;
            this._lblTotalCpu.Text = "a";
            // 
            // _lblAppRam
            // 
            this._lblAppRam.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this._lblAppRam.Location = new System.Drawing.Point(6, 43);
            this._lblAppRam.Name = "_lblAppRam";
            this._lblAppRam.Size = new System.Drawing.Size(247, 17);
            this._lblAppRam.TabIndex = 1;
            this._lblAppRam.Text = "a";
            // 
            // _lblAppCpu
            // 
            this._lblAppCpu.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this._lblAppCpu.Location = new System.Drawing.Point(6, 26);
            this._lblAppCpu.Name = "_lblAppCpu";
            this._lblAppCpu.Size = new System.Drawing.Size(247, 17);
            this._lblAppCpu.TabIndex = 0;
            this._lblAppCpu.Text = "a";
            // 
            // btnCapture
            // 
            this.btnCapture.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.btnCapture.Image = global::Decklink_Capture.Properties.Resources.record;
            this.btnCapture.Location = new System.Drawing.Point(12, 346);
            this.btnCapture.Name = "btnCapture";
            this.btnCapture.Size = new System.Drawing.Size(75, 23);
            this.btnCapture.TabIndex = 2;
            this.btnCapture.Text = "Record";
            this.btnCapture.TextImageRelation = System.Windows.Forms.TextImageRelation.ImageBeforeText;
            this.btnCapture.UseVisualStyleBackColor = true;
            this.btnCapture.Click += new System.EventHandler(this.btnCapture_Click);
            // 
            // _lblInfo2
            // 
            this._lblInfo2.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this._lblInfo2.Location = new System.Drawing.Point(259, 346);
            this._lblInfo2.Name = "_lblInfo2";
            this._lblInfo2.Size = new System.Drawing.Size(222, 18);
            this._lblInfo2.TabIndex = 11;
            // 
            // frmMain
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.Color.DarkGray;
            this.ClientSize = new System.Drawing.Size(788, 675);
            this.Controls.Add(this._lblInfo2);
            this.Controls.Add(this.groupBox2);
            this.Controls.Add(this._lblInfo);
            this.Controls.Add(this.splitContainer1);
            this.Controls.Add(this.groupBox1);
            this.Controls.Add(this.btnCapture);
            this.Controls.Add(this.menuStrip1);
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.MainMenuStrip = this.menuStrip1;
            this.Name = "frmMain";
            this.Text = "Multichannel Video Capture";
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.frmMain_FormClosing);
            this.Load += new System.EventHandler(this.frmMain_Load);
            this.menuStrip1.ResumeLayout(false);
            this.menuStrip1.PerformLayout();
            this.groupBox1.ResumeLayout(false);
            this.splitContainer1.Panel2.ResumeLayout(false);
            this.splitContainer1.Panel2.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.splitContainer1)).EndInit();
            this.splitContainer1.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox1)).EndInit();
            this.groupBox2.ResumeLayout(false);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.MenuStrip menuStrip1;
        private System.Windows.Forms.ToolStripMenuItem fileToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem exitToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem toolsToolStripMenuItem;
        private System.Windows.Forms.Button btnCapture;
        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.TextBox _txtFileName;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.SplitContainer splitContainer1;
        private System.Windows.Forms.TextBox _txtVideoFormat;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.TextBox _txtOutputDir;
        private System.Windows.Forms.Label label2;
        private SixChannelTrackbar sixChannelTrackbar1;
        private System.Windows.Forms.Button _btnBrowseOutputDir;
        private System.Windows.Forms.TextBox _txtAudioFormat;
        private System.Windows.Forms.Label label6;
        private System.Windows.Forms.TextBox _txtInputVideoCard;
        private System.Windows.Forms.Label label5;
        private ucVUMeter ucVUMeter1;
        private System.Windows.Forms.Timer _timer;
        private System.Windows.Forms.Label _lblInfo;
        private System.Windows.Forms.PictureBox pictureBox1;
        private System.Windows.Forms.Timer _resourcesTimer;
        private System.Windows.Forms.GroupBox groupBox2;
        private System.Windows.Forms.Label _lblAppRam;
        private System.Windows.Forms.Label _lblAppCpu;
        private System.Windows.Forms.Label _lblTotalCpu;
        private System.Windows.Forms.Label _lblTotalRam;
        private System.Windows.Forms.Label _lblDriveFreeSpace;
        private System.Windows.Forms.ToolStripMenuItem helpToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem aboutToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem configurationToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem trimVideoToolStripMenuItem;
        private System.Windows.Forms.Button _btnTrimVideo;
        private System.Windows.Forms.Label _lblInfo2;
    }
}

