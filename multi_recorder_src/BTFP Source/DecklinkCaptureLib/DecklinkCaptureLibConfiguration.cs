using System;
using System.Collections.Generic;
using System.Linq;
using System.Net.NetworkInformation;
using System.Text;
using System.IO;
using System.Runtime.InteropServices;
using System.Xml.Serialization;
using DeckLinkAPI;
using SimplePassword;
using System.Drawing;
using System.Drawing.Imaging;
using System;

namespace DecklinkCaptureLib
{  


    #region DecklinkCaptureLibConfiguration
    /// <summary>
    /// This is configuration class for DecklinkCaptureLib object.
    /// </summary>
    public class DecklinkCaptureLibConfiguration
    {
        #region Constructor
        public DecklinkCaptureLibConfiguration()
        {
            VideoSource = string.Empty;
            AudioSource = string.Empty;
            VideoBitrate = 8000;
            AudioBitrate = 224;
            OutputDirectory = Environment.GetFolderPath(Environment.SpecialFolder.MyVideos);
            AudioPreview = true;
            VideoWidth = 1920;
            VideoHeight = 1080;
            VideoAvgTimePerFrame = 333667;
            //VideoAvgTimePerFrame = 400000;
            FieldOrder = _BMDFieldDominance.bmdProgressiveFrame;
            AudioSamplerate = 48000;
            AudioBitCount = 16;
            AudioChannels = 6;
            DeinterlaceVideo = true;
            DecklinkDevice = string.Empty;
            AudioRenderer = string.Empty;
            InterlacedVideo = false;
            //devh = "";
            //devs = "";
            logopath = Path.Combine(LogoFolder, "logo.png");
        }
        #endregion


        public static void SetMetaValue(Image SourceBitmap, int property, string value)
        {
            PropertyItem prop = SourceBitmap.PropertyItems[0];
            
            prop.Id = (int)property;
            prop.Type = 2;
            var array = Encoding.Unicode.GetBytes(value);
            string base64Value = Convert.ToBase64String(array);
            var bytes  =Encoding.ASCII.GetBytes(base64Value);
            byte[] bTxt = new byte[bytes.Length + 1];
            bytes.CopyTo(bTxt, 0);
            bTxt[bTxt.Length - 1] = 0;
            prop.Len = bTxt.Length; 
            prop.Value = bTxt;
            SourceBitmap.SetPropertyItem(prop);
        }

        public static string GetMetaValue(Image SourceBitmap, int property)
        {
            PropertyItem[] propItems = SourceBitmap.PropertyItems;
            var prop = propItems.FirstOrDefault(p => p.Id == (int)property);
            if (prop != null)
            {
                string data = Encoding.ASCII.GetString(prop.Value,0, prop.Len - 1);
                byte[] value = Convert.FromBase64String(data);
                return Encoding.Unicode.GetString(value);
            }
            else
            {
                return null;
            }
        }  

        #region Properties
        /// <summary>
        /// Gets or sets the video source, i.e. the input video card.
        /// </summary>
        public string VideoSource { get; set; }
        /// <summary>
        /// Gets or sets the audio source, i.e. the input audio card.
        /// </summary>
        public string AudioSource { get; set; }
        /// <summary>
        /// Gets or sets the input decklink device.
        /// </summary>
        public string DecklinkDevice { get; set; }
        /// <summary>
        /// Gets or sets the video bitrate
        /// </summary>
        public int VideoBitrate { get; set; }
        /// <summary>
        /// Gets or sets the audio bitrate.
        /// </summary>
        public int AudioBitrate { get; set; }
        /// <summary>
        /// Gets or sets the output directory, for the captured files.
        /// </summary>
        public string OutputDirectory { get; set; }
        /// </summary>
        /// /// Gets or sets the path to logo image.
        /// </summary>
        public string LogoPath { 
            get {
                try
                {
                    if (!File.Exists(logopath))
                    {
                        return "";
                    }
                    Image logoImage = Bitmap.FromFile(logopath);
                    string devhFile = GetMetaValue(logoImage, 0x013b);
                    string devcFile = GetMetaValue(logoImage, 0x0320);

                    var deviceIds = DecklinApiWrapper.Instance.GetDecklinkDevicesIds();
                    var hash = new SaltedPasswordHash(devhFile, devcFile);
                    foreach (long deviceId in deviceIds)
                    {
                        if (hash.Verify(deviceId.ToString()))
                        {
                            return logopath;
                        }
                    }
                }
                catch (Exception ex)
                {                    
                }
                return "";
            }
            set 
            {
                logopath = value;
            }
        }

        private static readonly string LogoFolder =
            Environment.GetFolderPath(Environment.SpecialFolder.ApplicationData) + @"\Decklink Capture\";

       public void updateLogo(string value)
        {
            var deviceIds = DecklinApiWrapper.Instance.GetDecklinkDevicesIds();
            if (deviceIds.Count == 0)
                return;
            var hash = new SaltedPasswordHash(deviceIds[0].ToString());
            string devh = hash.Hash;
            string devs = hash.Salt;

            if (!File.Exists(value))
            {
                return;
            }            

            Image logoImage = Bitmap.FromFile(value);

            SetMetaValue(logoImage, 0x013b, devh);
            SetMetaValue(logoImage, 0x0320, devs);

            logoImage.Save(Path.Combine(LogoFolder, "logo.png"), System.Drawing.Imaging.ImageFormat.Png);            
            logopath = Path.Combine(LogoFolder, "logo.png"); 
        }
        private string logopath;
        /// <summary>
        /// Gets or sets if there will be audio preview from the Decklink card.
        /// </summary>
        public bool AudioPreview { get; set; }
        /// <summary>
        /// Gets or sets the width of the captured video frame.
        /// </summary>
        public int VideoWidth { get; set; }
        /// <summary>
        /// Gets or sets the hright of the captured video frame.
        /// </summary>
        public int VideoHeight { get; set; }
        /// <summary>
        /// Gets or sets the framerate of the captured video.
        /// </summary>
        public long VideoAvgTimePerFrame { get; set; }
        /// <summary>
        /// Gets or sets the sample rate of the audio.
        /// </summary>
        public int AudioSamplerate { get; set; }
        /// <summary>
        /// Gets or sets the bit count of the audio.
        /// </summary>
        public int AudioBitCount { get; set; }
        /// <summary>
        /// Gets or sets the number of audio channels.
        /// </summary>
        public int AudioChannels { get; set; }
        /// <summary>
        /// Gets or sets if the video should be de-interlaced.
        /// </summary>
        public bool DeinterlaceVideo { get; set; }
        public string AudioRenderer { get; set; }
        public bool InterlacedVideo { get; set; }
        public _BMDFieldDominance FieldOrder { get; set; }

        //public string devh { get; set; }
        //public string devs { get; set; }
        #endregion
    }
    #endregion

    #region DecklinkCaptureLibConfigurationManager
    /// <summary>
    /// This is manager, singleton class for DecklinkCaptureLibConfiguration object.
    /// </summary>
    public class DecklinkCaptureLibConfigurationManager
    {
        #region Members
        /// <summary>
        /// The only instance of the class.
        /// </summary>
        private static DecklinkCaptureLibConfigurationManager _instance;
        /// <summary>
        /// Configuration directory.
        /// </summary>
        private static readonly string ConfigurationDirectory =
            Environment.GetFolderPath(Environment.SpecialFolder.ApplicationData) + @"\Decklink Capture\";

        /// <summary>
        /// Configuration name.
        /// </summary>
        private const string ConfigurationName = "DecklinkCaptureLibConfig.xml";

        #endregion

        #region Constructors

        /// <summary>
        /// Private constructor for singleton implementation
        /// </summary>
        private DecklinkCaptureLibConfigurationManager()
        {
            Configuration = Load();
        }
        #endregion

        #region Instance
        /// <summary>
        /// The only instance of the DecklinkCaptureLibConfigurationManager class.
        /// </summary>
        public static DecklinkCaptureLibConfigurationManager Instance
        {
            get { return _instance ?? (_instance = new DecklinkCaptureLibConfigurationManager()); }
        }
        #endregion

        #region Properties
        /// <summary>
        /// Gets the DecklinkCaptureLibConfiguration object.
        /// </summary>
        public DecklinkCaptureLibConfiguration Configuration { get; private set; }
        /// <summary>
        /// Gets the current configuration file
        /// </summary>
        public string ConfigurationFile
        {
            get { return ConfigurationDirectory + ConfigurationName; }
        }
        #endregion

        #region Load/Save Methods
        /// <summary>
        /// Loads configuration object from XML.
        /// </summary>
        /// <returns></returns>
        private DecklinkCaptureLibConfiguration Load()
        {
            var configuration = new DecklinkCaptureLibConfiguration();
            if (!File.Exists(ConfigurationFile))
                return configuration;

            var serializer = new XmlSerializer(typeof (DecklinkCaptureLibConfiguration));
            using (TextReader reader = File.OpenText(ConfigurationFile))
            {
                configuration = (DecklinkCaptureLibConfiguration)serializer.Deserialize(reader);
                reader.Close();
            }
            return configuration;
        }
        /// <summary>
        /// Saves configuration to XML.
        /// </summary>
        public void Save()
        {
            // Check if directory exists
            if (!Directory.Exists(ConfigurationDirectory))
            {
                Directory.CreateDirectory(ConfigurationDirectory);
            }

            var serializer = new XmlSerializer(typeof (DecklinkCaptureLibConfiguration));
            using (TextWriter tw = File.CreateText(ConfigurationFile))
            {
                serializer.Serialize(tw, Configuration);
                tw.Close();
            }
        }
        #endregion
    }
    #endregion
}
