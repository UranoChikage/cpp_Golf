//--------------------------------------------------------------//
//	"scene_sprite_test.cpp"										//
//		スプライトテスト										//
//													2026/01/01	//
//														Ichii	//
//--------------------------------------------------------------//
#include "../../framework.h"
#include "../../framework/vn_environment.h"

//初期化関数
bool SceneSpriteTest::initialize()
{
	//スプライトの作成
	pSprite = new vnSprite((float)vnMainFrame::screenCenterX, (float)vnMainFrame::screenCenterY, 512.0f, 512.0f, L"data/image/checker.png");

	registerObject(pSprite);

	return true;
}

//終了関数
void SceneSpriteTest::terminate()
{
	//スプライトの削除
	deleteObject(pSprite);
}

//処理関数
void SceneSpriteTest::execute()
{
	float value_p = 1.0f;
	float value_s = 0.01f;
	float value_r = 1.0f * 3.141592f / 180.0f;

	if (vnKeyboard::trg(DIK_UP) && --Cursor < 0)
	{
		Cursor = OperationMax - 1;
	}
	else if (vnKeyboard::trg(DIK_DOWN) && ++Cursor >= OperationMax)
	{
		Cursor = 0;
	}
	else if (vnKeyboard::on(DIK_RIGHT))
	{
		switch (Cursor)
		{
		case PositionX:	pSprite->posX += value_p;	break;
		case PositionY:	pSprite->posY += value_p;	break;
		case ScaleX:	pSprite->scaleX += value_s;	break;
		case ScaleY:	pSprite->scaleY += value_s;	break;
		case Rotate:	pSprite->rot += value_r;	break;
		}
	}
	else if (vnKeyboard::on(DIK_LEFT))
	{
		switch (Cursor)
		{
		case PositionX:	pSprite->posX -= value_p;	break;
		case PositionY:	pSprite->posY -= value_p;	break;
		case ScaleX:	pSprite->scaleX -= value_s;	break;
		case ScaleY:	pSprite->scaleY -= value_s;	break;
		case Rotate:	pSprite->rot -= value_r;	break;
		}
	}
	else if (vnKeyboard::on(DIK_SPACE))
	{
		switch (Cursor)
		{
		case PositionX:	pSprite->posX = (float)vnMainFrame::screenCenterX;	break;
		case PositionY:	pSprite->posY = (float)vnMainFrame::screenCenterY;	break;
		case ScaleX:	pSprite->scaleX = 1.0f;	break;
		case ScaleY:	pSprite->scaleY = 1.0f;	break;
		case Rotate:	pSprite->rot = 0.0f;	break;
		}
	}

	float x = 30.0f;
	float y = 10.0f;
	int line = 0;
	int interval = 16;
	vnFont::print(x - 20.0f, (float)(y + Cursor * interval), L"→");
	vnFont::print(x, y + (float)(line++ * interval), L"Position X : %.3f", pSprite->posX);
	vnFont::print(x, y + (float)(line++ * interval), L"Position Y : %.3f", pSprite->posY);
	vnFont::print(x, y + (float)(line++ * interval), L"Scale X    : %.3f", pSprite->scaleX);
	vnFont::print(x, y + (float)(line++ * interval), L"Scale Y    : %.3f", pSprite->scaleY);
	vnFont::print(x, y + (float)(line++ * interval), L"Rotation   : %.3f", pSprite->rot / 3.1415192f * 180.0f);

	vnScene::execute();
}

//描画関数
void SceneSpriteTest::render()
{
	vnScene::render();
}

