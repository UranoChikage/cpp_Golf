#pragma once

class SceneMyGame : public vnScene
{
private:
	//=== Box ===
	vnModel* pBoxModel;
	BoxCollider* pBoxCollider;
	vnCollide::stSegment boxRay;

	//=== Sphere ===
	vnModel* pSphereModel;
	SphereCollider* pSphereCollider;
	vnCollide::stSegment sphereRay;

	//=== Mesh ===
	vnModel* pMeshModel;
	MeshCollider* pMeshCollider;
	vnCollide::stSegment meshRay;

	CollisionManager* pTerrainManager;
	CollisionManager* pObstacleManager;

	//=== Ball(仮テスト用、実体はBallsManagerが所有) ===
	Ball* pBall;
	vnModel* pBallModel; // ボールの見た目用

	//=== 地形(でこぼこ道) ===
	vnModel* pGroundModel;
	MeshCollider* pGroundCollider;

	//=== カメラ(注視点を中心にオービット) ===
	float radius;
	float theta;
	float phi;
	void updateCamera();

	void DebugDrawResult(const wchar_t* label, float y,
		const vnCollide::stSegment& ray, bool result, const XMVECTOR& hit, const XMVECTOR& nor);


public:
	//初期化
	bool initialize();
	//終了
	void terminate();

	//処理
	void execute();
	//描画
	void render();
};

