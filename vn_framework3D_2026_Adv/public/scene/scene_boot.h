//--------------------------------------------------------------//
//	"scene_boot.h"												//
//		シーン(起動/シーンセレクト)								//
//													2026/01/01	//
//														Ichii	//
//--------------------------------------------------------------//
#pragma once

class SceneBoot : public vnScene
{
private:
	int Cursor;
	
public:
	bool initialize();
	void terminate();
	
	void execute();
	void render();
};
