//--------------------------------------------------------------//
//	"vn_sound.h"												//
//		WAVEファイル再生クラス(XAudio2)							//
//													2026/01/01	//
//														Ichii	//
//--------------------------------------------------------------//
#pragma once

class vnSound
{
private:
	IXAudio2SourceVoice* pSourceVoice;
	XAUDIO2_BUFFER xbuf;

	UINT64			lastSamplesPlayed;
	UINT64			lastSamplesStopped;
	WAVEFORMATEX	wfex;
	
public:
	//コンストラクタ
	vnSound(const WCHAR *path);
	//デストラクタ
	~vnSound();
	
	bool play(bool start=true);	//再生	start=true : 先頭から再生, start=false : 前回の停止位置から再生
	bool stop();				//停止
	
	bool isPlaying();	//再生中か調べる
	bool isStopped();	//停止中か調べる
	
	//各種設定
	bool setVolume(float volume);			//減衰 : DSBVOLUME_MAX (減衰なし:0) から DSBVOLUME_MIN (無音:-10,000)
	bool setFrequencyRatio(float frequency);//オーディオ バッファが再生されている周波数(Hz 単位) / デフォルト値 : DSBFREQUENCY_ORIGINAL
	
	//各種取得
	float getVolume();
	float getFrequencyRatio();

	float getPlayingTime();

	float getCurrentPlayingPosition();

	static float convertSecond(UINT64 data, int samplesPerSec, int bitsPerSample, int channels);
};

