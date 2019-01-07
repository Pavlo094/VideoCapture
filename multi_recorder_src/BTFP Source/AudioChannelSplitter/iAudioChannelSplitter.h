#pragma once

// {227FFEC6-068F-4817-BDB2-51D27F7A555F}
DEFINE_GUID(CLSID_AudioChannelSplitter, 
0x227ffec6, 0x68f, 0x4817, 0xbd, 0xb2, 0x51, 0xd2, 0x7f, 0x7a, 0x55, 0x5f);



#define MAX_CHANNELS	6

struct AudioChannelSplitterParameters {
	int MixMatrix[MAX_CHANNELS][MAX_CHANNELS];
};

#ifdef __cplusplus
extern "C" {
#endif
	
	// {F60A301D-F9EC-4692-BBCE-428A6EA88A45}
	DEFINE_GUID(IID_IAudioChannelSplitter, 
		0xf60a301d, 0xf9ec, 0x4692, 0xbb, 0xce, 0x42, 0x8a, 0x6e, 0xa8, 0x8a, 0x45);	
	DECLARE_INTERFACE_(IAudioChannelSplitter, IUnknown)
	{
		STDMETHOD(get_Params) (THIS_
			AudioChannelSplitterParameters *irp
			) PURE;

		STDMETHOD(put_Params) (THIS_
			AudioChannelSplitterParameters irp
			) PURE;
	};

#ifdef __cplusplus
}
#endif