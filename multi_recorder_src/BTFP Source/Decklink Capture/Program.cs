using System;
using System.Collections.Generic;
using System.Linq;
using System.Windows.Forms;

namespace Decklink_Capture
{
    static class Program
    {
        /// <summary>
        /// The main entry point for the application.
        /// MTAThread for using the EVR
        /// </summary>
        [MTAThread]
        //[STAThread]
        static void Main()
        {
            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);
            Application.Run(new frmMain());
        }
    }
}
