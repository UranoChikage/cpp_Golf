//--------------------------------------------------------------//
//	"vn_XAudio2.h"												//
//		XAudio2ä«óù												//
//													2026/01/01	//
//														Ichii	//
//--------------------------------------------------------------//
#pragma once

#include <ks.h>
#include <ksmedia.h>

class vnXAudio2
{
private:
	static IXAudio2* pXAudio;
	static IXAudio2MasteringVoice* pMasteringVoice;
	static XAUDIO2_VOICE_DETAILS MasterVoiceDetails;
	static DWORD dwChannelMask;

public:
	//[ÉVÉXÉeÉÄä÷êî
	static HRESULT initialize();
	static void terminate();
	//]

	static HRESULT CreateSourceVoice(IXAudio2SourceVoice **pSource, WAVEFORMATEX *wfex);

	static const XAUDIO2_VOICE_DETAILS* const masterVoiceDetails() { return &MasterVoiceDetails; }

	static DWORD channelMask() { return dwChannelMask; }
};