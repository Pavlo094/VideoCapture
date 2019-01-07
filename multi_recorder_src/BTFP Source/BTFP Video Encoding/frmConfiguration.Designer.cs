namespace BTFP_Video_Encoding
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
            this.tabControl1 = new System.Windows.Forms.TabControl();
            this.tabPage1 = new System.Windows.Forms.TabPage();
            this._encodingJudges = new BTFP_Video_Encoding.ctrlEncodingParams();
            this._encodingPreview = new BTFP_Video_Encoding.ctrlEncodingParams();
            this._encodingSales = new BTFP_Video_Encoding.ctrlEncodingParams();
            this.tabPage2 = new System.Windows.Forms.TabPage();
            this._btnErrorFolder = new System.Windows.Forms.Button();
            this._txtErrorFolder = new System.Windows.Forms.TextBox();
            this.label2 = new System.Windows.Forms.Label();
            this._btnFinishedFolder = new System.Windows.Forms.Button();
            this._txtFinishedFolder = new System.Windows.Forms.TextBox();
            this.label1 = new System.Windows.Forms.Label();
            this._btnMonitorFolder = new System.Windows.Forms.Button();
            this._txtMonitorFolder = new System.Windows.Forms.TextBox();
            this.label10 = new System.Windows.Forms.Label();
            this.tabPage3 = new System.Windows.Forms.TabPage();
            this._numParallelJobs = new System.Windows.Forms.NumericUpDown();
            this.label3 = new System.Windows.Forms.Label();
            this.btnCancel = new System.Windows.Forms.Button();
            this.btnOK = new System.Windows.Forms.Button();
            this.tabControl1.SuspendLayout();
            this.tabPage1.SuspendLayout();
            this.tabPage2.SuspendLayout();
            this.tabPage3.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this._numParallelJobs)).BeginInit();
            this.SuspendLayout();
            // 
            // tabControl1
            // 
            this.tabControl1.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.tabControl1.Controls.Add(this.tabPage1);
            this.tabControl1.Controls.Add(this.tabPage2);
            this.tabControl1.Controls.Add(this.tabPage3);
            this.tabControl1.Location = new System.Drawing.Point(12, 12);
            this.tabControl1.Name = "tabControl1";
            this.tabControl1.SelectedIndex = 0;
            this.tabControl1.Size = new System.Drawing.Size(581, 733);
            this.tabControl1.TabIndex = 0;
            // 
            // tabPage1
            // 
            this.tabPage1.BackColor = System.Drawing.Color.DarkGray;
            this.tabPage1.Controls.Add(this._encodingJudges);
            this.tabPage1.Controls.Add(this._encodingPreview);
            this.tabPage1.Controls.Add(this._encodingSales);
            this.tabPage1.Location = new System.Drawing.Point(4, 22);
            this.tabPage1.Name = "tabPage1";
            this.tabPage1.Padding = new System.Windows.Forms.Padding(3);
            this.tabPage1.Size = new System.Drawing.Size(573, 707);
            this.tabPage1.TabIndex = 0;
            this.tabPage1.Text = "Encoding parameters";
            // 
            // _encodingJudges
            // 
            this._encodingJudges.BackColor = System.Drawing.Color.DarkGray;
            this._encodingJudges.Location = new System.Drawing.Point(0, 476);
            this._encodingJudges.Name = "_encodingJudges";
            this._encodingJudges.Size = new System.Drawing.Size(561, 231);
            this._encodingJudges.SkipVideoTranscodingOption = true;
            this._encodingJudges.TabIndex = 2;
            this._encodingJudges.Title = "MP4 JUDGES";
            // 
            // _encodingPreview
            // 
            this._encodingPreview.BackColor = System.Drawing.Color.DarkGray;
            this._encodingPreview.Location = new System.Drawing.Point(0, 246);
            this._encodingPreview.Name = "_encodingPreview";
            this._encodingPreview.Size = new System.Drawing.Size(561, 229);
            this._encodingPreview.SkipVideoTranscodingOption = true;
            this._encodingPreview.TabIndex = 1;
            this._encodingPreview.Title = "MP4 PREVIEW";
            // 
            // _encodingSales
            // 
            this._encodingSales.BackColor = System.Drawing.Color.DarkGray;
            this._encodingSales.Location = new System.Drawing.Point(0, 6);
            this._encodingSales.Name = "_encodingSales";
            this._encodingSales.Size = new System.Drawing.Size(561, 234);
            this._encodingSales.SkipVideoTranscodingOption = true;
            this._encodingSales.TabIndex = 0;
            this._encodingSales.Title = "MP4 SALES";
            // 
            // tabPage2
            // 
            this.tabPage2.BackColor = System.Drawing.Color.DarkGray;
            this.tabPage2.Controls.Add(this._btnErrorFolder);
            this.tabPage2.Controls.Add(this._txtErrorFolder);
            this.tabPage2.Controls.Add(this.label2);
            this.tabPage2.Controls.Add(this._btnFinishedFolder);
            this.tabPage2.Controls.Add(this._txtFinishedFolder);
            this.tabPage2.Controls.Add(this.label1);
            this.tabPage2.Controls.Add(this._btnMonitorFolder);
            this.tabPage2.Controls.Add(this._txtMonitorFolder);
            this.tabPage2.Controls.Add(this.label10);
            this.tabPage2.Location = new System.Drawing.Point(4, 22);
            this.tabPage2.Name = "tabPage2";
            this.tabPage2.Padding = new System.Windows.Forms.Padding(3);
            this.tabPage2.Size = new System.Drawing.Size(573, 656);
            this.tabPage2.TabIndex = 1;
            this.tabPage2.Text = "Folders";
            // 
            // _btnErrorFolder
            // 
            this._btnErrorFolder.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this._btnErrorFolder.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this._btnErrorFolder.Location = new System.Drawing.Point(536, 148);
            this._btnErrorFolder.Name = "_btnErrorFolder";
            this._btnErrorFolder.Size = new System.Drawing.Size(31, 23);
            this._btnErrorFolder.TabIndex = 40;
            this._btnErrorFolder.Text = "...";
            this._btnErrorFolder.UseVisualStyleBackColor = true;
            this._btnErrorFolder.Click += new System.EventHandler(this._btnErrorFolder_Click);
            // 
            // _txtErrorFolder
            // 
            this._txtErrorFolder.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this._txtErrorFolder.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this._txtErrorFolder.Location = new System.Drawing.Point(6, 150);
            this._txtErrorFolder.Name = "_txtErrorFolder";
            this._txtErrorFolder.Size = new System.Drawing.Size(524, 20);
            this._txtErrorFolder.TabIndex = 39;
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.label2.Location = new System.Drawing.Point(3, 134);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(95, 13);
            this.label2.TabIndex = 38;
            this.label2.Text = "ERROR FOLDER:";
            // 
            // _btnFinishedFolder
            // 
            this._btnFinishedFolder.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this._btnFinishedFolder.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this._btnFinishedFolder.Location = new System.Drawing.Point(536, 89);
            this._btnFinishedFolder.Name = "_btnFinishedFolder";
            this._btnFinishedFolder.Size = new System.Drawing.Size(31, 23);
            this._btnFinishedFolder.TabIndex = 37;
            this._btnFinishedFolder.Text = "...";
            this._btnFinishedFolder.UseVisualStyleBackColor = true;
            this._btnFinishedFolder.Click += new System.EventHandler(this._btnFinishedFolder_Click);
            // 
            // _txtFinishedFolder
            // 
            this._txtFinishedFolder.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this._txtFinishedFolder.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this._txtFinishedFolder.Location = new System.Drawing.Point(6, 91);
            this._txtFinishedFolder.Name = "_txtFinishedFolder";
            this._txtFinishedFolder.Size = new System.Drawing.Size(524, 20);
            this._txtFinishedFolder.TabIndex = 36;
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.label1.Location = new System.Drawing.Point(3, 75);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(106, 13);
            this.label1.TabIndex = 35;
            this.label1.Text = "FINISHED FOLDER:";
            // 
            // _btnMonitorFolder
            // 
            this._btnMonitorFolder.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this._btnMonitorFolder.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this._btnMonitorFolder.Location = new System.Drawing.Point(536, 33);
            this._btnMonitorFolder.Name = "_btnMonitorFolder";
            this._btnMonitorFolder.Size = new System.Drawing.Size(31, 23);
            this._btnMonitorFolder.TabIndex = 34;
            this._btnMonitorFolder.Text = "...";
            this._btnMonitorFolder.UseVisualStyleBackColor = true;
            this._btnMonitorFolder.Click += new System.EventHandler(this._btnMonitorFolder_Click);
            // 
            // _txtMonitorFolder
            // 
            this._txtMonitorFolder.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this._txtMonitorFolder.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this._txtMonitorFolder.Location = new System.Drawing.Point(6, 35);
            this._txtMonitorFolder.Name = "_txtMonitorFolder";
            this._txtMonitorFolder.Size = new System.Drawing.Size(524, 20);
            this._txtMonitorFolder.TabIndex = 33;
            // 
            // label10
            // 
            this.label10.AutoSize = true;
            this.label10.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.label10.Location = new System.Drawing.Point(3, 19);
            this.label10.Name = "label10";
            this.label10.Size = new System.Drawing.Size(107, 13);
            this.label10.TabIndex = 32;
            this.label10.Text = "MONITOR FOLDER:";
            // 
            // tabPage3
            // 
            this.tabPage3.BackColor = System.Drawing.Color.DarkGray;
            this.tabPage3.Controls.Add(this._numParallelJobs);
            this.tabPage3.Controls.Add(this.label3);
            this.tabPage3.Location = new System.Drawing.Point(4, 22);
            this.tabPage3.Name = "tabPage3";
            this.tabPage3.Size = new System.Drawing.Size(573, 656);
            this.tabPage3.TabIndex = 2;
            this.tabPage3.Text = "Misc";
            // 
            // _numParallelJobs
            // 
            this._numParallelJobs.Enabled = false;
            this._numParallelJobs.Location = new System.Drawing.Point(126, 19);
            this._numParallelJobs.Minimum = new decimal(new int[] {
            1,
            0,
            0,
            0});
            this._numParallelJobs.Name = "_numParallelJobs";
            this._numParallelJobs.Size = new System.Drawing.Size(85, 20);
            this._numParallelJobs.TabIndex = 1;
            this._numParallelJobs.Value = new decimal(new int[] {
            1,
            0,
            0,
            0});
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(3, 21);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(117, 13);
            this.label3.TabIndex = 0;
            this.label3.Text = "Number of parallel jobs:";
            // 
            // btnCancel
            // 
            this.btnCancel.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.btnCancel.Image = global::BTFP_Video_Encoding.Properties.Resources.Cancel;
            this.btnCancel.Location = new System.Drawing.Point(518, 751);
            this.btnCancel.Name = "btnCancel";
            this.btnCancel.Size = new System.Drawing.Size(75, 23);
            this.btnCancel.TabIndex = 6;
            this.btnCancel.Text = "Cancel";
            this.btnCancel.TextImageRelation = System.Windows.Forms.TextImageRelation.ImageBeforeText;
            this.btnCancel.UseVisualStyleBackColor = true;
            // 
            // btnOK
            // 
            this.btnOK.DialogResult = System.Windows.Forms.DialogResult.OK;
            this.btnOK.Image = global::BTFP_Video_Encoding.Properties.Resources.OK;
            this.btnOK.Location = new System.Drawing.Point(437, 751);
            this.btnOK.Name = "btnOK";
            this.btnOK.Size = new System.Drawing.Size(75, 23);
            this.btnOK.TabIndex = 7;
            this.btnOK.Text = "OK";
            this.btnOK.TextImageRelation = System.Windows.Forms.TextImageRelation.ImageBeforeText;
            this.btnOK.UseVisualStyleBackColor = true;
            // 
            // frmConfiguration
            // 
            this.AcceptButton = this.btnOK;
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.Color.DarkGray;
            this.CancelButton = this.btnCancel;
            this.ClientSize = new System.Drawing.Size(605, 779);
            this.Controls.Add(this.btnOK);
            this.Controls.Add(this.btnCancel);
            this.Controls.Add(this.tabControl1);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.Fixed3D;
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "frmConfiguration";
            this.Text = "Configuration";
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.frmConfiguration_FormClosing);
            this.Load += new System.EventHandler(this.frmConfiguration_Load);
            this.tabControl1.ResumeLayout(false);
            this.tabPage1.ResumeLayout(false);
            this.tabPage2.ResumeLayout(false);
            this.tabPage2.PerformLayout();
            this.tabPage3.ResumeLayout(false);
            this.tabPage3.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this._numParallelJobs)).EndInit();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.TabControl tabControl1;
        private System.Windows.Forms.TabPage tabPage1;
        private System.Windows.Forms.TabPage tabPage2;
        private System.Windows.Forms.Button btnOK;
        private System.Windows.Forms.Button btnCancel;
        private ctrlEncodingParams _encodingPreview;
        private ctrlEncodingParams _encodingSales;
        private ctrlEncodingParams _encodingJudges;
        private System.Windows.Forms.Button _btnErrorFolder;
        private System.Windows.Forms.TextBox _txtErrorFolder;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Button _btnFinishedFolder;
        private System.Windows.Forms.TextBox _txtFinishedFolder;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Button _btnMonitorFolder;
        private System.Windows.Forms.TextBox _txtMonitorFolder;
        private System.Windows.Forms.Label label10;
        private System.Windows.Forms.TabPage tabPage3;
        private System.Windows.Forms.NumericUpDown _numParallelJobs;
        private System.Windows.Forms.Label label3;
    }
}