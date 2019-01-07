// StereoDownmixer.h: interface for the CStereoDownmixer class.
// 
//////////////////////////////////////////////////////////////////////
//  CStereoDownmixer: (A DirectShow (DirectX 8.0) filter)
//
//  Purpose: This filter is a basic Transform Filter to process video
//        
//  Usage: 
//  1.  Register the filter. 
//      regsvr32 StereoDownmixer.ax
//
//  2.  Insert the filter into the graph to process video data (live 
//      capture or video files)
//      a.  Use the GraphEdt.exe (in DirectX SDK) to build a graph 
//      b.  To use this filter in App, include iStereoDownmixer.h for
//          definition of interfaces and build the graph in the App.
///////////////////////////////////////////////////////////////////////
#pragma once

#define CURRENT_VERSION_NUMBER 20000

/*
Versions: 2.00 Beta = 20000
*/

class CStereoDownmixer
    : public CTransformFilter,     // Derive from Transform Filter
      public IStereoDownmixer,       // Interface of CStereoDownmixer
      public ISpecifyPropertyPages,// Having property page
      public CPersistStream        // Persist the object
{
public:

    DECLARE_IUNKNOWN;

// Constructor && Deconstructor
    static CUnknown *WINAPI CreateInstance(LPUNKNOWN punk, HRESULT *phr);
    CStereoDownmixer(TCHAR *tszName, LPUNKNOWN punk, HRESULT *phr);
    ~CStereoDownmixer();

// Overrriden from CTransformFilter base class
    // Check if the transform type can be accomplished
    HRESULT CheckInputType(const CMediaType* mtIn);
    HRESULT CheckTransform(const CMediaType *mtIn, const CMediaType *mtOut);
	HRESULT CompleteConnect(PIN_DIRECTION direction,IPin *pReceivePin);
	HRESULT BreakConnect(PIN_DIRECTION dir);
    HRESULT DecideBufferSize(IMemAllocator *pAlloc,
                 ALLOCATOR_PROPERTIES *pProperties);
    HRESULT GetMediaType(int iPosition, CMediaType *pMediaType);
    // This function is called to process each new frame in the video.
    HRESULT Transform(IMediaSample *pIn, IMediaSample *pOut);
	CMediaType MakeOutputCMediaTypeFromParams(const StereoDownmixerParams &params, const CMediaType *InputMediaType=NULL);
	STDMETHODIMP_(BOOL) IsThisInputMediaTypeAcceptableAndMatchesParams(const AM_MEDIA_TYPE &media_type, const StereoDownmixerParams &params);
	BOOL LoadCustomMixMatrix(StereoDownmixerParams::Page3Params p3params);
    
	//For handling Single Instance mode
	STDMETHODIMP JoinFilterGraph(IFilterGraph *pGraph,LPCWSTR pName);

    // Reveals the interface the filter support
    STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, void ** ppv);
    // ISpecifyPropertyPages interface
    STDMETHODIMP GetPages(CAUUID *pPages);

// IStereoDownmixer interface to custom the filter
    // Get the current parameters of the filter
    STDMETHODIMP GetParams(StereoDownmixerParams *irp, StereoDownmixerParams::SectionChoose WhichParams);
    // Set the parameters of the filter
    STDMETHODIMP SetParams(StereoDownmixerParams *irp, StereoDownmixerParams::SectionChoose WhichParams);
	// Tell the property page if the pins are connected
	STDMETHODIMP IsInputPinConnected();
	STDMETHODIMP IsOutputPinConnected();
	// See if upstream filter can provide this type. If not connected returns VFW_E_NOT_CONNECTED
	STDMETHODIMP CanThisFormatBeProvidedFromUpstream(StereoDownmixerParams *irp);
	// Ask the downstream filter if this output format is acceptable
	STDMETHODIMP IsThisFormatAcceptedDownstream(StereoDownmixerParams *irp);
	// Fetch current formats in use (these methods return VFW_E_NOT_CONNECTED if the pins are not connected)
	STDMETHODIMP GetCurrentInputMediaType(CMediaType &mt);
	STDMETHODIMP GetCurrentOutputMediaType(CMediaType &mt);

// CPersistStream stuff -- Save the config persistently
// These functions are used to serialize object, (like save or load
// graph file (*.grf) in GraphEdt.exe. It will be automaticly called 
// when you save filter to file.
    HRESULT WriteToStream(IStream *pStream);
    HRESULT ReadFromStream(IStream *pStream);
    int     SizeMax() { return 13 * STR_MAX_LENGTH; }
    // This is the only method of IPersist
    STDMETHODIMP GetClassID(CLSID *pClsid);

protected:
  
    // Save the configuration into registry
    void    saveConfig();
    // Read Config from registry
    void    readConfig(); 

	//Obsolete, not used anymore:
	//CMediaType m_OutputType;

    // Parameters controling the filter behavior
    StereoDownmixerParams m_StereoDownmixerParams;

	//Custom mix-matrix stuff
	double m_CustMixMatrix[18][18];
	//Mix-matrix helping stuff
	double m_tempInputs[18];
	double m_tempOutputs[18];

	//For handling Single Instance mode
	static int sm_FilterInstancesCounter;
};
