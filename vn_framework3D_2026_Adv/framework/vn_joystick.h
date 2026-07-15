//--------------------------------------------------------------//
//	"vn_joystick.h"												//
//		ジョイスティック入力(DirectInput)						//
//													2026/01/01	//
//														Ichii	//
//--------------------------------------------------------------//
#pragma once

class vnJoystick
{
public:
	static bool isValid(int id=0);			//ジョイスティックが有効か調べる
	
	static bool on(int button, int id=0);	//ボタンが押されているか調べる(0～32)
	static bool trg(int button, int id=0);	//ボタンが押された瞬間か調べる(0～32)
	static bool rel(int button, int id=0);	//ボタンが離された瞬間か調べる(0～32)
	
	static float leftX(int id=0);			//左スティックの左右状態(-1.0～1.0)
	static float leftY(int id=0);			//左スティックの上下状態(-1.0～1.0)
	
	static float rightX(int id=0);			//右スティックの左右状態(-1.0～1.0)
	static float rightY(int id=0);			//右スティックの上下状態(-1.0～1.0)
	
	static int	pov(int id=0);				//ポイント / ビュー (ハットスイッチ) 
};
