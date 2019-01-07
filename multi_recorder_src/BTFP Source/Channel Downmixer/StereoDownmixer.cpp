// StereoDownmixer.cpp: implementation of CStereoDownmixer class.
// 
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"

#include <streams.h>     // DirectShow
#include <initguid.h>    // DEFINE_GUID to declare an EXTERN_C const.
#include <stdio.h>

#include "iStereoDownmixer.h"
#include "StereoDownmixer.h"
#include "StereoDownmixerPropP1.h"
#include "StereoDownmixerPropP2.h"
#include "StereoDownmixerPropP3.h"

#include <ks.h>
#include <ksmedia.h>

#include <limits.h>

#define TRANSFORM_NAME L"Channel Downmixer by Trombettworks"

//Counting instances running
int CStereoDownmixer::sm_FilterInstancesCounter = 0;

// setup data - allows the self-registration to work.


//See DirectX Docs page "Audio Subtypes" to understand this. 
//I wanted to put it as input pin mediasubtype but it doesn't work.
//So I used the KSDATAFORMAT_SUBTYPE_PCM instead.
//
//GUID MEDIASUBTYPE_WAVEFORMATEXTENSIBLE = {
//	WAVE_FORMAT_EXTENSIBLE,
//	0,
//	0x0010,
//	{0x80, 0x00, 0x00, 0xAA, 0x00, 0x38, 0x9B, 0x71}
//};

const AMOVIESETUP_MEDIATYPE sudInputPinType =
{
	&MEDIATYPE_Audio,
	&KSDATAFORMAT_SUBTYPE_PCM
};

const AMOVIESETUP_MEDIATYPE sudOutputPinType =
{
	&MEDIATYPE_Audio,			//Major type
	&KSDATAFORMAT_SUBTYPE_PCM	//Minor type = PCM => Format tag can be only FORMAT_WaveFormatEx (that is, 1 or 2 channels)
};

const AMOVIESETUP_PIN psudPins[] =
{ { L"Input"            // strName
  , FALSE               // bRendered
  , FALSE               // bOutput
  , FALSE               // bZero
  , FALSE               // bMany
  , &CLSID_NULL         // clsConnectsToFilter
  , L""                 // strConnectsToPin
  , 1                   // nTypes
  , &sudInputPinType    // lpTypes
  }
, { L"Output"           // strName
  , FALSE               // bRendered
  , TRUE                // bOutput
  , FALSE               // bZero
  , FALSE               // bMany
  , &CLSID_NULL         // clsConnectsToFilter
  , L""                 // strConnectsToPin
  , 1                   // nTypes
  , &sudOutputPinType   // lpTypes
  }
};

//Anybody has better ideas for the dwMerit? I think the following is fair:

AMOVIESETUP_FILTER sudStereoDownmixer_NormalPriority =
{ &CLSID_StereoDownmixer              // clsID
, TRANSFORM_NAME                    // strName
, MERIT_UNLIKELY+100                // dwMerit
, 2                                 // nPins
, psudPins };                       // lpPin


AMOVIESETUP_FILTER sudStereoDownmixer_HighPriority =
{ &CLSID_StereoDownmixer
, TRANSFORM_NAME
, MERIT_PREFERRED+10
, 2
, psudPins };


// Needed for the CreateInstance mechanism
CFactoryTemplate g_Templates[]=
    {   { TRANSFORM_NAME
        , &CLSID_StereoDownmixer
        , CStereoDownmixer::CreateInstance
        , NULL
        , &sudStereoDownmixer_HighPriority },
        
		{ TRANSFORM_NAME L" Properties1"
        , &CLSID_StereoDownmixerPropP1
        , CStereoDownmixerPropP1::CreateInstance },
		{ TRANSFORM_NAME L" Properties2"
        , &CLSID_StereoDownmixerPropP2
        , CStereoDownmixerPropP2::CreateInstance },
		{ TRANSFORM_NAME L" Properties3"
        , &CLSID_StereoDownmixerPropP3
        , CStereoDownmixerPropP3::CreateInstance }
    };
int g_cTemplates = sizeof(g_Templates)/sizeof(g_Templates[0]);

///////////////////////////////////////////////////////////////////////
// CreateInstance: for COM to create a CStereoDownmixer object
///////////////////////////////////////////////////////////////////////
CUnknown * WINAPI CStereoDownmixer::CreateInstance(LPUNKNOWN punk, HRESULT *phr) {

    CStereoDownmixer *pNewObject = new CStereoDownmixer(NAME("StereoDownmixer"), punk, phr );
    if (pNewObject == NULL) {
        *phr = E_OUTOFMEMORY;
    }

    return pNewObject;
}

///////////////////////////////////////////////////////////////////////
// CStereoDownmixer: Constructor
///////////////////////////////////////////////////////////////////////
CStereoDownmixer::CStereoDownmixer(TCHAR *tszName, LPUNKNOWN punk, HRESULT *phr)
    : CTransformFilter (tszName, punk, CLSID_StereoDownmixer),
      CPersistStream(punk, phr)
{
	//Zeroing the custom mixmatrix before doing anything
	int i,j;
	for (i=0;i<18;i++)
	{
		for (j=0;j<18;j++) m_CustMixMatrix[i][j]=0;
	}


    readConfig();							// read previous use value from registry

	// ????? 
    //SetParams(&m_StereoDownmixerParams);	// Set Filter parameters


	sm_FilterInstancesCounter++;
}

///////////////////////////////////////////////////////////////////////
// ~CStereoDownmixer: Destructor
///////////////////////////////////////////////////////////////////////
CStereoDownmixer::~CStereoDownmixer()
{
	sm_FilterInstancesCounter--;
}

///////////////////////////////////////////////////////////////////////
// saveConfig: write the parameters into the registry
///////////////////////////////////////////////////////////////////////
void CStereoDownmixer::saveConfig()
{
    char sz[STR_MAX_LENGTH];

#define WRITEPROFINT(var)	sprintf(sz, "%d", m_StereoDownmixerParams.var);	\
							WriteProfileStringA("ChannelDownmixer", #var, sz);
    
	//Bookmark: optionscodehere write	//To quickly find the point with Visual Studio

	//Write parameters into registry (or win.ini file)

	//--- General

	//Output format
	WRITEPROFINT(p1.bIsMultichannel);
	WRITEPROFINT(p1.wFormatTag);
	WRITEPROFINT(p1.nChannels);
	WRITEPROFINT(p1.dwChannelMask);
	WRITEPROFINT(p1.wBitsPerSample);


	//Input requirements
	WRITEPROFINT(p1.RequiredInputBitDepth);

	WRITEPROFINT(p1.bRequireInputNumberOfChannels);
	WRITEPROFINT(p1.RequiredInputNumberOfChannels);

	WRITEPROFINT(p1.bRequireInputSamplerate);
	WRITEPROFINT(p1.RequiredInputSamplerate);

	//Raise merit and single instance
	WRITEPROFINT(p1.bRaiseMeritAndSingleInstance);


	//--- Gains:
	WRITEPROFINT(p2.InputEnableBitmask);
	WRITEPROFINT(p2.OutputEnableBitmask);

	WRITEPROFINT(p2.bEnableInputGains);
	WRITEPROFINT(p2.bEnableOutputGains);
	WRITEPROFINT(p2.bEnableMasterVolume);
	WRITEPROFINT(p2.MasterVolumeGain);

	WRITEPROFINT(p2.I.FL);
	WRITEPROFINT(p2.I.FR);
	WRITEPROFINT(p2.I.FC);
	WRITEPROFINT(p2.I.LF);
	WRITEPROFINT(p2.I.BL);
	WRITEPROFINT(p2.I.BR);
	WRITEPROFINT(p2.I.FLC);
	WRITEPROFINT(p2.I.FRC);
	WRITEPROFINT(p2.I.BC);
	WRITEPROFINT(p2.I.SL);
	WRITEPROFINT(p2.I.SR);
	WRITEPROFINT(p2.I.TC);
	WRITEPROFINT(p2.I.TFL);
	WRITEPROFINT(p2.I.TFC);
	WRITEPROFINT(p2.I.TFR);
	WRITEPROFINT(p2.I.TBL);
	WRITEPROFINT(p2.I.TBC);
	WRITEPROFINT(p2.I.TBR);

	WRITEPROFINT(p2.I.bJoinFLFR);
	WRITEPROFINT(p2.I.bJoinBLBR);
	WRITEPROFINT(p2.I.bJoinFLCFRC);
	WRITEPROFINT(p2.I.bJoinSLSR);
	WRITEPROFINT(p2.I.bJoinTFLTFR);
	WRITEPROFINT(p2.I.bJoinTBLTBR);

	WRITEPROFINT(p2.O.FL);
	WRITEPROFINT(p2.O.FR);
	WRITEPROFINT(p2.O.FC);
	WRITEPROFINT(p2.O.LF);
	WRITEPROFINT(p2.O.BL);
	WRITEPROFINT(p2.O.BR);
	WRITEPROFINT(p2.O.FLC);
	WRITEPROFINT(p2.O.FRC);
	WRITEPROFINT(p2.O.BC);
	WRITEPROFINT(p2.O.SL);
	WRITEPROFINT(p2.O.SR);
	WRITEPROFINT(p2.O.TC);
	WRITEPROFINT(p2.O.TFL);
	WRITEPROFINT(p2.O.TFC);
	WRITEPROFINT(p2.O.TFR);
	WRITEPROFINT(p2.O.TBL);
	WRITEPROFINT(p2.O.TBC);
	WRITEPROFINT(p2.O.TBR);

	WRITEPROFINT(p2.O.bJoinFLFR);
	WRITEPROFINT(p2.O.bJoinBLBR);
	WRITEPROFINT(p2.O.bJoinFLCFRC);
	WRITEPROFINT(p2.O.bJoinSLSR);
	WRITEPROFINT(p2.O.bJoinTFLTFR);
	WRITEPROFINT(p2.O.bJoinTBLTBR);

	//--- Mix matrix
	WRITEPROFINT(p3.bCustomMixMatrix);
	WriteProfileStringA("ChannelDownmixer", "CustomMixMatrixFilename", m_StereoDownmixerParams.p3.CustomMixMatrixFilename);
}

///////////////////////////////////////////////////////////////////////
// readConfig: read the parameters from the registry
///////////////////////////////////////////////////////////////////////
void CStereoDownmixer::readConfig()
{
	//Bookmark: optionscodehere read

    //Read parameters from registry

	#define READPROFINT(var,default_value) m_StereoDownmixerParams.var = GetProfileInt("ChannelDownmixer", #var, default_value);

	////Check which is the last registered version number
	//int LastRegisteredVersion=GetProfileInt("ChannelDownmixer","LastRegisteredVersion",0);
	//if (LastRegisteredVersion != CURRENT_VERSION_NUMBER)
	//{
	//	DllRegisterServer();
	//}

	//Output format
	READPROFINT(p1.bIsMultichannel,0);
	READPROFINT(p1.wFormatTag,WAVE_FORMAT_PCM);
	READPROFINT(p1.nChannels,2);
	READPROFINT(p1.dwChannelMask,KSAUDIO_SPEAKER_5POINT1);
	READPROFINT(p1.wBitsPerSample,16);


	//Input requirements
	READPROFINT(p1.RequiredInputBitDepth,16);

	READPROFINT(p1.bRequireInputNumberOfChannels,0);
	READPROFINT(p1.RequiredInputNumberOfChannels,5);

	READPROFINT(p1.bRequireInputSamplerate,0);
	READPROFINT(p1.RequiredInputSamplerate,48000);


	//Raise Merit and single instance
	READPROFINT(p1.bRaiseMeritAndSingleInstance,FALSE); //If it's not present in registry it means that the user has upgraded from an earlier version
													 //and so we must reflect the low merit with the absence of this option

	//--- Gains:
	READPROFINT(p2.InputEnableBitmask,0xffffffff);
	READPROFINT(p2.OutputEnableBitmask,0xffffffff);

	READPROFINT(p2.bEnableInputGains,FALSE);
	READPROFINT(p2.bEnableOutputGains,FALSE);
	READPROFINT(p2.bEnableMasterVolume,FALSE);
	READPROFINT(p2.MasterVolumeGain,100);

	READPROFINT(p2.I.FL,100);
	READPROFINT(p2.I.FR,100);
	READPROFINT(p2.I.FC,100);
	READPROFINT(p2.I.LF,100);
	READPROFINT(p2.I.BL,100);
	READPROFINT(p2.I.BR,100);
	READPROFINT(p2.I.FLC,100);
	READPROFINT(p2.I.FRC,100);
	READPROFINT(p2.I.BC,100);
	READPROFINT(p2.I.SL,100);
	READPROFINT(p2.I.SR,100);
	READPROFINT(p2.I.TC,100);
	READPROFINT(p2.I.TFL,100);
	READPROFINT(p2.I.TFC,100);
	READPROFINT(p2.I.TFR,100);
	READPROFINT(p2.I.TBL,100);
	READPROFINT(p2.I.TBC,100);
	READPROFINT(p2.I.TBR,100);

	READPROFINT(p2.I.bJoinFLFR,TRUE);
	READPROFINT(p2.I.bJoinBLBR,TRUE);
	READPROFINT(p2.I.bJoinFLCFRC,TRUE);
	READPROFINT(p2.I.bJoinSLSR,TRUE);
	READPROFINT(p2.I.bJoinTFLTFR,TRUE);
	READPROFINT(p2.I.bJoinTBLTBR,TRUE);

	READPROFINT(p2.O.FL,100);
	READPROFINT(p2.O.FR,100);
	READPROFINT(p2.O.FC,100);
	READPROFINT(p2.O.LF,100);
	READPROFINT(p2.O.BL,100);
	READPROFINT(p2.O.BR,100);
	READPROFINT(p2.O.FLC,100);
	READPROFINT(p2.O.FRC,100);
	READPROFINT(p2.O.BC,100);
	READPROFINT(p2.O.SL,100);
	READPROFINT(p2.O.SR,100);
	READPROFINT(p2.O.TC,100);
	READPROFINT(p2.O.TFL,100);
	READPROFINT(p2.O.TFC,100);
	READPROFINT(p2.O.TFR,100);
	READPROFINT(p2.O.TBL,100);
	READPROFINT(p2.O.TBC,100);
	READPROFINT(p2.O.TBR,100);

	READPROFINT(p2.O.bJoinFLFR,TRUE);
	READPROFINT(p2.O.bJoinBLBR,TRUE);
	READPROFINT(p2.O.bJoinFLCFRC,TRUE);
	READPROFINT(p2.O.bJoinSLSR,TRUE);
	READPROFINT(p2.O.bJoinTFLTFR,TRUE);
	READPROFINT(p2.O.bJoinTBLTBR,TRUE);

	//--- Mix matrix
	READPROFINT(p3.bCustomMixMatrix,100);
	GetProfileStringA("ChannelDownmixer", "CustomMixMatrixFilename", "", m_StereoDownmixerParams.p3.CustomMixMatrixFilename, MAX_PATH);
	LoadCustomMixMatrix(m_StereoDownmixerParams.p3);
}

///////////////////////////////////////////////////////////////////////
// CheckInputType: Check if the input type can be done
///////////////////////////////////////////////////////////////////////
HRESULT CStereoDownmixer::CheckInputType(const CMediaType *mtIn)
{
	//We accept multichannel audio.
	//Let's do 5.1 only for now

	/*
	WAVEFORMATEX found (reverse engineered from a working one) when 2 channels:

	wFormatTag=1;
	nChannels=2;
	nSamplesPerSec=48000;
	nAvgBytesPerSec=192000;
	nBlockAlign=4;
	wBitsPerSample=16;
	cbSize=0;

	Corresponding AM_MEDIA_TYPE:
	
	majortype = GUID {73647561-0000-0010-8000-00aa00389b71};
	minortype = GUID {00000001-0000-0010-8000-00aa00389b71};
	bFixedSizeSamples = 1;
	bTemporalCompression = 0;
	lSampleSize = 0x9000; // = 36864
	formattype = GUID {05589f81-c356-11ce-bf01-00aa0055595a};
	pUnk = 0;
	cbFormat = 0x12;
	pbFormat = .....;


	*/

	//Debug:
	/*
	if (mtIn->subtype==MEDIASUBTYPE_DOLBY_AC3_SPDIF)
	{
		MessageBox(0,"DETECTED ATTEMPT TO INPUT AC3 OVER SPDIF!","DETECTED ATTEMPT TO INPUT AC3 OVER SPDIF!",MB_ICONEXCLAMATION);

		char str[525];
		sprintf(str,"Detected AC3 over SPDIF incoming connection which will be accepted:\n\
			Majortype\t= {%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X}\n\
			Minortype\t= {%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X}\n\
			FormatType\t= {%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X}",

			mtIn->majortype.Data1,
			mtIn->majortype.Data2,
			mtIn->majortype.Data3,
			mtIn->majortype.Data4[0],
			mtIn->majortype.Data4[1],
			mtIn->majortype.Data4[2],
			mtIn->majortype.Data4[3],
			mtIn->majortype.Data4[4],
			mtIn->majortype.Data4[5],
			mtIn->majortype.Data4[6],
			mtIn->majortype.Data4[7],

			mtIn->subtype.Data1,
			mtIn->subtype.Data2,
			mtIn->subtype.Data3,
			mtIn->subtype.Data4[0],
			mtIn->subtype.Data4[1],
			mtIn->subtype.Data4[2],
			mtIn->subtype.Data4[3],
			mtIn->subtype.Data4[4],
			mtIn->subtype.Data4[5],
			mtIn->subtype.Data4[6],
			mtIn->subtype.Data4[7],

			mtIn->formattype.Data1,
			mtIn->formattype.Data2,
			mtIn->formattype.Data3,
			mtIn->formattype.Data4[0],
			mtIn->formattype.Data4[1],
			mtIn->formattype.Data4[2],
			mtIn->formattype.Data4[3],
			mtIn->formattype.Data4[4],
			mtIn->formattype.Data4[5],
			mtIn->formattype.Data4[6],
			mtIn->formattype.Data4[7]
			);

		MessageBox(NULL,str,"Incoming attempt",0);

		return S_OK;
		//End debug
	}
	*/

	if (IsThisInputMediaTypeAcceptableAndMatchesParams(*mtIn,m_StereoDownmixerParams)) 
	{

		//Debug:
		/*
		char str[525];
		sprintf(str,"Detected acceptable incoming connection:\n\
			Majortype\t= {%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X}\n\
			Minortype\t= {%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X}\n\
			FormatType\t= {%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X}",

			mtIn->majortype.Data1,
			mtIn->majortype.Data2,
			mtIn->majortype.Data3,
			mtIn->majortype.Data4[0],
			mtIn->majortype.Data4[1],
			mtIn->majortype.Data4[2],
			mtIn->majortype.Data4[3],
			mtIn->majortype.Data4[4],
			mtIn->majortype.Data4[5],
			mtIn->majortype.Data4[6],
			mtIn->majortype.Data4[7],

			mtIn->subtype.Data1,
			mtIn->subtype.Data2,
			mtIn->subtype.Data3,
			mtIn->subtype.Data4[0],
			mtIn->subtype.Data4[1],
			mtIn->subtype.Data4[2],
			mtIn->subtype.Data4[3],
			mtIn->subtype.Data4[4],
			mtIn->subtype.Data4[5],
			mtIn->subtype.Data4[6],
			mtIn->subtype.Data4[7],

			mtIn->formattype.Data1,
			mtIn->formattype.Data2,
			mtIn->formattype.Data3,
			mtIn->formattype.Data4[0],
			mtIn->formattype.Data4[1],
			mtIn->formattype.Data4[2],
			mtIn->formattype.Data4[3],
			mtIn->formattype.Data4[4],
			mtIn->formattype.Data4[5],
			mtIn->formattype.Data4[6],
			mtIn->formattype.Data4[7]
			);

		//MessageBox(NULL,str,"Incoming attempt",0);

		WAVEFORMATEXTENSIBLE wfex = *(WAVEFORMATEXTENSIBLE*) (mtIn->pbFormat);
		//End debug
		*/

		return S_OK;
	}
	else return VFW_E_TYPE_NOT_ACCEPTED;
}

///////////////////////////////////////////////////////////////////////
// Checktransform: Check a transform can be done between these formats
///////////////////////////////////////////////////////////////////////
HRESULT CStereoDownmixer::CheckTransform(const CMediaType *mtIn, const CMediaType *mtOut)
{
	//The output type must come out from MakeOutputCMediaTypeFromParams for our params
	//I can transform only to THAT sample type
	CMediaType myoutput=MakeOutputCMediaTypeFromParams(m_StereoDownmixerParams,mtIn);
	
	if (*mtOut==myoutput) return S_OK;
	else return VFW_E_TYPE_NOT_ACCEPTED;
}

HRESULT CStereoDownmixer::CompleteConnect(PIN_DIRECTION direction,IPin *pReceivePin)
{
	
	return S_OK;
}

HRESULT CStereoDownmixer::BreakConnect(PIN_DIRECTION dir)
{
	return S_OK;
}

// DecideBufferSize: Tell the output pin's allocator what size buffers 
// we require. Let's ask a buffer as big as the input one, to make things simpler.
// (A smaller one could be OK too)
// So we can only do this when the input is connected
HRESULT CStereoDownmixer::DecideBufferSize(IMemAllocator *pAlloc,
                             ALLOCATOR_PROPERTIES *pProperties)
{
    // Is the input pin connected
    if (m_pInput->IsConnected() == FALSE) {
        return E_UNEXPECTED;
    }

    ASSERT(pAlloc);
    ASSERT(pProperties);
    HRESULT hr = NOERROR;

	//Let' say we want some buffers... just to make sure the downstream filters 
	//don't starve and can cache data

	//But beware: a too high number of buffers will cause a noticeable delay when adjusting volume

#define NUMBER_OF_BUFFERS 10

    pProperties->cBuffers = NUMBER_OF_BUFFERS;

	//But we need input pin's allocator buffer size and the input type 
	//to determine the buffer size required for us

	CMediaType input_mt;
	hr = m_pInput->ConnectionMediaType(&input_mt); if (FAILED(hr)) return hr;
	
	DWORD DestByteSizeOfSamples = m_StereoDownmixerParams.p1.wBitsPerSample / 8;
	DWORD DestNumberOfChannels = m_StereoDownmixerParams.p1.nChannels;
	DWORD SourceByteSizeOfSamples = ((WAVEFORMATEX*)input_mt.Format())->wBitsPerSample / 8;
	DWORD SourceNumberOfChannels = ((WAVEFORMATEX*)input_mt.Format())->nChannels;

    ALLOCATOR_PROPERTIES InProps;
    IMemAllocator * pInAlloc = NULL;
    hr = m_pInput->GetAllocator(&pInAlloc);
    if (SUCCEEDED (hr))
    {
        hr = pInAlloc->GetProperties (&InProps);
        if (SUCCEEDED (hr))
        {
            pProperties->cbBuffer = ((InProps.cbBuffer * DestByteSizeOfSamples * DestNumberOfChannels) / SourceByteSizeOfSamples) / SourceNumberOfChannels + 200 ; //+200 for safety
        }
        pInAlloc->Release();
    }

    if (FAILED(hr)) return hr;

    ASSERT(pProperties->cbBuffer);

	//As far as the alignament is concerned, we must align to nBlockAlign 
	//(I think... somebody please check this)
	
	//Since at this time the output is still not officially connected, I will
	//create the output media type again in order to retrieve the nBlockAlign.
	pProperties->cbAlign=((WAVEFORMATEX*)MakeOutputCMediaTypeFromParams(m_StereoDownmixerParams).Format())->nBlockAlign;
	
	//Old:
	//pProperties->cbAlign=((WAVEFORMATEX*)m_OutputType.Format())->nBlockAlign;


    // Ask the allocator to reserve us some sample memory, NOTE the function
    // can succeed (that is return NOERROR) but still not have allocated the
    // memory that we requested, so we must check we got whatever we wanted

    ALLOCATOR_PROPERTIES Actual;
    hr = pAlloc->SetProperties(pProperties,&Actual);
    if (FAILED(hr)) {
        return hr;
    }

    ASSERT( Actual.cBuffers == NUMBER_OF_BUFFERS );
	ASSERT( Actual.cbBuffer >= pProperties->cbBuffer );
	ASSERT( Actual.cbAlign == pProperties->cbAlign );

    if ( pProperties->cBuffers > Actual.cBuffers ||
         pProperties->cbBuffer > Actual.cbBuffer ||
		 pProperties->cbAlign  > Actual.cbAlign) {
                return E_FAIL;
    }
    return NOERROR;
}


// GetMediaType: I support one type: stereo PCM waveform 
// with the same bits and samplerate as the input one.
// This type is only available if my input is connected.
HRESULT CStereoDownmixer::GetMediaType(int iPosition, CMediaType *pMediaType)
{
	// Is the input pin connected?
    if (m_pInput->IsConnected() == FALSE) return E_UNEXPECTED;

	// This should never happen
    if (iPosition < 0) return E_INVALIDARG;

    // Do we have more items to offer? No, only one.
    if (iPosition > 0) return VFW_S_NO_MORE_ITEMS;

	*pMediaType=MakeOutputCMediaTypeFromParams(m_StereoDownmixerParams);

    return NOERROR;
}

// Downmix and put out
HRESULT CStereoDownmixer::Transform(IMediaSample *pIn, IMediaSample *pOut)
{
	//Let's copy the common parameters to the dest sample
	// Copy the sample times
	REFERENCE_TIME TimeStart, TimeEnd;
	if (NOERROR == pIn->GetTime(&TimeStart, &TimeEnd)) {
		pOut->SetTime(&TimeStart, &TimeEnd);
	}

	LONGLONG MediaStart, MediaEnd;
	if (pIn->GetMediaTime(&MediaStart,&MediaEnd) == NOERROR) {
		pOut->SetMediaTime(&MediaStart,&MediaEnd);
	}

	// Copy the Sync point property
	HRESULT hr = pIn->IsSyncPoint();
	if (hr == S_OK) pOut->SetSyncPoint(TRUE);
	else if (hr == S_FALSE) pOut->SetSyncPoint(FALSE);
	else {  // an unexpected error has occured...
		return E_UNEXPECTED;
	}

	// Copy the preroll property
    hr = pIn->IsPreroll();
    if (hr == S_OK) pOut->SetPreroll(TRUE);
    else if (hr == S_FALSE) pOut->SetPreroll(FALSE);
    else {  // an unexpected error has occured...
        return E_UNEXPECTED;
    }

    // Copy the discontinuity property
    hr = pIn->IsDiscontinuity();
    if (hr == S_OK) pOut->SetDiscontinuity(TRUE);
    else if (hr == S_FALSE) pOut->SetDiscontinuity(FALSE);
    else {  // an unexpected error has occured...
        return E_UNEXPECTED;
    }


	//We never use SetMediaType: we never change output type, I hope also the input type won't change.
	//pOut->SetMediaType(&something);


	//Input format:
	DWORD SourceDataLength			=	pIn->GetActualDataLength();

	WAVEFORMATEXTENSIBLE input_wfex; memset((void*)&input_wfex,0,sizeof(input_wfex));
	if (((WAVEFORMATEX *)m_pInput->CurrentMediaType().Format())->wFormatTag==WAVE_FORMAT_PCM) memcpy(&input_wfex,m_pInput->CurrentMediaType().Format(),sizeof(WAVEFORMATEX));
	else if (((WAVEFORMATEX *)m_pInput->CurrentMediaType().Format())->wFormatTag==WAVE_FORMAT_EXTENSIBLE) memcpy(&input_wfex,m_pInput->CurrentMediaType().Format(),sizeof(WAVEFORMATEXTENSIBLE));
#ifdef _DEBUG
	else ASSERT(NULL); //This shouldn't happen!!
#endif
	
	DWORD SourceByteSizeOfSample	=	input_wfex.Format.wBitsPerSample / 8;
	DWORD SourceNumberOfChannels	=	input_wfex.Format.nChannels;
	DWORD SourceBlockSize			=	input_wfex.Format.nBlockAlign;

	//We don't know if it's a WAVEFORMATEXTENSIBLE or a plain old WAVEFORMATEX, so:
	DWORD SourceValidBitsPerSample	=	(input_wfex.Format.wFormatTag==WAVE_FORMAT_EXTENSIBLE) 
													? input_wfex.Samples.wValidBitsPerSample
													: input_wfex.Format.wBitsPerSample
													;

	DWORD SourceChannelMask			=	(input_wfex.Format.wFormatTag==WAVE_FORMAT_EXTENSIBLE) 
													? input_wfex.dwChannelMask
													: (
														input_wfex.Format.nChannels==2 ? SPEAKER_FRONT_LEFT|SPEAKER_FRONT_RIGHT 
														: SPEAKER_FRONT_CENTER
														)	//Was it mono or stereo? If it was mono we consider it a front center.
													;

	


	//Input and output have an equal number of samples per channel
	DWORD NumberOfSamplesPerChannel	=	SourceDataLength / SourceBlockSize; // /SourceByteSizeOfSamples / SourceNumberOfChannels;
	

	//Output format:
	WAVEFORMATEXTENSIBLE output_wfex; memset((void*)&output_wfex,0,sizeof(output_wfex));
	if (((WAVEFORMATEX *)m_pOutput->CurrentMediaType().Format())->wFormatTag==WAVE_FORMAT_PCM) memcpy(&output_wfex,m_pOutput->CurrentMediaType().Format(),sizeof(WAVEFORMATEX));
	else if (((WAVEFORMATEX *)m_pOutput->CurrentMediaType().Format())->wFormatTag==WAVE_FORMAT_EXTENSIBLE) memcpy(&output_wfex,m_pOutput->CurrentMediaType().Format(),sizeof(WAVEFORMATEXTENSIBLE));
#ifdef _DEBUG
	else ASSERT(NULL); //This shouldn't happen!!
#endif

	DWORD DestByteSizeOfSamples		=	output_wfex.Format.wBitsPerSample / 8;
	DWORD DestNumberOfChannels		=	output_wfex.Format.nChannels;
	DWORD DestBlockSize				=	output_wfex.Format.nBlockAlign;
	
	DWORD DestValidBitsPerSample			=	DestByteSizeOfSamples * 8;
	ASSERT(DestValidBitsPerSample==output_wfex.Format.wBitsPerSample);
	ASSERT(output_wfex.Format.wFormatTag==WAVE_FORMAT_PCM || DestValidBitsPerSample==output_wfex.Samples.wValidBitsPerSample);
	
	DWORD DestChannelMask			=	(output_wfex.Format.wFormatTag==WAVE_FORMAT_EXTENSIBLE) 
													? output_wfex.dwChannelMask
													: (
														output_wfex.Format.nChannels==2 ? SPEAKER_FRONT_LEFT|SPEAKER_FRONT_RIGHT
														: SPEAKER_FRONT_CENTER
														)
													;



	BYTE *pSourceBuffer; pIn->GetPointer(&pSourceBuffer);
	BYTE *pDestBuffer; pOut->GetPointer(&pDestBuffer);
	

	// Put the right actual data length
	pOut->SetActualDataLength(NumberOfSamplesPerChannel * DestByteSizeOfSamples * DestNumberOfChannels); //Each one is 16 bits stereo		//SourceDataLength * 2 / SourceNumberOfChannels); //From nChannels to two

	//Copy and downmix the samples

	//Obsolete:
	//__int64 LeftOutputSample, RightOutputSample; //Output samples

	__int64 FL,FR,FC,LF,BL,BR,FLC,FRC,BC,SL,SR,TC,TFL,TFC,TFR,TBL,TBC,TBR; //Input samples. We handle up to 17.1 .
	int temp_int;
	BYTE *srcbuf_chanptr,*destbuf_chanptr;
	DWORD NonvalidBitsTrasher=0xFFFFFFFF; NonvalidBitsTrasher <<= 32 - SourceValidBitsPerSample;

	DWORD UnusedBytesUpToDword= 4 - SourceByteSizeOfSample; 
	//We have DWORD variables for input waves. 
	//If the input is e.g. 24 bits the unused space is 1 byte.
	//We use the DWORD input variables so that the upper bits are always valid data
	//while the lower bits might contain nonvalid data and in that case we zero the 
	//nonsignificant bits doing   &=NonvalidBitsTrasher 

	//Is the dwChannelMask OK, or there are missing bits set / extra bits set?
	//See docs at http://www.microsoft.com/hwdev/tech/audio/multichaud.asp
	{
		if (SourceNumberOfChannels > 32) SourceNumberOfChannels=32; //What??


		//Code equal to the one in StereoDownmixerPropP2::UpdateView();

		DWORD SetBitCounter=0;
		DWORD ChannelMaskClone=SourceChannelMask;
		while (ChannelMaskClone)
		{
			if (ChannelMaskClone & 0x1) SetBitCounter++;
			ChannelMaskClone >>= 1;
		}

		int AddBits=((signed)SourceNumberOfChannels) - ((signed)SetBitCounter);

		//Adding bits from the lowest significant one
		DWORD BitAdder=0x1;
		while (AddBits > 0)
		{
			while (SourceChannelMask & BitAdder) BitAdder <<= 1; //This bit is at 1 already
			SourceChannelMask |= BitAdder;
			AddBits--;
		}

		//Clearing bits from the higher value one
		DWORD BitClearer=0x80000000;
		while (AddBits < 0)
		{
			while ((SourceChannelMask & BitClearer)==0) BitClearer >>= 1; //This bit is at 0 already
			SourceChannelMask ^= BitClearer;
			AddBits++;
		}

		//Now we have exactly SourceNumberOfChannels bits at 1 into SourceChannelMask
	}


	//Debug:
//	ASSERT(NULL);
//	ASSERT(SourceByteSizeOfSample == 3);
//	ASSERT(UnusedBytesUpToDword == 1);


	for (	DWORD cursamp = 0 ; 
			cursamp < NumberOfSamplesPerChannel ; 
			cursamp++
			)
	{
		FL=FR=FC=LF=BL=BR=FLC=FRC=BC=SL=SR=TC=TFL=TFC=TFR=TBL=TBC=TBR=0;
		srcbuf_chanptr=pSourceBuffer;
		destbuf_chanptr=pDestBuffer;


		//	Fetching channels which are present:
		//	The channels which are present MUST appear in this order:
		//	SPEAKER_FRONT_LEFT            
		//	SPEAKER_FRONT_RIGHT           
		//	SPEAKER_FRONT_CENTER          
		//	SPEAKER_LOW_FREQUENCY         
		//	SPEAKER_BACK_LEFT             
		//	SPEAKER_BACK_RIGHT            
		//	SPEAKER_FRONT_LEFT_OF_CENTER  
		//	SPEAKER_FRONT_RIGHT_OF_CENTER 
		//	SPEAKER_BACK_CENTER           
		//	SPEAKER_SIDE_LEFT             
		//	SPEAKER_SIDE_RIGHT            
		//	SPEAKER_TOP_CENTER            
		//	SPEAKER_TOP_FRONT_LEFT        
		//	SPEAKER_TOP_FRONT_CENTER      
		//	SPEAKER_TOP_FRONT_RIGHT       
		//	SPEAKER_TOP_BACK_LEFT         
		//	SPEAKER_TOP_BACK_CENTER       
		//	SPEAKER_TOP_BACK_RIGHT        


		//Many times the same thing ---> macro
		//To understand the line beginning with 
		//if (SourceByteSizeOfSample==1) 
		//remember that 8 bits samples are unsigned, and we have to convert them to a signed value.
		#define FETCH_CHANNEL(id,var)	\
			if (SourceChannelMask & id)	\
			{	\
				temp_int=0;	\
				memcpy( ((BYTE*)&temp_int)+UnusedBytesUpToDword , srcbuf_chanptr , SourceByteSizeOfSample);	\
				temp_int &= NonvalidBitsTrasher;	\
				if (SourceByteSizeOfSample==1) temp_int -= 0x80000000;	\
				var=temp_int;	\
				srcbuf_chanptr+=SourceByteSizeOfSample;	\
			}

		FETCH_CHANNEL(SPEAKER_FRONT_LEFT           ,FL );
		FETCH_CHANNEL(SPEAKER_FRONT_RIGHT          ,FR );
		FETCH_CHANNEL(SPEAKER_FRONT_CENTER         ,FC );
		FETCH_CHANNEL(SPEAKER_LOW_FREQUENCY        ,LF );
		FETCH_CHANNEL(SPEAKER_BACK_LEFT            ,BL );
		FETCH_CHANNEL(SPEAKER_BACK_RIGHT           ,BR );
		FETCH_CHANNEL(SPEAKER_FRONT_LEFT_OF_CENTER ,FLC);
		FETCH_CHANNEL(SPEAKER_FRONT_RIGHT_OF_CENTER,FRC);
		FETCH_CHANNEL(SPEAKER_BACK_CENTER          ,BC );
		FETCH_CHANNEL(SPEAKER_SIDE_LEFT            ,SL );
		FETCH_CHANNEL(SPEAKER_SIDE_RIGHT           ,SR );
		FETCH_CHANNEL(SPEAKER_TOP_CENTER           ,TC );
		FETCH_CHANNEL(SPEAKER_TOP_FRONT_LEFT       ,TFL);
		FETCH_CHANNEL(SPEAKER_TOP_FRONT_CENTER     ,TFC);
		FETCH_CHANNEL(SPEAKER_TOP_FRONT_RIGHT      ,TFR);
		FETCH_CHANNEL(SPEAKER_TOP_BACK_LEFT        ,TBL);
		FETCH_CHANNEL(SPEAKER_TOP_BACK_CENTER      ,TBC);
		FETCH_CHANNEL(SPEAKER_TOP_BACK_RIGHT       ,TBR);


		//Input enables
		if ((m_StereoDownmixerParams.p2.InputEnableBitmask & SPEAKER_FRONT_LEFT           )==0) FL =0;
		if ((m_StereoDownmixerParams.p2.InputEnableBitmask & SPEAKER_FRONT_RIGHT          )==0) FR =0;
		if ((m_StereoDownmixerParams.p2.InputEnableBitmask & SPEAKER_FRONT_CENTER         )==0) FC =0;
		if ((m_StereoDownmixerParams.p2.InputEnableBitmask & SPEAKER_LOW_FREQUENCY        )==0) LF =0;
		if ((m_StereoDownmixerParams.p2.InputEnableBitmask & SPEAKER_BACK_LEFT            )==0) BL =0;
		if ((m_StereoDownmixerParams.p2.InputEnableBitmask & SPEAKER_BACK_RIGHT           )==0) BR =0;
		if ((m_StereoDownmixerParams.p2.InputEnableBitmask & SPEAKER_FRONT_LEFT_OF_CENTER )==0) FLC=0;
		if ((m_StereoDownmixerParams.p2.InputEnableBitmask & SPEAKER_FRONT_RIGHT_OF_CENTER)==0) FRC=0;
		if ((m_StereoDownmixerParams.p2.InputEnableBitmask & SPEAKER_BACK_CENTER          )==0) BC =0;
		if ((m_StereoDownmixerParams.p2.InputEnableBitmask & SPEAKER_SIDE_LEFT            )==0) SL =0;
		if ((m_StereoDownmixerParams.p2.InputEnableBitmask & SPEAKER_SIDE_RIGHT           )==0) SR =0;
		if ((m_StereoDownmixerParams.p2.InputEnableBitmask & SPEAKER_TOP_CENTER           )==0) TC =0;
		if ((m_StereoDownmixerParams.p2.InputEnableBitmask & SPEAKER_TOP_FRONT_LEFT       )==0) TFL=0;
		if ((m_StereoDownmixerParams.p2.InputEnableBitmask & SPEAKER_TOP_FRONT_CENTER     )==0) TFC=0;
		if ((m_StereoDownmixerParams.p2.InputEnableBitmask & SPEAKER_TOP_FRONT_RIGHT      )==0) TFR=0;
		if ((m_StereoDownmixerParams.p2.InputEnableBitmask & SPEAKER_TOP_BACK_LEFT        )==0) TBL=0;
		if ((m_StereoDownmixerParams.p2.InputEnableBitmask & SPEAKER_TOP_BACK_CENTER      )==0) TBC=0;
		if ((m_StereoDownmixerParams.p2.InputEnableBitmask & SPEAKER_TOP_BACK_RIGHT       )==0) TBR=0;


		//Pre-mix input gains
		if (m_StereoDownmixerParams.p2.bEnableInputGains)
		{
			FL  = (FL  * m_StereoDownmixerParams.p2.I.FL ) / 100;
			FR  = (FR  * m_StereoDownmixerParams.p2.I.FR ) / 100;
			FC  = (FC  * m_StereoDownmixerParams.p2.I.FC ) / 100;
			LF  = (LF  * m_StereoDownmixerParams.p2.I.LF ) / 100;
			BL  = (BL  * m_StereoDownmixerParams.p2.I.BL ) / 100;
			BR  = (BR  * m_StereoDownmixerParams.p2.I.BR ) / 100;
			FLC = (FLC * m_StereoDownmixerParams.p2.I.FLC) / 100;
			FRC = (FRC * m_StereoDownmixerParams.p2.I.FRC) / 100;
			BC  = (BC  * m_StereoDownmixerParams.p2.I.BC ) / 100;
			SL  = (SL  * m_StereoDownmixerParams.p2.I.SL ) / 100;
			SR  = (SR  * m_StereoDownmixerParams.p2.I.SR ) / 100;
			TC  = (TC  * m_StereoDownmixerParams.p2.I.TC ) / 100;
			TFL = (TFL * m_StereoDownmixerParams.p2.I.TFL) / 100;
			TFC = (TFC * m_StereoDownmixerParams.p2.I.TFC) / 100;
			TFR = (TFR * m_StereoDownmixerParams.p2.I.TFR) / 100;
			TBL = (TBL * m_StereoDownmixerParams.p2.I.TBL) / 100;
			TBC = (TBC * m_StereoDownmixerParams.p2.I.TBC) / 100;
			TBR = (TBR * m_StereoDownmixerParams.p2.I.TBR) / 100;
		}


		//------------------------------------
		
		//Now we downmix every channel we don't have at output to the FL and FR channels,
		//except the FLC and FRC which will be downmixed also to the FC, IF PRESENT

		//Dolby Surround downmix algorithm kindly suggested by S_O on the everwicked.com forum
		//S_O was not sure about the FLC and FRC speakers: I'm trying the algorithm below
		
		//Dolby Surround downmix algorithm guidelines as provided by S_O
		//
		//  FL = FL + 0.707(FC + FLC + FRC) + 0.707(RL + RR) + 0.707 LF
		//  FR = FR + 0.707(FC + FLC + FRC) - 0.707(RL + RR) + 0.707 LF

		//------------------------------------


		// - - - D O W N M I X I N G - - -

		//Custom downmixing matrix or internal one?
		if (m_StereoDownmixerParams.p3.bCustomMixMatrix)
		{
			//Custom downmixing matrix

			//Push the inputs into this temporary array for matrix computation
			m_tempInputs[0 ] = (double) FL ;
			m_tempInputs[1 ] = (double) FR ;
			m_tempInputs[2 ] = (double) FC ;
			m_tempInputs[3 ] = (double) LF ;
			m_tempInputs[4 ] = (double) BL ;
			m_tempInputs[5 ] = (double) BR ;
			m_tempInputs[6 ] = (double) FLC;
			m_tempInputs[7 ] = (double) FRC;
			m_tempInputs[8 ] = (double) BC ;
			m_tempInputs[9 ] = (double) SL ;
			m_tempInputs[10] = (double) SR ;
			m_tempInputs[11] = (double) TC ;
			m_tempInputs[12] = (double) TFL;
			m_tempInputs[13] = (double) TFC;
			m_tempInputs[14] = (double) TFR;
			m_tempInputs[15] = (double) TBL;
			m_tempInputs[16] = (double) TBC;
			m_tempInputs[17] = (double) TBR;


			//Zeroing the temporary outputs array 
			//(so we can use the += operator during the matrix computation):
			{
				for (int i=0;i<18;i++) {m_tempOutputs[i]=0;}
			}


			int i,j;

			//The column number is I and the row number is J. 
			//Read the description of the inputs and outputs in property page number 3:
			//
			//<< You can imagine to have the inputs coming from the top of the matrix and the 
			//   outputs at the left of it.
			//   The inputs are multiplied by the values in the columns of the matrix and sent left 
			//   to the outputs. For example a 1.50000 value in the second column third row means 
			//   that the FR input is multiplied by 1.5 and added to the FC output. >>
			//

			
			//Unwise unoptimized algorithm (always computes all rows and all columns):
			//
			//	for (i=0;i<18;i++)
			//	{
			//		for (j=0;j<18;j++)
			//		{
			//			m_tempOutputs[j] += m_CustMixMatrix[j][i] * m_tempInputs[i];
			//		}
			//	}


			//Wise optimized algorithm: only compute those rows for which we have an input and 
			//those columns for which we will output the variable.

			DWORD InputBit;
			DWORD OutputBit;

			InputBit=0x1;
			i=0;
			while (i<18)
			{
				if (SourceChannelMask & InputBit)	//If this results zero, we don't have the input channel number I, so it's useless to compute the following stuff.
				{
					OutputBit=0x1;
					j=0;
					while (j<18)
					{
						if (DestChannelMask & OutputBit)	//If this result is zero, we won't output that variable anyway, so we don't compute it.
						{
							m_tempOutputs[j] += m_CustMixMatrix[j][i] * m_tempInputs[i];
						}

						OutputBit <<= 1;
						j++;
					}
				}

				InputBit <<= 1;
				i++;
			}
			

			//Fetch the outputs from the temporary output array
			FL  = (__int64) m_tempOutputs[0 ];
			FR  = (__int64) m_tempOutputs[1 ];
			FC  = (__int64) m_tempOutputs[2 ];
			LF  = (__int64) m_tempOutputs[3 ];
			BL  = (__int64) m_tempOutputs[4 ];
			BR  = (__int64) m_tempOutputs[5 ];
			FLC = (__int64) m_tempOutputs[6 ];
			FRC = (__int64) m_tempOutputs[7 ];
			BC  = (__int64) m_tempOutputs[8 ];
			SL  = (__int64) m_tempOutputs[9 ];
			SR  = (__int64) m_tempOutputs[10];
			TC  = (__int64) m_tempOutputs[11];
			TFL = (__int64) m_tempOutputs[12];
			TFC = (__int64) m_tempOutputs[13];
			TFR = (__int64) m_tempOutputs[14];
			TBL = (__int64) m_tempOutputs[15];
			TBC = (__int64) m_tempOutputs[16];
			TBR = (__int64) m_tempOutputs[17];
		}
		else	//Internal dowmixing matrix
		{
			
			//Sqrt(2) / 2 = 0.70710678118654752440084436210485
			#define HALF_POWER 0.70710678118

			#define _7POINT1_SPEAKERS_DOWNMIX_POWER_TO_NEAR	0.85000000000	//0.85^2 = 0,7225
			#define _7POINT1_SPEAKERS_DOWNMIX_POWER_TO_FAR	0.52678268764	//0.52678268764^2 + 0.85^2 = 0,9999999999972218087696 ~= 1 OK

			#define ONE_THIRD_OF_POWER 0.57735026919

			if (DestNumberOfChannels == 1)	//Downmix to MONO!!!
			{
				FC += FL+FR+LF+BL+BR+FLC+FRC+BC+SL+SR+TC+TFL+TFR+TFC+TBL+TBR+TBC;

				FL=FR=LF=BL=BR=FLC=FRC=BC=SL=SR=TC=TFL=TFR=TFC=TBL=TBR=TBC=0; //All downmixed.
			}
			else //Downmix to at least stereo
			{


				//
				//        ------------      DOWNMIXING TOP SPEAKERS      ------------
				//


				//SPEAKER_TOP_BACK_LEFT and SPEAKER_TOP_BACK_RIGHT--->
				if (   ((SourceChannelMask & SPEAKER_TOP_BACK_LEFT )!=0 && (DestChannelMask & SPEAKER_TOP_BACK_LEFT )==0)
					|| ((SourceChannelMask & SPEAKER_TOP_BACK_RIGHT)!=0 && (DestChannelMask & SPEAKER_TOP_BACK_RIGHT)==0)
					)
				{
					if (DestChannelMask & SPEAKER_TOP_CENTER)
					{
						TC += __int64(double(HALF_POWER) * (TBL+TBR));

						if ((DestChannelMask & SPEAKER_BACK_LEFT)!=0 && (DestChannelMask & SPEAKER_BACK_RIGHT)!=0)
						{
							BL += __int64(double(HALF_POWER) * (TBL));
							BR += __int64(double(HALF_POWER) * (TBR));
						}
						else
						{
							FL += __int64(double(HALF_POWER) * (TBL));
							FR += __int64(double(HALF_POWER) * (TBR));
						}
					}
					else
					{
						if ((DestChannelMask & SPEAKER_BACK_LEFT)!=0 && (DestChannelMask & SPEAKER_BACK_RIGHT)!=0)
						{
							BL += TBL;
							BR += TBR;
						}
						else 
						{
							FL += TBL;
							FR += TBR;
						}
					}

					TBL=TBR=0;
				}
				//<---SPEAKER_TOP_BACK_LEFT and SPEAKER_TOP_BACK_RIGHT

				//SPEAKER_TOP_BACK_CENTER--->
				if ((SourceChannelMask & SPEAKER_TOP_BACK_CENTER)!=0 && (DestChannelMask & SPEAKER_TOP_BACK_CENTER)==0)
				{
					//16 possibilities, depending upon the presence of:
					//
					//   SPEAKER_TOP_BACK_LEFT + SPEAKER_TOP_BACK_RIGHT
					//or SPEAKER_TOP_CENTER, SPEAKER_BACK_CENTER, and SPEAKER_BACK_LEFT + SPEAKER_BACK_RIGHT.

					if ((DestChannelMask & SPEAKER_TOP_BACK_LEFT)!=0 && (DestChannelMask & SPEAKER_TOP_BACK_RIGHT)!=0)
					{
						TBL += __int64(double(HALF_POWER) * (TBC));
						TBR += __int64(double(HALF_POWER) * (TBC));
					}
					else
					{
						//Downmix partially to the ground

						if ((DestChannelMask & SPEAKER_TOP_CENTER)==0 && (DestChannelMask & SPEAKER_BACK_CENTER)==0) 
						{
							if ((DestChannelMask & SPEAKER_BACK_LEFT)!=0 && (DestChannelMask & SPEAKER_BACK_RIGHT)!=0)
							{
								BL += __int64(double(HALF_POWER) * (TBC));
								BR += __int64(double(HALF_POWER) * (TBC));
							}
							else
							{
								FL += __int64(double(HALF_POWER) * (TBC));
								FR += __int64(double(HALF_POWER) * (TBC));
							}
						}
						else if ((DestChannelMask & SPEAKER_TOP_CENTER)==0 && (DestChannelMask & SPEAKER_BACK_CENTER)!=0) 
						{
							BC += TBC;
						}
						else if ((DestChannelMask & SPEAKER_TOP_CENTER)!=0 && (DestChannelMask & SPEAKER_BACK_CENTER)==0) 
						{
							//Share power among 3 speakers, but not evenly: the ground back speakers should 
							//provide half of the power overall (0.25 each), and the top center one the other half.
							//
							//If the ground back speakers are absent, we use the ground front speakers.

							TC += __int64(double(HALF_POWER) * (TBC));

							if ((DestChannelMask & SPEAKER_BACK_LEFT)!=0 && (DestChannelMask & SPEAKER_BACK_RIGHT)!=0)
							{
								BL += TBC / 2;
								BR += TBC / 2;
							}
							else
							{
								FL += TBC / 2;
								FR += TBC / 2;
							}
						}
						else if ((DestChannelMask & SPEAKER_TOP_CENTER)!=0 && (DestChannelMask & SPEAKER_BACK_CENTER)!=0) 
						{
							TC += __int64(double(HALF_POWER) * (TBC));
							BC += __int64(double(HALF_POWER) * (TBC));
						}
					}

					TBC=0;
				}
				//<---SPEAKER_TOP_BACK_CENTER


				//SPEAKER_TOP_FRONT_LEFT and SPEAKER_TOP_FRONT_RIGHT--->
				if (   ((SourceChannelMask & SPEAKER_TOP_FRONT_LEFT )!=0 && (DestChannelMask & SPEAKER_TOP_FRONT_LEFT )==0)
					|| ((SourceChannelMask & SPEAKER_TOP_FRONT_RIGHT)!=0 && (DestChannelMask & SPEAKER_TOP_FRONT_RIGHT)==0)
					)
				{
					if (DestChannelMask & SPEAKER_TOP_CENTER)
					{
						TC += __int64(double(HALF_POWER) * (TFL+TFR));
						FL += __int64(double(HALF_POWER) * (TFL));
						FR += __int64(double(HALF_POWER) * (TFR));
					}
					else
					{
						FL += TFL;
						FR += TFR;
					}

					TFL=TFR=0;
				}
				//<---SPEAKER_TOP_FRONT_LEFT and SPEAKER_TOP_FRONT_RIGHT

				//SPEAKER_TOP_FRONT_CENTER--->
				if ((SourceChannelMask & SPEAKER_TOP_FRONT_CENTER)!=0 && (DestChannelMask & SPEAKER_TOP_FRONT_CENTER)==0)
				{
					//8 possibilities, depending upon the presence of 
					//   SPEAKER_TOP_FRONT_LEFT+SPEAKER_TOP_FRONT_RIGHT
					//or SPEAKER_TOP_CENTER, SPEAKER_FRONT_CENTER 

					if ((DestChannelMask & SPEAKER_TOP_FRONT_LEFT)!=0 && (DestChannelMask & SPEAKER_TOP_FRONT_RIGHT)!=0) 
					{
						TFL += __int64(double(HALF_POWER) * (TFC));
						TFR += __int64(double(HALF_POWER) * (TFC));
					}
					else
					{
						//Downmix partially to the ground

						if ((DestChannelMask & SPEAKER_TOP_CENTER)==0 && (DestChannelMask & SPEAKER_FRONT_CENTER)==0) 
						{
							FL += __int64(double(HALF_POWER) * (TFC));
							FR += __int64(double(HALF_POWER) * (TFC));
						}
						if ((DestChannelMask & SPEAKER_TOP_CENTER)==0 && (DestChannelMask & SPEAKER_FRONT_CENTER)!=0) 
						{
							FC += TFC;
						}
						if ((DestChannelMask & SPEAKER_TOP_CENTER)!=0 && (DestChannelMask & SPEAKER_FRONT_CENTER)==0) 
						{
							//Share power among 3 speakers, but not evenly: the ground front speakers should 
							//provide half of the power overall (0.25 each), and the top center one the other half.

							TC += __int64(double(HALF_POWER) * (TFC));
							FL += TFC / 2;
							FR += TFC / 2;
						}
						if ((DestChannelMask & SPEAKER_TOP_CENTER)!=0 && (DestChannelMask & SPEAKER_FRONT_CENTER)!=0) 
						{
							TC += __int64(double(HALF_POWER) * (TFC));
							FC += __int64(double(HALF_POWER) * (TFC));
						}
					}

					TFC=0;
				}
				//<---SPEAKER_TOP_FRONT_CENTER

				//SPEAKER_TOP_CENTER--->
				if ((SourceChannelMask & SPEAKER_TOP_CENTER)!=0 && (DestChannelMask & SPEAKER_TOP_CENTER)==0)
				{
					//Do we have 4.0 ?
					if ((DestChannelMask & SPEAKER_BACK_LEFT)!=0 && (DestChannelMask & SPEAKER_BACK_RIGHT)!=0)
					{
						//Share power among 4 speakers = [sqrt(0.25) * sample = 0.5 * sample] to each
						FL += TC / 2;
						FR += TC / 2;
						BL += TC / 2;
						BR += TC / 2;
					}
					else
					{
						//Downmix to 2.0
						FL += __int64(double(HALF_POWER) * (TC));
						FR += __int64(double(HALF_POWER) * (TC));
					}

					TC=0;
				}
				//<---SPEAKER_TOP_CENTER



				//
				//        ------------      DOWNMIXING GROUND SPEAKERS      ------------
				//



				//SPEAKER_SIDE_LEFT and SPEAKER_SIDE_RIGHT--->
				if (
					   ((SourceChannelMask & SPEAKER_SIDE_LEFT )!=0 && (DestChannelMask & SPEAKER_SIDE_LEFT )==0)
					|| ((SourceChannelMask & SPEAKER_SIDE_RIGHT)!=0 && (DestChannelMask & SPEAKER_SIDE_RIGHT)==0)
					)
				{
					if ((DestChannelMask & SPEAKER_BACK_LEFT)!=0 && (DestChannelMask & SPEAKER_BACK_RIGHT)!=0)
					{
						FL += __int64(double(HALF_POWER) * (SL));
						BL += __int64(double(HALF_POWER) * (SL));

						FR += __int64(double(HALF_POWER) * (SR));
						BR += __int64(double(HALF_POWER) * (SR));
					}
					else
					{
						FL += SL;
						FR += BR;
					}

					SL=SR=0;
				}
				//<---SPEAKER_SIDE_LEFT and SPEAKER_SIDE_RIGHT


				//SPEAKER_BACK_CENTER--->
				if ((SourceChannelMask & SPEAKER_BACK_CENTER)!=0 && (DestChannelMask & SPEAKER_BACK_CENTER)==0)
				{
					if ((DestChannelMask & SPEAKER_BACK_LEFT)!=0 && (DestChannelMask & SPEAKER_BACK_RIGHT)!=0)
					{
						BL += __int64(double(HALF_POWER) * (BC));
						BR += __int64(double(HALF_POWER) * (BC));
					}
					else 
					{
						FL += __int64(double(HALF_POWER) * (BC));
						FR += __int64(double(HALF_POWER) * (BC));
					}

					BC=0;
				}
				//<---SPEAKER_BACK_CENTER


				//<---SPEAKER_FRONT_LEFT_OF_CENTER and SPEAKER_FRONT_RIGHT_OF_CENTER
				if (
					   ((SourceChannelMask & SPEAKER_FRONT_LEFT_OF_CENTER )!=0 && (DestChannelMask & SPEAKER_FRONT_LEFT_OF_CENTER )==0)
					|| ((SourceChannelMask & SPEAKER_FRONT_RIGHT_OF_CENTER)!=0 && (DestChannelMask & SPEAKER_FRONT_RIGHT_OF_CENTER)==0)
					)
				{
					if (DestChannelMask & SPEAKER_FRONT_CENTER)
					{
						FC += __int64(double(HALF_POWER) * (__int64(FLC) + __int64(FRC)));
						FL += __int64(double(HALF_POWER) * __int64(FLC));
						FR += __int64(double(HALF_POWER) * __int64(FRC));
					}
					else
					{
						FL +=  __int64(
									  double(_7POINT1_SPEAKERS_DOWNMIX_POWER_TO_NEAR) * __int64(FLC)
									+ double(_7POINT1_SPEAKERS_DOWNMIX_POWER_TO_FAR) * __int64(FRC)
								);

						FR +=  __int64(
									  double(_7POINT1_SPEAKERS_DOWNMIX_POWER_TO_NEAR) * __int64(FRC)
									+ double(_7POINT1_SPEAKERS_DOWNMIX_POWER_TO_FAR) * __int64(FLC)
								);
					}

					FLC=FRC=0; //Downmixed.
				}
				//SPEAKER_FRONT_LEFT_OF_CENTER and SPEAKER_FRONT_RIGHT_OF_CENTER--->


				//SPEAKER_BACK_LEFT and SPEAKER_BACK_RIGHT--->
				if (
					   ((SourceChannelMask & SPEAKER_BACK_LEFT )!=0 && (DestChannelMask & SPEAKER_BACK_LEFT )==0)
					|| ((SourceChannelMask & SPEAKER_BACK_RIGHT)!=0 && (DestChannelMask & SPEAKER_BACK_RIGHT)==0)
					)
				{
					//If we have the SPEAKER_BACK_CENTER we downmix to that one too
					if (DestChannelMask & SPEAKER_BACK_CENTER)
					{
						BC += __int64(double(HALF_POWER) * (BL+BR));
						FL += __int64(double(HALF_POWER) * (BL));
						FR += __int64(double(HALF_POWER) * (BR));
					}
					else
					{
						FL += BL;
						FR += BR;
					}

					//Quasi-Dolby Surround downmix (deprecated)
					//
					// FL += __int64(double(HALF_POWER) * (BL+BR));
					// FR -= __int64(double(HALF_POWER) * (BL+BR));

					BL=BR=0;
				}
				//<---SPEAKER_BACK_LEFT and SPEAKER_BACK_RIGHT
				

				//SPEAKER_LOW_FREQUENCY--->
				if ((SourceChannelMask & SPEAKER_LOW_FREQUENCY)!=0 && (DestChannelMask & SPEAKER_LOW_FREQUENCY)==0)
				{
					FL += __int64(double(HALF_POWER) * (LF));
					FR += __int64(double(HALF_POWER) * (LF));

					LF=0;
				}
				//<---SPEAKER_LOW_FREQUENCY


				//SPEAKER_FRONT_CENTER--->
				if ((SourceChannelMask & SPEAKER_FRONT_CENTER)!=0 && (DestChannelMask & SPEAKER_FRONT_CENTER)==0)
				{
					FL += __int64(double(HALF_POWER) * (FC));
					FR += __int64(double(HALF_POWER) * (FC));

					FC=0;
				}
				//<---SPEAKER_FRONT_CENTER
			}

			//END OF INTERNAL DOWNMIXING

		}	
		//END OF DOWNMIXING

		
		//------------------------------------


		//Post-mix output gains
		if (m_StereoDownmixerParams.p2.bEnableOutputGains)
		{
			FL  = (FL  * m_StereoDownmixerParams.p2.O.FL ) / 100;
			FR  = (FR  * m_StereoDownmixerParams.p2.O.FR ) / 100;
			FC  = (FC  * m_StereoDownmixerParams.p2.O.FC ) / 100;
			LF  = (LF  * m_StereoDownmixerParams.p2.O.LF ) / 100;
			BL  = (BL  * m_StereoDownmixerParams.p2.O.BL ) / 100;
			BR  = (BR  * m_StereoDownmixerParams.p2.O.BR ) / 100;
			FLC = (FLC * m_StereoDownmixerParams.p2.O.FLC) / 100;
			FRC = (FRC * m_StereoDownmixerParams.p2.O.FRC) / 100;

			BC  = (BC  * m_StereoDownmixerParams.p2.O.BC ) / 100;
			SL  = (SL  * m_StereoDownmixerParams.p2.O.SL ) / 100;
			SR  = (SR  * m_StereoDownmixerParams.p2.O.SR ) / 100;
			TC  = (TC  * m_StereoDownmixerParams.p2.O.TC ) / 100;
			TFL = (TFL * m_StereoDownmixerParams.p2.O.TFL) / 100;
			TFC = (TFC * m_StereoDownmixerParams.p2.O.TFC) / 100;
			TFR = (TFR * m_StereoDownmixerParams.p2.O.TFR) / 100;
			TBL = (TBL * m_StereoDownmixerParams.p2.O.TBL) / 100;
			TBC = (TBC * m_StereoDownmixerParams.p2.O.TBC) / 100;
			TBR = (TBR * m_StereoDownmixerParams.p2.O.TBR) / 100;
		}


		//Master volume
		if (m_StereoDownmixerParams.p2.bEnableMasterVolume)
		{
			FL  = (FL  * m_StereoDownmixerParams.p2.MasterVolumeGain) / 100;
			FR  = (FR  * m_StereoDownmixerParams.p2.MasterVolumeGain) / 100;
			FC  = (FC  * m_StereoDownmixerParams.p2.MasterVolumeGain) / 100;
			LF  = (LF  * m_StereoDownmixerParams.p2.MasterVolumeGain) / 100;
			BL  = (BL  * m_StereoDownmixerParams.p2.MasterVolumeGain) / 100;
			BR  = (BR  * m_StereoDownmixerParams.p2.MasterVolumeGain) / 100;
			FLC = (FLC * m_StereoDownmixerParams.p2.MasterVolumeGain) / 100;
			FRC = (FRC * m_StereoDownmixerParams.p2.MasterVolumeGain) / 100;

			BC  = (BC  * m_StereoDownmixerParams.p2.MasterVolumeGain) / 100;
			SL  = (SL  * m_StereoDownmixerParams.p2.MasterVolumeGain) / 100;
			SR  = (SR  * m_StereoDownmixerParams.p2.MasterVolumeGain) / 100;
			TC  = (TC  * m_StereoDownmixerParams.p2.MasterVolumeGain) / 100;
			TFL = (TFL * m_StereoDownmixerParams.p2.MasterVolumeGain) / 100;
			TFC = (TFC * m_StereoDownmixerParams.p2.MasterVolumeGain) / 100;
			TFR = (TFR * m_StereoDownmixerParams.p2.MasterVolumeGain) / 100;
			TBL = (TBL * m_StereoDownmixerParams.p2.MasterVolumeGain) / 100;
			TBC = (TBC * m_StereoDownmixerParams.p2.MasterVolumeGain) / 100;
			TBR = (TBR * m_StereoDownmixerParams.p2.MasterVolumeGain) / 100;
		}



		//------------------------------------


		//Clipping audio too high
		#define CLIP_CHAN(var) if (var > INT_MAX) var=INT_MAX; if (var < INT_MIN) var=INT_MIN;
		CLIP_CHAN(FL );
		CLIP_CHAN(FR );
		CLIP_CHAN(FC );
		CLIP_CHAN(LF );
		CLIP_CHAN(BL );
		CLIP_CHAN(BR );
		CLIP_CHAN(FLC);
		CLIP_CHAN(FRC);

		CLIP_CHAN(BC );
		CLIP_CHAN(SL );
		CLIP_CHAN(SR );
		CLIP_CHAN(TC );
		CLIP_CHAN(TFL);
		CLIP_CHAN(TFC);
		CLIP_CHAN(TFR);
		CLIP_CHAN(TBL);
		CLIP_CHAN(TBC);
		CLIP_CHAN(TBR);


		//------------------------------------


		//Now, think about providing the right output bit per sample

		FL	>>= 32-(DestByteSizeOfSamples*8);
		FR	>>= 32-(DestByteSizeOfSamples*8);
		FC	>>= 32-(DestByteSizeOfSamples*8);
		LF	>>= 32-(DestByteSizeOfSamples*8);
		BL	>>= 32-(DestByteSizeOfSamples*8);
		BR	>>= 32-(DestByteSizeOfSamples*8);
		FLC	>>= 32-(DestByteSizeOfSamples*8);
		FRC	>>= 32-(DestByteSizeOfSamples*8);

		BC 	>>= 32-(DestByteSizeOfSamples*8);
		SL 	>>= 32-(DestByteSizeOfSamples*8);
		SR 	>>= 32-(DestByteSizeOfSamples*8);
		TC 	>>= 32-(DestByteSizeOfSamples*8);
		TFL	>>= 32-(DestByteSizeOfSamples*8);
		TFC	>>= 32-(DestByteSizeOfSamples*8);
		TFR	>>= 32-(DestByteSizeOfSamples*8);
		TBL	>>= 32-(DestByteSizeOfSamples*8);
		TBC	>>= 32-(DestByteSizeOfSamples*8);
		TBR	>>= 32-(DestByteSizeOfSamples*8);



		//8 bits samples are unsigned while we have signed values. So fix this
		if (DestByteSizeOfSamples==1)
		{
			FL	=	(__int64)(char)(((char)FL )-0x80);
			FR	=	(__int64)(char)(((char)FR )-0x80);
			FC	=	(__int64)(char)(((char)FC )-0x80);
			LF	=	(__int64)(char)(((char)LF )-0x80);
			BL	=	(__int64)(char)(((char)BL )-0x80);
			BR	=	(__int64)(char)(((char)BR )-0x80);
			FLC	=	(__int64)(char)(((char)FLC)-0x80);
			FRC	=	(__int64)(char)(((char)FRC)-0x80);

			BC 	=	(__int64)(char)(((char)BC )-0x80);
			SL 	=	(__int64)(char)(((char)SL )-0x80);
			SR 	=	(__int64)(char)(((char)SR )-0x80);
			TC 	=	(__int64)(char)(((char)TC )-0x80);
			TFL	=	(__int64)(char)(((char)TFL)-0x80);
			TFC	=	(__int64)(char)(((char)TFC)-0x80);
			TFR	=	(__int64)(char)(((char)TFR)-0x80);
			TBL	=	(__int64)(char)(((char)TBL)-0x80);
			TBC	=	(__int64)(char)(((char)TBC)-0x80);
			TBR	=	(__int64)(char)(((char)TBR)-0x80);
		}
		

		//------------------------------------


		//Copying to dest

		#define PUT_CHANNEL(id,var)	\
			if (DestChannelMask & id)	\
			{	\
				memcpy(destbuf_chanptr,&var,DestByteSizeOfSamples);	\
				destbuf_chanptr+=DestByteSizeOfSamples;	\
			}
		
		PUT_CHANNEL(SPEAKER_FRONT_LEFT           , FL);
		PUT_CHANNEL(SPEAKER_FRONT_RIGHT          , FR);
		PUT_CHANNEL(SPEAKER_FRONT_CENTER         , FC);
		PUT_CHANNEL(SPEAKER_LOW_FREQUENCY        , LF);
		PUT_CHANNEL(SPEAKER_BACK_LEFT            , BL);
		PUT_CHANNEL(SPEAKER_BACK_RIGHT           , BR);
		PUT_CHANNEL(SPEAKER_FRONT_LEFT_OF_CENTER , FLC);
		PUT_CHANNEL(SPEAKER_FRONT_RIGHT_OF_CENTER, FRC);

		PUT_CHANNEL(SPEAKER_BACK_CENTER          , BC );
		PUT_CHANNEL(SPEAKER_SIDE_LEFT            , SL );
		PUT_CHANNEL(SPEAKER_SIDE_RIGHT           , SR );
		PUT_CHANNEL(SPEAKER_TOP_CENTER           , TC );
		PUT_CHANNEL(SPEAKER_TOP_FRONT_LEFT       , TFL);
		PUT_CHANNEL(SPEAKER_TOP_FRONT_CENTER     , TFC);
		PUT_CHANNEL(SPEAKER_TOP_FRONT_RIGHT      , TFR);
		PUT_CHANNEL(SPEAKER_TOP_BACK_LEFT        , TBL);
		PUT_CHANNEL(SPEAKER_TOP_BACK_CENTER      , TBC);
		PUT_CHANNEL(SPEAKER_TOP_BACK_RIGHT       , TBR);


		//Advancing pointers
		pSourceBuffer+=SourceBlockSize;
		pDestBuffer+=DestBlockSize;
	}

	return NOERROR;
}

//Only call this if our input pin is connected (we need this for samplerate). 
//Or provide input type as second param
CMediaType CStereoDownmixer::MakeOutputCMediaTypeFromParams(const StereoDownmixerParams &params, const CMediaType *InputMediaType)
{
	ASSERT(InputMediaType!=NULL || m_pInput->IsConnected());

	CMediaType ret;

	BOOL bNeedExtensible=FALSE;
	if (params.p1.bIsMultichannel) bNeedExtensible=TRUE;
	if (params.p1.wBitsPerSample > 16) bNeedExtensible=TRUE;
		
	//We get the input type and setup our only possible output type

	//We don't know if it is a waveformatex or waveformatextensible. But this doesn't matter since we only need
	//the samplerate which is contained in both, at the same offset.
	WAVEFORMATEX	input_wfe = (InputMediaType!=NULL) 
										? (* (WAVEFORMATEX*) InputMediaType->Format())
										: (* (WAVEFORMATEX*) m_pInput->CurrentMediaType().Format())
										;

	//We will use the fact that the WAVEFORMATEXTENSIBLE begins with a WAVEFORMATEX struct
	WAVEFORMATEXTENSIBLE wfex;
	wfex.Format.cbSize				=	(bNeedExtensible) ? sizeof(WAVEFORMATEXTENSIBLE) : sizeof(WAVEFORMATEX);
	wfex.Format.wFormatTag			=	(bNeedExtensible) ? WAVE_FORMAT_EXTENSIBLE : WAVE_FORMAT_PCM;

	//Debug:
	/*
	wfex.Format.nChannels			=	2;
	wfex.Format.wBitsPerSample		=	16;
	wfex.Format.nSamplesPerSec		=	48000;
	wfex.Format.nBlockAlign			=	4; //2*2
	wfex.Format.nAvgBytesPerSec		=	192000; //2*2*48000
	*/	

	wfex.Format.nChannels			=	params.p1.nChannels;
	wfex.Format.wBitsPerSample		=	params.p1.wBitsPerSample;
	wfex.Format.nSamplesPerSec		=	input_wfe.nSamplesPerSec;

	wfex.Format.nBlockAlign			=	(wfex.Format.wBitsPerSample / 8) * wfex.Format.nChannels;
	wfex.Format.nAvgBytesPerSec		=	wfex.Format.nBlockAlign * wfex.Format.nSamplesPerSec;
	
	//These three will be copied into the return media type only if bIsMultichannel is TRUE
	wfex.Samples.wValidBitsPerSample=	params.p1.wBitsPerSample; //Equal to the wfex.wBitsPerSample
	wfex.dwChannelMask		=	params.p1.dwChannelMask;
	wfex.SubFormat			=	KSDATAFORMAT_SUBTYPE_PCM;

	ret.majortype	=	MEDIATYPE_Audio;
	ret.subtype		=	MEDIASUBTYPE_PCM;
	ret.bFixedSizeSamples		=	TRUE;
	ret.bTemporalCompression	=	FALSE;

	//POSSIBLE ERROR - PLEASE CHECK : I don't know what is this lSampleSize. I will just copy the source.
	ret.lSampleSize	=	(InputMediaType!=NULL) ? InputMediaType->lSampleSize : m_pInput->CurrentMediaType().lSampleSize;
	
	ret.formattype	=	FORMAT_WaveFormatEx;
	ret.pUnk		=	NULL;

	ret.SetFormat((BYTE*)&wfex,wfex.Format.cbSize);

	return ret;
}

STDMETHODIMP_(BOOL) CStereoDownmixer::IsThisInputMediaTypeAcceptableAndMatchesParams(const AM_MEDIA_TYPE &media_type, const StereoDownmixerParams &params)
{
	//MajorType when S/PDIF by Elecard Video Decoder  {73647561-0000-0010-8000-00AA00389B71}
	//Subtype when S/PDIF by Elecard Video Decoder    {00000001-0000-0010-8000-00AA00389B71}
	//FormatType when S/PDIF by Elecard Video Decoder {05589F81-C356-11CE-BF01-00AA0055595A}

	//It must be an audio type of course
	if (media_type.formattype != FORMAT_WaveFormatEx) return FALSE;

	//I accept only multichannel inputs 7.1 or subsets of 7.1 with 32 bits of sample size or less.
	//Any samplerate.


	//Either WAVE_FORMAT_PCM or WAVE_FORMAT_EXTENSIBLE. I don't accept other types.
	if ( ((WAVEFORMATEX*)media_type.pbFormat)->wFormatTag != WAVE_FORMAT_PCM 
		 && ((WAVEFORMATEX*)media_type.pbFormat)->wFormatTag != WAVE_FORMAT_EXTENSIBLE) return FALSE;

	//If it is a WAVE_FORMAT_EXTENSIBLE it must be a KSDATAFORMAT_SUBTYPE_PCM (I don't support WAVE_FORMAT_IEEE_FLOAT or the like)
	if ( ((WAVEFORMATEX*)media_type.pbFormat)->wFormatTag == WAVE_FORMAT_EXTENSIBLE
		 && ((WAVEFORMATEXTENSIBLE*)media_type.pbFormat)->SubFormat!=KSDATAFORMAT_SUBTYPE_PCM) return FALSE;


	//Only 17.1 or subsets of that (checking it has less than 18 speakers is NOT enough 
	//because the channel mask also must not have any bits set in the high word, 
	//anyway the check on nChannels is still required in case the channel mask is not complete)
	if (((WAVEFORMATEX*)media_type.pbFormat)->nChannels > 18) return FALSE;
	if ( 
			(((WAVEFORMATEXTENSIBLE*)media_type.pbFormat)->dwChannelMask | 0x3FFFF) != 0x3FFFF
	   ) return VFW_E_TYPE_NOT_ACCEPTED;

	//Only 32 bits per sample or less
	if ( ((WAVEFORMATEX*)media_type.pbFormat)->wBitsPerSample > 32 ) return FALSE;


	// --- If we have input requirements, also check these:
	if (params.p1.bRequireInputNumberOfChannels 
		&& params.p1.RequiredInputNumberOfChannels != ((WAVEFORMATEX*)media_type.pbFormat)->nChannels) return FALSE;
	
	if (params.p1.bRequireInputSamplerate 
		&& params.p1.RequiredInputSamplerate != ((WAVEFORMATEX*)media_type.pbFormat)->nSamplesPerSec) return FALSE;

	if (params.p1.RequiredInputBitDepth!=0
		&& params.p1.RequiredInputBitDepth != ((WAVEFORMATEX*)media_type.pbFormat)->wBitsPerSample) return FALSE;

	return TRUE;
}

BOOL CStereoDownmixer::LoadCustomMixMatrix(StereoDownmixerParams::Page3Params p3params)
{
	if (p3params.bCustomMixMatrix==FALSE) return TRUE; //No matrix to load

	FILE * pfile=fopen(p3params.CustomMixMatrixFilename,"r");
	if (pfile==NULL) return FALSE;

	//Zero the matrix (so that unfetched values stay zero):
	{
		int i,j;
		for (i=0;i<18;i++)
		{
			for (j=0;j<18;j++) m_CustMixMatrix[i][j]=0;
		}
	}
	char linebuffer[1024];
	int linecounter=0;
	int numbercounter;

	while (linecounter<18 && fgets(linebuffer,1024,pfile))
	{
		numbercounter=0;
		char *token=strtok(linebuffer,"\t \r\n");
		while (token!=NULL && numbercounter<18)
		{
			m_CustMixMatrix[linecounter][numbercounter]=atof(token);

			numbercounter++;
			token=strtok(NULL,"\t \r\n");
		}

		linecounter++;
	}

	fclose(pfile);

	//Debug:
	/*
	char msg[2048];
	sprintf(msg,"first 6x6 matrix follows:\n\
		%f\t%f\t%f\t%f\t%f\t%f\t\n\
		%f\t%f\t%f\t%f\t%f\t%f\t\n\
		%f\t%f\t%f\t%f\t%f\t%f\t\n\
		%f\t%f\t%f\t%f\t%f\t%f\t\n\
		%f\t%f\t%f\t%f\t%f\t%f\t\n\
		%f\t%f\t%f\t%f\t%f\t%f\t\n  ",
		m_CustMixMatrix[0][0],m_CustMixMatrix[0][1],m_CustMixMatrix[0][2],m_CustMixMatrix[0][3],m_CustMixMatrix[0][4],m_CustMixMatrix[0][5],
		m_CustMixMatrix[1][0],m_CustMixMatrix[1][1],m_CustMixMatrix[1][2],m_CustMixMatrix[1][3],m_CustMixMatrix[1][4],m_CustMixMatrix[1][5],
		m_CustMixMatrix[2][0],m_CustMixMatrix[2][1],m_CustMixMatrix[2][2],m_CustMixMatrix[2][3],m_CustMixMatrix[2][4],m_CustMixMatrix[2][5],
		m_CustMixMatrix[3][0],m_CustMixMatrix[3][1],m_CustMixMatrix[3][2],m_CustMixMatrix[3][3],m_CustMixMatrix[3][4],m_CustMixMatrix[3][5],
		m_CustMixMatrix[4][0],m_CustMixMatrix[4][1],m_CustMixMatrix[4][2],m_CustMixMatrix[4][3],m_CustMixMatrix[4][4],m_CustMixMatrix[4][5],
		m_CustMixMatrix[5][0],m_CustMixMatrix[5][1],m_CustMixMatrix[5][2],m_CustMixMatrix[5][3],m_CustMixMatrix[5][4],m_CustMixMatrix[5][5]
		);
	MessageBox(0,msg,0,0);
	*/

	return TRUE;
}

HRESULT CStereoDownmixer::JoinFilterGraph(IFilterGraph *pGraph,LPCWSTR pName)
{
	////Is it in Single Instance mode?
	//if (m_StereoDownmixerParams.p1.bRaiseMeritAndSingleInstance && sm_FilterInstancesCounter>1)
	//{
	//	return E_FAIL;
	//}

	return CTransformFilter::JoinFilterGraph(pGraph,pName);
}

///////////////////////////////////////////////////////////////////////
// NonDelegatingQueryInterface:
//   Reveals IStereoDownmixer, ISpecifyPropertyPages, IPersistStream
///////////////////////////////////////////////////////////////////////
STDMETHODIMP CStereoDownmixer::NonDelegatingQueryInterface(REFIID riid, void **ppv)
{
    CheckPointer(ppv,E_POINTER);

    if (riid == IID_IStereoDownmixer) {
        return GetInterface((IStereoDownmixer *) this, ppv);
    } else if (riid == IID_IPersistStream) {
        return GetInterface((IPersistStream *) this, ppv);
    } else if (riid == IID_ISpecifyPropertyPages) {
        return GetInterface((ISpecifyPropertyPages *) this, ppv);
    } else {
        return CTransformFilter::NonDelegatingQueryInterface(riid, ppv);
    }
}

///////////////////////////////////////////////////////////////////////
// GetParams: Get the filter parameters to a given destination
///////////////////////////////////////////////////////////////////////
STDMETHODIMP CStereoDownmixer::GetParams(StereoDownmixerParams *irp, StereoDownmixerParams::SectionChoose WhichParams)
{
	if (WhichParams == StereoDownmixerParams::SectionChoose::general) irp->p1=m_StereoDownmixerParams.p1;
	else if (WhichParams == StereoDownmixerParams::SectionChoose::gains) 
	{
		//DEBUG:
		//char str[1000];
		//sprintf(str,"Inside GetParams-gains - FL input =%d  , FL Input Enable =%d",m_StereoDownmixerParams.p2.I.FL,m_StereoDownmixerParams.p2.InputEnableBitmask & SPEAKER_FRONT_LEFT);
		//MessageBox(0,str,0,0);

		irp->p2=m_StereoDownmixerParams.p2;
	}
	else if (WhichParams == StereoDownmixerParams::SectionChoose::mixmatrix) irp->p3=m_StereoDownmixerParams.p3;

    return NOERROR;
}

///////////////////////////////////////////////////////////////////////
// SetParams: Set the filter parameters
///////////////////////////////////////////////////////////////////////
STDMETHODIMP CStereoDownmixer::SetParams(StereoDownmixerParams *irp, StereoDownmixerParams::SectionChoose WhichParams)
{
	if (WhichParams == StereoDownmixerParams::SectionChoose::general)
	{
		//Handle the Raise Merit And Single Instance separately:
		//Has it changed?
		if (m_StereoDownmixerParams.p1.bRaiseMeritAndSingleInstance!=irp->p1.bRaiseMeritAndSingleInstance)
		{
			//Reregister the filter
			g_Templates[0].m_pAMovieSetup_Filter = (irp->p1.bRaiseMeritAndSingleInstance == TRUE)
				? &sudStereoDownmixer_HighPriority
				: &sudStereoDownmixer_NormalPriority
				;
			AMovieDllRegisterServer2(TRUE);
		}

		//Copy all the params (ALSO the Raise Merit And Single Instance)
		m_StereoDownmixerParams.p1 = irp->p1;
	}
	else if (WhichParams == StereoDownmixerParams::SectionChoose::gains) 
	{
		m_StereoDownmixerParams.p2=irp->p2;
	}
	else if (WhichParams == StereoDownmixerParams::SectionChoose::mixmatrix)
	{
		if (TRUE==LoadCustomMixMatrix(irp->p3)) m_StereoDownmixerParams.p3=irp->p3;
		else return E_FAIL;
	}


    SetDirty(TRUE);

    // Save the config whenever the use changes it
    saveConfig();

    return NOERROR;
}

STDMETHODIMP CStereoDownmixer::IsInputPinConnected()
{
	if (m_pInput->IsConnected()) return S_OK;
	else return S_FALSE;
}

STDMETHODIMP CStereoDownmixer::IsOutputPinConnected()
{
	if (m_pOutput->IsConnected()) return S_OK;
	else return S_FALSE;
}

STDMETHODIMP CStereoDownmixer::CanThisFormatBeProvidedFromUpstream(StereoDownmixerParams *irp)
{
	//Debug:
	//int counter=0;

	BOOL bFound=FALSE;
	AM_MEDIA_TYPE *pMediaType;

	IPin *pUpstream=NULL;
	m_pInput->ConnectedTo(&pUpstream);
	if (pUpstream==NULL) return VFW_E_NOT_CONNECTED;

	IEnumMediaTypes *enum_mt=NULL;
	pUpstream->EnumMediaTypes(&enum_mt);
	if (enum_mt==NULL) {pUpstream->Release(); return E_FAIL;}

	while (S_OK==enum_mt->Next(1,&pMediaType,NULL))
	{
		//Debug:
		//counter++;

		if (IsThisInputMediaTypeAcceptableAndMatchesParams(*pMediaType,*irp)) bFound=TRUE;

		DeleteMediaType(pMediaType);
	}
	
	enum_mt->Release();
	pUpstream->Release();

	//Debug:
	//char str[400];
	//sprintf(str,"CStereoDownmixer::CanThisFormatBeProvidedFromUpstream: Found %d media types.\nCorrect one found? %s",counter,bFound ? "Yes":"No");
	//MessageBox(0,str,0,0);

	if (bFound==TRUE) return S_OK;
	else return S_FALSE;
}

STDMETHODIMP CStereoDownmixer::IsThisFormatAcceptedDownstream(StereoDownmixerParams *irp)
{
	if (!m_pOutput->IsConnected()) return S_OK;
	
	CMediaType &proposed=MakeOutputCMediaTypeFromParams(*irp); //As per C++ specs, the life of the temporary return object will be extended to the end of the function

	IPin *pDownstreamPin=NULL;
	m_pOutput->ConnectedTo(&pDownstreamPin);
	if (pDownstreamPin==NULL) return S_OK; //Shouldn't happen, but...
	HRESULT hres=pDownstreamPin->QueryAccept(&proposed);
	pDownstreamPin->Release();
	return hres;
}

STDMETHODIMP CStereoDownmixer::GetCurrentInputMediaType(CMediaType &mt)
{
	return m_pInput->ConnectionMediaType(&mt);
}

STDMETHODIMP CStereoDownmixer::GetCurrentOutputMediaType(CMediaType &mt)
{
	return m_pOutput->ConnectionMediaType(&mt);
}

///////////////////////////////////////////////////////////////////////
// GetClassID: This is the only method of IPersist
///////////////////////////////////////////////////////////////////////
STDMETHODIMP CStereoDownmixer::GetClassID(CLSID *pClsid)
{
    return CBaseFilter::GetClassID(pClsid);
}


#define WRITEOUT(var)  hr = pStream->Write(&var, sizeof(var), NULL); \
               if (FAILED(hr)) return hr;

#define READIN(var)    hr = pStream->Read(&var, sizeof(var), NULL); \
               if (FAILED(hr)) return hr;

///////////////////////////////////////////////////////////////////////
// WriteToStream: called when save the filter to stream
///////////////////////////////////////////////////////////////////////
HRESULT CStereoDownmixer::WriteToStream(IStream *pStream)
{
    HRESULT hr;

	//Bookmark: optionscodehere write

	//--- General:

	//Output format
	WRITEOUT(m_StereoDownmixerParams.p1.bIsMultichannel);
    WRITEOUT(m_StereoDownmixerParams.p1.wFormatTag);
	WRITEOUT(m_StereoDownmixerParams.p1.nChannels);
	WRITEOUT(m_StereoDownmixerParams.p1.dwChannelMask);
	WRITEOUT(m_StereoDownmixerParams.p1.wBitsPerSample);

	//Input requirements
	WRITEOUT(m_StereoDownmixerParams.p1.RequiredInputBitDepth);

	WRITEOUT(m_StereoDownmixerParams.p1.bRequireInputNumberOfChannels);
	WRITEOUT(m_StereoDownmixerParams.p1.RequiredInputNumberOfChannels);

	WRITEOUT(m_StereoDownmixerParams.p1.bRequireInputSamplerate);
	WRITEOUT(m_StereoDownmixerParams.p1.RequiredInputSamplerate);

	//We don't save the Raise Merit and single instance. 
	//Only on registry that one


	//--- Gains:
	WRITEOUT(m_StereoDownmixerParams.p2.InputEnableBitmask);
	WRITEOUT(m_StereoDownmixerParams.p2.OutputEnableBitmask);

	WRITEOUT(m_StereoDownmixerParams.p2.bEnableInputGains);
	WRITEOUT(m_StereoDownmixerParams.p2.bEnableOutputGains);
	WRITEOUT(m_StereoDownmixerParams.p2.bEnableMasterVolume);
	WRITEOUT(m_StereoDownmixerParams.p2.MasterVolumeGain);

	WRITEOUT(m_StereoDownmixerParams.p2.I.FL);
	WRITEOUT(m_StereoDownmixerParams.p2.I.FR);
	WRITEOUT(m_StereoDownmixerParams.p2.I.FC);
	WRITEOUT(m_StereoDownmixerParams.p2.I.LF);
	WRITEOUT(m_StereoDownmixerParams.p2.I.BL);
	WRITEOUT(m_StereoDownmixerParams.p2.I.BR);
	WRITEOUT(m_StereoDownmixerParams.p2.I.FLC);
	WRITEOUT(m_StereoDownmixerParams.p2.I.FRC);
	WRITEOUT(m_StereoDownmixerParams.p2.I.BC);
	WRITEOUT(m_StereoDownmixerParams.p2.I.SL);
	WRITEOUT(m_StereoDownmixerParams.p2.I.SR);
	WRITEOUT(m_StereoDownmixerParams.p2.I.TC);
	WRITEOUT(m_StereoDownmixerParams.p2.I.TFL);
	WRITEOUT(m_StereoDownmixerParams.p2.I.TFC);
	WRITEOUT(m_StereoDownmixerParams.p2.I.TFR);
	WRITEOUT(m_StereoDownmixerParams.p2.I.TBL);
	WRITEOUT(m_StereoDownmixerParams.p2.I.TBC);
	WRITEOUT(m_StereoDownmixerParams.p2.I.TBR);

	WRITEOUT(m_StereoDownmixerParams.p2.I.bJoinFLFR);
	WRITEOUT(m_StereoDownmixerParams.p2.I.bJoinBLBR);
	WRITEOUT(m_StereoDownmixerParams.p2.I.bJoinFLCFRC);
	WRITEOUT(m_StereoDownmixerParams.p2.I.bJoinSLSR);
	WRITEOUT(m_StereoDownmixerParams.p2.I.bJoinTFLTFR);
	WRITEOUT(m_StereoDownmixerParams.p2.I.bJoinTBLTBR);

	WRITEOUT(m_StereoDownmixerParams.p2.O.FL);
	WRITEOUT(m_StereoDownmixerParams.p2.O.FR);
	WRITEOUT(m_StereoDownmixerParams.p2.O.FC);
	WRITEOUT(m_StereoDownmixerParams.p2.O.LF);
	WRITEOUT(m_StereoDownmixerParams.p2.O.BL);
	WRITEOUT(m_StereoDownmixerParams.p2.O.BR);
	WRITEOUT(m_StereoDownmixerParams.p2.O.FLC);
	WRITEOUT(m_StereoDownmixerParams.p2.O.FRC);
	WRITEOUT(m_StereoDownmixerParams.p2.O.BC);
	WRITEOUT(m_StereoDownmixerParams.p2.O.SL);
	WRITEOUT(m_StereoDownmixerParams.p2.O.SR);
	WRITEOUT(m_StereoDownmixerParams.p2.O.TC);
	WRITEOUT(m_StereoDownmixerParams.p2.O.TFL);
	WRITEOUT(m_StereoDownmixerParams.p2.O.TFC);
	WRITEOUT(m_StereoDownmixerParams.p2.O.TFR);
	WRITEOUT(m_StereoDownmixerParams.p2.O.TBL);
	WRITEOUT(m_StereoDownmixerParams.p2.O.TBC);
	WRITEOUT(m_StereoDownmixerParams.p2.O.TBR);

	WRITEOUT(m_StereoDownmixerParams.p2.O.bJoinFLFR);
	WRITEOUT(m_StereoDownmixerParams.p2.O.bJoinBLBR);
	WRITEOUT(m_StereoDownmixerParams.p2.O.bJoinFLCFRC);
	WRITEOUT(m_StereoDownmixerParams.p2.O.bJoinSLSR);
	WRITEOUT(m_StereoDownmixerParams.p2.O.bJoinTFLTFR);
	WRITEOUT(m_StereoDownmixerParams.p2.O.bJoinTBLTBR);

	//--- Mix matrix
	WRITEOUT(m_StereoDownmixerParams.p3.bCustomMixMatrix);
	pStream->Write(m_StereoDownmixerParams.p3.CustomMixMatrixFilename, MAX_PATH, NULL);
	
    return NOERROR;
}

///////////////////////////////////////////////////////////////////////
// ReadFromStream: called when read from stream
///////////////////////////////////////////////////////////////////////
HRESULT CStereoDownmixer::ReadFromStream(IStream *pStream)
{
    HRESULT hr;

	//Bookmark: optionscodehere read

	//--- General:

	//Output format
	READIN(m_StereoDownmixerParams.p1.bIsMultichannel);
    READIN(m_StereoDownmixerParams.p1.wFormatTag);
	READIN(m_StereoDownmixerParams.p1.nChannels);
	READIN(m_StereoDownmixerParams.p1.dwChannelMask);
	READIN(m_StereoDownmixerParams.p1.wBitsPerSample);

	//Input requirements
	READIN(m_StereoDownmixerParams.p1.RequiredInputBitDepth);

	READIN(m_StereoDownmixerParams.p1.bRequireInputNumberOfChannels);
	READIN(m_StereoDownmixerParams.p1.RequiredInputNumberOfChannels);

	READIN(m_StereoDownmixerParams.p1.bRequireInputSamplerate);
	READIN(m_StereoDownmixerParams.p1.RequiredInputSamplerate);

	//We read the Raise Merit And Single Instance, but from the registry!
	m_StereoDownmixerParams.p1.bRaiseMeritAndSingleInstance = GetProfileInt("ChannelDownmixer", "bRaiseMeritAndSingleInstance", FALSE); //Default is false. The reason is explained in the comments of the funcion CStereoDownmixer::readConfig()

	//--- Gains:
	READIN(m_StereoDownmixerParams.p2.InputEnableBitmask);
	READIN(m_StereoDownmixerParams.p2.OutputEnableBitmask);

	READIN(m_StereoDownmixerParams.p2.bEnableInputGains);
	READIN(m_StereoDownmixerParams.p2.bEnableOutputGains);
	READIN(m_StereoDownmixerParams.p2.bEnableMasterVolume);
	READIN(m_StereoDownmixerParams.p2.MasterVolumeGain);

	READIN(m_StereoDownmixerParams.p2.I.FL);
	READIN(m_StereoDownmixerParams.p2.I.FR);
	READIN(m_StereoDownmixerParams.p2.I.FC);
	READIN(m_StereoDownmixerParams.p2.I.LF);
	READIN(m_StereoDownmixerParams.p2.I.BL);
	READIN(m_StereoDownmixerParams.p2.I.BR);
	READIN(m_StereoDownmixerParams.p2.I.FLC);
	READIN(m_StereoDownmixerParams.p2.I.FRC);
	READIN(m_StereoDownmixerParams.p2.I.BC);
	READIN(m_StereoDownmixerParams.p2.I.SL);
	READIN(m_StereoDownmixerParams.p2.I.SR);
	READIN(m_StereoDownmixerParams.p2.I.TC);
	READIN(m_StereoDownmixerParams.p2.I.TFL);
	READIN(m_StereoDownmixerParams.p2.I.TFC);
	READIN(m_StereoDownmixerParams.p2.I.TFR);
	READIN(m_StereoDownmixerParams.p2.I.TBL);
	READIN(m_StereoDownmixerParams.p2.I.TBC);
	READIN(m_StereoDownmixerParams.p2.I.TBR);


	READIN(m_StereoDownmixerParams.p2.I.bJoinFLFR);
	READIN(m_StereoDownmixerParams.p2.I.bJoinBLBR);
	READIN(m_StereoDownmixerParams.p2.I.bJoinFLCFRC);
	READIN(m_StereoDownmixerParams.p2.I.bJoinSLSR);
	READIN(m_StereoDownmixerParams.p2.I.bJoinTFLTFR);
	READIN(m_StereoDownmixerParams.p2.I.bJoinTBLTBR);

	READIN(m_StereoDownmixerParams.p2.O.FL );
	READIN(m_StereoDownmixerParams.p2.O.FR );
	READIN(m_StereoDownmixerParams.p2.O.FC );
	READIN(m_StereoDownmixerParams.p2.O.LF );
	READIN(m_StereoDownmixerParams.p2.O.BL );
	READIN(m_StereoDownmixerParams.p2.O.BR );
	READIN(m_StereoDownmixerParams.p2.O.FLC);
	READIN(m_StereoDownmixerParams.p2.O.FRC);
	READIN(m_StereoDownmixerParams.p2.O.BC );
	READIN(m_StereoDownmixerParams.p2.O.SL );
	READIN(m_StereoDownmixerParams.p2.O.SR );
	READIN(m_StereoDownmixerParams.p2.O.TC );
	READIN(m_StereoDownmixerParams.p2.O.TFL);
	READIN(m_StereoDownmixerParams.p2.O.TFC);
	READIN(m_StereoDownmixerParams.p2.O.TFR);
	READIN(m_StereoDownmixerParams.p2.O.TBL);
	READIN(m_StereoDownmixerParams.p2.O.TBC);
	READIN(m_StereoDownmixerParams.p2.O.TBR);

	READIN(m_StereoDownmixerParams.p2.O.bJoinFLFR);
	READIN(m_StereoDownmixerParams.p2.O.bJoinBLBR);
	READIN(m_StereoDownmixerParams.p2.O.bJoinFLCFRC);
	READIN(m_StereoDownmixerParams.p2.O.bJoinSLSR);
	READIN(m_StereoDownmixerParams.p2.O.bJoinTFLTFR);
	READIN(m_StereoDownmixerParams.p2.O.bJoinTBLTBR);

	//--- Mix matrix
	READIN(m_StereoDownmixerParams.p3.bCustomMixMatrix);
	pStream->Read(m_StereoDownmixerParams.p3.CustomMixMatrixFilename, MAX_PATH, NULL);
	LoadCustomMixMatrix(m_StereoDownmixerParams.p3);

    return NOERROR;
}

///////////////////////////////////////////////////////////////////////
// GetPages: Returns the clsid's of the property pages we support
///////////////////////////////////////////////////////////////////////
STDMETHODIMP CStereoDownmixer::GetPages(CAUUID *pPages)
{
    pPages->cElems = 3;
    pPages->pElems = (GUID *) CoTaskMemAlloc(3*sizeof(GUID));
    if (pPages->pElems == NULL) {
        return E_OUTOFMEMORY;
    }
    pPages->pElems[0] = CLSID_StereoDownmixerPropP1;
	pPages->pElems[1] = CLSID_StereoDownmixerPropP2;
	pPages->pElems[2] = CLSID_StereoDownmixerPropP3;
    return NOERROR;
}

///////////////////////////////////////////////////////////////////////
// DllRegisterServer
///////////////////////////////////////////////////////////////////////
STDAPI DllRegisterServer()
{
	//Bookmark: optionscodehere write

	//Clear the settings to default 16 bit stereo!
	
	//Reregistering this filter is the only way a Windows Media Player user
	//has to revert settings he/she has saved to registry (or win.ini file)
	//and are unacceptable to the audio renderer.

	char sz[STR_MAX_LENGTH];

	//Write parameters into registry (or win.ini file)

	//Cheat with the local variable name so that we can use the same macro as above!
	StereoDownmixerParams m_StereoDownmixerParams;

	//--- General

	//Output format
	m_StereoDownmixerParams.p1.bIsMultichannel=FALSE;
	m_StereoDownmixerParams.p1.wFormatTag=WAVE_FORMAT_PCM;
	m_StereoDownmixerParams.p1.nChannels=2;
	m_StereoDownmixerParams.p1.dwChannelMask=KSAUDIO_SPEAKER_5POINT1;
	m_StereoDownmixerParams.p1.wBitsPerSample=16;

	//Input requirements
	m_StereoDownmixerParams.p1.RequiredInputBitDepth=0;

	m_StereoDownmixerParams.p1.bRequireInputNumberOfChannels=FALSE;
	m_StereoDownmixerParams.p1.RequiredInputNumberOfChannels=6;

	m_StereoDownmixerParams.p1.bRequireInputSamplerate=FALSE;
	m_StereoDownmixerParams.p1.RequiredInputSamplerate=48000;

	//Raise Merit and single instance
	m_StereoDownmixerParams.p1.bRaiseMeritAndSingleInstance=TRUE;

	//--- Gains
	m_StereoDownmixerParams.p2.InputEnableBitmask=0xffffffff;
	m_StereoDownmixerParams.p2.OutputEnableBitmask=0xffffffff;

	m_StereoDownmixerParams.p2.bEnableInputGains=FALSE;
	m_StereoDownmixerParams.p2.bEnableOutputGains=FALSE;
	m_StereoDownmixerParams.p2.bEnableMasterVolume=FALSE;
	m_StereoDownmixerParams.p2.MasterVolumeGain=100;
	
	m_StereoDownmixerParams.p2.I.FL = 
		m_StereoDownmixerParams.p2.I.FR = 
		m_StereoDownmixerParams.p2.I.FC = 
		m_StereoDownmixerParams.p2.I.LF = 
		m_StereoDownmixerParams.p2.I.BL = 
		m_StereoDownmixerParams.p2.I.BR = 
		m_StereoDownmixerParams.p2.I.FLC = 
		m_StereoDownmixerParams.p2.I.FRC = 
		m_StereoDownmixerParams.p2.I.BC = 
		m_StereoDownmixerParams.p2.I.SL = 
		m_StereoDownmixerParams.p2.I.SR = 
		m_StereoDownmixerParams.p2.I.TC = 
		m_StereoDownmixerParams.p2.I.TFL = 
		m_StereoDownmixerParams.p2.I.TFC =
		m_StereoDownmixerParams.p2.I.TFR = 
		m_StereoDownmixerParams.p2.I.TBL = 
		m_StereoDownmixerParams.p2.I.TBC = 
		m_StereoDownmixerParams.p2.I.TBR = 100;

	m_StereoDownmixerParams.p2.I.bJoinFLFR=TRUE;
	m_StereoDownmixerParams.p2.I.bJoinBLBR=TRUE;
	m_StereoDownmixerParams.p2.I.bJoinFLCFRC=TRUE;
	m_StereoDownmixerParams.p2.I.bJoinSLSR=TRUE;
	m_StereoDownmixerParams.p2.I.bJoinTFLTFR=TRUE;
	m_StereoDownmixerParams.p2.I.bJoinTBLTBR=TRUE;

	m_StereoDownmixerParams.p2.O.FL = 
		m_StereoDownmixerParams.p2.O.FR = 
		m_StereoDownmixerParams.p2.O.FC = 
		m_StereoDownmixerParams.p2.O.LF = 
		m_StereoDownmixerParams.p2.O.BL = 
		m_StereoDownmixerParams.p2.O.BR = 
		m_StereoDownmixerParams.p2.O.FLC = 
		m_StereoDownmixerParams.p2.O.FRC = 
		m_StereoDownmixerParams.p2.O.BC = 
		m_StereoDownmixerParams.p2.O.SL = 
		m_StereoDownmixerParams.p2.O.SR = 
		m_StereoDownmixerParams.p2.O.TC = 
		m_StereoDownmixerParams.p2.O.TFL = 
		m_StereoDownmixerParams.p2.O.TFC =
		m_StereoDownmixerParams.p2.O.TFR = 
		m_StereoDownmixerParams.p2.O.TBL = 
		m_StereoDownmixerParams.p2.O.TBC = 
		m_StereoDownmixerParams.p2.O.TBR = 100;

	m_StereoDownmixerParams.p2.O.bJoinFLFR=TRUE;
	m_StereoDownmixerParams.p2.O.bJoinBLBR=TRUE;
	m_StereoDownmixerParams.p2.O.bJoinFLCFRC=TRUE;
	m_StereoDownmixerParams.p2.O.bJoinSLSR=TRUE;
	m_StereoDownmixerParams.p2.O.bJoinTFLTFR=TRUE;
	m_StereoDownmixerParams.p2.O.bJoinTBLTBR=TRUE;

	m_StereoDownmixerParams.p3.bCustomMixMatrix=FALSE;
	m_StereoDownmixerParams.p3.CustomMixMatrixFilename[0]='\0';
		



	//--- General

	//Output format
	WRITEPROFINT(p1.bIsMultichannel);
	WRITEPROFINT(p1.wFormatTag);
	WRITEPROFINT(p1.nChannels);
	WRITEPROFINT(p1.dwChannelMask);
	WRITEPROFINT(p1.wBitsPerSample);

	//Input requirements
	WRITEPROFINT(p1.RequiredInputBitDepth);

	WRITEPROFINT(p1.bRequireInputNumberOfChannels);
	WRITEPROFINT(p1.RequiredInputNumberOfChannels);

	WRITEPROFINT(p1.bRequireInputSamplerate);
	WRITEPROFINT(p1.RequiredInputSamplerate);

	//Raise Merit and single instance
	WRITEPROFINT(p1.bRaiseMeritAndSingleInstance);

	//--- Gains:
	WRITEPROFINT(p2.InputEnableBitmask);
	WRITEPROFINT(p2.OutputEnableBitmask);

	WRITEPROFINT(p2.bEnableInputGains);
	WRITEPROFINT(p2.bEnableOutputGains);
	WRITEPROFINT(p2.bEnableMasterVolume);
	WRITEPROFINT(p2.MasterVolumeGain);

	WRITEPROFINT(p2.I.FL);
	WRITEPROFINT(p2.I.FR);
	WRITEPROFINT(p2.I.FC);
	WRITEPROFINT(p2.I.LF);
	WRITEPROFINT(p2.I.BL);
	WRITEPROFINT(p2.I.BR);
	WRITEPROFINT(p2.I.FLC);
	WRITEPROFINT(p2.I.FRC);
	WRITEPROFINT(p2.I.BC);
	WRITEPROFINT(p2.I.SL);
	WRITEPROFINT(p2.I.SR);
	WRITEPROFINT(p2.I.TC);
	WRITEPROFINT(p2.I.TFL);
	WRITEPROFINT(p2.I.TFC);
	WRITEPROFINT(p2.I.TFR);
	WRITEPROFINT(p2.I.TBL);
	WRITEPROFINT(p2.I.TBC);
	WRITEPROFINT(p2.I.TBR);

	WRITEPROFINT(p2.I.bJoinFLFR);
	WRITEPROFINT(p2.I.bJoinBLBR);
	WRITEPROFINT(p2.I.bJoinFLCFRC);
	WRITEPROFINT(p2.I.bJoinSLSR);
	WRITEPROFINT(p2.I.bJoinTFLTFR);
	WRITEPROFINT(p2.I.bJoinTBLTBR);


	WRITEPROFINT(p2.O.FL);
	WRITEPROFINT(p2.O.FR);
	WRITEPROFINT(p2.O.FC);
	WRITEPROFINT(p2.O.LF);
	WRITEPROFINT(p2.O.BL);
	WRITEPROFINT(p2.O.BR);
	WRITEPROFINT(p2.O.FLC);
	WRITEPROFINT(p2.O.FRC);
	WRITEPROFINT(p2.O.BC);
	WRITEPROFINT(p2.O.SL);
	WRITEPROFINT(p2.O.SR);
	WRITEPROFINT(p2.O.TC);
	WRITEPROFINT(p2.O.TFL);
	WRITEPROFINT(p2.O.TFC);
	WRITEPROFINT(p2.O.TFR);
	WRITEPROFINT(p2.O.TBL);
	WRITEPROFINT(p2.O.TBC);
	WRITEPROFINT(p2.O.TBR);

	WRITEPROFINT(p2.O.bJoinFLFR);
	WRITEPROFINT(p2.O.bJoinBLBR);
	WRITEPROFINT(p2.O.bJoinFLCFRC);
	WRITEPROFINT(p2.O.bJoinSLSR);
	WRITEPROFINT(p2.O.bJoinTFLTFR);
	WRITEPROFINT(p2.O.bJoinTBLTBR);

	//--- Mix matrix
	WRITEPROFINT(p3.bCustomMixMatrix);
	WriteProfileStringA("ChannelDownmixer", "CustomMixMatrixFilename", m_StereoDownmixerParams.p3.CustomMixMatrixFilename);
	
	//Change the value for LastRegisteredVersion in registry
	itoa(CURRENT_VERSION_NUMBER,sz,10);
	WriteProfileStringA("ChannelDownmixer","LastRegisteredVersion",sz);

    return AMovieDllRegisterServer2( TRUE );
}

///////////////////////////////////////////////////////////////////////
// DllUnregisterServer
///////////////////////////////////////////////////////////////////////
STDAPI DllUnregisterServer()
{
    return AMovieDllRegisterServer2( FALSE );
}
