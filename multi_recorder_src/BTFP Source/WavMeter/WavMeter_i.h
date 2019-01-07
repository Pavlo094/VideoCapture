#pragma once

#ifndef WAVMETER_H
#define WAVMETER_H

// {FE8C6FB9-E9CE-4BD9-944E-0DFF0654FEC4}
static const GUID CLSID_WavMeter = 
{ 0xfe8c6fb9, 0xe9ce, 0x4bd9, { 0x94, 0x4e, 0xd, 0xff, 0x6, 0x54, 0xfe, 0xc4 } };

// {B8BED859-8ED1-4682-A040-7CDE1FCE9F54}
DEFINE_GUID(IID_IWavMeter, 
0xb8bed859, 0x8ed1, 0x4682, 0xa0, 0x40, 0x7c, 0xde, 0x1f, 0xce, 0x9f, 0x54);

#ifdef __cplusplus
extern "C"{
#endif

// define IWavMeter interface
MIDL_INTERFACE("B8BED859-8ED1-4682-A040-7CDE1FCE9F54")
IWavMeter : public IUnknown
{
public:
	// Get audio peaks. 6 channels max.
	STDMETHOD(GetPeaks)(double Peaks[]) PURE;
	// Set audio volume. 6 channels max. You can set volume for each channel separately.
	STDMETHOD(SetVolume)(double Volumes[]) PURE;
};

#ifdef __cplusplus
}
#endif

#endif// WAVMETER_H