#pragma once

#ifndef AUDIOMIXER_I_H
#define AUDIOMIXER_I_H

// {719D1F20-A427-4D63-BA1B-36FBC0E1F85F}
DEFINE_GUID(IID_IAudioMixer, 
0x719d1f20, 0xa427, 0x4d63, 0xba, 0x1b, 0x36, 0xfb, 0xc0, 0xe1, 0xf8, 0x5f);



#ifdef __cplusplus
extern "C"{
#endif

// define IWavMeter interface
MIDL_INTERFACE("719D1F20-A427-4D63-BA1B-36FBC0E1F85F")
IAudioMixer : public IUnknown
{
public:
	// Set audio volume.
	STDMETHOD(SetVolume)(double volume) PURE;
};

#ifdef __cplusplus
}
#endif

#endif// AUDIOMIXER_I_H