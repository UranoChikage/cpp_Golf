#include "../framework.h"
#include "../framework/vn_environment.h"

void CameraController::SetTarget(XMVECTOR* targetPos)
{
	target = targetPos;
}
void CameraController::CameraUpdate(const float deltaTime)
{
	if (target == nullptr) return;

	dirCon.DirUpdate();

	XMVECTOR offset;

	{
		XMVECTOR v = XMVectorSet(0, 2, 0, 0);
		// ターゲットの後ろ上からカメラを配置
		offset = XMVector3Normalize(-dirCon.GetDirection()) * 20.0f + v;
	}
	XMVECTOR desiredPos = *target + offset;

	// スムーズに移動
	XMVECTOR newPos = XMVectorLerp(*vnCamera::getPosition(), desiredPos, deltaTime * 5.0f);
	vnCamera::setPosition(&newPos);

	{
		XMVECTOR v = XMVectorSet(0, 1, 0, 0);
		// lookAt的なの
		XMVECTOR lookTarget = *target + v;
		vnCamera::setTarget(&lookTarget);
	}
}