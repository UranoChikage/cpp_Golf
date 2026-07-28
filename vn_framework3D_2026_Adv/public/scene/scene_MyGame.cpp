#include "../../framework.h"
#include "../../framework/vn_environment.h"

#define DEDAULT_SLERP_RATE	(0.1f)
//初期化
bool SceneMyGame::initialize()
{
	pSky = new vnModel(L"data/model/", L"skydome.vnm");
	pSky->setRenderFlag(vnObject::eRenderFlag::Lighting, false);	//ライティング無効
	//シーンの基底クラスへ登録
	registerObject(pSky);


	pTerrainManager = new CollisionManager();
	pObstacleManager = new CollisionManager();


	//=== Box (x=-3) ===
	pBoxModel = new vnModel(L"data/model/primitive/", L"cube_soft.vnm");
	pBoxModel->setPosition(-3.0f, 0.0f, 0.0f);
	pBoxModel->setScale(1.0f, 1.0f, 3.0f);
	pBoxModel->setRotationZ(45);
	registerObject(pBoxModel);

	pBoxCollider = new BoxCollider(*pBoxModel->getPosition(), *pBoxModel->getScale() * 1.0f);
	pBoxCollider->SetRotate(*pBoxModel->getRotation());
	pObstacleManager->Add(pBoxCollider);

	//=== Sphere (x=0) ===
	pSphereModel = new vnModel(L"data/model/primitive/", L"sphere.vnm");
	pSphereModel->setPosition(0.0f, 0.0f, 0.0f);
	pSphereModel->setScale(1.0f, 1.0f, 1.0f);
	registerObject(pSphereModel);

	pSphereCollider = new SphereCollider(*pSphereModel->getPosition(), 1.0f);
	pObstacleManager->Add(pSphereCollider);

	//=== Mesh (x=3) ===
	pMeshModel = new vnModel(L"data/model/primitive/", L"cone.vnm");
	pMeshModel->setPosition(3.0f, 0.0f, 0.0f);
	pMeshModel->setScale(1.0f, 1.0f, 1.0f);
	registerObject(pMeshModel);
	pMeshModel->postExecute(); // World行列を確定させてからTrianglesを構築する

	pMeshCollider = new MeshCollider(MeshCollider::BuildTriangles(pMeshModel));
	pObstacleManager->Add(pMeshCollider);

	//=== 床　===
	pGroundModel = new vnModel(L"data/model/", L"noised_ground.vnm");
	pGroundModel->setScaleX(3.0f);
	pGroundModel->setScaleZ(3.0f);
	registerObject(pGroundModel);
	pGroundModel->postExecute(); // World行列を確定させてからTrianglesを構築する
	//pGroundModel->setRenderEnable(false); // 見た目のメッシュを消してコライダーのワイヤーフレームだけ見たい時はコメントアウトを外す

	pGroundCollider = new MeshCollider(MeshCollider::BuildTriangles(pGroundModel));
	pTerrainManager->Add(pGroundCollider);

	//=== Ball===
	pBall = BallsManager::GetInstance().AddBall(0, 1.0f, pTerrainManager, pObstacleManager);
	pBall->GetPhysicsBody()->setPosition(-3.0f, 3.0f, 1.0f);

	pBallModel = new vnModel(L"data/model/primitive/", L"sphere.vnm");
	pBallModel->setScale(1.0f, 1.0f, 1.2f);
	pBallModel->useQuaternion(true);
	//pBallModel->setRenderEnable(false);
	registerObject(pBallModel);

	//=== カメラ(DirectionController) ===
	pCamCon = new CameraController(&DirCon);
	ballPos = pBall->GetPosition();
	pCamCon->SetTarget(&ballPos);
	pCamCon->CameraUpdate(1.0f); // 最初のフレームでいきなりターゲット位置まで寄せておく

	//===　ショット操作(DirectionController) ===
	pShotInput = new ShotInput(pBall, &DirCon);

	//=== パーティクル ===
	vnEmitter::stEmitterDesc shotDesc;
	swprintf_s(shotDesc.Texture, L"%s", L"data/image/particle/particle006.png");
	shotDesc.ColorMax = XMVectorSet(1.0f, 1.0f, 0.5f, 1.0f);
	pShotEmitter = new vnEmitter(&shotDesc);
	pShotEmitter->SetMaxSize(3.0f);
	pShotEmitter->SetSpeed(0.0f); //その場にポンと出したいのでスピードは殺す
	pShotEmitter->SetGrowSize(true); //徐々に大きくする
	pShotEmitter->SetFadeAlpha(false);
	pShotEmitter->setParent(pBallModel);
	pShotEmitter->setEmit(false);
	registerObject(pShotEmitter);

	vnEmitter::stEmitterDesc hitDesc;
	swprintf_s(hitDesc.Texture, L"%s", L"data/image/particle/particle005.png");
	hitDesc.ColorMax = XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);
	pHitEmitter = new vnEmitter(&hitDesc);
	pHitEmitter->setParent(pBallModel);
	pHitEmitter->setEmit(false);
	registerObject(pHitEmitter);

	//=== 音 ===
	pShotSE = new vnSound(L"data/sound/cursor4.wav");
	pHitSE = new vnSound(L"data/sound/hit.wav");

	//ショットした瞬間にパーティクルとSEを1個だけバーストさせる(見た目はSet〜で設定済み)
	pBall->OnShotAdded = [this](IBall*)
		{
			pShotEmitter->setEmit(true, 1);
			pShotSE->play();
		};
	//地形/障害物にヒットした瞬間にパーティクルとSEをバーストさせる
	pBall->GetPhysicsBody()->OnBounce = [this](const XMVECTOR&, const XMVECTOR&)
		{
			hitEmitTimer = 0.1f;
			pHitEmitter->setEmit(true);
			pHitSE->play();
		};



	return true;
}
//終了
void SceneMyGame::terminate()
{
	deleteObject(pBoxModel);
	deleteObject(pSphereModel);
	deleteObject(pMeshModel);
	deleteObject(pBallModel);
	deleteObject(pGroundModel);
	deleteObject(pShotEmitter);
	deleteObject(pHitEmitter);
	deleteObject(pSky);

	delete pShotSE;
	delete pHitSE;
	delete pTerrainManager;
	delete pObstacleManager;
	delete pCamCon;
	delete pShotInput;
	BallsManager::GetInstance().RemoveBall(0);
}

//処理
void SceneMyGame::execute()
{
	vnDebugDraw::Grid();
	vnDebugDraw::Axis();

	//=== 床のコライダーを可視化 ===
	//DebugDrawMeshCollider(pGroundCollider, 0xff00ffff); // 黄色：地面のMeshCollider

	//=== Ball===
	pBall->GetPhysicsBody()->Step(1.0f / 60.0f);
	ballPos = pBall->GetPosition();
	pBallModel->setPosition(&ballPos);
	XMVECTOR q = pBall->GetRotate();
	pBallModel->setQuaternion(&q);
	vnFont::print(20.0f, 60.0f, L"Ball Pos(%.2f,%.2f,%.2f)",
		XMVectorGetX(ballPos), XMVectorGetY(ballPos), XMVectorGetZ(ballPos));

	//=== Ballのコライダー(SphereShape)を可視化 ===
	DebugDrawSphere(ballPos, pBall->GetRadius(), 0xffff00ff); // マゼンタ：Ballのコライダー

	//=== カメラ(マウス操作) ===
	pCamCon->CameraUpdate(1.0f / 60.0f);

	//===　ショット操作(DirectionController) ===
	pShotInput->ShotInputUpdate(1.0f / 60.0f);
	vnFont::print(20.0f, 80.0f, L"Power:%.0f%%", pShotInput->GetMeterValue() * 100.0f);
	vnFont::print(20.0f, 100.0f, L"IsMoving:%d", pBall->GetPhysicsBody()->GetIsMoving());

	//=== パーティクルのバースト時間経過管理 ===
	if (hitEmitTimer > 0.0f)
	{
		hitEmitTimer -= 1.0f / 60.0f;
		if (hitEmitTimer <= 0.0f) pHitEmitter->setEmit(false);
	}

	vnScene::execute();
}
//描画
void SceneMyGame::render()
{
	vnScene::render();
	UIManager::GetInstance().Render();
}

void SceneMyGame::DebugDrawMeshCollider(MeshCollider* pCollider, DWORD color)
{
	const auto& triangles = pCollider->GetTriangles();
	for (const auto& tri : triangles)
	{
		vnDebugDraw::Line(&tri.v[0], &tri.v[1], color);
		vnDebugDraw::Line(&tri.v[1], &tri.v[2], color);
		vnDebugDraw::Line(&tri.v[2], &tri.v[0], color);
	}
}

void SceneMyGame::DebugDrawSphere(const XMVECTOR& center, float radius, DWORD color)
{
	const int segments = 24;
	float cx = XMVectorGetX(center);
	float cy = XMVectorGetY(center);
	float cz = XMVectorGetZ(center);

	for (int i = 0; i < segments; i++)
	{
		float a0 = XM_2PI * i / segments;
		float a1 = XM_2PI * (i + 1) / segments;

		//XZ平面(横から見た輪郭)
		vnDebugDraw::Line(
			cx + cosf(a0) * radius, cy, cz + sinf(a0) * radius,
			cx + cosf(a1) * radius, cy, cz + sinf(a1) * radius, color);

		//XY平面
		vnDebugDraw::Line(
			cx + cosf(a0) * radius, cy + sinf(a0) * radius, cz,
			cx + cosf(a1) * radius, cy + sinf(a1) * radius, cz, color);

		//YZ平面
		vnDebugDraw::Line(
			cx, cy + cosf(a0) * radius, cz + sinf(a0) * radius,
			cx, cy + cosf(a1) * radius, cz + sinf(a1) * radius, color);
	}
}
