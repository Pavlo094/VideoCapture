// warning disable
#pragma warning(disable: 4097 4511 4512 4514 4705)

#include <InitGuid.h>
#include "WavMeter_i.h"

// Define 6 peaks, i.e. channels
#define PEAKS 6
// Define -96dB default peak value
#define PEAK_VALUE -96

// CWavMeter class, which is DirectShow IBaseFilter. This class calculates audio peaks
// and has the ability to change audio volume per channel. It supports maximum of 6 channels.
// Audio must be 2, 4 or 6 channels, 16 bit and 48000Hz.
class CWavMeterFilter : public CTransInPlaceFilter, public IWavMeter
{
public:
	// Query interface
	STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, void ** ppv);
	// Declare IUnknown interface
	DECLARE_IUNKNOWN;
	// Constructor
	CWavMeterFilter(LPUNKNOWN pUnk, HRESULT *phr);
	// Destructor
	~CWavMeterFilter();
	// Create instance
	static CUnknown * WINAPI CreateInstance(LPUNKNOWN pUnk, HRESULT *phr);

public:
	// This method is called on each transform with samples.
    HRESULT Transform(IMediaSample *pIn);
	// Checks if we support input type
    HRESULT CheckInputType(const CMediaType* pMediaType) ;
	// Checks if we support transformation between input and output type.
	HRESULT CheckTransform(const CMediaType *mtIn, const CMediaType *mtOut);
	// Get media type
    HRESULT GetMediaType(int iPosition, CMediaType *pMediaType);

public:
	// Interface members
	STDMETHODIMP GetPeaks(double Peaks[]);
	STDMETHODIMP SetVolume(double Volumes[]);

private:
	// Get VU peaks
	HRESULT CalculatePeaks(PBYTE pb, long cb);

private:
	// The peaks
	double m_Peaks[PEAKS];
	// Volumes
	double m_Volumes[PEAKS];
	// Channels in play
	short m_channels;
	// Critical section
	CCritSec m_CritSec;
	CCritSec m_VolumeCritSec;

};