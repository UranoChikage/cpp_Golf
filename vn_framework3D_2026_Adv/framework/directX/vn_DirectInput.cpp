//--------------------------------------------------------------//
//	"vn_DirectInput.cpp"										//
//		DirectInput管理											//
//													2026/01/01	//
//														Ichii	//
//--------------------------------------------------------------//
#include "../../framework.h"
#include "../vn_environment.h"

LPDIRECTINPUT8			vnDirectInput::pDI=NULL;				// DirectInputオブジェクト
LPDIRECTINPUTDEVICE8	vnDirectInput::pDIKeyboard=NULL;		// keyboard
LPDIRECTINPUTDEVICE8	vnDirectInput::pDIMouse=NULL;			// mouse
LPDIRECTINPUTDEVICE8	vnDirectInput::pDIDev[vnJOYSTICK_MAX];	// joystick

DIMOUSESTATE2	vnDirectInput::diMouse;	// マウスの状態
int				vnDirectInput::Mouse_x;	// マウスの座標X
int				vnDirectInput::Mouse_y;	// マウスの座標Y
BYTE			vnDirectInput::diKeyboard[256];	// キーボード用
BYTE			vnDirectInput::oldKeyboard[256];	// 1フレーム前のキーボードの状態
DIJOYSTATE		vnDirectInput::diJoystick[vnJOYSTICK_MAX];
DIJOYSTATE		vnDirectInput::oldJoystick[vnJOYSTICK_MAX];

int				vnDirectInput::JoystickButtonNum = sizeof(diJoystick[0].rgbButtons)/sizeof(BYTE);
int				vnDirectInput::JoystickAxisRange = 1000;
float			vnDirectInput::JoystickAxisRangeI = 1.0f/(float)JoystickAxisRange;

HRESULT vnDirectInput::initialize(void *hinst)
{
	HRESULT	hr;

	//IDirectInput8インターフェイスの取得
	hr = DirectInput8Create((HINSTANCE)hinst, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&pDI, NULL);
	if( FAILED(hr) )
	{
		return hr;
	}
	
	//ジョイスティック情報の初期化
	for(int i=0; i<vnJOYSTICK_MAX; i++)
	{
		pDIDev[i]=NULL;
		ZeroMemory(&diJoystick[i],sizeof(DIJOYSTATE));
		ZeroMemory(&oldJoystick[i],sizeof(DIJOYSTATE));
	}
	
	//IDirectInputDevice8インターフェイスの取得
	//マウス
    hr = pDI->CreateDevice(GUID_SysMouse, &pDIMouse, NULL);
	if( FAILED(hr) )
	{
		terminate();
		return E_FAIL;
    }
	hr = pDIMouse->SetDataFormat(&c_dfDIMouse2);//データフォーマットの設定
	if( FAILED(hr) )
	{
		terminate();
		return E_FAIL;
	}
	
	//キーボード
	hr = pDI->CreateDevice(GUID_SysKeyboard, &pDIKeyboard, NULL);
	if( FAILED(hr) )
	{
		terminate();
		return E_FAIL;
	}
	hr = pDIKeyboard->SetDataFormat(&c_dfDIKeyboard);
	if( FAILED(hr) )
	{
		terminate();
		return E_FAIL;
	}
	
	//ジョイスティック
	//デバイスの列挙
	hr = pDI->EnumDevices(DI8DEVCLASS_GAMECTRL,EnumJoysticksCallback,NULL,DIEDFL_ATTACHEDONLY);
	if(FAILED(hr))
	{
		terminate();
		return E_FAIL;
	}
	//データ形式の設定
	for(int i=0; i<vnJOYSTICK_MAX; i++)
	{
		if(pDIDev[i]==NULL)continue;
		
		hr = pDIDev[i]->SetDataFormat(&c_dfDIJoystick);
		if(FAILED(hr))
		{
			return E_FAIL;
		}
		//フォアグラウンド&非排他モードの設定
	/*	hr = pDIDev[2]->SetCooperativeLevel(hwndApp,DISCL_NONEEXCLUSIVE | DISCL_FOREGROUND);
		if(FAILED(hr))
		{
			return E_FAIL;
		}*/
		//コールバック関数を使って各軸のモードを設定
		hr = pDIDev[i]->EnumObjects(EnumAxisCallback,&i,DIDFT_AXIS);
		if(FAILED(hr))
		{
			return E_FAIL;
		}
		//軸モードを絶対値モードに設定
		DIPROPDWORD diprop;
		diprop.diph.dwSize = sizeof(diprop);
		diprop.diph.dwHeaderSize = sizeof(diprop.diph);
		diprop.diph.dwObj = 0;
		diprop.diph.dwHow = DIPH_DEVICE;
		diprop.dwData = DIPROPAXISMODE_ABS;
		hr = pDIDev[i]->SetProperty(DIPROP_AXISMODE, &diprop.diph);
		if(FAILED(hr))
		{
			return E_FAIL;
		}
		//バッファリングデータを取得するためバッファサイズを設定
		diprop.dwData = 100;//DIDEVICE_BUFFERSIZE;
		hr = pDIDev[i]->SetProperty(DIPROP_BUFFERSIZE,&diprop.diph);
		if(FAILED(hr))
		{
			return E_FAIL;
		}
		//入力制御開始
		pDIDev[i]->Acquire();
	}
	
	if( FAILED(hr) )	return hr;
	return	S_OK;
}

void vnDirectInput::terminate()
{
	if(pDIKeyboard)	pDIKeyboard->Unacquire();
	SAFE_RELEASE(pDIKeyboard)
	
	if(pDIMouse)	pDIMouse->Unacquire();
	SAFE_RELEASE(pDIMouse)
	
	for(int i=0; i<vnJOYSTICK_MAX; i++)
	{
		if(pDIDev[i])	pDIDev[i]->Unacquire();
		SAFE_RELEASE(pDIDev[i]);
	}
	SAFE_RELEASE( pDI );
}

//ジョイスティックを列挙
BOOL CALLBACK vnDirectInput::EnumJoysticksCallback(const DIDEVICEINSTANCE *pdidInstance, VOID *pContext)
{
	DIDEVCAPS g_diDevCaps;	//ジョイスティックの能力
	HRESULT hr;
	
	//空きを探す
	int id=-1;
	for(int i=0; i<vnJOYSTICK_MAX; i++)
	{
		if(pDIDev[i]==NULL)
		{
			id = i;
			break;
		}
	}
	//空きがなければ終わり
	if(id<0)
	{
		return DIENUM_STOP;
	}
	
	//列挙されたジョイスティックへのインターフェイスを取得
	hr = pDI->CreateDevice(pdidInstance->guidInstance, &pDIDev[id], NULL);
	if(FAILED(hr))
	{
		return DIENUM_CONTINUE;
	}
	//ジョイスティックの能力を調べる
	g_diDevCaps.dwSize = sizeof(DIDEVCAPS);
	hr = pDIDev[id]->GetCapabilities(&g_diDevCaps);
	if(FAILED(hr))
	{
		return DIENUM_CONTINUE;
	}
	if(id>=vnJOYSTICK_MAX)
	{
		return DIENUM_STOP;
	}
	return DIENUM_CONTINUE;
}

//ジョイスティックの軸を列挙
BOOL CALLBACK vnDirectInput::EnumAxisCallback(LPCDIDEVICEOBJECTINSTANCE lpddoi, LPVOID pvRef)
{
	int id = *((int*)pvRef);
	
	HRESULT hr;
	//軸の値の範囲を設定
	DIPROPRANGE diprg;
	ZeroMemory(&diprg, sizeof(diprg));
	diprg.diph.dwSize = sizeof(diprg);
	diprg.diph.dwHeaderSize = sizeof(diprg.diph);
	diprg.diph.dwObj = lpddoi->dwType;
	diprg.diph.dwHow = DIPH_BYID;
	diprg.lMin = -JoystickAxisRange;
	diprg.lMax = +JoystickAxisRange;
	hr = pDIDev[id]->SetProperty(DIPROP_RANGE, &diprg.diph);
	if(FAILED(hr))
	{
		return DIENUM_STOP;
	}
	return DIENUM_CONTINUE;
}

void vnDirectInput::setMousePosition(int x, int y)
{
	Mouse_x = x;
	Mouse_y = y;
}

void vnDirectInput::KeyManager()
{
	HRESULT hr;
	
	//マウス
	hr = pDIMouse->GetDeviceState(sizeof(DIMOUSESTATE2), &diMouse);
	if (FAILED(hr))
	{
		hr = pDIMouse->Acquire();
	}
	
	//キーボード
	memcpy(oldKeyboard, diKeyboard, sizeof(oldKeyboard));	//前フレームのキー状態を保存
	hr = pDIKeyboard->GetDeviceState(256, diKeyboard);
	if (FAILED(hr))
	{
		hr = pDIKeyboard->Acquire();
	}
	
	//ジョイスティック
	for(int i=0; i<vnJOYSTICK_MAX; i++)
	{
		if(pDIDev[i]==NULL)continue;
		memcpy(&oldJoystick[i],&diJoystick[i],sizeof(DIJOYSTATE));
		hr = pDIDev[i]->Poll();
		if(FAILED(hr))	hr = pDIDev[i]->Acquire();
		hr = pDIDev[i]->GetDeviceState(sizeof(DIJOYSTATE),&diJoystick[i]);
	//	if(FAILED(hr))	hr = pDIDev[i]->Acquire();
	}

	//マウスの状態を設定
	vnMouse::setMouseStatus();
}

int vnDirectInput::getMouseX()
{
	return Mouse_x;
}

int vnDirectInput::getMouseY()
{
	return Mouse_y;
}

DIMOUSESTATE2	*vnDirectInput::getMouseState()
{
	return &diMouse;
}

BYTE *vnDirectInput::getCurrentKeyboardState()
{
	return diKeyboard;
}

BYTE *vnDirectInput::getOldKeyboardState()
{
	return oldKeyboard;
}

DIJOYSTATE *vnDirectInput::getCurrentJoystick(int id)
{
	if(id<0 || id>=vnJOYSTICK_MAX)return NULL;
	return &diJoystick[id];
}

DIJOYSTATE *vnDirectInput::getOldJoystick(int id)
{
	if(id<0 || id>=vnJOYSTICK_MAX)return NULL;
	return &oldJoystick[id];
}

bool vnDirectInput::useJoystick(int id)
{
	if(id<0 || id>=vnJOYSTICK_MAX)return false;
	return pDIDev[id]!=NULL;
}

int vnDirectInput::getJoystickButtonNum()
{
	return JoystickButtonNum;
}

float vnDirectInput::getJoystickAxisRangeI()
{
	return JoystickAxisRangeI;
}
