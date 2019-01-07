using System;
using System.Collections;
using System.ComponentModel;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Data;
using System.Windows.Forms;
using System.Runtime.InteropServices;

namespace Decklink_Capture
{
	/// <summary>
	/// Base class for all controls
	/// </summary>
	public class ucBase : System.Windows.Forms.UserControl
	{
		#region RapaintTypeEnum
		/// <summary>
		/// RapaintTypeEnum
		/// </summary>
		public enum RepaintTypeEnum
		{
			All,
			Fill,
			Border,
			Text,
			Image
		}
		#endregion

		#region Members
		/// <summary>
		/// Members
		/// </summary>
		private Color m_BorderColorLeftTop = Color.LightGray;
		private Color m_BorderColorRightBottom = Color.DarkGray;
		private Color m_FillColorFirst = Color.FromArgb(64,64,64);
		private Color m_FillColorSecond = Color.Gray;
		#endregion

		#region Const
		/// <summary>
		/// Const
		/// </summary>
		public const int WM_NCLBUTTONDOWN = 0xA1;
		public const int HT_CAPTION = 0x2;
		#endregion

		#region user32.dll
		/// <summary>
		/// Import SendMessage
		/// </summary>
		/// <param name="hWnd"></param>
		/// <param name="Msg"></param>
		/// <param name="wParam"></param>
		/// <param name="lParam"></param>
		/// <returns></returns>
		[DllImportAttribute ("user32.dll")]
		public static extern int SendMessage(IntPtr hWnd, int Msg, int wParam, int lParam);
		/// <summary>
		/// Import RelaseCapture
		/// </summary>
		/// <returns></returns>
		[DllImportAttribute ("user32.dll")]
		public static extern bool ReleaseCapture();
		#endregion

		#region Constructor
		/// <summary>
		/// Constructor
		/// </summary>
		public ucBase()
		{
			// This call is required by the Windows.Forms Form Designer.
			InitializeComponent();
		}
		#endregion

		#region Component Designer generated code
		/// <summary> 
		/// Required method for Designer support - do not modify 
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
			// 
			// ucBase
			// 
			this.BackColor = System.Drawing.Color.FromArgb(((System.Byte)(64)), ((System.Byte)(64)), ((System.Byte)(64)));
			this.Font = new System.Drawing.Font("Verdana", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
			this.ForeColor = System.Drawing.Color.Gainsboro;
			this.Name = "ucBase";
			this.Resize += new System.EventHandler(this.ucBase_Resize);
			this.Paint += new System.Windows.Forms.PaintEventHandler(this.ucBase_Paint);

		}
		#endregion

		#region OnResize
		/// <summary>
		/// OnResize
		/// </summary>
		/// <param name="sender"></param>
		/// <param name="e"></param>
		private void ucBase_Resize(object sender, System.EventArgs e)
		{
			// repaint control
			Invalidate();
		}
		#endregion

		#region DockForm
		/// <summary>
		/// DockForm while moving
		/// </summary>
		/// <param name="MoveForm"></param>
		/// <param name="BaseForm"></param>
		/// <param name="Space"></param>
		protected void DockForm(Form MoveForm,Form BaseForm,int Space)
		{
			if ( (MoveForm.Left < (BaseForm.Left + BaseForm.Width + Space)) && (MoveForm.Left > (BaseForm.Left + BaseForm.Width - Space) ) )
				MoveForm.Left = BaseForm.Left + BaseForm.Width;
			if ( (MoveForm.Top < (BaseForm.Top + BaseForm.Height + Space) ) && (MoveForm.Top > (BaseForm.Top + BaseForm.Height - Space) ) ) 
				MoveForm.Top = BaseForm.Top + BaseForm.Height;
			if ( (MoveForm.Left > BaseForm.Left - Space) && (MoveForm.Left < BaseForm.Left + Space) )
				MoveForm.Left = BaseForm.Left;
			if ( (MoveForm.Top > BaseForm.Top - Space) && (MoveForm.Top < BaseForm.Top + Space) )
				MoveForm.Top = BaseForm.Top;
			if( (MoveForm.Left + MoveForm.Width) < (BaseForm.Left + Space) && (MoveForm.Left + MoveForm.Width) > (BaseForm.Left - Space ) )
				MoveForm.Left = BaseForm.Left - MoveForm.Width;
			if( (MoveForm.Top + MoveForm.Height) < (BaseForm.Top + BaseForm.Height + Space) && (MoveForm.Top + MoveForm.Height) > (BaseForm.Top + BaseForm.Height - Space) )
				MoveForm.Top = (BaseForm.Top + BaseForm.Height) - MoveForm.Height;
		}
		#endregion

		#region OnPaint
		/// <summary>
		/// OnPaint
		/// </summary>
		/// <param name="sender"></param>
		/// <param name="e"></param>
		private void ucBase_Paint(object sender, System.Windows.Forms.PaintEventArgs e)
		{
			// draw control
			if( e.ClipRectangle.IntersectsWith(this.ClientRectangle) )
				DrawControl(e.Graphics);			
		}
		#endregion

		#region DrawControl
		/// <summary>
		/// DrawControl
		/// </summary>
		/// <param name="g"></param>
		protected virtual void DrawControl(Graphics g)
		{
			// fill control
			FillControl(g);
			// draw borders
			DrawBorders(g);
			// draw text
			DrawText(g);
			// draw image
			DrawImage(g);
		}
		#endregion

		#region Repaint
		/// <summary>
		/// Reapint
		/// </summary>
		protected virtual void Repaint(RepaintTypeEnum RepaintType)
		{
			// create graphics object
			Graphics g = this.CreateGraphics();

			// draw control
			if( RepaintType == RepaintTypeEnum.All )
				DrawControl(g);
			else if( RepaintType == RepaintTypeEnum.Fill )
				FillControl(g);
			else if ( RepaintType == RepaintTypeEnum.Border )
				DrawBorders(g);
			else if ( RepaintType == RepaintTypeEnum.Text )
				DrawText(g);
			else if( RepaintType == RepaintTypeEnum.Image )
				DrawImage(g);
			else
				DrawControl(g);

			// dispose
			g.Dispose();
		}
		#endregion

		#region Virtual Functions
		/// <summary>
		/// FillControl
		/// </summary>
		/// <param name="g"></param>
		protected virtual void FillControl(Graphics g){}
		/// <summary>
		/// DrawBorders
		/// </summary>
		/// <param name="g"></param>
		protected virtual void DrawBorders(Graphics g){}
		/// <summary>
		/// DrawText
		/// </summary>
		/// <param name="g"></param>
		protected virtual void DrawText(Graphics g){}
		/// <summary>
		/// DrawImage
		/// </summary>
		/// <param name="g"></param>
		protected virtual void DrawImage(Graphics g){}
		#endregion

		#region BorderColors
		/// <summary>
		/// BorderColorLeftTop Property
		/// </summary>
		public Color BorderColorLeftTop
		{
			get{return m_BorderColorLeftTop;}
			set{m_BorderColorLeftTop=value;Invalidate();}
		}
		/// <summary>
		/// BorderColorRightBottom Property
		/// </summary>
		public Color BorderColorRightBottom
		{
			get{return m_BorderColorRightBottom;}
			set{m_BorderColorRightBottom=value;Invalidate();}
		}
		#endregion

		#region FillColors
		/// <summary>
		/// FillColorFirst
		/// </summary>
		public Color FillColorFirst
		{
			get{return m_FillColorFirst;}
			set{m_FillColorFirst=value;Invalidate();}
		}
		/// <summary>
		/// FillColorSecond
		/// </summary>
		public Color FillColorSecond
		{
			get{return m_FillColorSecond;}
			set{m_FillColorSecond=value;Invalidate();}
		}
		#endregion
	}
}
