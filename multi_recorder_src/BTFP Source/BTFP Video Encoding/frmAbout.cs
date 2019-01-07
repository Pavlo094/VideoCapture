using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using SimplePassword;
using System.Threading;

namespace Decklink_Capture
{
    public partial class frmAbout : Form
    {
        public frmAbout()
        {
            InitializeComponent();
        }

        private void frmAbout_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.KeyData == (Keys.Control | Keys.L))
            {
                openLogoDialog();
            }
        }

        private void openLogoDialog()
        {
            try
            {
                Password frm = new Password();
                frm.ShowDialog();

                if (frm.getData() != null)
                {
                    string salt = "UQTg87DSVnK+JzM8YVIkvaRcphTXgs/Dz4rz7Ft5ITHSomrdPU8WoiYdqpIOk1FJi2X/eeZsRyOC5MlL4zlXqGupwxJjPatFrJBKeLqftxj8/Xk7VjEjFSNnwAXVl2RjtGoaUe6K3Tp36LqyxMggQ0spNO/I1XNDy1ggHJsTq2u4WjyKaOoS+deC3sbdBEZVPvzKA8nia1uvZLamED5fAjcr62XzDmJfq9WkbRSu8kiVBVcelyDZHMbXZGj0xSXBzKDcqhFdTFCrgdRWk7ec0QmOe+zK2hRDlWDtc/ZrWao6GCjT7LGUOlBsGNx/JkP7EsjTb8XOCq0VtjmncDYP51VRkDe0a5snoSs0bWhtENdlphFa2ilmkmJ8zlPbjEWyhFYs4pwPEoY21fDn3CuhIRiQOAVm1ffc9Hj7PQgdm7NXR5wq8H6gYYMWjvP46MLfam11WiXH84JoVwSD1gV1fMZoy2ivIF2D4Tman8rqM507vk3lEI5xdQplnIvTJx0z6l3mJhRK3llCJVwklRndQjIMI+PBlg5A4GEkiBdvXtpU2fcsWoEXl6qWnOjAyzZhFrTbax0i2Gg+jE0khR3bb6UhwOxo0E/8pDI3KuOOV/SMsMd/pBwftZSpGZ/UE5IW87TpNLEPqDsjeU8amppw2z7o7zgMGkdJP2A0x92SMP4=";
                    string hash = "獁厙㳪ܣ霊斚⤆鹪뤜╊﵇蓷즍綩凵覧礡﷍哤ㅕ�孏쯋덑䐋悯콥ꉘᇑ";
                    var passwordHash = new SaltedPasswordHash(hash, salt);

                    if (!passwordHash.Verify(frm.getData()))
                    {
                        MessageBox.Show("Password is incorrect");
                        return;
                    }
                }
                // Create an instance of the open file dialog box.
                OpenFileDialog openFileDialog = new OpenFileDialog();

                // Set filter options and filter index.
                openFileDialog.Filter = "Image Files(*.BMP;*.JPG;*.PNG;*.GIF)|*.BMP;*.JPG;*.PNG;*.GIF|All files (*.*)|*.*";
                openFileDialog.FilterIndex = 1;
                openFileDialog.Multiselect = false;


                DialogInvoker invoker = new DialogInvoker(openFileDialog);
                var dr = invoker.Invoke();
                if (dr != DialogResult.OK) return;

                BTFPVideoEncodingLib.BTFPVideoEncodingLibConfigurationManager.Instance.Configuration.updateLogo(openFileDialog.FileName);
                BTFPVideoEncodingLib.BTFPVideoEncodingLibConfigurationManager.Instance.Save();

                updateLogo(BTFPVideoEncodingLib.BTFPVideoEncodingLibConfigurationManager.Instance.Configuration.LogoPath);
                //textLogoPath.Text = openFileDialog.FileName;
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.ToString());
            }
        }

        internal void updateLogo(string p)
        {
            if (p.Length != 0)
            {
                pictureBox1.Load(p);
            }
            else
            {
                pictureBox1.Image = BTFP_Video_Encoding.Properties.Resources.UnauthorizedUseLogo;
            }
        }
    }

    #region DialogInvoker
    /// <summary>
    /// Opens a specified Dialog box on an STA thread
    /// </summary>
    public class DialogInvoker
    {
        #region Members
        private string _fileName;
        private DialogResult _invokeResult;
        private CommonDialog _dialog;
        #endregion

        // Constructor is passed the dialog to use
        public DialogInvoker(CommonDialog dialog)
        {
            _invokeResult = DialogResult.None;
            _fileName = null;
            _dialog = dialog;
        }

        /// <summary>
        /// Start the thread and get the result
        /// </summary>
        /// <returns></returns>
        public DialogResult Invoke()
        {
            Thread thdInvokeThread;

            // No reason to waste a thread if we aren't MTA
            if (Thread.CurrentThread.GetApartmentState() == ApartmentState.MTA)
            {
                thdInvokeThread = new Thread(new ThreadStart(InvokeMethod));
                thdInvokeThread.SetApartmentState(ApartmentState.STA);

                thdInvokeThread.Start();
                thdInvokeThread.Join();
                thdInvokeThread = null;
            }
            else
            {
                // If we are already STA, just invoke the method
                InvokeMethod();
            }

            return _invokeResult;
        }
        /// <summary>
        /// Filename or selected path, depending if the dialog is OpenFileDialog, SaveFileDialog or FolderBrowserDialog
        /// </summary>
        public string Path
        {
            get { return _fileName; }
        }

        // The thread entry point
        private void InvokeMethod()
        {
            _invokeResult = _dialog.ShowDialog();

            if (_invokeResult == DialogResult.OK)
            {
                if (_dialog is OpenFileDialog)
                {
                    _fileName = ((OpenFileDialog)_dialog).FileName;
                }
                else if (_dialog is SaveFileDialog)
                {
                    _fileName = ((SaveFileDialog)_dialog).FileName;
                }
                else if (_dialog is FolderBrowserDialog)
                {
                    _fileName = ((FolderBrowserDialog)_dialog).SelectedPath;
                }
            }
        }
    }
    #endregion
}
