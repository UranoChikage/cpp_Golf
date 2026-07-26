#include "../framework.h"
#include "../framework/vn_environment.h"

DirectionController::DirectionController()
{
	sensitivity = 0.05f;
	yaw = 0.0f;
}
void  DirectionController::DirUpdate()
{
	float mx = (float)vnMouse::getDX(); // 前フレームからの移動量(絶対座標のgetXだと止まってても回り続けた為)
	yaw += -mx * sensitivity; // 累積の回転量を更新
	//行列を使ってdirを更新
	XMMATRIX rotY = MatrixMath::RotationY(yaw); 
	XMVECTOR forward = XMVectorSet(0, 0, 1, 0);
	dir = MatrixMath::MultiplyVector(rotY, forward); // 回転行列をdirに適用
}