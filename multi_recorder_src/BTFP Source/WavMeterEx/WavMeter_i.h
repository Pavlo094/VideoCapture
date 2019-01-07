#pragma once

#ifndef WAVMETER_H
#define WAVMETER_H

// {BCA04069-E36C-40E4-9CD2-D08E7528F743}
static const GUID CLSID_WavMeterEx = 
{ 0xbca04069, 0xe36c, 0x40e4, { 0x9c, 0xd2, 0xd0, 0x8e, 0x75, 0x28, 0xf7, 0x43 } };

// {EF67001A-EA9B-4305-8F81-6D8A800C417E}
DEFINE_GUID(IID_IWavMeterEx, 
0xef67001a, 0xea9b, 0x4305, 0x8f, 0x81, 0x6d, 0x8a, 0x80, 0xc, 0x41, 0x7e);


#ifdef __cplusplus
extern "C"{
#endif

// define IWavMeter interface
MIDL_INTERFACE("EF67001A-EA9B-4305-8F81-6D8A800C417E")
IWavMeterEx : public IUnknown
{
public:
	// Get audio peaks. 6 channels max.
	STDMETHOD(GetPeaks)(double Peaks[]) PURE;
	// Set audio volume. 6 channels max. You can set volume for each channel separately.
	STDMETHOD(SetVolume)(double Volumes[]) PURE;
	// Set mute. 6 channels max. You can set mute for each channel separately.
	STDMETHOD(SetMute)(BOOL Mutes[]) PURE;
};

#ifdef __cplusplus
}
#endif

#endif// WAVMETER_H