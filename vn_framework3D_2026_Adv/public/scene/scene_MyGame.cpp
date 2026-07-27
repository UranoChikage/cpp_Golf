#include "../../framework.h"
#include "../../framework/vn_environment.h"

#define DEDAULT_SLERP_RATE	(0.1f)
//初期化
bool SceneMyGame::initialize()
{
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

	{
		XMVECTOR from = XMVectorSet(-3.0f, 0.0f, -2.0f, 1.0f);
		XMVECTOR to = XMVectorSet(-3.0f, 0.0f, 2.0f, 1.0f);
		boxRay.fromPoints(&from, &to);
	}

	//=== Sphere (x=0) ===
	pSphereModel = new vnModel(L"data/model/primitive/", L"sphere.vnm");
	pSphereModel->setPosition(0.0f, 0.0f, 0.0f);
	pSphereModel->setScale(1.0f, 1.0f, 1.0f);
	registerObject(pSphereModel);

	pSphereCollider = new SphereCollider(*pSphereModel->getPosition(), 1.0f);
	pObstacleManager->Add(pSphereCollider);

	{
		XMVECTOR from = XMVectorSet(0.0f, 0.0f, -2.0f, 1.0f);
		XMVECTOR to = XMVectorSet(0.0f, 0.0f, 2.0f, 1.0f);
		sphereRay.fromPoints(&from, &to);
	}

	//=== Mesh (x=3) ===
	pMeshModel = new vnModel(L"data/model/primitive/", L"cone.vnm");
	pMeshModel->setPosition(3.0f, 0.0f, 0.0f);
	pMeshModel->setScale(1.0f, 1.0f, 1.0f);
	registerObject(pMeshModel);
	pMeshModel->postExecute(); // World行列を確定させてからTrianglesを構築する

	pMeshCollider = new MeshCollider(MeshCollider::BuildTriangles(pMeshModel));
	pObstacleManager->Add(pMeshCollider);

	{
		XMVECTOR from = XMVectorSet(3.0f, 1.0f, -2.0f, 1.0f);
		XMVECTOR to = XMVectorSet(3.0f, 1.0f, 2.0f, 1.0f);
		meshRay.fromPoints(&from, &to);
	}

	//=== でこぼこ ===
	pGroundModel = new vnModel(L"data/model/", L"noised_ground.vnm");
	pGroundModel->setScaleX(2.0f);
	pGroundModel->setScaleZ(2.0f);
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
	delete pTerrainManager;
	delete pObstacleManager;
	delete pCamCon;
	delete pShotInput;
	BallsManager::GetInstance().RemoveBall(0);
}

//処理
void SceneMyGame::execute()
{
	XMVECTOR boxHit, boxNor;
	bool boxResult = pBoxCollider->IsCollide(boxRay, &boxHit, &boxNor);

	XMVECTOR sphereHit, sphereNor;
	bool sphereResult = pSphereCollider->IsCollide(sphereRay, &sphereHit, &sphereNor);

	XMVECTOR meshHit, meshNor;
	bool meshResult = pMeshCollider->IsCollide(meshRay, &meshHit, &meshNor);

	vnDebugDraw::Grid();
	vnDebugDraw::Axis();

	//=== 床(でこぼこ地面)のコライダーを可視化 ===
	//DebugDrawMeshCollider(pGroundCollider, 0xff00ffff); // 黄色：地面のMeshCollider

	vnFont::print(20.0f, 0, L"赤色の線はRay");
	DebugDrawResult(L"Box", 20.0f, boxRay, boxResult, boxHit, boxNor);
	DebugDrawResult(L"Sphere", 40.0f, sphereRay, sphereResult, sphereHit, sphereNor);
	DebugDrawResult(L"Mesh", 60.0f, meshRay, meshResult, meshHit, meshNor);

	//=== Ball===
	pBall->GetPhysicsBody()->Step(1.0f / 60.0f);
	ballPos = pBall->GetPosition();
	pBallModel->setPosition(&ballPos);
	XMVECTOR q = pBall->GetRotate();
	pBallModel->setQuaternion(&q);
	vnFont::print(20.0f, 80.0f, L"Ball Pos(%.2f,%.2f,%.2f)",
		XMVectorGetX(ballPos), XMVectorGetY(ballPos), XMVectorGetZ(ballPos));

	//=== Ballのコライダー(SphereShape)を可視化 ===
	DebugDrawSphere(ballPos, pBall->GetRadius(), 0xffff00ff); // マゼンタ：Ballのコライダー

	//=== カメラ(マウス操作) ===
	pCamCon->CameraUpdate(1.0f / 60.0f);

	//===　ショット操作(DirectionController) ===
	pShotInput->ShotInputUpdate(1.0f / 60.0f);
	vnFont::print(20.0f, 100.0f, L"Power:%.0f%%", pShotInput->GetMeterValue() * 100.0f);


	vnScene::execute();
}
//描画
void SceneMyGame::render()
{
	vnScene::render();
}

void SceneMyGame::DebugDrawResult(const wchar_t* label, float y,
	const vnCollide::stSegment& ray, bool result, const XMVECTOR& hit, const XMVECTOR& nor)
{
	XMVECTOR rayEnd = ray.Pos + ray.Dir * ray.Length;
	//A,B,G,Rの順で指定できる
	vnDebugDraw::Line(&ray.Pos, &rayEnd, 0xff0000ff); // 赤：Ray

	if (result)
	{
		XMVECTOR norEnd = hit + nor * 1.0f;
		vnDebugDraw::Line(&hit, &norEnd, 0xff00ff00); // 緑：法線
	}

	vnFont::print(20.0f, y, L"%s Hit:%d Pos(%.2f,%.2f,%.2f) Nor(%.2f,%.2f,%.2f)",
		label, result,
		XMVectorGetX(hit), XMVectorGetY(hit), XMVectorGetZ(hit),
		XMVectorGetX(nor), XMVectorGetY(nor), XMVectorGetZ(nor));
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
