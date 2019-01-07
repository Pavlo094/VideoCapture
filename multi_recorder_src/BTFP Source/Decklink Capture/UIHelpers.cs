using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace Decklink_Capture
{
    static class UIHelpers
    {
        public static void ShowError(string msg)
        {
#if DEBUG
            MessageBox.Show(msg);
            
#else
            MessageBox.Show(msg, "Decklink Capture Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
#endif
        }
        public static void ShowError(string msg, Exception ex)
        {
#if DEBUG
            MessageBox.Show(string.Format("{0}. Details: {1}", msg, ex.ToString()));
           
#else
            MessageBox.Show(string.Format("{0}. Details: {1}", msg, ex.Message), "Decklink Capture Error",
                MessageBoxButtons.OK, MessageBoxIcon.Error);
#endif
        }
        public static void ShowError(Exception ex)
        {
#if DEBUG
            MessageBox.Show(ex.ToString());
#else
            MessageBox.Show(ex.Message, "Decklink Capture Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
#endif
        }

        public static DialogResult ShowQuestion(string msg, string caption)
        {
            return MessageBox.Show(msg, caption, MessageBoxButtons.YesNoCancel, MessageBoxIcon.Question);
        }

        public static void ShowInfo(string msg, string caption)
        {
            MessageBox.Show(msg, caption, MessageBoxButtons.OK, MessageBoxIcon.Information);
        }
    }
}
