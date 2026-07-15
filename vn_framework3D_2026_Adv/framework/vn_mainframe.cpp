//--------------------------------------------------------------//
//	"vn_mainframe.cpp"											//
//		メインフレーム											//
//													2026/01/01	//
//														Ichii	//
//--------------------------------------------------------------//
#include "../framework.h"
#include "vn_environment.h"

#include <iostream>
#include <string>

#define FIXED_FRAMERATE (1)

#if FIXED_FRAMERATE
//固定フレームレートのためのパフォーマンスカウンター
bool performanceCounter;
LARGE_INTEGER freq, startCount, finishCount;
float count;
#endif

int frameCount = 0;
float framePerSecond = 0.0f;
float countTime = 0.0f;

extern eSceneTable initialScene;

//静的変数
int vnMainFrame::screenWidth;	//画面の幅
int vnMainFrame::screenHeight;	//画面の高さ
int vnMainFrame::screenCenterX;	//画面の中央(横)
int vnMainFrame::screenCenterY;	//画面の中央(縦)

float vnMainFrame::clearColor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };	//画面のクリアカラー

vnScene* vnMainFrame::pScene = NULL;

//メインフレームの初期化
bool vnMainFrame::initialize(void* hWnd, void* hInst, int width, int height)
{
	//画面サイズの設定
	screenWidth = width;
	screenHeight = height;
	screenCenterX = width / 2;
	screenCenterY = height / 2;

#if FIXED_FRAMERATE
	//パフォーマンスカウンタの初期化
	memset(&freq, 0, sizeof(freq));
	memset(&startCount, 0, sizeof(startCount));
	memset(&finishCount, 0, sizeof(finishCount));
	count = 0.0f;

	performanceCounter = QueryPerformanceFrequency(&freq);
#endif

	vnShader::initialize();

	vnDirect3D::initialize();

	vnDirect2D::initialize();

	vnDirectInput::initialize(hInst);

	vnXInput::initialize();

	vnXAudio2::initialize();

	vnFont::initialize();

	vnCamera::initialize();

	vnLight::initialize();

	vnDebugDraw::initialize();

	vnObject::initializeCommon();

	vnSprite::initializeCommon();

	switchScene();

	return true;
}

//終了
void vnMainFrame::terminate()
{
	terminateScene();

	vnSprite::terminateCommon();

	vnObject::terminateCommon();

	vnDebugDraw::terminate();

	vnLight::terminate();

	vnCamera::terminate();

	vnFont::terminate();

	vnShader::terminate();

	vnXAudio2::terminate();

	vnXInput::terminate();

	vnDirectInput::terminate();

	vnDirect2D::terminate();

	vnDirect3D::terminate();

}

//実行
void vnMainFrame::execute()
{
#if FIXED_FRAMERATE
	if (performanceCounter)
	{	//フレームの開始時間を取得
		QueryPerformanceCounter(&startCount);
	}
#endif

	HRESULT hr = S_OK;

	//入力管理
	vnDirectInput::KeyManager();
	vnXInput::KeyManager();

	//[通常処理開始

	//HOMEボタンで起動時のステージに戻る。
	if (vnKeyboard::trg(DIK_HOME))
	{
		switchScene(initialScene);
	}

	pScene->execute();
	//]通常処理終了


	//[Direct3D描画
	vnDirect3D::beginRender();

	vnDirect3D::getCommandList()->RSSetViewports(1, vnCamera::getViewport());
	pScene->render();
	vnDebugDraw::render();

	vnDirect3D::beginRender2D();

	pScene->render2D();

	vnDirect3D::finishRender();
	//]


	//[Direct2D 描画
	vnDirect2D::render();
	//]

#if FIXED_FRAMERATE
	//フレームレートの固定
	if (performanceCounter)
	{	//フレームの終了時間を取得
		static float frame_msec = 1.0f / 60.0f;
		QueryPerformanceCounter(&finishCount);
		count = ((float)(finishCount.QuadPart - startCount.QuadPart) / (float)freq.QuadPart);

		if (count < frame_msec)
		{	//1/60秒が経つまで待つ
			DWORD wait = (DWORD)((frame_msec - count) * 1000.0f - 0.5f);
			timeBeginPeriod(1);
			Sleep(wait);
			timeEndPeriod(1);
		}
	}
#endif

	//プレゼンテーション
	vnDirect3D::present();

	//ステージの切り替え
	switchScene();
}

void vnMainFrame::initializeScene(vnScene* p)
{
	if (p == NULL)return;
	pScene = p;
	pScene->initialize();
}

void vnMainFrame::terminateScene()
{
	if (pScene != NULL)
	{
		pScene->terminate();
		delete pScene;
	}
	pScene = NULL;
}
