namespace BTFP_Video_Encoding
{
    partial class ctrlEncodingJob
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
            this.components = new System.ComponentModel.Container();
            this._lblTitle = new System.Windows.Forms.Label();
            this._lblSales = new System.Windows.Forms.Label();
            this._prgSales = new System.Windows.Forms.ProgressBar();
            this._prgPreview = new System.Windows.Forms.ProgressBar();
            this.label1 = new System.Windows.Forms.Label();
            this._prgJudges = new System.Windows.Forms.ProgressBar();
            this.label2 = new System.Windows.Forms.Label();
            this._timerJob = new System.Windows.Forms.Timer(this.components);
            this._lblEndTime = new System.Windows.Forms.Label();
            this._lblRemainingRenderTime = new System.Windows.Forms.Label();
            this._lblRemainingRenderTimeInfo = new System.Windows.Forms.Label();
            this._lblEndTimeInfo = new System.Windows.Forms.Label();
            this.SuspendLayout();
            // 
            // _lblTitle
            // 
            this._lblTitle.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this._lblTitle.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
            this._lblTitle.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this._lblTitle.Location = new System.Drawing.Point(3, 0);
            this._lblTitle.Name = "_lblTitle";
            this._lblTitle.Size = new System.Drawing.Size(287, 23);
            this._lblTitle.TabIndex = 0;
            this._lblTitle.Text = "Title";
            this._lblTitle.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // _lblSales
            // 
            this._lblSales.AutoSize = true;
            this._lblSales.Location = new System.Drawing.Point(22, 78);
            this._lblSales.Name = "_lblSales";
            this._lblSales.Size = new System.Drawing.Size(44, 13);
            this._lblSales.TabIndex = 1;
            this._lblSales.Text = "SALES:";
            // 
            // _prgSales
            // 
            this._prgSales.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this._prgSales.Location = new System.Drawing.Point(72, 72);
            this._prgSales.Name = "_prgSales";
            this._prgSales.Size = new System.Drawing.Size(213, 23);
            this._prgSales.TabIndex = 2;
            // 
            // _prgPreview
            // 
            this._prgPreview.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this._prgPreview.Location = new System.Drawing.Point(72, 43);
            this._prgPreview.Name = "_prgPreview";
            this._prgPreview.Size = new System.Drawing.Size(213, 23);
            this._prgPreview.TabIndex = 4;
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(6, 48);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(60, 13);
            this.label1.TabIndex = 3;
            this.label1.Text = "PREVIEW:";
            // 
            // _prgJudges
            // 
            this._prgJudges.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this._prgJudges.Location = new System.Drawing.Point(72, 101);
            this._prgJudges.Name = "_prgJudges";
            this._prgJudges.Size = new System.Drawing.Size(213, 23);
            this._prgJudges.TabIndex = 6;
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(13, 107);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(53, 13);
            this.label2.TabIndex = 5;
            this.label2.Text = "JUDGES:";
            // 
            // _timerJob
            // 
            this._timerJob.Tick += new System.EventHandler(this._timerJob_Tick);
            // 
            // _lblEndTime
            // 
            this._lblEndTime.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this._lblEndTime.AutoSize = true;
            this._lblEndTime.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this._lblEndTime.Location = new System.Drawing.Point(13, 151);
            this._lblEndTime.Name = "_lblEndTime";
            this._lblEndTime.Size = new System.Drawing.Size(104, 13);
            this._lblEndTime.TabIndex = 24;
            this._lblEndTime.Text = "Estimated End Time:";
            // 
            // _lblRemainingRenderTime
            // 
            this._lblRemainingRenderTime.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this._lblRemainingRenderTime.AutoSize = true;
            this._lblRemainingRenderTime.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this._lblRemainingRenderTime.Location = new System.Drawing.Point(31, 134);
            this._lblRemainingRenderTime.Name = "_lblRemainingRenderTime";
            this._lblRemainingRenderTime.Size = new System.Drawing.Size(86, 13);
            this._lblRemainingRenderTime.TabIndex = 23;
            this._lblRemainingRenderTime.Text = "Remaining Time:";
            // 
            // _lblRemainingRenderTimeInfo
            // 
            this._lblRemainingRenderTimeInfo.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this._lblRemainingRenderTimeInfo.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this._lblRemainingRenderTimeInfo.Location = new System.Drawing.Point(123, 134);
            this._lblRemainingRenderTimeInfo.Name = "_lblRemainingRenderTimeInfo";
            this._lblRemainingRenderTimeInfo.Size = new System.Drawing.Size(162, 13);
            this._lblRemainingRenderTimeInfo.TabIndex = 25;
            this._lblRemainingRenderTimeInfo.Text = "Remaining";
            // 
            // _lblEndTimeInfo
            // 
            this._lblEndTimeInfo.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this._lblEndTimeInfo.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this._lblEndTimeInfo.Location = new System.Drawing.Point(123, 151);
            this._lblEndTimeInfo.Name = "_lblEndTimeInfo";
            this._lblEndTimeInfo.Size = new System.Drawing.Size(162, 13);
            this._lblEndTimeInfo.TabIndex = 26;
            this._lblEndTimeInfo.Text = "Remaining";
            // 
            // ctrlEncodingJob
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.Color.Gray;
            this.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
            this.Controls.Add(this._lblEndTimeInfo);
            this.Controls.Add(this._lblRemainingRenderTimeInfo);
            this.Controls.Add(this._lblEndTime);
            this.Controls.Add(this._lblRemainingRenderTime);
            this.Controls.Add(this._prgJudges);
            this.Controls.Add(this.label2);
            this.Controls.Add(this._prgPreview);
            this.Controls.Add(this.label1);
            this.Controls.Add(this._prgSales);
            this.Controls.Add(this._lblSales);
            this.Controls.Add(this._lblTitle);
            this.Name = "ctrlEncodingJob";
            this.Size = new System.Drawing.Size(293, 175);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Label _lblTitle;
        private System.Windows.Forms.Label _lblSales;
        private System.Windows.Forms.ProgressBar _prgSales;
        private System.Windows.Forms.ProgressBar _prgPreview;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.ProgressBar _prgJudges;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Timer _timerJob;
        private System.Windows.Forms.Label _lblEndTime;
        private System.Windows.Forms.Label _lblRemainingRenderTime;
        private System.Windows.Forms.Label _lblRemainingRenderTimeInfo;
        private System.Windows.Forms.Label _lblEndTimeInfo;
    }
}
