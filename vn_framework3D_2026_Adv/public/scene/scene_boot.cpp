//--------------------------------------------------------------//
//	"scene_boot.cpp"											//
//		シーン(起動/シーンセレクト)								//
//													2026/01/01	//
//														Ichii	//
//--------------------------------------------------------------//
#include "../../framework.h"
#include "../../framework/vn_environment.h"

#define CURSOR_X		(80.0f)		//カーソルのX座標
#define SCENE_NAME_X	(100.0f)	//シーン名のX座標
#define SCENE_NAME_Y	(100.0f)	//シーン名のY座標
#define SCENE_NAME_LINE	(20.0f)		//シーン名の改行幅

extern WCHAR SceneName[(int)eSceneTable::SceneMax][32];

//初期化関数
bool SceneBoot::initialize()
{
	Cursor = 0;
	return true;
}

//終了関数
void SceneBoot::terminate()
{
}

//処理関数
void SceneBoot::execute()
{
	if (vnKeyboard::trg(DIK_UP) && --Cursor < 0)
	{	//↑キーでカーソルを上に移動
		Cursor = (int)eSceneTable::SceneMax - 1;
	}
	else if (vnKeyboard::trg(DIK_DOWN) && ++Cursor >= (int)eSceneTable::SceneMax)
	{	//↓キーでカーソルを下に移動
		Cursor = 0;
	}
	else if (vnKeyboard::trg(DIK_SPACE))
	{	//SPACEキーでシーンを切り替え
		switchScene((eSceneTable)Cursor);
	}

	//カーソルの表示
	vnFont::print(CURSOR_X, SCENE_NAME_Y + SCENE_NAME_LINE * (float)Cursor, 0xffffff00, L"→");
	//シーン名の表示
	for (int i = 0; i < (int)eSceneTable::SceneMax; i++)
	{
		DWORD col = i == Cursor ? 0xffffff80 : 0xffffffff;
		vnFont::print(SCENE_NAME_X, SCENE_NAME_Y + SCENE_NAME_LINE * (float)i, col, SceneName[i]);
	}

	vnScene::execute();
}

//描画関数
void SceneBoot::render()
{
	vnScene::render();
}

