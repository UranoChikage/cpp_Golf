#include "../framework.h"
#include "../framework/vn_environment.h"

SphereShape::SphereShape(float r, CollisionManager* terrain, CollisionManager* obstacle)
{
	radius = r;
	terrainManager = terrain;
	obstacleManager = obstacle;
}

Contact SphereShape::QueryTerrain(const XMFLOAT3* pos)
{
	Contact contact;

	XMVECTOR normal;
	float floorY;

	if (MatrixMath::GetHitInfo(terrainManager, pos->x, pos->z,
		rayOriginHeight, rayMaxDistance, &floorY, &normal))
	{
		//めり込み具合
	//地形衝突判定
		float groundY = floorY + radius;
		float penetrationY = groundY - pos->y;
		contact.penetration = penetrationY;

		//接地判定
		bool hit = penetrationY >= 0;
		contact.isHit = hit;
	}

	XMVECTOR pos_v = XMLoadFloat3(pos);
	//接地点
	XMVECTOR point = pos_v - normal * radius;
	XMStoreFloat3(&contact.point, point);
	XMStoreFloat3(&contact.normal, normal);
	return contact;
}
bool SphereShape::QueryObstacle(const XMFLOAT3* pos, std::vector<Contact>& outContact)
{
	XMVECTOR center = XMLoadFloat3(pos);
	//obstacleManagerが持ってる全コライダーとの重なりをContactにまとめて詰めてもらう(CollisionManager::OverlapSphere)
	return obstacleManager->OverlapSphere(center, radius, outContact);
}
void SphereShape::UpdateOrientation(const XMFLOAT3* deltapos, const XMFLOAT3* contactNomal)
{
	XMVECTOR deltapos_v = XMLoadFloat3(deltapos);
	XMVECTOR normal_v = XMLoadFloat3(contactNomal);

	//移動量から回転を計算
	float movedistance = XMVectorGetX(XMVector3Length(deltapos_v));
	if (movedistance < 0.001f) return;

	float rollAngleDeg = XMConvertToDegrees(movedistance / radius); // ボールが転がった角度
	//回転軸を求めて回転行列を作る
	XMVECTOR rollAxis = XMVector3Cross(normal_v, XMVector3Normalize(deltapos_v)); // 転がる方向に対して垂直な軸
	XMMATRIX rollMatrix = MatrixMath::RotationAxis(rollAxis, rollAngleDeg);

	accumulatedRotation = rollMatrix * accumulatedRotation; // 回転を蓄積
}
