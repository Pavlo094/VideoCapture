namespace BTFP_Video_Encoding
{
    partial class Form1
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(Form1));
            this.menuStrip1 = new System.Windows.Forms.MenuStrip();
            this.fileToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.exitToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.toolsToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.configurationToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.helpToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.aboutToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.label1 = new System.Windows.Forms.Label();
            this._txtMonitorFolder = new System.Windows.Forms.TextBox();
            this._lstFilesForEncoding = new System.Windows.Forms.ListView();
            this.columnHeader1 = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this.groupBox2 = new System.Windows.Forms.GroupBox();
            this._lblDriveFreeSpace = new System.Windows.Forms.Label();
            this._lblTotalRam = new System.Windows.Forms.Label();
            this._lblTotalCpu = new System.Windows.Forms.Label();
            this._lblAppRam = new System.Windows.Forms.Label();
            this._lblAppCpu = new System.Windows.Forms.Label();
            this._lstEncodedFiles = new System.Windows.Forms.ListView();
            this.columnHeader2 = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this._encodedFilesImageList = new System.Windows.Forms.ImageList(this.components);
            this.label2 = new System.Windows.Forms.Label();
            this.label3 = new System.Windows.Forms.Label();
            this._btnClearEncodedList = new System.Windows.Forms.Button();
            this._chkShowOnlyWithErrors = new System.Windows.Forms.CheckBox();
            this._resourcesTimer = new System.Windows.Forms.Timer(this.components);
            this.tableLayoutPanel1 = new System.Windows.Forms.TableLayoutPanel();
            this.tableLayoutPanel2 = new System.Windows.Forms.TableLayoutPanel();
            this._pnlClearList = new System.Windows.Forms.Panel();
            this.tableLayoutPanel4 = new System.Windows.Forms.TableLayoutPanel();
            this.ctrlEncodingJobList1 = new BTFP_Video_Encoding.ctrlEncodingJobList();
            this._encodingControlTimer = new System.Windows.Forms.Timer(this.components);
            this.fileSystemWatcher1 = new System.IO.FileSystemWatcher();
            this._renderTimer = new System.Windows.Forms.Timer(this.components);
            this._btnPauseEncoding = new System.Windows.Forms.Button();
            this._btnStart = new System.Windows.Forms.Button();
            this.menuStrip1.SuspendLayout();
            this.groupBox2.SuspendLayout();
            this.tableLayoutPanel1.SuspendLayout();
            this.tableLayoutPanel2.SuspendLayout();
            this._pnlClearList.SuspendLayout();
            this.tableLayoutPanel4.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.fileSystemWatcher1)).BeginInit();
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
            this.menuStrip1.Size = new System.Drawing.Size(926, 24);
            this.menuStrip1.TabIndex = 0;
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
            this.exitToolStripMenuItem.Image = global::BTFP_Video_Encoding.Properties.Resources.exit;
            this.exitToolStripMenuItem.Name = "exitToolStripMenuItem";
            this.exitToolStripMenuItem.Size = new System.Drawing.Size(92, 22);
            this.exitToolStripMenuItem.Text = "E&xit";
            this.exitToolStripMenuItem.Click += new System.EventHandler(this.exitToolStripMenuItem_Click);
            // 
            // toolsToolStripMenuItem
            // 
            this.toolsToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.configurationToolStripMenuItem});
            this.toolsToolStripMenuItem.Name = "toolsToolStripMenuItem";
            this.toolsToolStripMenuItem.Size = new System.Drawing.Size(47, 20);
            this.toolsToolStripMenuItem.Text = "&Tools";
            // 
            // configurationToolStripMenuItem
            // 
            this.configurationToolStripMenuItem.Image = global::BTFP_Video_Encoding.Properties.Resources.configuration;
            this.configurationToolStripMenuItem.Name = "configurationToolStripMenuItem";
            this.configurationToolStripMenuItem.Size = new System.Drawing.Size(157, 22);
            this.configurationToolStripMenuItem.Text = "&Configuration...";
            this.configurationToolStripMenuItem.Click += new System.EventHandler(this.configurationToolStripMenuItem_Click);
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
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.label1.Location = new System.Drawing.Point(12, 37);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(122, 13);
            this.label1.TabIndex = 1;
            this.label1.Text = "MONITOR FOLDER:";
            // 
            // _txtMonitorFolder
            // 
            this._txtMonitorFolder.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this._txtMonitorFolder.Location = new System.Drawing.Point(15, 53);
            this._txtMonitorFolder.Name = "_txtMonitorFolder";
            this._txtMonitorFolder.ReadOnly = true;
            this._txtMonitorFolder.Size = new System.Drawing.Size(898, 20);
            this._txtMonitorFolder.TabIndex = 2;
            // 
            // _lstFilesForEncoding
            // 
            this._lstFilesForEncoding.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.columnHeader1});
            this._lstFilesForEncoding.Dock = System.Windows.Forms.DockStyle.Fill;
            this._lstFilesForEncoding.GridLines = true;
            this._lstFilesForEncoding.Location = new System.Drawing.Point(3, 32);
            this._lstFilesForEncoding.MultiSelect = false;
            this._lstFilesForEncoding.Name = "_lstFilesForEncoding";
            this._lstFilesForEncoding.Size = new System.Drawing.Size(283, 474);
            this._lstFilesForEncoding.TabIndex = 3;
            this._lstFilesForEncoding.UseCompatibleStateImageBehavior = false;
            this._lstFilesForEncoding.View = System.Windows.Forms.View.Details;
            // 
            // columnHeader1
            // 
            this.columnHeader1.Text = "Files";
            this.columnHeader1.Width = 266;
            // 
            // groupBox2
            // 
            this.groupBox2.BackColor = System.Drawing.Color.Gray;
            this.groupBox2.Controls.Add(this._lblDriveFreeSpace);
            this.groupBox2.Controls.Add(this._lblTotalRam);
            this.groupBox2.Controls.Add(this._lblTotalCpu);
            this.groupBox2.Controls.Add(this._lblAppRam);
            this.groupBox2.Controls.Add(this._lblAppCpu);
            this.groupBox2.Dock = System.Windows.Forms.DockStyle.Fill;
            this.groupBox2.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.groupBox2.Location = new System.Drawing.Point(3, 353);
            this.groupBox2.Name = "groupBox2";
            this.groupBox2.Size = new System.Drawing.Size(297, 118);
            this.groupBox2.TabIndex = 11;
            this.groupBox2.TabStop = false;
            this.groupBox2.Text = "SYSTEM INFO";
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
            this._lblTotalCpu.Size = new System.Drawing.Size(254, 17);
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
            // _lstEncodedFiles
            // 
            this._lstEncodedFiles.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.columnHeader2});
            this._lstEncodedFiles.Dock = System.Windows.Forms.DockStyle.Fill;
            this._lstEncodedFiles.GridLines = true;
            this._lstEncodedFiles.LargeImageList = this._encodedFilesImageList;
            this._lstEncodedFiles.Location = new System.Drawing.Point(3, 3);
            this._lstEncodedFiles.MultiSelect = false;
            this._lstEncodedFiles.Name = "_lstEncodedFiles";
            this._lstEncodedFiles.Size = new System.Drawing.Size(288, 433);
            this._lstEncodedFiles.SmallImageList = this._encodedFilesImageList;
            this._lstEncodedFiles.TabIndex = 12;
            this._lstEncodedFiles.UseCompatibleStateImageBehavior = false;
            this._lstEncodedFiles.View = System.Windows.Forms.View.Details;
            this._lstEncodedFiles.MouseClick += new System.Windows.Forms.MouseEventHandler(this._lstEncodedFiles_MouseClick);
            // 
            // columnHeader2
            // 
            this.columnHeader2.Text = "Files";
            this.columnHeader2.Width = 275;
            // 
            // _encodedFilesImageList
            // 
            this._encodedFilesImageList.ImageStream = ((System.Windows.Forms.ImageListStreamer)(resources.GetObject("_encodedFilesImageList.ImageStream")));
            this._encodedFilesImageList.TransparentColor = System.Drawing.Color.Transparent;
            this._encodedFilesImageList.Images.SetKeyName(0, "ok_check.png");
            this._encodedFilesImageList.Images.SetKeyName(1, "error_check.png");
            // 
            // label2
            // 
            this.label2.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.label2.AutoSize = true;
            this.label2.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.label2.Location = new System.Drawing.Point(3, 16);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(167, 13);
            this.label2.TabIndex = 13;
            this.label2.Text = "FILES READY TO ENCODE:";
            // 
            // label3
            // 
            this.label3.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.label3.AutoSize = true;
            this.label3.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.label3.Location = new System.Drawing.Point(601, 16);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(200, 13);
            this.label3.TabIndex = 14;
            this.label3.Text = "FILES ENCODED THIS SESSION:";
            // 
            // _btnClearEncodedList
            // 
            this._btnClearEncodedList.Location = new System.Drawing.Point(3, 3);
            this._btnClearEncodedList.Name = "_btnClearEncodedList";
            this._btnClearEncodedList.Size = new System.Drawing.Size(70, 23);
            this._btnClearEncodedList.TabIndex = 15;
            this._btnClearEncodedList.Text = "Clear";
            this._btnClearEncodedList.UseVisualStyleBackColor = true;
            this._btnClearEncodedList.Click += new System.EventHandler(this._btnClearEncodedList_Click);
            // 
            // _chkShowOnlyWithErrors
            // 
            this._chkShowOnlyWithErrors.AutoSize = true;
            this._chkShowOnlyWithErrors.Location = new System.Drawing.Point(79, 7);
            this._chkShowOnlyWithErrors.Name = "_chkShowOnlyWithErrors";
            this._chkShowOnlyWithErrors.Size = new System.Drawing.Size(126, 17);
            this._chkShowOnlyWithErrors.TabIndex = 16;
            this._chkShowOnlyWithErrors.Text = "Show only with errors";
            this._chkShowOnlyWithErrors.UseVisualStyleBackColor = true;
            this._chkShowOnlyWithErrors.CheckedChanged += new System.EventHandler(this._chkShowOnlyWithErrors_CheckedChanged);
            // 
            // _resourcesTimer
            // 
            this._resourcesTimer.Interval = 2000;
            this._resourcesTimer.Tick += new System.EventHandler(this._resourcesTimer_Tick);
            // 
            // tableLayoutPanel1
            // 
            this.tableLayoutPanel1.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.tableLayoutPanel1.ColumnCount = 3;
            this.tableLayoutPanel1.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 32.24044F));
            this.tableLayoutPanel1.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 34.42623F));
            this.tableLayoutPanel1.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 33.33333F));
            this.tableLayoutPanel1.Controls.Add(this._lstFilesForEncoding, 0, 1);
            this.tableLayoutPanel1.Controls.Add(this.label3, 2, 0);
            this.tableLayoutPanel1.Controls.Add(this.label2, 0, 0);
            this.tableLayoutPanel1.Controls.Add(this.tableLayoutPanel2, 2, 1);
            this.tableLayoutPanel1.Controls.Add(this.tableLayoutPanel4, 1, 1);
            this.tableLayoutPanel1.Location = new System.Drawing.Point(15, 108);
            this.tableLayoutPanel1.Name = "tableLayoutPanel1";
            this.tableLayoutPanel1.RowCount = 2;
            this.tableLayoutPanel1.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 5.893909F));
            this.tableLayoutPanel1.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 94.10609F));
            this.tableLayoutPanel1.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 20F));
            this.tableLayoutPanel1.Size = new System.Drawing.Size(898, 509);
            this.tableLayoutPanel1.TabIndex = 17;
            // 
            // tableLayoutPanel2
            // 
            this.tableLayoutPanel2.ColumnCount = 1;
            this.tableLayoutPanel2.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 50F));
            this.tableLayoutPanel2.Controls.Add(this._lstEncodedFiles, 0, 0);
            this.tableLayoutPanel2.Controls.Add(this._pnlClearList, 0, 1);
            this.tableLayoutPanel2.Dock = System.Windows.Forms.DockStyle.Fill;
            this.tableLayoutPanel2.Location = new System.Drawing.Point(601, 32);
            this.tableLayoutPanel2.Name = "tableLayoutPanel2";
            this.tableLayoutPanel2.RowCount = 2;
            this.tableLayoutPanel2.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 92.68817F));
            this.tableLayoutPanel2.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 7.311828F));
            this.tableLayoutPanel2.Size = new System.Drawing.Size(294, 474);
            this.tableLayoutPanel2.TabIndex = 15;
            // 
            // _pnlClearList
            // 
            this._pnlClearList.Controls.Add(this._chkShowOnlyWithErrors);
            this._pnlClearList.Controls.Add(this._btnClearEncodedList);
            this._pnlClearList.Dock = System.Windows.Forms.DockStyle.Fill;
            this._pnlClearList.Location = new System.Drawing.Point(3, 442);
            this._pnlClearList.Name = "_pnlClearList";
            this._pnlClearList.Size = new System.Drawing.Size(288, 29);
            this._pnlClearList.TabIndex = 13;
            // 
            // tableLayoutPanel4
            // 
            this.tableLayoutPanel4.ColumnCount = 1;
            this.tableLayoutPanel4.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 100F));
            this.tableLayoutPanel4.Controls.Add(this.groupBox2, 0, 1);
            this.tableLayoutPanel4.Controls.Add(this.ctrlEncodingJobList1, 0, 0);
            this.tableLayoutPanel4.Dock = System.Windows.Forms.DockStyle.Fill;
            this.tableLayoutPanel4.Location = new System.Drawing.Point(292, 32);
            this.tableLayoutPanel4.Name = "tableLayoutPanel4";
            this.tableLayoutPanel4.RowCount = 2;
            this.tableLayoutPanel4.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 73.97849F));
            this.tableLayoutPanel4.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 26.02151F));
            this.tableLayoutPanel4.Size = new System.Drawing.Size(303, 474);
            this.tableLayoutPanel4.TabIndex = 16;
            // 
            // ctrlEncodingJobList1
            // 
            this.ctrlEncodingJobList1.AutoScroll = true;
            this.ctrlEncodingJobList1.BackColor = System.Drawing.Color.Gray;
            this.ctrlEncodingJobList1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.ctrlEncodingJobList1.Location = new System.Drawing.Point(3, 3);
            this.ctrlEncodingJobList1.Name = "ctrlEncodingJobList1";
            this.ctrlEncodingJobList1.Size = new System.Drawing.Size(297, 344);
            this.ctrlEncodingJobList1.TabIndex = 12;
            // 
            // _encodingControlTimer
            // 
            this._encodingControlTimer.Interval = 500;
            this._encodingControlTimer.Tick += new System.EventHandler(this._encodingControlTimer_Tick);
            // 
            // fileSystemWatcher1
            // 
            this.fileSystemWatcher1.EnableRaisingEvents = true;
            this.fileSystemWatcher1.Filter = "*.mp4";
            this.fileSystemWatcher1.SynchronizingObject = this;
            this.fileSystemWatcher1.Created += new System.IO.FileSystemEventHandler(this.fileSystemWatcher1_Created);
            // 
            // _renderTimer
            // 
            this._renderTimer.Interval = 1000;
            this._renderTimer.Tick += new System.EventHandler(this._renderTimer_Tick);
            // 
            // _btnPauseEncoding
            // 
            this._btnPauseEncoding.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this._btnPauseEncoding.Image = global::BTFP_Video_Encoding.Properties.Resources.pause_2;
            this._btnPauseEncoding.Location = new System.Drawing.Point(151, 79);
            this._btnPauseEncoding.Name = "_btnPauseEncoding";
            this._btnPauseEncoding.Size = new System.Drawing.Size(130, 23);
            this._btnPauseEncoding.TabIndex = 18;
            this._btnPauseEncoding.Text = "Pause Encoding";
            this._btnPauseEncoding.TextImageRelation = System.Windows.Forms.TextImageRelation.ImageBeforeText;
            this._btnPauseEncoding.UseVisualStyleBackColor = true;
            this._btnPauseEncoding.Click += new System.EventHandler(this._btnPauseEncoding_Click);
            // 
            // _btnStart
            // 
            this._btnStart.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this._btnStart.Image = global::BTFP_Video_Encoding.Properties.Resources.play;
            this._btnStart.Location = new System.Drawing.Point(15, 79);
            this._btnStart.Name = "_btnStart";
            this._btnStart.Size = new System.Drawing.Size(130, 23);
            this._btnStart.TabIndex = 4;
            this._btnStart.Text = "Start Monitor";
            this._btnStart.TextImageRelation = System.Windows.Forms.TextImageRelation.ImageBeforeText;
            this._btnStart.UseVisualStyleBackColor = true;
            this._btnStart.Click += new System.EventHandler(this._btnStart_Click);
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.Color.DarkGray;
            this.ClientSize = new System.Drawing.Size(926, 623);
            this.Controls.Add(this._btnPauseEncoding);
            this.Controls.Add(this.tableLayoutPanel1);
            this.Controls.Add(this._btnStart);
            this.Controls.Add(this._txtMonitorFolder);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.menuStrip1);
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.MainMenuStrip = this.menuStrip1;
            this.Name = "Form1";
            this.Text = "Multichannel Video Encode";
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.Form1_FormClosing);
            this.Load += new System.EventHandler(this.Form1_Load);
            this.menuStrip1.ResumeLayout(false);
            this.menuStrip1.PerformLayout();
            this.groupBox2.ResumeLayout(false);
            this.tableLayoutPanel1.ResumeLayout(false);
            this.tableLayoutPanel1.PerformLayout();
            this.tableLayoutPanel2.ResumeLayout(false);
            this._pnlClearList.ResumeLayout(false);
            this._pnlClearList.PerformLayout();
            this.tableLayoutPanel4.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.fileSystemWatcher1)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.MenuStrip menuStrip1;
        private System.Windows.Forms.ToolStripMenuItem fileToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem exitToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem toolsToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem configurationToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem helpToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem aboutToolStripMenuItem;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.TextBox _txtMonitorFolder;
        private System.Windows.Forms.ListView _lstFilesForEncoding;
        private System.Windows.Forms.Button _btnStart;
        private System.Windows.Forms.GroupBox groupBox2;
        private System.Windows.Forms.Label _lblDriveFreeSpace;
        private System.Windows.Forms.Label _lblTotalRam;
        private System.Windows.Forms.Label _lblTotalCpu;
        private System.Windows.Forms.Label _lblAppRam;
        private System.Windows.Forms.Label _lblAppCpu;
        private System.Windows.Forms.ListView _lstEncodedFiles;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.Button _btnClearEncodedList;
        private System.Windows.Forms.CheckBox _chkShowOnlyWithErrors;
        private System.Windows.Forms.Timer _resourcesTimer;
        private System.Windows.Forms.TableLayoutPanel tableLayoutPanel1;
        private System.Windows.Forms.TableLayoutPanel tableLayoutPanel2;
        private System.Windows.Forms.TableLayoutPanel tableLayoutPanel4;
        private System.Windows.Forms.Panel _pnlClearList;
        private System.Windows.Forms.ColumnHeader columnHeader1;
        private ctrlEncodingJobList ctrlEncodingJobList1;
        private System.Windows.Forms.Timer _encodingControlTimer;
        private System.Windows.Forms.ColumnHeader columnHeader2;
        private System.Windows.Forms.ImageList _encodedFilesImageList;
        private System.IO.FileSystemWatcher fileSystemWatcher1;
        private System.Windows.Forms.Button _btnPauseEncoding;
        private System.Windows.Forms.Timer _renderTimer;
    }
}

