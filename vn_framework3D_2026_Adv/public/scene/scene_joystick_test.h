//--------------------------------------------------------------//
//	"scene_joystick_test.h"										//
//		ジョイスティックテスト									//
//													2026/01/01	//
//														Ichii	//
//--------------------------------------------------------------//
#pragma once

class SceneJoystickTest : public vnScene
{
private:
	bool testXInput;				//DirectInput / XInput 切り替え
	bool vibrate[XUSER_MAX_COUNT];	//XInput:振動の操作

	void executeDInput();
	void executeXInput();

public:
	
	bool initialize();
	void terminate();
	
	void execute();
	void render();
};
