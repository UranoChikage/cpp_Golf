//--------------------------------------------------------------//
//	"scene_joystick_test.cpp"									//
//		ジョイスティックテスト									//
//													2026/01/01	//
//														Ichii	//
//--------------------------------------------------------------//
#include "../../framework.h"
#include "../../framework/vn_environment.h"

#define LINE_POS(y)	((float)(y)*16.0f+40.0f)

//初期化関数
bool SceneJoystickTest::initialize()
{
	testXInput = false;

	for (int i = 0; i < XUSER_MAX_COUNT; i++)
	{
		vibrate[i] = false;
	}

	return true;
}

//終了関数
void SceneJoystickTest::terminate()
{
	for (int i = 0; i < XUSER_MAX_COUNT; i++)
	{
		vnXInput::vibration();
	}
	vnFont::setColor(0xffffffff);
}

//処理関数
void SceneJoystickTest::execute()
{
	if (vnKeyboard::trg(DIK_F1))
	{
		testXInput = !testXInput;
	}

	vnFont::setColor(0xffffffff);
	if (testXInput == false)
	{
		vnFont::print(10.0f, 10.0f, L"test mode : DirectInput");
		vnFont::print(230.0f, 10.0f, L"(press F1 to switch test mode)");
		executeDInput();
	}
	else
	{
		vnFont::print(10.0f, 10.0f, L"test mode : XInput");
		vnFont::print(230.0f, 10.0f, L"(press F1 to switch test mode)");
		executeXInput();
	}

	vnScene::execute();
}

void SceneJoystickTest::executeDInput()
{
	for(int i=0; i<vnJOYSTICK_MAX; i++)
	{
		float x = 250.0f * (float)i + 100.0f;
		int y=0;
		DWORD col = 0xffffffff;
		if(vnJoystick::isValid(i)==true)
		{
			vnFont::print(x,LINE_POS(y),col, L"Joystick[%d] : enable.", i);
		}
		else
		{
			col = 0x80ffffff;
			vnFont::print(x, LINE_POS(y), col, L"Joystick[%d] : disable.", i);
		}
		y+=2;
		
		vnFont::print(x, LINE_POS(y), col, L"leftX : %.3f", vnJoystick::leftX(i));	y++;
		vnFont::print(x, LINE_POS(y), col, L"leftY : %.3f", vnJoystick::leftY(i));	y++;
		vnFont::print(x, LINE_POS(y), col, L"rightX : %.3f", vnJoystick::rightX(i));	y++;
		vnFont::print(x, LINE_POS(y), col, L"rightY : %.3f", vnJoystick::rightY(i));	y++;
		
		y++;
		
		vnFont::print(x, LINE_POS(y), col, L"pov : %d", vnJoystick::pov(i));	y++;
		
		y++;
		for(int j=0; j<vnDirectInput::getJoystickButtonNum(); j++)
		{
			vnFont::print(x, LINE_POS(y), col, L"Button[%d] : %d", j, vnJoystick::on(j, i));
			y++;
		}
	}
	vnFont::setColor(0xffffffff);
}

void SceneJoystickTest::executeXInput()
{
	for (int i = 0; i < XUSER_MAX_COUNT; i++)
	{
		float x = 250.0f * (float)i + 100.0f;
		int y = 0;
		DWORD col = 0xffffffff;
		if (vnXInput::isValid(i) == true)
		{
			vnFont::print(x, LINE_POS(y), col, L"Controller[%d] : enable.", i);
		}
		else
		{
			col = 0x80ffffff;
			vnFont::print(x, LINE_POS(y), col, L"Controller[%d] : disable.", i);
		}
		y += 2;

		vnFont::print(x, LINE_POS(y), col, L"leftX : %.3f", vnXInput::leftX(i));	y++;
		vnFont::print(x, LINE_POS(y), col, L"leftY : %.3f", vnXInput::leftY(i));	y++;
		vnFont::print(x, LINE_POS(y), col, L"rightX : %.3f", vnXInput::rightX(i));	y++;
		vnFont::print(x, LINE_POS(y), col, L"rightY : %.3f", vnXInput::rightY(i));	y++;

		y++;

		vnFont::print(x, LINE_POS(y), col, L"leftT : %.3f", vnXInput::leftTrigger(i));	y++;
		vnFont::print(x, LINE_POS(y), col, L"rightT : %.3f", vnXInput::rightTrigger(i));	y++;

		y++;

		vnFont::print(x, LINE_POS(y), col, L"DPAD_UP : %d", vnXInput::on(XINPUT_GAMEPAD_DPAD_UP, i));	y++;
		vnFont::print(x, LINE_POS(y), col, L"DPAD_DOWN : %d", vnXInput::on(XINPUT_GAMEPAD_DPAD_DOWN, i));	y++;
		vnFont::print(x, LINE_POS(y), col, L"DPAD_LEFT : %d", vnXInput::on(XINPUT_GAMEPAD_DPAD_LEFT, i));	y++;
		vnFont::print(x, LINE_POS(y), col, L"DPAD_RIGHT : %d", vnXInput::on(XINPUT_GAMEPAD_DPAD_RIGHT, i));	y++;

		y++;

		vnFont::print(x, LINE_POS(y), col, L"START : %d", vnXInput::on(XINPUT_GAMEPAD_START, i));	y++;
		vnFont::print(x, LINE_POS(y), col, L"BACK : %d", vnXInput::on(XINPUT_GAMEPAD_BACK, i));	y++;

		y++;

		vnFont::print(x, LINE_POS(y), col, L"LEFT_THUMB : %d", vnXInput::on(XINPUT_GAMEPAD_LEFT_THUMB, i));	y++;
		vnFont::print(x, LINE_POS(y), col, L"RIGHT_THUMB : %d", vnXInput::on(XINPUT_GAMEPAD_RIGHT_THUMB, i));	y++;

		y++;

		vnFont::print(x, LINE_POS(y), col, L"LEFT_SHOULDER : %d", vnXInput::on(XINPUT_GAMEPAD_LEFT_SHOULDER, i));	y++;
		vnFont::print(x, LINE_POS(y), col, L"RIGHT_SHOULDER : %d", vnXInput::on(XINPUT_GAMEPAD_RIGHT_SHOULDER, i));	y++;

		y++;

		vnFont::print(x, LINE_POS(y), col, L"A : %d", vnXInput::on(XINPUT_GAMEPAD_A, i));	y++;
		vnFont::print(x, LINE_POS(y), col, L"B : %d", vnXInput::on(XINPUT_GAMEPAD_B, i));	y++;
		vnFont::print(x, LINE_POS(y), col, L"X : %d", vnXInput::on(XINPUT_GAMEPAD_X, i));	y++;
		vnFont::print(x, LINE_POS(y), col, L"Y : %d", vnXInput::on(XINPUT_GAMEPAD_Y, i));	y++;

		y++;

		vnFont::print(x, LINE_POS(y), col, L"press \"%d\" to vibrate", i+1);	y++;

		if (vnKeyboard::trg(DIK_1 + i))
		{
			vibrate[i] = !vibrate[i];
			vnXInput::vibration(vibrate[i] ? 0x7fff : 0, i);
		}
	}
	vnFont::setColor(0xffffffff);
}

//描画関数
void SceneJoystickTest::render()
{
	vnScene::render();
}
