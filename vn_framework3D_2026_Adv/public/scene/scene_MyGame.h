#pragma once

class SceneMyGame : public vnScene
{
private:
	//=== Box ===
	vnModel* pBoxModel;
	BoxCollider* pBoxCollider;

	//=== Sphere ===
	vnModel* pSphereModel;
	SphereCollider* pSphereCollider;

	//=== Mesh ===
	vnModel* pMeshModel;
	MeshCollider* pMeshCollider;

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

	//MeshColliderの三角形をワイヤーフレームで描画する(コライダーの抜け・ズレ確認用)
	void DebugDrawMeshCollider(MeshCollider* pCollider, DWORD color);
	//球コライダーを3平面の円でワイヤーフレーム描画する
	void DebugDrawSphere(const XMVECTOR& center, float radius, DWORD color);



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

