//--------------------------------------------------------------//
//	"vn_XInput.h"												//
//		XInput管理												//
//													2026/01/01	//
//														Ichii	//
//--------------------------------------------------------------//
#pragma once

class vnXInput
{
private:
	static constexpr float triggerMax = 1.0f / 255.0f;
	static constexpr float analogMax = 1.0f / 32768.0f;

	static constexpr int connectionCheckInterval = 60;	//接続チェックを行う間隔(フレーム)

	static int connectionCheckCount;

	struct stState
	{
		DWORD			enable;
		XINPUT_STATE	state;
		XINPUT_STATE	past;
		float			LeftTrigger;
		float			RightTrigger;
		float			LeftX;
		float			LeftY;
		float			RightX;
		float			RightY;
	};
	static stState state[XUSER_MAX_COUNT];	//最大4

public:
	//[システム関数
	static HRESULT initialize();
	static void terminate();
	static void KeyManager();
	//]

	static bool isValid(int id = 0);			//コントローラーが有効か調べる

	static bool on(WORD button, int id = 0);	//ボタンが押されているか調べる
	static bool trg(WORD button, int id = 0);	//ボタンが押された瞬間か調べる
	static bool rel(WORD button, int id = 0);	//ボタンが離された瞬間か調べる

	static float leftX(int id = 0);				//左スティックの左右状態(-1.0～1.0)
	static float leftY(int id = 0);				//左スティックの上下状態(-1.0～1.0)

	static float rightX(int id = 0);			//右スティックの左右状態(-1.0～1.0)
	static float rightY(int id = 0);			//右スティックの上下状態(-1.0～1.0)

	static float leftTrigger(int id = 0);		//左トリガーの押下状態(0.0～1.0)
	static float rightTrigger(int id = 0);		//右トリガーの押下状態(0.0～1.0)

	static void vibration(WORD speed = 0, int id = 0);			//振動(左右のモーター共通 / 0で停止)
	static void vibration(WORD left, WORD right, int id = 0);	//振動(左右のモーター独立 / 0で停止)

};

//Document :
//https://docs.microsoft.com/ja-jp/windows/win32/xinput/getting-started-with-xinput

//Reference :
//https://docs.microsoft.com/ja-jp/windows/win32/api/xinput/ns-xinput-xinput_gamepad

/*
デバイスボタン

XINPUT_GAMEPAD_DPAD_UP			0x0001
XINPUT_GAMEPAD_DPAD_DOWN		0x0002
XINPUT_GAMEPAD_DPAD_LEFT		0x0004
XINPUT_GAMEPAD_DPAD_RIGHT		0x0008
XINPUT_GAMEPAD_START			0x0010
XINPUT_GAMEPAD_BACK				0x0020
XINPUT_GAMEPAD_LEFT_THUMB		0x0040
XINPUT_GAMEPAD_RIGHT_THUMB		0x0080
XINPUT_GAMEPAD_LEFT_SHOULDER	0x0100
XINPUT_GAMEPAD_RIGHT_SHOULDER	0x0200
XINPUT_GAMEPAD_A				0x1000
XINPUT_GAMEPAD_B				0x2000
XINPUT_GAMEPAD_X				0x4000
XINPUT_GAMEPAD_Y				0x8000

*/