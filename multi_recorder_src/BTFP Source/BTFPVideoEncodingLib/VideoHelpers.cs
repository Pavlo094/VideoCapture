using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using DirectShowLib;

namespace BTFPVideoEncodingLib
{
    /// <summary>
    /// This class contains different methods helpers for DirectShow and simillar.
    /// </summary>
    public class VideoHelpers
    {
        /// <summary>
        /// Returns a list of names of installed decklink input video devices on the system.
        /// </summary>
        /// <returns></returns>
        public static List<string> GetDecklinkVideoInputDevices()
        {
            var devices = DsDevice.GetDevicesOfCat(FilterCategory.VideoInputDevice);
            return (from d in devices where d.Name.Contains("Decklink") select d.Name).ToList();
        }
        /// <summary>
        /// Returns a list of names of installed decklink input audio devices on the system.
        /// </summary>
        /// <returns></returns>
        public static List<string> GetDecklinkAudioInputDevices()
        {
            var devices = DsDevice.GetDevicesOfCat(FilterCategory.AudioInputDevice);
            return (from d in devices where d.Name.Contains("Decklink") select d.Name).ToList();
        }

        public static List<string> GetDecklinkWdmDevices()
        {
            var devices = DsDevice.GetDevicesOfCat(FilterCategory.AMKSCapture);
            var lst = new List<string>();
            foreach (var d in devices)
            {
                if (string.IsNullOrEmpty(d.Name)) continue;
                if (d.Name.Contains("Blackmagic"))
                {
                    lst.Add(d.Name);
                }
            }
            return lst;
        }
        /// <summary>
        /// Loads registered or not-registered filter directly from library.
        /// </summary>
        /// <param name="libFileName">Full path to the library where filter is defined.</param>
        /// <param name="clsid">CLSID of the filter.</param>
        /// <returns></returns>
        public static IBaseFilter LoadFilter(string libFileName, Guid clsid)
        {
            // Load HINSTANCE from the dll
            IntPtr hDllPtr = NativeHelpers.CoLoadLibrary(libFileName, true);
            // Check if it is loaded
            if (hDllPtr == IntPtr.Zero)
            {
                throw new Exception(string.Format("Failed to load: \"{0}\". HRESULT: {1}", libFileName, Marshal.GetHRForLastWin32Error()));
            }

            // Get the address of the DllGetClassObject function
            IntPtr fPtr = NativeHelpers.GetProcAddress(hDllPtr, "DllGetClassObject");
            // Check if we have found the funcion pointer
            if (fPtr == IntPtr.Zero)
            {
                throw new Exception(string.Format("Failed to get proc address of \"DllGetClassObject\" function. HRESULT: {0}",
                    Marshal.GetHRForLastWin32Error()));
            }

            // Get the function pointer (delegate)
            DllGetClassObjectDelegate dgcs =
                (DllGetClassObjectDelegate)
                    Marshal.GetDelegateForFunctionPointer(fPtr, typeof(DllGetClassObjectDelegate));

            // Now get the class factory from the DllGetClassObject function.
            Guid factoryIid = typeof(IClassFactory).GUID;
            object factoryPtr;
            int hr = (int)dgcs(clsid, factoryIid, out factoryPtr);
            DsError.ThrowExceptionForHR(hr);

            // Convert from IUnknown to IClassFactory
            IClassFactory factory = (IClassFactory)factoryPtr;//Marshal.GetObjectForIUnknown(factoryPtr);

            // Finally, query for the IBaseFilter interface
            IntPtr bfPtr;
            Guid bfIid = typeof(IBaseFilter).GUID;
            factory.CreateInstance(null, bfIid, out bfPtr);

            return (IBaseFilter)Marshal.GetObjectForIUnknown(bfPtr);
        }
        /// <summary>
        /// Loads registered or not-registered filter directly from library.
        /// </summary>
        /// <param name="libFileName">Full path to the library where filter is defined.</param>
        /// <param name="clsid">CLSID of the filter.</param>
        /// <returns></returns>
        public static IBaseFilter LoadFilter(string libFileName, Guid clsid, ref IntPtr hDllPtr)
        {
            if (hDllPtr == IntPtr.Zero)
            {
                // Load HINSTANCE from the dll
                hDllPtr = NativeHelpers.CoLoadLibrary(libFileName, true);
                // Check if it is loaded
                if (hDllPtr == IntPtr.Zero)
                {
                    throw new Exception(string.Format("Failed to load: \"{0}\". HRESULT: {1}", libFileName,
                        Marshal.GetHRForLastWin32Error()));
                }
            }

            // Get the address of the DllGetClassObject function
            IntPtr fPtr = NativeHelpers.GetProcAddress(hDllPtr, "DllGetClassObject");
            // Check if we have found the funcion pointer
            if (fPtr == IntPtr.Zero)
            {
                throw new Exception(string.Format("Failed to get proc address of \"DllGetClassObject\" function. HRESULT: {0}",
                    Marshal.GetHRForLastWin32Error()));
            }

            // Get the function pointer (delegate)
            DllGetClassObjectDelegate dgcs =
                (DllGetClassObjectDelegate)
                    Marshal.GetDelegateForFunctionPointer(fPtr, typeof(DllGetClassObjectDelegate));

            // Now get the class factory from the DllGetClassObject function.
            Guid factoryIid = typeof(IClassFactory).GUID;
            object factoryPtr;
            int hr = (int)dgcs(clsid, factoryIid, out factoryPtr);
            DsError.ThrowExceptionForHR(hr);

            // Convert from IUnknown to IClassFactory
            IClassFactory factory = (IClassFactory)factoryPtr;//Marshal.GetObjectForIUnknown(factoryPtr);

            // Finally, query for the IBaseFilter interface
            IntPtr bfPtr;
            Guid bfIid = typeof(IBaseFilter).GUID;
            factory.CreateInstance(null, bfIid, out bfPtr);

            Marshal.ReleaseComObject(factory);

            return (IBaseFilter)Marshal.GetObjectForIUnknown(bfPtr);
        }

        /// <summary>
        /// This is the delegate describing the DllGetClassObject function which retrieves the class object from a DLL object handler or object application.
        /// </summary>
        /// <param name="clsid">The CLSID that will associate the correct data and code.</param>
        /// <param name="iid">A reference to the identifier of the interface that the caller is to use to communicate with the class object.</param>
        /// <param name="ppv">The address of a pointer variable that receives the interface pointer requested in riid.</param>
        /// <returns></returns>
        //delegate int DllGetClassObjectDelegate(ref Guid clsid, ref Guid iid, out IntPtr ppv);
        [UnmanagedFunctionPointer(CallingConvention.StdCall)]
        public delegate uint DllGetClassObjectDelegate(
            [MarshalAs(UnmanagedType.LPStruct)] Guid rclsid, [MarshalAs(UnmanagedType.LPStruct)] Guid riid,
            [MarshalAs(UnmanagedType.IUnknown, IidParameterIndex = 1)] out object ppv);

        public static string FiltersDirectory
        {
            get { return AppDomain.CurrentDomain.BaseDirectory + "\\Filters\\"; }
        }

        public static List<string> GetAudioRendereres()
        {
            var devices = DsDevice.GetDevicesOfCat(FilterCategory.AudioRendererCategory);
            return devices.Select(d => d.Name).ToList();
        }
        public static UInt32 MakeFourCC(char ch0, char ch1, char ch2, char ch3)
        {
            UInt32 result = ((UInt32)(byte)(ch0) | ((UInt32)(byte)(ch1) << 8) |
                    ((UInt32)(byte)(ch2) << 16) | ((UInt32)(byte)(ch3) << 24));

            return result;
        }
    }
}
