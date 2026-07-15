//--------------------------------------------------------------//
//	"scene_font_test.h"											//
//		シーン：フォントテスト									//
//													2026/01/01	//
//														Ichii	//
//--------------------------------------------------------------//
#pragma once

class SceneFontTest : public vnScene
{
private:
	int Cursor;		//選択
	int FontNum;	//使用できるフォントの数

	IDWriteTextFormat** textFormat;	//作成されたフォント
	
public:	
	//初期化
	bool initialize();

	//終了
	void terminate();

	//処理
	void execute();

	//描画
	void render();
};
