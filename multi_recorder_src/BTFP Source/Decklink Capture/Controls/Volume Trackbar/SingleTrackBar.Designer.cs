namespace Decklink_Capture
{
    partial class SingleTrackBar
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(SingleTrackBar));
            this.pointerControl1 = new PointerControl();
            this.SuspendLayout();
            // 
            // pointerControl1
            // 
            this.pointerControl1.BackgroundImage = ((System.Drawing.Image)(resources.GetObject("pointerControl1.BackgroundImage")));
            this.pointerControl1.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Center;
            this.pointerControl1.Location = new System.Drawing.Point(3, 3);
            this.pointerControl1.Name = "pointerControl1";
            this.pointerControl1.Size = new System.Drawing.Size(24, 36);
            this.pointerControl1.TabIndex = 0;
            // 
            // SingleTrackBar
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.Controls.Add(this.pointerControl1);
            this.Name = "SingleTrackBar";
            this.Size = new System.Drawing.Size(30, 210);
            this.ResumeLayout(false);

        }

        #endregion

        private PointerControl pointerControl1;



    }
}
