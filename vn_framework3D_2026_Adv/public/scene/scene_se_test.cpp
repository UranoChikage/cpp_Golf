//--------------------------------------------------------------//
//	"scene_se_test.h"											//
//		SEテスト												//
//													2026/01/01	//
//														Ichii	//
//--------------------------------------------------------------//
#include "../../framework.h"
#include "../../framework/vn_environment.h"

#define FILE_PATH_MAX	(256)

WCHAR seFile[][FILE_PATH_MAX] = 
{
	L"data/sound/decision1.wav",
	L"data/sound/decision2.wav",
	L"data/sound/decision3.wav",
	L"data/sound/decision7.wav",
	L"data/sound/decision17.wav",
	L"data/sound/decision21.wav",
	L"data/sound/decision22.wav",
	L"data/sound/decision23.wav",
	L"data/sound/cursor1.wav",
	L"data/sound/cursor2.wav",
	L"data/sound/cursor3.wav",
	L"data/sound/cursor4.wav",
	L"data/sound/cursor10.wav",
	L"data/sound/cancel1.wav",
};

//初期化関数
bool SceneSeTest::initialize()
{
	fileNum = sizeof(seFile)/(sizeof(WCHAR)*FILE_PATH_MAX);
	
	pSound = new vnSound*[fileNum];
	for(int i=0; i<fileNum; i++)
	{
		pSound[i] = new vnSound(seFile[i]);
	}
	return true;
}

//終了関数
void SceneSeTest::terminate()
{
	if (pSound != NULL)
	{
		for (int i = 0; i < fileNum; i++)
		{
			if (pSound[i] == NULL)continue;
			delete pSound[i];
			pSound[i] = NULL;
		}
		delete[] pSound;
		pSound = NULL;
	}
}

//処理関数
void SceneSeTest::execute()
{
	if(vnMouse::trg())
	{
		int i = (vnMouse::getY() - 100) / 32;
		if(i>=0 && i<fileNum)
		{
			if(vnMouse::getX()>400 && vnMouse::getX()<450)
			{
				if(pSound[i]->isPlaying()==false)
				{
					pSound[i]->play();
				}
				else
				{
					pSound[i]->stop();
				}
			}
			else if (vnMouse::getX() > 450 && vnMouse::getX() < 500)
			{
				pSound[i]->stop();
			}
			else if(vnMouse::getX()>660 && vnMouse::getX()<680)
			{
				pSound[i]->setVolume(pSound[i]->getVolume()-0.1f);
			}
			else if(vnMouse::getX()>800 && vnMouse::getX()<820)
			{
				pSound[i]->setVolume(pSound[i]->getVolume()+0.1f);
			}
			else if (vnMouse::getX() > 960 && vnMouse::getX() < 980)
			{
				pSound[i]->setFrequencyRatio(pSound[i]->getFrequencyRatio() - 0.1f);
			}
			else if (vnMouse::getX() > 1100 && vnMouse::getX() < 1120)
			{
				pSound[i]->setFrequencyRatio(pSound[i]->getFrequencyRatio() + 0.1f);
			}
		}
	}
	
	vnFont::print(100.0f, 68.0f, L"File");
	vnFont::print(400.0f, 68.0f, L"Control");
	vnFont::print(700.0f, 68.0f, L"Volume");
	vnFont::print(1000.0f, 68.0f, L"FreqRatio");
	
	for (int i = 0; i<fileNum; i++)
	{
		float y=100.0f+ 32.0f*(float)i;
		
		vnFont::setColor(pSound[i]->isPlaying() ? 0xffffff00 : 0xffffffff);
		
		WCHAR* extension = wcsrchr(seFile[i], L'/');
		vnFont::print(100.0f, y, L"%s", extension!=NULL ? ++extension : seFile[i]);
		
		vnFont::print(400.0f, y, L"Play");
		vnFont::print(450.0f, y, L"Stop");
		
		vnFont::print(660.0f, y, L"－");
		vnFont::print(700.0f, y, L"%.3f", pSound[i]->getVolume());
		vnFont::print(800.0f, y, L"＋");

		vnFont::print(960.0f, y, L"－");
		vnFont::print(1000.0f, y, L"%.3f", pSound[i]->getFrequencyRatio());
		vnFont::print(1100.0f, y, L"＋");
	}
	vnScene::execute();
}

//描画関数
void SceneSeTest::render()
{
	vnScene::render();
}

