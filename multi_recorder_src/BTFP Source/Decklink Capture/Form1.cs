using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace Decklink_Capture
{
    public partial class Password : Form
    {
        public Password()
        {
            isOk = false;
            InitializeComponent();
        }

        public String getData()
        {
            if (isOk)
                return this.passwordBox.Text;
            else
                return null;
        }

        private void button1_Click(object sender, EventArgs e)
        {
            isOk = true;            
            Close();
        }

        private bool isOk { get; set; }
    }
}
