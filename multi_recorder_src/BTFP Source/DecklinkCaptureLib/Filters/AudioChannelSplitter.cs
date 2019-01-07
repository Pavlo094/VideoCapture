using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using DirectShowLib;

namespace DecklinkCaptureLib.Filters
{
    [ComImport, Guid("227FFEC6-068F-4817-BDB2-51D27F7A555F")]
    class AudioChannelSplitter
    {
    }

    class AudioChannelSplitterLoader
    {
        private const string FilterName = "AudioChannelSplitter.ax";

        public static IBaseFilter LoadFilter()
        {
            return VideoHelpers.LoadFilter(VideoHelpers.FiltersDirectory + FilterName, typeof(AudioChannelSplitter).GUID);
        }
    }
}
