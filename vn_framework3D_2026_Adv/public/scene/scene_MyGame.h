#pragma once

class SceneMyGame : public vnScene
{
private:
	vnModel* pBall;
public:
	//‰Šú‰»
	bool initialize();
	//I—¹
	void terminate();

	//ˆ—
	void execute();
	//•`‰æ
	void render();
};

