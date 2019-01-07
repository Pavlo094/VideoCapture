namespace Decklink_Capture
{
    partial class frmConfiguration
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(frmConfiguration));
            this._cmbInputVideoDevices = new System.Windows.Forms.ComboBox();
            this.label1 = new System.Windows.Forms.Label();
            this.label3 = new System.Windows.Forms.Label();
            this._numVideoBitrate = new System.Windows.Forms.NumericUpDown();
            this._numAudioBitrate = new System.Windows.Forms.NumericUpDown();
            this.label4 = new System.Windows.Forms.Label();
            this.label5 = new System.Windows.Forms.Label();
            this._txtOutputFolder = new System.Windows.Forms.TextBox();
            this._btnOutputFolder = new System.Windows.Forms.Button();
            this.btnOK = new System.Windows.Forms.Button();
            this.btnCancel = new System.Windows.Forms.Button();
            this._chkAudioPreview = new System.Windows.Forms.CheckBox();
            this.label6 = new System.Windows.Forms.Label();
            this.label7 = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this._cmbAudioRenderer = new System.Windows.Forms.ComboBox();
            this._chkInterlacedVideo = new System.Windows.Forms.CheckBox();
            this._cmbVideoFormat = new System.Windows.Forms.ComboBox();
            this.label8 = new System.Windows.Forms.Label();
            ((System.ComponentModel.ISupportInitialize)(this._numVideoBitrate)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this._numAudioBitrate)).BeginInit();
            this.SuspendLayout();
            // 
            // _cmbInputVideoDevices
            // 
            this._cmbInputVideoDevices.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this._cmbInputVideoDevices.FormattingEnabled = true;
            this._cmbInputVideoDevices.Location = new System.Drawing.Point(12, 25);
            this._cmbInputVideoDevices.Name = "_cmbInputVideoDevices";
            this._cmbInputVideoDevices.Size = new System.Drawing.Size(258, 21);
            this._cmbInputVideoDevices.TabIndex = 0;
            this._cmbInputVideoDevices.SelectedIndexChanged += new System.EventHandler(this._cmbInputVideoDevices_SelectedIndexChanged);
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(9, 9);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(101, 13);
            this.label1.TabIndex = 1;
            this.label1.Text = "Input Decklink Card";
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(9, 58);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(66, 13);
            this.label3.TabIndex = 6;
            this.label3.Text = "Video bitrate";
            // 
            // _numVideoBitrate
            // 
            this._numVideoBitrate.Location = new System.Drawing.Point(12, 74);
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
            this._numVideoBitrate.Size = new System.Drawing.Size(120, 20);
            this._numVideoBitrate.TabIndex = 7;
            this._numVideoBitrate.Value = new decimal(new int[] {
            100,
            0,
            0,
            0});
            // 
            // _numAudioBitrate
            // 
            this._numAudioBitrate.Location = new System.Drawing.Point(293, 74);
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
            this._numAudioBitrate.Size = new System.Drawing.Size(120, 20);
            this._numAudioBitrate.TabIndex = 9;
            this._numAudioBitrate.Value = new decimal(new int[] {
            100,
            0,
            0,
            0});
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(290, 58);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(66, 13);
            this.label4.TabIndex = 8;
            this.label4.Text = "Audio bitrate";
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Location = new System.Drawing.Point(12, 189);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(68, 13);
            this.label5.TabIndex = 10;
            this.label5.Text = "Output folder";
            // 
            // _txtOutputFolder
            // 
            this._txtOutputFolder.Location = new System.Drawing.Point(12, 205);
            this._txtOutputFolder.Name = "_txtOutputFolder";
            this._txtOutputFolder.Size = new System.Drawing.Size(502, 20);
            this._txtOutputFolder.TabIndex = 11;
            // 
            // _btnOutputFolder
            // 
            this._btnOutputFolder.Location = new System.Drawing.Point(520, 203);
            this._btnOutputFolder.Name = "_btnOutputFolder";
            this._btnOutputFolder.Size = new System.Drawing.Size(31, 23);
            this._btnOutputFolder.TabIndex = 12;
            this._btnOutputFolder.Text = "...";
            this._btnOutputFolder.UseVisualStyleBackColor = true;
            this._btnOutputFolder.Click += new System.EventHandler(this._btnOutputFolder_Click);
            // 
            // btnOK
            // 
            this.btnOK.DialogResult = System.Windows.Forms.DialogResult.OK;
            this.btnOK.Image = global::Decklink_Capture.Properties.Resources.OK;
            this.btnOK.Location = new System.Drawing.Point(395, 271);
            this.btnOK.Name = "btnOK";
            this.btnOK.Size = new System.Drawing.Size(75, 23);
            this.btnOK.TabIndex = 5;
            this.btnOK.Text = "OK";
            this.btnOK.TextImageRelation = System.Windows.Forms.TextImageRelation.ImageBeforeText;
            this.btnOK.UseVisualStyleBackColor = true;
            // 
            // btnCancel
            // 
            this.btnCancel.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.btnCancel.Image = global::Decklink_Capture.Properties.Resources.Cancel;
            this.btnCancel.Location = new System.Drawing.Point(476, 271);
            this.btnCancel.Name = "btnCancel";
            this.btnCancel.Size = new System.Drawing.Size(75, 23);
            this.btnCancel.TabIndex = 4;
            this.btnCancel.Text = "Cancel";
            this.btnCancel.TextImageRelation = System.Windows.Forms.TextImageRelation.ImageBeforeText;
            this.btnCancel.UseVisualStyleBackColor = true;
            // 
            // _chkAudioPreview
            // 
            this._chkAudioPreview.AutoSize = true;
            this._chkAudioPreview.Location = new System.Drawing.Point(293, 100);
            this._chkAudioPreview.Name = "_chkAudioPreview";
            this._chkAudioPreview.Size = new System.Drawing.Size(93, 17);
            this._chkAudioPreview.TabIndex = 13;
            this._chkAudioPreview.Text = "Audio preview";
            this._chkAudioPreview.UseVisualStyleBackColor = true;
            this._chkAudioPreview.CheckedChanged += new System.EventHandler(this._chkAudioPreview_CheckedChanged);
            // 
            // label6
            // 
            this.label6.AutoSize = true;
            this.label6.Location = new System.Drawing.Point(138, 76);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(30, 13);
            this.label6.TabIndex = 14;
            this.label6.Text = "Kb/s";
            // 
            // label7
            // 
            this.label7.AutoSize = true;
            this.label7.Location = new System.Drawing.Point(419, 76);
            this.label7.Name = "label7";
            this.label7.Size = new System.Drawing.Size(30, 13);
            this.label7.TabIndex = 15;
            this.label7.Text = "Kb/s";
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(290, 120);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(81, 13);
            this.label2.TabIndex = 17;
            this.label2.Text = "Audio Renderer";
            // 
            // _cmbAudioRenderer
            // 
            this._cmbAudioRenderer.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this._cmbAudioRenderer.FormattingEnabled = true;
            this._cmbAudioRenderer.Location = new System.Drawing.Point(293, 136);
            this._cmbAudioRenderer.Name = "_cmbAudioRenderer";
            this._cmbAudioRenderer.Size = new System.Drawing.Size(258, 21);
            this._cmbAudioRenderer.TabIndex = 16;
            // 
            // _chkInterlacedVideo
            // 
            this._chkInterlacedVideo.AutoSize = true;
            this._chkInterlacedVideo.Location = new System.Drawing.Point(12, 100);
            this._chkInterlacedVideo.Name = "_chkInterlacedVideo";
            this._chkInterlacedVideo.Size = new System.Drawing.Size(73, 17);
            this._chkInterlacedVideo.TabIndex = 18;
            this._chkInterlacedVideo.Text = "Interlaced";
            this._chkInterlacedVideo.UseVisualStyleBackColor = true;
            this._chkInterlacedVideo.Visible = false;
            // 
            // _cmbVideoFormat
            // 
            this._cmbVideoFormat.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this._cmbVideoFormat.FormattingEnabled = true;
            this._cmbVideoFormat.Location = new System.Drawing.Point(293, 25);
            this._cmbVideoFormat.Name = "_cmbVideoFormat";
            this._cmbVideoFormat.Size = new System.Drawing.Size(258, 21);
            this._cmbVideoFormat.TabIndex = 0;
            // 
            // label8
            // 
            this.label8.AutoSize = true;
            this.label8.Location = new System.Drawing.Point(290, 9);
            this.label8.Name = "label8";
            this.label8.Size = new System.Drawing.Size(69, 13);
            this.label8.TabIndex = 1;
            this.label8.Text = "Video Format";
            // 
            // frmConfiguration
            // 
            this.AcceptButton = this.btnOK;
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(215)))), ((int)(((byte)(215)))), ((int)(((byte)(215)))));
            this.CancelButton = this.btnCancel;
            this.ClientSize = new System.Drawing.Size(563, 306);
            this.Controls.Add(this._chkInterlacedVideo);
            this.Controls.Add(this.label2);
            this.Controls.Add(this._cmbAudioRenderer);
            this.Controls.Add(this.label7);
            this.Controls.Add(this.label6);
            this.Controls.Add(this._chkAudioPreview);
            this.Controls.Add(this._btnOutputFolder);
            this.Controls.Add(this._txtOutputFolder);
            this.Controls.Add(this.label5);
            this.Controls.Add(this._numAudioBitrate);
            this.Controls.Add(this.label4);
            this.Controls.Add(this._numVideoBitrate);
            this.Controls.Add(this.label3);
            this.Controls.Add(this.btnOK);
            this.Controls.Add(this.btnCancel);
            this.Controls.Add(this.label8);
            this.Controls.Add(this.label1);
            this.Controls.Add(this._cmbVideoFormat);
            this.Controls.Add(this._cmbInputVideoDevices);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.Fixed3D;
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.MaximizeBox = false;
            this.Name = "frmConfiguration";
            this.Text = "Configuration";
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.frmConfiguration_FormClosing);
            this.Load += new System.EventHandler(this.frmConfiguration_Load);
            ((System.ComponentModel.ISupportInitialize)(this._numVideoBitrate)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this._numAudioBitrate)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.ComboBox _cmbInputVideoDevices;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Button btnCancel;
        private System.Windows.Forms.Button btnOK;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.NumericUpDown _numVideoBitrate;
        private System.Windows.Forms.NumericUpDown _numAudioBitrate;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.TextBox _txtOutputFolder;
        private System.Windows.Forms.Button _btnOutputFolder;
        private System.Windows.Forms.CheckBox _chkAudioPreview;
        private System.Windows.Forms.Label label6;
        private System.Windows.Forms.Label label7;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.ComboBox _cmbAudioRenderer;
        private System.Windows.Forms.CheckBox _chkInterlacedVideo;
        private System.Windows.Forms.ComboBox _cmbVideoFormat;
        private System.Windows.Forms.Label label8;
    }
}