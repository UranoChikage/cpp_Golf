//--------------------------------------------------------------//
//	"scene_table.cpp"											//
//		シーンテーブル											//
//													2026/01/01	//
//														Ichii	//
//--------------------------------------------------------------//
#include "../../framework.h"
#include "../../framework/vn_environment.h"

//起動時のシーン
#if _DEBUG
eSceneTable initialScene = eSceneTable::Boot;
#else
eSceneTable initialScene = eSceneTable::Boot;
#endif
//現在のシーン
eSceneTable currentScene = (eSceneTable)-1;
//切り替え予約のシーン
eSceneTable reserveScene = initialScene;


//各シーンの名前
WCHAR SceneName[(int)eSceneTable::SceneMax][32] =
{
	L"Boot",
	L"Field Test",
	L"Ground Test",
	L"Sprite Test",
	L"Joystick Test",
	L"SE Test",
	L"Font Test",
	L"My Game",
};

//ステージ切り替え関数(予約)
void switchScene(eSceneTable scene)
{
	reserveScene = scene;
}

//現在のステージ取得(実処理)
void switchScene()
{
	if (currentScene == reserveScene)
	{
		return;
	}

	vnMainFrame::terminateScene();

	vnScene* pScene = NULL;
	switch (reserveScene)
	{
	case Boot:			pScene = new SceneBoot();			break;
	case FieldTest:		pScene = new SceneFieldTest();		break;
	case GroundTest:	pScene = new SceneGroundTest();		break;
	case SpriteTest:	pScene = new SceneSpriteTest();		break;
	case JoystickTest:	pScene = new SceneJoystickTest();	break;
	case SeTest:		pScene = new SceneSeTest();			break;
	case FontTest:		pScene = new SceneFontTest();		break;
	case MyGame:		pScene = new SceneMyGame();		    break;
	}

	vnMainFrame::initializeScene(pScene);

	currentScene = reserveScene;
}
