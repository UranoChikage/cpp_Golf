//--------------------------------------------------------------//
//	"vn_sound.cpp"												//
//		WAVEファイル再生クラス(XAudio2)							//
//													2026/01/01	//
//														Ichii	//
//--------------------------------------------------------------//
#include "../framework.h"
#include "vn_environment.h"

vnSound::vnSound(const WCHAR *path)
{
	HRESULT hr;
    MMCKINFO mSrcWaveFile;
    MMCKINFO mSrcWaveFmt;
    MMCKINFO mSrcWaveData;
    LPWAVEFORMATEX wf;

	pSourceVoice = NULL;

	// WAVファイルをロード
    HMMIO hSrc;
    hSrc = mmioOpen( (LPWSTR)path, NULL,MMIO_ALLOCBUF|MMIO_READ|MMIO_COMPAT );
	if (!hSrc)
	{
		//    DEBUG( "WAVファイルロードエラー\n","" );
		return;
	}

    // 'WAVE'チャンクチェック
    ZeroMemory( &mSrcWaveFile,sizeof(mSrcWaveFile) );
	hr = mmioDescend( hSrc,&mSrcWaveFile,NULL,MMIO_FINDRIFF );
    if( mSrcWaveFile.fccType!=mmioFOURCC('W','A','V','E') ) 
	{
    //    DEBUG( "WAVEチャンクチェックエラー\n","" );
        mmioClose( hSrc,0 );
        return;
    }

    // 'fmt 'チャンクチェック
    ZeroMemory( &mSrcWaveFmt,sizeof(mSrcWaveFmt) );
	hr = mmioDescend( hSrc,&mSrcWaveFmt,&mSrcWaveFile,MMIO_FINDCHUNK );
    if( mSrcWaveFmt.ckid!=mmioFOURCC('f','m','t',' ') )
	{
    //    DEBUG( "fmt チャンクチェックエラー\n","" );
        mmioClose( hSrc,0 );
        return;
    }

    // ヘッダサイズの計算
    int iSrcHeaderSize = mSrcWaveFmt.cksize;
    if( iSrcHeaderSize<sizeof(WAVEFORMATEX) )
        iSrcHeaderSize=sizeof(WAVEFORMATEX);

    // ヘッダメモリ確保
    wf = (LPWAVEFORMATEX)new BYTE[iSrcHeaderSize];
    if( !wf )
	{
    //    DEBUG( "メモリ確保エラー\n","" );
        mmioClose( hSrc,0 );
        return;
    }
    ZeroMemory( wf,iSrcHeaderSize );

    // WAVEフォーマットのロード
	hr = mmioRead(hSrc, (char*)wf, mSrcWaveFmt.cksize);
    if( FAILED(hr) )
	{
     //   DEBUG( "WAVEフォーマットロードエラー\n","" );
        delete[] wf;
        mmioClose( hSrc,0 );
        return;
    }


    // fmtチャンクに戻る
    mmioAscend( hSrc,&mSrcWaveFmt,0 );

    // dataチャンクを探す
    while(1)
	{
        // 検索
		hr = mmioDescend( hSrc,&mSrcWaveData,&mSrcWaveFile,0 );
        if( FAILED(hr) )
		{
         //   DEBUG( "dataチャンクが見つからない\n","" );
            delete[] wf;
            mmioClose( hSrc,0 );
            return;
        }
        if( mSrcWaveData.ckid==mmioStringToFOURCC(L"data",0) )
            break;
        // 次のチャンクへ
		hr = mmioAscend( hSrc,&mSrcWaveData,0 );
    }

	vnFont::output(L"%s\n", path);
	vnFont::output(L"チャンネル数       = %d\n", wf->nChannels);
	vnFont::output(L"サンプリングレート = %d\n", wf->nSamplesPerSec);
	vnFont::output(L"ビットレート       = %d\n", wf->wBitsPerSample);
	vnFont::output(L"データサイズ       = %d\n", mSrcWaveData.cksize);

	const BYTE* buffer = new BYTE[mSrcWaveData.cksize];
	mmioRead(hSrc, (char*)buffer, mSrcWaveData.cksize);

	//ソースボイスの作成
	hr = vnXAudio2::CreateSourceVoice(&pSourceVoice, wf);
	assert(hr == S_OK);

	//再生する波形データの設定
	xbuf.pAudioData = (const BYTE*)buffer;
	xbuf.Flags = XAUDIO2_END_OF_STREAM;
	xbuf.AudioBytes = mSrcWaveData.cksize;

	hr = pSourceVoice->SubmitSourceBuffer(&xbuf);
	assert(hr == S_OK);

	lastSamplesPlayed = 0;
	lastSamplesStopped = 0;
	memcpy(&wfex, wf, sizeof(WAVEFORMATEX));

	// ヘッダ用メモリを開放
	delete[] wf;

	// WAVを閉じる
	mmioClose(hSrc, 0);

}

vnSound::~vnSound()
{
	pSourceVoice->DestroyVoice();
	if(xbuf.pAudioData)delete[] xbuf.pAudioData;
}

bool vnSound::play(bool start)
{
	if(pSourceVoice)
	{
		HRESULT hr;
		if (start)
		{
			hr = pSourceVoice->Stop(0, 0);
			//pSourceVoice->ExitLoop();
			hr = pSourceVoice->Discontinuity();
			hr = pSourceVoice->FlushSourceBuffers();
			hr = pSourceVoice->SubmitSourceBuffer(&xbuf);
		}
		XAUDIO2_VOICE_STATE stat;
		pSourceVoice->GetState(&stat);
		lastSamplesPlayed = stat.SamplesPlayed;

		hr = pSourceVoice->Start(0, 0);
		return hr == S_OK;
	}
	return false;
}

bool vnSound::stop()
{
	if (pSourceVoice)
	{
		HRESULT hr = pSourceVoice->Stop(0, 0);

		XAUDIO2_VOICE_STATE stat;
		pSourceVoice->GetState(&stat);
		lastSamplesStopped = stat.SamplesPlayed;

		return hr == S_OK;
	}
	return false;
}

bool vnSound::isPlaying(void)
{
	if (pSourceVoice)
	{
		XAUDIO2_VOICE_STATE stat;
		pSourceVoice->GetState(&stat);
		return stat.SamplesPlayed > 0 && stat.SamplesPlayed != lastSamplesStopped;
	}
	return false;
}

bool vnSound::isStopped()
{
	return !isPlaying();
}

bool vnSound::setVolume(float volume)
{
	if (pSourceVoice)
	{
		if (volume > XAUDIO2_MAX_VOLUME_LEVEL)
		{
			volume = XAUDIO2_MAX_VOLUME_LEVEL;
		}
		else if (volume < 0.0f/* -XAUDIO2_MAX_VOLUME_LEVEL*/)
		{
			volume = 0.0f/* -XAUDIO2_MAX_VOLUME_LEVEL*/;
		}
		HRESULT hr = pSourceVoice->SetVolume(volume);
		return hr == S_OK;
	}
	return false;
}

bool vnSound::setFrequencyRatio(float frequency)
{
	if (pSourceVoice)
	{
		HRESULT hr = pSourceVoice->SetFrequencyRatio(frequency);
		return hr == S_OK;
	}
	return false;
}

float vnSound::getVolume()
{
	float ret = 0.0f;
	if (pSourceVoice)
	{
		pSourceVoice->GetVolume(&ret);
	}
	return ret;
}

float vnSound::getFrequencyRatio()
{
	if (pSourceVoice)
	{
		float ret = 0.0f;
		pSourceVoice->GetFrequencyRatio(&ret);
		return ret;

	}
	return 0.0f;
}

float vnSound::getPlayingTime()
{
	if (pSourceVoice)
	{
		return convertSecond(xbuf.AudioBytes, wfex.nSamplesPerSec, wfex.wBitsPerSample, wfex.nChannels);
	}
	return 0.0f;
}

float vnSound::getCurrentPlayingPosition()
{
	if (pSourceVoice)
	{
		XAUDIO2_VOICE_STATE stat;
		pSourceVoice->GetState(&stat);
		UINT64 data = stat.SamplesPlayed >= lastSamplesPlayed ? (stat.SamplesPlayed - lastSamplesPlayed) : 0;
		return (float)data / (float)(wfex.nSamplesPerSec);
	}
	return 0.0f;
}



float vnSound::convertSecond(UINT64 data, int samplesPerSec, int bitsPerSample, int channels)
{
	return (float)data / (float)(samplesPerSec * (bitsPerSample / 8) * channels);
}
