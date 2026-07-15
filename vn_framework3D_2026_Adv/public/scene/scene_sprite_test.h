//--------------------------------------------------------------//
//	"scene_sprite_test.h"										//
//		スプライトテスト										//
//													2026/01/01	//
//														Ichii	//
//--------------------------------------------------------------//
#pragma once

class SceneSpriteTest : public vnScene
{
private:
	vnSprite* pSprite;

	//操作の種類
	enum eOperation
	{
		PositionX,
		PositionY,
		ScaleX,
		ScaleY,
		Rotate,
		OperationMax,
	};

	int	Cursor;

public:
	bool initialize();
	void terminate();
	
	void execute();
	void render();
};
