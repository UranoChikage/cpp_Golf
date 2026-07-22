#pragma once

class SphereShape : public ICollisionShape
{
private:
	//円の半径
	float radius = 1.0f;
	//レイキャストパラメーター
	float rayOriginHeight = 0.1f; // 地形の高さを測るためのレイの開始高さ
	float rayMaxDistance = 100.0f; // 地形の高さを測るためのレイの最大距離
	CollisionManager* terrainManager = nullptr; // 地形コライダーを管理してるManager
	CollisionManager* obstacleManager = nullptr; // 障害物コライダーを管理してるManager

	XMMATRIX accumulatedRotation = XMMatrixIdentity();

public:
	SphereShape(float r, CollisionManager* terrain, CollisionManager* obstacle);

	float GetRadius() const { return radius; }
	XMMATRIX GetRotation() const { return accumulatedRotation; }

	Contact QueryTerrain(const XMFLOAT3* pos) override;
	bool QueryObstacle(const XMFLOAT3* pos, std::vector<Contact>& outContact) override;
	void UpdateOrientation(const XMFLOAT3* deltapos, const XMFLOAT3* contactNomal) override;
};
