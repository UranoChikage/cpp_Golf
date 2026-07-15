#include "../../framework.h"
#include "../../framework/vn_environment.h"

#define DEDAULT_SLERP_RATE	(0.1f)
//‰Šú‰»
bool SceneMyGame::initialize()
{
	pBall = new vnModel(L"data/model/primitive", L"cube_soft.vnm");
	vnCamera::setPosition(0.0f, 1.0f, -5.0f);
	vnCamera::setTarget(0.0f, 1.0f, 0.0f);
	return true;
}
//I—¹
void SceneMyGame::terminate()
{
	deleteObject(pBall);
}

//ˆ—
void SceneMyGame::execute()
{

}
//•`‰æ
void SceneMyGame::render()
{

}