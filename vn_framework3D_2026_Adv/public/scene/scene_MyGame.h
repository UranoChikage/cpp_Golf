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
	XMVECTOR ballPos; // カメラのターゲットとしてアドレスを渡し続けるためメンバで持つ

	//=== 地形(でこぼこ道) ===
	vnModel* pGroundModel;
	MeshCollider* pGroundCollider;



	//=== 方向管理(マウス操作) ===
	DirectionController DirCon;
	//=== カメラ(DirectionController) ===
	CameraController* pCamCon;

	//===　ショット操作(DirectionController) ===
	ShotInput *pShotInput;

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

