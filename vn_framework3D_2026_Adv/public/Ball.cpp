#include "../framework.h"
#include "../framework/vn_environment.h"

Ball::Ball(float radius, CollisionManager* terrainManager,
	CollisionManager* obstacleManager)
{

	shape = new SphereShape(radius, terrainManager, obstacleManager);
	pb = new PhysicsBody();
	pb->SetCollisionShape(shape);
	pb->SetOwner(this);

	isActive = true;//仮ですぐにゲーム開始状態にする
}
Ball::~Ball()
{
	delete pb;
	delete shape;
}
XMVECTOR Ball::GetPosition()const
{
	return *pb->getPosition();
}
void Ball::Shot(const XMVECTOR& dir, float pow)
{
	if (!isActive) return;
	float launchAngle = 0.8f; // 打ち出し角度（0.8は約45度）
	if (!pb->GetIsMoving())
		if (OnShotAdded)OnShotAdded(this);

	XMVECTOR up = XMVectorSet(0, 1, 0, 0);
	XMVECTOR launchDir = XMVector3Normalize(dir + up * launchAngle); // 少し上向きにして打ち出す

	pb->AddForce(launchDir * pow, ForceMode::Impulse);
}

void Ball::Deactivate()
{
	if (!isActive) return;
	isActive = false;
}