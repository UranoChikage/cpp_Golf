//--------------------------------------------------------------//
//	"vn_mouse.cpp"												//
//		ƒ}ƒEƒX“ü—Í(DirectInput)									//
//													2026/01/01	//
//														Ichii	//
//--------------------------------------------------------------//
#include "../framework.h"
#include "vn_environment.h"

int	vnMouse::x = 0;
int	vnMouse::y = 0;
int	vnMouse::r = 0;
int	vnMouse::dx = 0;
int vnMouse::dy = 0;
int	vnMouse::On = 0;
int	vnMouse::Trg = 0;
int	vnMouse::Rel = 0;

int	vnMouse::getX(void){return x;}
int	vnMouse::getY(void){return y;}
int	vnMouse::getR(void){return r;}
int	vnMouse::getDX(void){return dx;}
int	vnMouse::getDY(void){return dy;}
bool vnMouse::on(eButton btn){return btn&On?true:false;}
bool vnMouse::trg(eButton btn){return btn&Trg?true:false;}
bool vnMouse::rel(eButton btn){return btn&Rel?true:false;}

void vnMouse::setMouseStatus(void)
{
	x = vnDirectInput::getMouseX();
	y = vnDirectInput::getMouseY();
	r = vnDirectInput::getMouseState()->lZ;
	dx = vnDirectInput::getMouseState()->lX;
	dy = vnDirectInput::getMouseState()->lY;
	int Old=On;
	On = 0;
	if(vnDirectInput::getMouseState()->rgbButtons[0])On|=LT;
	if(vnDirectInput::getMouseState()->rgbButtons[1])On|=RT;
	if(vnDirectInput::getMouseState()->rgbButtons[2])On|=MD;
	
	Trg = (On^Old)&On;
	Rel = (On^Old)&Old;
}

