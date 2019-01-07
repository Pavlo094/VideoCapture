namespace BTFPVideoEncodingLibTest
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
            this.label1 = new System.Windows.Forms.Label();
            this._txtInput = new System.Windows.Forms.TextBox();
            this._btnInputBrowse = new System.Windows.Forms.Button();
            this._btnEncodeSales = new System.Windows.Forms.Button();
            this._btnEncodePreview = new System.Windows.Forms.Button();
            this._btnEncodeJudges = new System.Windows.Forms.Button();
            this._prgSales = new System.Windows.Forms.ProgressBar();
            this.label2 = new System.Windows.Forms.Label();
            this.label3 = new System.Windows.Forms.Label();
            this._prgPreview = new System.Windows.Forms.ProgressBar();
            this.label4 = new System.Windows.Forms.Label();
            this._prgJudges = new System.Windows.Forms.ProgressBar();
            this.timer1 = new System.Windows.Forms.Timer(this.components);
            this._btnEncodeAll = new System.Windows.Forms.Button();
            this._timerJob = new System.Windows.Forms.Timer(this.components);
            this.SuspendLayout();
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(12, 9);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(50, 13);
            this.label1.TabIndex = 0;
            this.label1.Text = "Input File";
            // 
            // _txtInput
            // 
            this._txtInput.Location = new System.Drawing.Point(12, 25);
            this._txtInput.Name = "_txtInput";
            this._txtInput.Size = new System.Drawing.Size(518, 20);
            this._txtInput.TabIndex = 1;
            this._txtInput.Text = "c:\\video\\2.mp4";
            // 
            // _btnInputBrowse
            // 
            this._btnInputBrowse.Location = new System.Drawing.Point(536, 23);
            this._btnInputBrowse.Name = "_btnInputBrowse";
            this._btnInputBrowse.Size = new System.Drawing.Size(41, 23);
            this._btnInputBrowse.TabIndex = 2;
            this._btnInputBrowse.Text = "...";
            this._btnInputBrowse.UseVisualStyleBackColor = true;
            this._btnInputBrowse.Click += new System.EventHandler(this._btnInputBrowse_Click);
            // 
            // _btnEncodeSales
            // 
            this._btnEncodeSales.Location = new System.Drawing.Point(12, 51);
            this._btnEncodeSales.Name = "_btnEncodeSales";
            this._btnEncodeSales.Size = new System.Drawing.Size(122, 23);
            this._btnEncodeSales.TabIndex = 3;
            this._btnEncodeSales.Text = "Encode Sales";
            this._btnEncodeSales.UseVisualStyleBackColor = true;
            this._btnEncodeSales.Click += new System.EventHandler(this._btnEncodeSales_Click);
            // 
            // _btnEncodePreview
            // 
            this._btnEncodePreview.Location = new System.Drawing.Point(140, 51);
            this._btnEncodePreview.Name = "_btnEncodePreview";
            this._btnEncodePreview.Size = new System.Drawing.Size(122, 23);
            this._btnEncodePreview.TabIndex = 4;
            this._btnEncodePreview.Text = "Encode Preview";
            this._btnEncodePreview.UseVisualStyleBackColor = true;
            this._btnEncodePreview.Click += new System.EventHandler(this._btnEncodePreview_Click);
            // 
            // _btnEncodeJudges
            // 
            this._btnEncodeJudges.Location = new System.Drawing.Point(268, 51);
            this._btnEncodeJudges.Name = "_btnEncodeJudges";
            this._btnEncodeJudges.Size = new System.Drawing.Size(122, 23);
            this._btnEncodeJudges.TabIndex = 5;
            this._btnEncodeJudges.Text = "Encode Judges";
            this._btnEncodeJudges.UseVisualStyleBackColor = true;
            this._btnEncodeJudges.Click += new System.EventHandler(this._btnEncodeJudges_Click);
            // 
            // _prgSales
            // 
            this._prgSales.Location = new System.Drawing.Point(12, 104);
            this._prgSales.Name = "_prgSales";
            this._prgSales.Size = new System.Drawing.Size(558, 23);
            this._prgSales.TabIndex = 6;
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(12, 88);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(76, 13);
            this.label2.TabIndex = 7;
            this.label2.Text = "Sales progress";
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(12, 135);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(88, 13);
            this.label3.TabIndex = 9;
            this.label3.Text = "Preview progress";
            // 
            // _prgPreview
            // 
            this._prgPreview.Location = new System.Drawing.Point(12, 151);
            this._prgPreview.Name = "_prgPreview";
            this._prgPreview.Size = new System.Drawing.Size(558, 23);
            this._prgPreview.TabIndex = 8;
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(12, 185);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(84, 13);
            this.label4.TabIndex = 11;
            this.label4.Text = "Judges progress";
            // 
            // _prgJudges
            // 
            this._prgJudges.Location = new System.Drawing.Point(12, 201);
            this._prgJudges.Name = "_prgJudges";
            this._prgJudges.Size = new System.Drawing.Size(558, 23);
            this._prgJudges.TabIndex = 10;
            // 
            // timer1
            // 
            this.timer1.Tick += new System.EventHandler(this.timer1_Tick);
            // 
            // _btnEncodeAll
            // 
            this._btnEncodeAll.Location = new System.Drawing.Point(396, 51);
            this._btnEncodeAll.Name = "_btnEncodeAll";
            this._btnEncodeAll.Size = new System.Drawing.Size(122, 23);
            this._btnEncodeAll.TabIndex = 12;
            this._btnEncodeAll.Text = "Encode All";
            this._btnEncodeAll.UseVisualStyleBackColor = true;
            this._btnEncodeAll.Click += new System.EventHandler(this._btnEncodeAll_Click);
            // 
            // _timerJob
            // 
            this._timerJob.Tick += new System.EventHandler(this._timerJob_Tick);
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(582, 270);
            this.Controls.Add(this._btnEncodeAll);
            this.Controls.Add(this.label4);
            this.Controls.Add(this._prgJudges);
            this.Controls.Add(this.label3);
            this.Controls.Add(this._prgPreview);
            this.Controls.Add(this.label2);
            this.Controls.Add(this._prgSales);
            this.Controls.Add(this._btnEncodeJudges);
            this.Controls.Add(this._btnEncodePreview);
            this.Controls.Add(this._btnEncodeSales);
            this.Controls.Add(this._btnInputBrowse);
            this.Controls.Add(this._txtInput);
            this.Controls.Add(this.label1);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.Fixed3D;
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "Form1";
            this.Text = "Form1";
            this.Load += new System.EventHandler(this.Form1_Load);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.TextBox _txtInput;
        private System.Windows.Forms.Button _btnInputBrowse;
        private System.Windows.Forms.Button _btnEncodeSales;
        private System.Windows.Forms.Button _btnEncodePreview;
        private System.Windows.Forms.Button _btnEncodeJudges;
        private System.Windows.Forms.ProgressBar _prgSales;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.ProgressBar _prgPreview;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.ProgressBar _prgJudges;
        private System.Windows.Forms.Timer timer1;
        private System.Windows.Forms.Button _btnEncodeAll;
        private System.Windows.Forms.Timer _timerJob;
    }
}

