//--------------------------------------------------------------//
//	"vn_XAudio2.cpp"											//
//		XAudio2ŠÇ—												//
//													2026/01/01	//
//														Ichii	//
//--------------------------------------------------------------//
#include "../../framework.h"
#include "../vn_environment.h"

//#define vnXAUDIO2_OUTPUT_INFO (1)

IXAudio2* vnXAudio2::pXAudio = NULL;
IXAudio2MasteringVoice* vnXAudio2::pMasteringVoice = NULL;
XAUDIO2_VOICE_DETAILS vnXAudio2::MasterVoiceDetails = {};
DWORD vnXAudio2::vnXAudio2::dwChannelMask = 0;

HRESULT vnXAudio2::initialize()
{
	HRESULT hr;

	hr = CoInitializeEx(0, COINIT_MULTITHREADED);
	assert(hr == S_OK);

	hr = XAudio2Create(&pXAudio);
	assert(hr == S_OK);

	hr = pXAudio->CreateMasteringVoice(&pMasteringVoice);
	assert(hr == S_OK);

	hr = pMasteringVoice->GetChannelMask(&dwChannelMask);

#ifdef vnXAUDIO2_OUTPUT_INFO
	//Speaker Positions:
	vnFont::output(L"Active Speaker Positions\n");
	if (dwChannelMask & SPEAKER_FRONT_LEFT)				vnFont::output(L"\tSPEAKER_FRONT_LEFT\n");
	if (dwChannelMask & SPEAKER_FRONT_RIGHT)			vnFont::output(L"\tSPEAKER_FRONT_RIGHT\n");
	if (dwChannelMask & SPEAKER_FRONT_CENTER)			vnFont::output(L"\tSPEAKER_FRONT_CENTER\n");
	if (dwChannelMask & SPEAKER_LOW_FREQUENCY)			vnFont::output(L"\tSPEAKER_LOW_FREQUENCY\n");
	if (dwChannelMask & SPEAKER_BACK_LEFT)				vnFont::output(L"\tSPEAKER_BACK_LEFT\n");
	if (dwChannelMask & SPEAKER_BACK_RIGHT)				vnFont::output(L"\tSPEAKER_BACK_RIGHT\n");
	if (dwChannelMask & SPEAKER_FRONT_LEFT_OF_CENTER)	vnFont::output(L"\tSPEAKER_FRONT_LEFT_OF_CENTER\n");
	if (dwChannelMask & SPEAKER_FRONT_RIGHT_OF_CENTER)	vnFont::output(L"\tSPEAKER_FRONT_RIGHT_OF_CENTER\n");
	if (dwChannelMask & SPEAKER_BACK_CENTER)			vnFont::output(L"\tSPEAKER_BACK_CENTER\n");
	if (dwChannelMask & SPEAKER_SIDE_LEFT)				vnFont::output(L"\tSPEAKER_SIDE_LEFT\n");
	if (dwChannelMask & SPEAKER_SIDE_RIGHT)				vnFont::output(L"\tSPEAKER_SIDE_RIGHT\n");
	if (dwChannelMask & SPEAKER_TOP_CENTER)				vnFont::output(L"\tSPEAKER_TOP_CENTER\n");
	if (dwChannelMask & SPEAKER_TOP_FRONT_LEFT)			vnFont::output(L"\tSPEAKER_TOP_FRONT_LEFT\n");
	if (dwChannelMask & SPEAKER_TOP_FRONT_CENTER)		vnFont::output(L"\tSPEAKER_TOP_FRONT_CENTER\n");
	if (dwChannelMask & SPEAKER_TOP_FRONT_RIGHT)		vnFont::output(L"\tSPEAKER_TOP_FRONT_RIGHT\n");
	if (dwChannelMask & SPEAKER_TOP_BACK_LEFT)			vnFont::output(L"\tSPEAKER_TOP_BACK_LEFT\n");
	if (dwChannelMask & SPEAKER_TOP_BACK_CENTER)		vnFont::output(L"\tSPEAKER_TOP_BACK_CENTER\n");
	if (dwChannelMask & SPEAKER_TOP_BACK_RIGHT)			vnFont::output(L"\tSPEAKER_TOP_BACK_RIGHT\n");

	//DirectSound Speaker Config
	vnFont::output(L"DirectSound Speaker Config\n");
	switch (dwChannelMask)
	{
	case KSAUDIO_SPEAKER_MONO:		vnFont::output(L"\tKSAUDIO_SPEAKER_MONO\n");	break;
	case KSAUDIO_SPEAKER_1POINT1:	vnFont::output(L"\tKSAUDIO_SPEAKER_1POINT1\n"); break;
	case KSAUDIO_SPEAKER_STEREO:	vnFont::output(L"\tKSAUDIO_SPEAKER_STEREO\n");	break;
	case KSAUDIO_SPEAKER_2POINT1:	vnFont::output(L"\tKSAUDIO_SPEAKER_2POINT1\n"); break;
	case KSAUDIO_SPEAKER_3POINT0:	vnFont::output(L"\tKSAUDIO_SPEAKER_3POINT0\n"); break;
	case KSAUDIO_SPEAKER_3POINT1:	vnFont::output(L"\tKSAUDIO_SPEAKER_3POINT1\n"); break;
	case KSAUDIO_SPEAKER_QUAD:		vnFont::output(L"\tKSAUDIO_SPEAKER_QUAD\n");	break;
	case KSAUDIO_SPEAKER_SURROUND:	vnFont::output(L"\tKSAUDIO_SPEAKER_SURROUND\n");break;
	case KSAUDIO_SPEAKER_5POINT0:	vnFont::output(L"\tKSAUDIO_SPEAKER_5POINT0\n"); break;
	case KSAUDIO_SPEAKER_5POINT1:	vnFont::output(L"\tKSAUDIO_SPEAKER_5POINT1\n"); break;
	case KSAUDIO_SPEAKER_7POINT0:	vnFont::output(L"\tKSAUDIO_SPEAKER_7POINT0\n"); break;
	case KSAUDIO_SPEAKER_7POINT1:	vnFont::output(L"\tKSAUDIO_SPEAKER_7POINT1\n"); break;
	case KSAUDIO_SPEAKER_5POINT1_SURROUND:	vnFont::output(L"\tKSAUDIO_SPEAKER_5POINT1_SURROUND\n"); break;
	case KSAUDIO_SPEAKER_7POINT1_SURROUND:	vnFont::output(L"\tKSAUDIO_SPEAKER_7POINT1_SURROUND\n"); break;
	}
#endif

	pMasteringVoice->GetVoiceDetails(&MasterVoiceDetails);

	return S_OK;
}

void vnXAudio2::terminate()
{
	if (pMasteringVoice)
	{
		pMasteringVoice->DestroyVoice();
		pMasteringVoice = NULL;
	}
	SAFE_RELEASE(pXAudio);

	CoUninitialize();
}

HRESULT vnXAudio2::CreateSourceVoice(IXAudio2SourceVoice** pSource, WAVEFORMATEX* wfex)
{
	return pXAudio->CreateSourceVoice(pSource, wfex);
}
