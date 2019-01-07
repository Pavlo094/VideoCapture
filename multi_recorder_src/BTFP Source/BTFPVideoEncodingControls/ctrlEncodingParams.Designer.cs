namespace BTFP_Video_Encoding
{
    partial class ctrlEncodingParams
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
            this._lblVideoBitrateKbps = new System.Windows.Forms.Label();
            this._numVideoBitrate = new System.Windows.Forms.NumericUpDown();
            this._lblVideoBitrate = new System.Windows.Forms.Label();
            this._grpParameters = new System.Windows.Forms.GroupBox();
            this._btnAudioMap = new System.Windows.Forms.Button();
            this._chkSkipVideoTranscode = new System.Windows.Forms.CheckBox();
            this._txtFilenameAppendix = new System.Windows.Forms.TextBox();
            this.label9 = new System.Windows.Forms.Label();
            this._btnOutputFolder = new System.Windows.Forms.Button();
            this._txtOutputFolder = new System.Windows.Forms.TextBox();
            this.label10 = new System.Windows.Forms.Label();
            this.label8 = new System.Windows.Forms.Label();
            this._cmbVolumeBoost = new System.Windows.Forms.ComboBox();
            this.label7 = new System.Windows.Forms.Label();
            this._numAudioBitrate = new System.Windows.Forms.NumericUpDown();
            this.label4 = new System.Windows.Forms.Label();
            this.label5 = new System.Windows.Forms.Label();
            this._numResolutionHeight = new System.Windows.Forms.NumericUpDown();
            this._lblVideoResolutionX = new System.Windows.Forms.Label();
            this._numResolutionWidth = new System.Windows.Forms.NumericUpDown();
            this._lblVideoResolution = new System.Windows.Forms.Label();
            this._chkBypassEncoding = new System.Windows.Forms.CheckBox();
            this._chkInterlacedVideo = new System.Windows.Forms.CheckBox();
            ((System.ComponentModel.ISupportInitialize)(this._numVideoBitrate)).BeginInit();
            this._grpParameters.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this._numAudioBitrate)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this._numResolutionHeight)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this._numResolutionWidth)).BeginInit();
            this.SuspendLayout();
            // 
            // _lblVideoBitrateKbps
            // 
            this._lblVideoBitrateKbps.AutoSize = true;
            this._lblVideoBitrateKbps.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this._lblVideoBitrateKbps.Location = new System.Drawing.Point(169, 26);
            this._lblVideoBitrateKbps.Name = "_lblVideoBitrateKbps";
            this._lblVideoBitrateKbps.Size = new System.Drawing.Size(30, 13);
            this._lblVideoBitrateKbps.TabIndex = 17;
            this._lblVideoBitrateKbps.Text = "Kb/s";
            // 
            // _numVideoBitrate
            // 
            this._numVideoBitrate.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this._numVideoBitrate.Location = new System.Drawing.Point(81, 24);
            this._numVideoBitrate.Maximum = new decimal(new int[] {
            100000,
            0,
            0,
            0});
            this._numVideoBitrate.Minimum = new decimal(new int[] {
            100,
            0,
            0,
            0});
            this._numVideoBitrate.Name = "_numVideoBitrate";
            this._numVideoBitrate.Size = new System.Drawing.Size(82, 20);
            this._numVideoBitrate.TabIndex = 16;
            this._numVideoBitrate.Value = new decimal(new int[] {
            100,
            0,
            0,
            0});
            // 
            // _lblVideoBitrate
            // 
            this._lblVideoBitrate.AutoSize = true;
            this._lblVideoBitrate.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this._lblVideoBitrate.Location = new System.Drawing.Point(9, 26);
            this._lblVideoBitrate.Name = "_lblVideoBitrate";
            this._lblVideoBitrate.Size = new System.Drawing.Size(66, 13);
            this._lblVideoBitrate.TabIndex = 15;
            this._lblVideoBitrate.Text = "Video bitrate";
            // 
            // _grpParameters
            // 
            this._grpParameters.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this._grpParameters.Controls.Add(this._chkInterlacedVideo);
            this._grpParameters.Controls.Add(this._btnAudioMap);
            this._grpParameters.Controls.Add(this._chkSkipVideoTranscode);
            this._grpParameters.Controls.Add(this._txtFilenameAppendix);
            this._grpParameters.Controls.Add(this.label9);
            this._grpParameters.Controls.Add(this._btnOutputFolder);
            this._grpParameters.Controls.Add(this._txtOutputFolder);
            this._grpParameters.Controls.Add(this.label10);
            this._grpParameters.Controls.Add(this.label8);
            this._grpParameters.Controls.Add(this._cmbVolumeBoost);
            this._grpParameters.Controls.Add(this.label7);
            this._grpParameters.Controls.Add(this._numAudioBitrate);
            this._grpParameters.Controls.Add(this.label4);
            this._grpParameters.Controls.Add(this.label5);
            this._grpParameters.Controls.Add(this._numResolutionHeight);
            this._grpParameters.Controls.Add(this._lblVideoResolutionX);
            this._grpParameters.Controls.Add(this._numResolutionWidth);
            this._grpParameters.Controls.Add(this._lblVideoResolution);
            this._grpParameters.Controls.Add(this._numVideoBitrate);
            this._grpParameters.Controls.Add(this._lblVideoBitrate);
            this._grpParameters.Controls.Add(this._lblVideoBitrateKbps);
            this._grpParameters.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this._grpParameters.Location = new System.Drawing.Point(7, 25);
            this._grpParameters.Name = "_grpParameters";
            this._grpParameters.Size = new System.Drawing.Size(546, 203);
            this._grpParameters.TabIndex = 18;
            this._grpParameters.TabStop = false;
            this._grpParameters.Text = "groupBox1";
            // 
            // _btnAudioMap
            // 
            this._btnAudioMap.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this._btnAudioMap.Location = new System.Drawing.Point(393, 78);
            this._btnAudioMap.Name = "_btnAudioMap";
            this._btnAudioMap.Size = new System.Drawing.Size(82, 23);
            this._btnAudioMap.TabIndex = 35;
            this._btnAudioMap.Text = "Audio Map";
            this._btnAudioMap.UseVisualStyleBackColor = true;
            this._btnAudioMap.Click += new System.EventHandler(this._btnAudioMap_Click);
            // 
            // _chkSkipVideoTranscode
            // 
            this._chkSkipVideoTranscode.AutoSize = true;
            this._chkSkipVideoTranscode.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this._chkSkipVideoTranscode.Location = new System.Drawing.Point(139, 95);
            this._chkSkipVideoTranscode.Name = "_chkSkipVideoTranscode";
            this._chkSkipVideoTranscode.Size = new System.Drawing.Size(130, 17);
            this._chkSkipVideoTranscode.TabIndex = 34;
            this._chkSkipVideoTranscode.Text = "Don\'t transcode video";
            this._chkSkipVideoTranscode.UseVisualStyleBackColor = true;
            this._chkSkipVideoTranscode.CheckedChanged += new System.EventHandler(this._chkSkipVideoTranscode_CheckedChanged);
            // 
            // _txtFilenameAppendix
            // 
            this._txtFilenameAppendix.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this._txtFilenameAppendix.Location = new System.Drawing.Point(12, 177);
            this._txtFilenameAppendix.Name = "_txtFilenameAppendix";
            this._txtFilenameAppendix.Size = new System.Drawing.Size(151, 20);
            this._txtFilenameAppendix.TabIndex = 33;
            // 
            // label9
            // 
            this.label9.AutoSize = true;
            this.label9.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.label9.Location = new System.Drawing.Point(9, 161);
            this.label9.Name = "label9";
            this.label9.Size = new System.Drawing.Size(95, 13);
            this.label9.TabIndex = 32;
            this.label9.Text = "Filename appendix";
            // 
            // _btnOutputFolder
            // 
            this._btnOutputFolder.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this._btnOutputFolder.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this._btnOutputFolder.Location = new System.Drawing.Point(507, 132);
            this._btnOutputFolder.Name = "_btnOutputFolder";
            this._btnOutputFolder.Size = new System.Drawing.Size(31, 23);
            this._btnOutputFolder.TabIndex = 31;
            this._btnOutputFolder.Text = "...";
            this._btnOutputFolder.UseVisualStyleBackColor = true;
            this._btnOutputFolder.Click += new System.EventHandler(this._btnOutputFolder_Click);
            // 
            // _txtOutputFolder
            // 
            this._txtOutputFolder.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this._txtOutputFolder.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this._txtOutputFolder.Location = new System.Drawing.Point(12, 134);
            this._txtOutputFolder.Name = "_txtOutputFolder";
            this._txtOutputFolder.Size = new System.Drawing.Size(489, 20);
            this._txtOutputFolder.TabIndex = 30;
            // 
            // label10
            // 
            this.label10.AutoSize = true;
            this.label10.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.label10.Location = new System.Drawing.Point(9, 118);
            this.label10.Name = "label10";
            this.label10.Size = new System.Drawing.Size(68, 13);
            this.label10.TabIndex = 29;
            this.label10.Text = "Output folder";
            // 
            // label8
            // 
            this.label8.AutoSize = true;
            this.label8.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.label8.Location = new System.Drawing.Point(481, 52);
            this.label8.Name = "label8";
            this.label8.Size = new System.Drawing.Size(20, 13);
            this.label8.TabIndex = 27;
            this.label8.Text = "dB";
            // 
            // _cmbVolumeBoost
            // 
            this._cmbVolumeBoost.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this._cmbVolumeBoost.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this._cmbVolumeBoost.FormattingEnabled = true;
            this._cmbVolumeBoost.Location = new System.Drawing.Point(393, 49);
            this._cmbVolumeBoost.Name = "_cmbVolumeBoost";
            this._cmbVolumeBoost.Size = new System.Drawing.Size(82, 21);
            this._cmbVolumeBoost.TabIndex = 26;
            // 
            // label7
            // 
            this.label7.AutoSize = true;
            this.label7.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.label7.Location = new System.Drawing.Point(321, 52);
            this.label7.Name = "label7";
            this.label7.Size = new System.Drawing.Size(71, 13);
            this.label7.TabIndex = 25;
            this.label7.Text = "Volume boost";
            // 
            // _numAudioBitrate
            // 
            this._numAudioBitrate.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this._numAudioBitrate.Location = new System.Drawing.Point(393, 24);
            this._numAudioBitrate.Maximum = new decimal(new int[] {
            100000,
            0,
            0,
            0});
            this._numAudioBitrate.Minimum = new decimal(new int[] {
            100,
            0,
            0,
            0});
            this._numAudioBitrate.Name = "_numAudioBitrate";
            this._numAudioBitrate.Size = new System.Drawing.Size(82, 20);
            this._numAudioBitrate.TabIndex = 23;
            this._numAudioBitrate.Value = new decimal(new int[] {
            100,
            0,
            0,
            0});
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.label4.Location = new System.Drawing.Point(321, 26);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(66, 13);
            this.label4.TabIndex = 22;
            this.label4.Text = "Audio bitrate";
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.label5.Location = new System.Drawing.Point(481, 26);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(30, 13);
            this.label5.TabIndex = 24;
            this.label5.Text = "Kb/s";
            // 
            // _numResolutionHeight
            // 
            this._numResolutionHeight.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this._numResolutionHeight.Location = new System.Drawing.Point(187, 50);
            this._numResolutionHeight.Maximum = new decimal(new int[] {
            100000,
            0,
            0,
            0});
            this._numResolutionHeight.Minimum = new decimal(new int[] {
            100,
            0,
            0,
            0});
            this._numResolutionHeight.Name = "_numResolutionHeight";
            this._numResolutionHeight.Size = new System.Drawing.Size(82, 20);
            this._numResolutionHeight.TabIndex = 21;
            this._numResolutionHeight.Value = new decimal(new int[] {
            100,
            0,
            0,
            0});
            // 
            // _lblVideoResolutionX
            // 
            this._lblVideoResolutionX.AutoSize = true;
            this._lblVideoResolutionX.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this._lblVideoResolutionX.Location = new System.Drawing.Point(169, 52);
            this._lblVideoResolutionX.Name = "_lblVideoResolutionX";
            this._lblVideoResolutionX.Size = new System.Drawing.Size(12, 13);
            this._lblVideoResolutionX.TabIndex = 20;
            this._lblVideoResolutionX.Text = "x";
            // 
            // _numResolutionWidth
            // 
            this._numResolutionWidth.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this._numResolutionWidth.Location = new System.Drawing.Point(81, 50);
            this._numResolutionWidth.Maximum = new decimal(new int[] {
            100000,
            0,
            0,
            0});
            this._numResolutionWidth.Minimum = new decimal(new int[] {
            100,
            0,
            0,
            0});
            this._numResolutionWidth.Name = "_numResolutionWidth";
            this._numResolutionWidth.Size = new System.Drawing.Size(82, 20);
            this._numResolutionWidth.TabIndex = 19;
            this._numResolutionWidth.Value = new decimal(new int[] {
            100,
            0,
            0,
            0});
            // 
            // _lblVideoResolution
            // 
            this._lblVideoResolution.AutoSize = true;
            this._lblVideoResolution.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this._lblVideoResolution.Location = new System.Drawing.Point(9, 52);
            this._lblVideoResolution.Name = "_lblVideoResolution";
            this._lblVideoResolution.Size = new System.Drawing.Size(57, 13);
            this._lblVideoResolution.TabIndex = 18;
            this._lblVideoResolution.Text = "Resolution";
            // 
            // _chkBypassEncoding
            // 
            this._chkBypassEncoding.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this._chkBypassEncoding.AutoSize = true;
            this._chkBypassEncoding.Location = new System.Drawing.Point(434, 7);
            this._chkBypassEncoding.Name = "_chkBypassEncoding";
            this._chkBypassEncoding.Size = new System.Drawing.Size(118, 17);
            this._chkBypassEncoding.TabIndex = 19;
            this._chkBypassEncoding.Text = "Bypass this encode";
            this._chkBypassEncoding.UseVisualStyleBackColor = true;
            this._chkBypassEncoding.CheckedChanged += new System.EventHandler(this._chkBypassEncoding_CheckedChanged);
            // 
            // _chkInterlacedVideo
            // 
            this._chkInterlacedVideo.AutoSize = true;
            this._chkInterlacedVideo.CheckAlign = System.Drawing.ContentAlignment.MiddleRight;
            this._chkInterlacedVideo.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this._chkInterlacedVideo.Location = new System.Drawing.Point(21, 76);
            this._chkInterlacedVideo.Name = "_chkInterlacedVideo";
            this._chkInterlacedVideo.Size = new System.Drawing.Size(73, 17);
            this._chkInterlacedVideo.TabIndex = 36;
            this._chkInterlacedVideo.Text = "Interlaced";
            this._chkInterlacedVideo.UseVisualStyleBackColor = true;
            // 
            // ctrlEncodingParams
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.Color.DarkGray;
            this.Controls.Add(this._chkBypassEncoding);
            this.Controls.Add(this._grpParameters);
            this.Name = "ctrlEncodingParams";
            this.Size = new System.Drawing.Size(561, 231);
            ((System.ComponentModel.ISupportInitialize)(this._numVideoBitrate)).EndInit();
            this._grpParameters.ResumeLayout(false);
            this._grpParameters.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this._numAudioBitrate)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this._numResolutionHeight)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this._numResolutionWidth)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Label _lblVideoBitrateKbps;
        private System.Windows.Forms.NumericUpDown _numVideoBitrate;
        private System.Windows.Forms.Label _lblVideoBitrate;
        private System.Windows.Forms.GroupBox _grpParameters;
        private System.Windows.Forms.NumericUpDown _numResolutionHeight;
        private System.Windows.Forms.Label _lblVideoResolutionX;
        private System.Windows.Forms.NumericUpDown _numResolutionWidth;
        private System.Windows.Forms.Label _lblVideoResolution;
        private System.Windows.Forms.CheckBox _chkBypassEncoding;
        private System.Windows.Forms.NumericUpDown _numAudioBitrate;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.Label label8;
        private System.Windows.Forms.ComboBox _cmbVolumeBoost;
        private System.Windows.Forms.Label label7;
        private System.Windows.Forms.TextBox _txtFilenameAppendix;
        private System.Windows.Forms.Label label9;
        private System.Windows.Forms.Button _btnOutputFolder;
        private System.Windows.Forms.TextBox _txtOutputFolder;
        private System.Windows.Forms.Label label10;
        private System.Windows.Forms.CheckBox _chkSkipVideoTranscode;
        private System.Windows.Forms.Button _btnAudioMap;
        private System.Windows.Forms.CheckBox _chkInterlacedVideo;
    }
}
