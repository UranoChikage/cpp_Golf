#include "../../framework.h"
#include "../../framework/vn_environment.h"

#define DEDAULT_SLERP_RATE	(0.1f)
//初期化
bool SceneMyGame::initialize()
{
	pBall = new vnModel(L"data/model/primitive", L"cube_soft.vnm");
	vnCamera::setPosition(0.0f, 1.0f, -5.0f);
	vnCamera::setTarget(0.0f, 1.0f, 0.0f);
	return true;
}
//終了
void SceneMyGame::terminate()
{
	deleteObject(pBall);
}

//処理
void SceneMyGame::execute()
{

}
//描画
void SceneMyGame::render()
{

}