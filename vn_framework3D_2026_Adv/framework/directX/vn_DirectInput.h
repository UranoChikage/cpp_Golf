//--------------------------------------------------------------//
//	"vn_DirectInput.h"											//
//		DirectInput管理											//
//													2026/01/01	//
//														Ichii	//
//--------------------------------------------------------------//
#pragma once

#define vnJOYSTICK_MAX	(4)	//使用できるジョイスティックの数

class vnDirectInput
{
private:
	static LPDIRECTINPUT8		pDI;					// DirectInputオブジェクト
	static LPDIRECTINPUTDEVICE8	pDIKeyboard;			// デバイス:keyboard
	static LPDIRECTINPUTDEVICE8	pDIMouse;				// デバイス:mouse
	static LPDIRECTINPUTDEVICE8	pDIDev[vnJOYSTICK_MAX];	// デバイス:joystick
	
	static DIMOUSESTATE2	diMouse;			// マウス用
	static int				Mouse_x;			// マウスの座標
	static int				Mouse_y;			// マウスの座標
	static BYTE				diKeyboard[256];	// キーボード用
	static BYTE				oldKeyboard[256];	// 1フレーム前のキーボードの状態
	static DIJOYSTATE		diJoystick[vnJOYSTICK_MAX];
	static DIJOYSTATE		oldJoystick[vnJOYSTICK_MAX];
	
	static int				JoystickButtonNum;
	static int				JoystickAxisRange;
	static float			JoystickAxisRangeI;
	
	static BOOL CALLBACK EnumJoysticksCallback(const DIDEVICEINSTANCE* pdidInstance, VOID* pContext);
	static BOOL CALLBACK EnumAxisCallback(LPCDIDEVICEOBJECTINSTANCE lpddoi, LPVOID pvRef);

public:
	static HRESULT initialize(void *hinst);
	static void terminate();
	
	static void setMousePosition(int x, int y);
	static void KeyManager();
	
	static int getMouseX();
	static int getMouseY();
	static DIMOUSESTATE2	*getMouseState();
	static BYTE				*getCurrentKeyboardState();
	static BYTE				*getOldKeyboardState();
	static DIJOYSTATE		*getCurrentJoystick(int id=0);
	static DIJOYSTATE		*getOldJoystick(int id=0);
	static bool				useJoystick(int id=0);
	
	static int		getJoystickButtonNum();
	static float	getJoystickAxisRangeI();
};
