using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Xml.Serialization;
using DeckLinkAPI;
using SimplePassword;
using DecklinkCaptureLib;
using System.Drawing;
using System.Drawing.Imaging;

namespace BTFPVideoEncodingLib
{
    #region BTFPVideoEncodingLibConfigurationManager
    /// <summary>
    /// This is singleton implementation that manages access to BTFPVideoEncodingLibConfiguration configuration class.
    /// </summary>
    public class BTFPVideoEncodingLibConfigurationManager
    {
        #region Members
        /// <summary>
        /// The only instance of the class.
        /// </summary>
        private static BTFPVideoEncodingLibConfigurationManager _instance;
        /// <summary>
        /// Configuration directory.
        /// </summary>
        private static readonly string ConfigurationDirectory =
            Environment.GetFolderPath(Environment.SpecialFolder.ApplicationData) + @"\BTFP Video Encoding\";

        /// <summary>
        /// Configuration name.
        /// </summary>
        private const string ConfigurationName = "BTFPVideoEncodingConfig.xml";
        #endregion

        #region Constructors
        /// <summary>
        /// Private constructor for singleton implementation
        /// </summary>
        private BTFPVideoEncodingLibConfigurationManager()
        {
            Configuration = Load();
        }
        #endregion

        #region Instance
        /// <summary>
        /// The only instance of the DecklinkCaptureLibConfigurationManager class.
        /// </summary>
        public static BTFPVideoEncodingLibConfigurationManager Instance
        {
            get { return _instance ?? (_instance = new BTFPVideoEncodingLibConfigurationManager()); }
        }
        #endregion

        #region Properties
        /// <summary>
        /// Gets the DecklinkCaptureLibConfiguration object.
        /// </summary>
        public BTFPVideoEncodingLibConfiguration Configuration { get; private set; }
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
        private BTFPVideoEncodingLibConfiguration Load()
        {
            var configuration = new BTFPVideoEncodingLibConfiguration();
            if (!File.Exists(ConfigurationFile))
                return configuration;

            var serializer = new XmlSerializer(typeof(BTFPVideoEncodingLibConfiguration));
            using (TextReader reader = File.OpenText(ConfigurationFile))
            {
                configuration = (BTFPVideoEncodingLibConfiguration)serializer.Deserialize(reader);
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

            var serializer = new XmlSerializer(typeof(BTFPVideoEncodingLibConfiguration));
            using (TextWriter tw = File.CreateText(ConfigurationFile))
            {
                serializer.Serialize(tw, Configuration);
                tw.Close();
            }
        }
        #endregion
    }
    #endregion

    #region BTFPVideoEncodingLibConfiguration
    public class BTFPVideoEncodingLibConfiguration
    {
        #region Constructor
        /// <summary>
        /// Default constructor.
        /// </summary>
        public BTFPVideoEncodingLibConfiguration()
        {
            SalesEncodingParams = BTFPEncodingParameters.LoadDefaultSalesParams();
            PreviewEncodingParams = BTFPEncodingParameters.LoadDefaultPreviewParams();
            JudgesEncodingParams = BTFPEncodingParameters.LoadDefaultJudgesParams();
            NumberOfEncodingThreads = 1;
            DirectoryMonitor = Environment.GetFolderPath(Environment.SpecialFolder.MyVideos) + "\\Monitor Folder\\";
            DirectoryFinished = Environment.GetFolderPath(Environment.SpecialFolder.MyVideos) + "\\Finished Folder\\";
            DirectoryErrored = Environment.GetFolderPath(Environment.SpecialFolder.MyVideos) + "\\Error Folder\\";
            logopath = Path.Combine(LogoFolder, "logo.png");
        }
        #endregion

        #region Properties
        /// <summary>
        /// Gets or sets encoding parameters for creating SALES videos.
        /// </summary>
        public BTFPEncodingParameters SalesEncodingParams { get; set; }
        /// <summary>
        /// Gets or sets encoding parameters for creating PREVIEW videos.
        /// </summary>
        public BTFPEncodingParameters PreviewEncodingParams { get; set; }
        /// <summary>
        /// Gets or sets encoding parameters for creating JUDGES videos.
        /// </summary>
        public BTFPEncodingParameters JudgesEncodingParams { get; set; }
        /// <summary>
        /// Gets or sets number of threads that should work in parallel.
        /// </summary>
        public int NumberOfEncodingThreads { get; set; }
        /// <summary>
        /// Gets or sets the directory that is monitored for input files.
        /// </summary>
        public string DirectoryMonitor { get; set; }
        /// <summary>
        /// Gets or sets the directory where input video files that are finished with encoding are moved.
        /// </summary>
        public string DirectoryFinished { get; set; }
        /// <summary>
        /// Gets or sets the directory where input video files that are errored while encoding are moved.
        /// </summary>
        public string DirectoryErrored { get; set; }

        public static void SetMetaValue(Image SourceBitmap, int property, string value)
        {
            PropertyItem prop = SourceBitmap.PropertyItems[0];

            prop.Id = (int)property;
            prop.Type = 2;
            var array = Encoding.Unicode.GetBytes(value);
            string base64Value = Convert.ToBase64String(array);
            var bytes = Encoding.ASCII.GetBytes(base64Value);
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
                string data = Encoding.ASCII.GetString(prop.Value, 0, prop.Len - 1);
                byte[] value = Convert.FromBase64String(data);
                return Encoding.Unicode.GetString(value);
            }
            else
            {
                return null;
            }
        }  

        public string LogoPath
        {
            get
            {
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
                    var hash = new SaltedPasswordHash(devh, devs);
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
        public string devh { get; set; }
        public string devs { get; set; }
        #endregion
    }
    #endregion

    #region BTFPEncodingParameters
    /// <summary>
    /// This class holds parameters for video/audio encoding of the different output files
    /// produced by the BTFPVideoEncoding application.
    /// </summary>
    [Serializable()]
    public class BTFPEncodingParameters
    {
        #region Encoding Params Names

        public const string SalesParamsName = "SalesParams";
        public const string PreviewParamsName = "PreviewParams";
        public const string JudgesParamsName = "JudgesParams";
        #endregion

        #region Constructor
        /// <summary>
        /// Default constructor.
        /// </summary>
        public BTFPEncodingParameters()
        {
            VideoBitrate = 8000;
            AudioBitrate = 128;
            VideoWidth = 1920;
            VideoHeight = 1080;
            OutputDirectory = Environment.GetFolderPath(Environment.SpecialFolder.MyVideos);
            FileNameAppendix = "_";
            AudioVolumeBoost = 0;
            SkipThisEncode = false;
            SkipVideoTranscode = false;
            AudioChannelsMap = new int[6, 6];
            InterlacedVideo = true;
            //for (int i = 0; i < 6; i++)
            //{
            //    for (int j = 0; j < 6; j++)
            //    {
            //        if (i == j)
            //        {
            //            AudioChannelsMap[i, j] = 1;
            //        }
            //    }
            //}
        }
        #endregion

        #region Properties
        /// <summary>
        /// Gets or sets the video bitrate
        /// </summary>
        public int VideoBitrate { get; set; }
        /// <summary>
        /// Gets or sets the audio bitrate.
        /// </summary>
        public int AudioBitrate { get; set; }
        /// <summary>
        /// Gets or sets the width of the captured video frame.
        /// </summary>
        public int VideoWidth { get; set; }
        /// <summary>
        /// Gets or sets the hright of the captured video frame.
        /// </summary>
        public int VideoHeight { get; set; }
        /// <summary>
        /// Gets or sets the output directory, for the captured files.
        /// </summary>
        public string OutputDirectory { get; set; }
        /// <summary>
        /// Gets or sets the appendix added to the output file.
        /// </summary>
        public string FileNameAppendix { get; set; }
        /// <summary>
        /// Gets or sets the audio volume boost applied to output file, in decibels.
        /// </summary>
        public int AudioVolumeBoost { get; set; }
        /// <summary>
        /// Gets or sets if this specific encode should be skipped.
        /// </summary>
        public bool SkipThisEncode { get; set; }
        /// <summary>
        /// Gets or sets the name of the configuration.
        /// </summary>
        public string ConfigurationName { get; set; }
        /// <summary>
        /// Gets or sets if the video should be transcoded too.
        /// </summary>
        public bool SkipVideoTranscode { get; set; }
        /// <summary>
        /// Gets or sets the audio channel remapping matrix.
        /// </summary>
        [XmlIgnore()]
        public int [,] AudioChannelsMap { get; set; }

        public int[][] AudioChannelsMapSerializable
        {
            get
            {
                int[][] temp = new int[6][];
                for (int i = 0; i < 6; i++)
                {
                    temp[i] = new int[6];
                }
                for (int i = 0; i < 6; i++)
                {
                    for (int j = 0; j < 6; j++)
                    {
                        temp[i][j] = AudioChannelsMap[i, j];
                    }
                }

                return temp;
            }
            set
            {
                for (int i = 0; i < 6; i++)
                {
                    for (int j = 0; j < 6; j++)
                    {
                        AudioChannelsMap[i, j] = value[i][j];
                    }
                }
            }
        }
        public bool InterlacedVideo { get; set; }
        #endregion

        #region Defaults
        /// <summary>
        /// Creates new instance of BTFPEncodingParameters class and sets it's parameters
        /// to SALES output videos.
        /// </summary>
        /// <returns></returns>
        public static BTFPEncodingParameters LoadDefaultSalesParams()
        {
            var parameters = new BTFPEncodingParameters();
            parameters.VideoWidth = 1920;
            parameters.VideoHeight = 1080;
            parameters.VideoBitrate = 8000;
            parameters.AudioBitrate = 128;
            parameters.OutputDirectory = Environment.GetFolderPath(Environment.SpecialFolder.MyVideos) +
                                         "\\MP4 Sales Folder\\";
            parameters.FileNameAppendix = string.Empty;
            parameters.AudioVolumeBoost = 0;
            parameters.SkipThisEncode = false;
            parameters.SkipVideoTranscode = true;

            parameters.ConfigurationName = SalesParamsName;

            for (int i = 0; i < 6; i++)
            {
                for (int j = 0; j < 6; j++)
                {
                    if (i == 0 && j == 0)
                    {
                        parameters.AudioChannelsMap[0, 0] = 1;
                    }
                    else if (i == 1 && j == 1)
                    {
                        parameters.AudioChannelsMap[1, 1] = 1;
                    }
                    else
                    {
                        parameters.AudioChannelsMap[i, j] = 0;
                    }
                }
            }

            return parameters;
        }
        /// <summary>
        /// Creates new instance of BTFPEncodingParameters class and sets it's parameters
        /// to PREVIEW output videos.
        /// </summary>
        /// <returns></returns>
        public static BTFPEncodingParameters LoadDefaultPreviewParams()
        {
            var parameters = new BTFPEncodingParameters();
            parameters.VideoWidth = 1280;
            parameters.VideoHeight = 720;
            parameters.VideoBitrate = 8000;
            parameters.AudioBitrate = 128;
            parameters.OutputDirectory = Environment.GetFolderPath(Environment.SpecialFolder.MyVideos) +
                                         "\\MP4 Preview Folder\\";
            parameters.FileNameAppendix = "_MP4_PVW";
            parameters.AudioVolumeBoost = 0;
            parameters.SkipThisEncode = false;
            parameters.SkipVideoTranscode = false;

            parameters.ConfigurationName = PreviewParamsName;

            for (int i = 0; i < 6; i++)
            {
                for (int j = 0; j < 6; j++)
                {
                    if (i == 0 && j == 0)
                    {
                        parameters.AudioChannelsMap[0, 0] = 1;
                    }
                    else if (i == 1 && j == 1)
                    {
                        parameters.AudioChannelsMap[1, 1] = 1;
                    }
                    else
                    {
                        parameters.AudioChannelsMap[i, j] = 0;
                    }
                }
            }

            return parameters;
        }
        /// <summary>
        /// Creates new instance of BTFPEncodingParameters class and sets it's parameters
        /// to JUDGES output videos.
        /// </summary>
        /// <returns></returns>
        public static BTFPEncodingParameters LoadDefaultJudgesParams()
        {
            var parameters = new BTFPEncodingParameters();
            parameters.VideoWidth = 1280;
            parameters.VideoHeight = 720;
            parameters.VideoBitrate = 3000;
            parameters.AudioBitrate = 128;
            parameters.OutputDirectory = Environment.GetFolderPath(Environment.SpecialFolder.MyVideos) +
                                         "\\MP4 Judges Folder\\";
            parameters.FileNameAppendix = string.Empty;
            parameters.AudioVolumeBoost = 0;
            parameters.SkipThisEncode = false;
            parameters.SkipVideoTranscode = false;

            parameters.ConfigurationName = JudgesParamsName;

            for (int i = 0; i < 6; i++)
            {
                for (int j = 0; j < 6; j++)
                {
                    if (i == 2 && j == 2)
                    {
                        parameters.AudioChannelsMap[2, 2] = 1;
                    }
                    else if (i == 3 && j == 3)
                    {
                        parameters.AudioChannelsMap[3, 3] = 1;
                    }
                    else if (i == 4 && j == 4)
                    {
                        parameters.AudioChannelsMap[4, 4] = 1;
                    }
                    else if (i == 5 && j == 5)
                    {
                        parameters.AudioChannelsMap[5, 5] = 1;
                    }
                    else
                    {
                        parameters.AudioChannelsMap[i, j] = 0;
                    }
                }
            }

            return parameters;
        }
        #endregion
    }
    #endregion

    public static class ArryExtensionMethods
    {
        public static object[][] switchArray(this object[,] inputArray)
        {
            object[][] outputArray = new object[inputArray.GetLength(0)][];
            for (int i = 0; i < inputArray.GetLength(0); i++)
            {
                outputArray[i] = new object[inputArray.GetLength(1)];
                for (int j = 0; j < inputArray.GetLength(1); j++)
                {
                    outputArray[i][j] = inputArray[i, j];
                }
            }

            return outputArray;
        }
    }
}
