using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Forms;


namespace DecklinkCaptureLib
{
    /// <summary>
    /// This class creates invisible window. This is used for receiving windows messages and
    /// filtering DirectShow ones.
    /// </summary>
    internal sealed class MsgOnlyWindow : NativeWindow
    {
        public MsgOnlyWindow()
        {
            var cp = new CreateParams {Style = 0, ExStyle = 0, ClassStyle = 0, Caption = GetType().Name};

            if (Environment.OSVersion.Platform == PlatformID.Win32NT)
            {
                cp.Parent = (IntPtr) (-3);//HWND_MESSAGE
            }

            CreateHandle(cp);
        }

        protected override void WndProc(ref Message m)
        {
            switch (m.Msg)
            {
                case DecklinkCaptureLib.WM_GRAPHNOTIFY:
                    DecklinkCaptureLib.Instance.HandleEvents();
                    break;
            }
            base.WndProc(ref m);
        }
    }
}
