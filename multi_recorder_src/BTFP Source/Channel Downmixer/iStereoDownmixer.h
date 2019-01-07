// iStereoDownmixer.h: interface for the CStereoDownmixer class
//
//////////////////////////////////////////////////////////////////////
// Classes defined:
//
//   GUIDs of Filter CStereoDownmixer, Interface IStereoDownmixer and 
//       its property page CStereoDownmixerProp
//   StereoDownmixerParams:
//       The parameters that control the CStereoDownmixer
//   IStereoDownmixer:
//       The interface that controls the CStereoDownmixer.
//////////////////////////////////////////////////////////////////////
#pragma once

// The parameter control the CStereoDownmixer.
struct StereoDownmixerParams {

	enum SectionChoose
	{
		general,
		gains,
		mixmatrix
	};

	struct Page1Params	//General
	{
		//Output format
		BOOL bIsMultichannel;	//To distinguish between plain old stereo and WAVE_FORMAT_EXTENSIBLE 2 channels. 
								//(It's probably the same thing to most destination devices... but just for completeness)

		DWORD wFormatTag;		//WAVE_FORMAT_PCM or WAVE_FORMAT_EXTENSIBLE
		WORD nChannels;			//1 or 2 if WAVE_FORMAT_PCM, 2 or more if WAVE_FORMAT_EXTENSIBLE
		DWORD dwChannelMask;	//Meaningful only for WAVE_FORMAT_EXTENSIBLE
		WORD wBitsPerSample;
		

		//Input requirements
		DWORD RequiredInputBitDepth;

		BOOL bRequireInputNumberOfChannels;
		DWORD RequiredInputNumberOfChannels;

		BOOL bRequireInputSamplerate;
		DWORD RequiredInputSamplerate;

		//Filter priority and single instance
		BOOL bRaiseMeritAndSingleInstance;
	} p1;

	struct Page2Params	//Gains
	{
		DWORD InputEnableBitmask;
		DWORD OutputEnableBitmask;
		BOOL bEnableInputGains;
		BOOL bEnableOutputGains;
		BOOL bEnableMasterVolume;
		DWORD MasterVolumeGain;

		struct InputGains
		{
			DWORD FL ;
			DWORD FR ;
			DWORD FC ;
			DWORD LF ;
			DWORD BL ;
			DWORD BR ;
			DWORD FLC;
			DWORD FRC;
			DWORD BC ;
			DWORD SL ;
			DWORD SR ;
			DWORD TC ;
			DWORD TFL;
			DWORD TFC;
			DWORD TFR;
			DWORD TBL;
			DWORD TBC;
			DWORD TBR;
			BOOL bJoinFLFR;
			BOOL bJoinBLBR;
			BOOL bJoinFLCFRC;
			BOOL bJoinSLSR;
			BOOL bJoinTFLTFR;
			BOOL bJoinTBLTBR;
		} I;
		struct OutputGains
		{
			DWORD FL;
			DWORD FR;
			DWORD FC;
			DWORD LF;
			DWORD BL;
			DWORD BR;
			DWORD FLC;
			DWORD FRC;
			DWORD BC;
			DWORD SL;
			DWORD SR;
			DWORD TC;
			DWORD TFL;
			DWORD TFC;
			DWORD TFR;
			DWORD TBL;
			DWORD TBC;
			DWORD TBR;
			BOOL bJoinFLFR;
			BOOL bJoinBLBR;
			BOOL bJoinFLCFRC;
			BOOL bJoinSLSR;
			BOOL bJoinTFLTFR;
			BOOL bJoinTBLTBR;
		} O;
	} p2;

	struct Page3Params	//Mix matrix
	{
		BOOL bCustomMixMatrix; //Has user selected a custom matrix?
		char CustomMixMatrixFilename[MAX_PATH]; //Selected matrix from the file (in order of appearance within the file)
	} p3;
};

// {5f534f6b-82f0-465b-9124-15913f3ac306}
DEFINE_GUID(CLSID_StereoDownmixer, 
0x5f534f6b, 0x82f0, 0x465b, 0x91, 0x24, 0x15, 0x91, 0x3f, 0x3a, 0xc3, 0x6);

// {45d8b751-bc68-4f20-afb9-dd309ed0f009}
DEFINE_GUID(CLSID_StereoDownmixerPropP1, 
0x45d8b751, 0xbc68, 0x4f20, 0xaf, 0xb9, 0xdd, 0x30, 0x9e, 0xd0, 0xf0, 0x9);

// {45d8b751-bc68-4f20-afb9-dd309ed0f00a}
DEFINE_GUID(CLSID_StereoDownmixerPropP2, 
0x45d8b751, 0xbc68, 0x4f20, 0xaf, 0xb9, 0xdd, 0x30, 0x9e, 0xd0, 0xf0, 0xa);

// {45d8b751-bc68-4f20-afb9-dd309ed0f00b}
DEFINE_GUID(CLSID_StereoDownmixerPropP3, 
0x45d8b751, 0xbc68, 0x4f20, 0xaf, 0xb9, 0xdd, 0x30, 0x9e, 0xd0, 0xf0, 0xb);

// {c0bee2f1-a674-4b63-84d2-cf6737eed2ee}
DEFINE_GUID(IID_IStereoDownmixer, 
0xc0bee2f1, 0xa674, 0x4b63, 0x84, 0xd2, 0xcf, 0x67, 0x37, 0xee, 0xd2, 0xee);

#ifdef __cplusplus
extern "C" {
#endif

DECLARE_INTERFACE_(IStereoDownmixer, IUnknown)
{
    // Get the parameters of the filter
    STDMETHOD(GetParams) (THIS_
		StereoDownmixerParams *irp,
		StereoDownmixerParams::SectionChoose WhichParams
		) PURE;

    // Set the parameters of the filter
    STDMETHOD(SetParams) (THIS_
		StereoDownmixerParams *irp,
		StereoDownmixerParams::SectionChoose WhichParams
		) PURE;

	STDMETHOD(IsInputPinConnected) (THIS_) PURE;

	STDMETHOD(IsOutputPinConnected) (THIS_) PURE;

	STDMETHOD(GetCurrentInputMediaType) (THIS_
		CMediaType &mt
		);

	STDMETHOD(GetCurrentOutputMediaType) (THIS_
		CMediaType &mt
		);
	
	STDMETHOD(CanThisFormatBeProvidedFromUpstream) (THIS_
		StereoDownmixerParams *irp
		) PURE;

	STDMETHOD(IsThisFormatAcceptedDownstream) (THIS_
		StereoDownmixerParams *irp
		) PURE;

	STDMETHOD_(BOOL,IsThisInputMediaTypeAcceptableAndMatchesParams) (THIS_
		const AM_MEDIA_TYPE &media_type, 
		const StereoDownmixerParams &params
		) PURE;
};

#ifdef __cplusplus
}
#endif

