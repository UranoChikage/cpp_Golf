//--------------------------------------------------------------//
//	"vn_joystick.cpp"											//
//		ジョイスティック入力(DirectInput)						//
//													2026/01/01	//
//														Ichii	//
//--------------------------------------------------------------//
#include "../framework.h"
#include "vn_environment.h"

bool vnJoystick::isValid(int id)
{
	return vnDirectInput::useJoystick(id);
}

bool vnJoystick::on(int button, int id)
{
	if(button<0 || button>=vnDirectInput::getJoystickButtonNum())return false;
	DIJOYSTATE *js = vnDirectInput::getCurrentJoystick(id);
	return js->rgbButtons[button]!=0;
}

bool vnJoystick::trg(int button, int id)
{
	if(button<0 || button>= vnDirectInput::getJoystickButtonNum())return false;
	DIJOYSTATE *jsc = vnDirectInput::getCurrentJoystick(id);
	DIJOYSTATE *jso = vnDirectInput::getOldJoystick(id);
	return jsc->rgbButtons[button]!=0 && jso->rgbButtons[button]==0;
}

bool vnJoystick::rel(int button, int id)
{
	if(button<0 || button>= vnDirectInput::getJoystickButtonNum())return false;
	DIJOYSTATE *jsc = vnDirectInput::getCurrentJoystick(id);
	DIJOYSTATE *jso = vnDirectInput::getOldJoystick(id);
	return jsc->rgbButtons[button]==0 && jso->rgbButtons[button]!=0;
}

float vnJoystick::leftX(int id)
{
	DIJOYSTATE *js = vnDirectInput::getCurrentJoystick(id);
	return (float)js->lX*vnDirectInput::getJoystickAxisRangeI();
}

float vnJoystick::leftY(int id)
{
	DIJOYSTATE *js = vnDirectInput::getCurrentJoystick(id);
	return (float)js->lY*vnDirectInput::getJoystickAxisRangeI();
}

float vnJoystick::rightX(int id)
{
	DIJOYSTATE *js = vnDirectInput::getCurrentJoystick(id);
	return (float)js->lZ*vnDirectInput::getJoystickAxisRangeI();
}

float vnJoystick::rightY(int id)
{
	DIJOYSTATE *js = vnDirectInput::getCurrentJoystick(id);
	return (float)js->lRz*vnDirectInput::getJoystickAxisRangeI();
}

int vnJoystick::pov(int id)
{
	DIJOYSTATE *js = vnDirectInput::getCurrentJoystick(id);
	return js->rgdwPOV[0];
}
