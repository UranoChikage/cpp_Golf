//--------------------------------------------------------------//
//	"vn_mainframe.h"											//
//		メインフレーム											//
//													2026/01/01	//
//														Ichii	//
//--------------------------------------------------------------//
#pragma once

class vnMainFrame
{
private:
	static vnScene* pScene;

public:
	static int screenWidth;		//画面の幅
	static int screenHeight;	//画面の高さ
	static int screenCenterX;	//画面の中央(横)
	static int screenCenterY;	//画面の中央(縦)

	static float clearColor[4];	//画面のクリアカラー(RGBA:0.0～1.0)

	static void initializeScene(vnScene* p);	//シーンの初期化
	static void terminateScene();				//シーンの終了

	//初期化
	static bool initialize(void* hWnd, void* hInst, int width, int height);

	//終了
	static void terminate();

	//実行
	static void execute();
};
