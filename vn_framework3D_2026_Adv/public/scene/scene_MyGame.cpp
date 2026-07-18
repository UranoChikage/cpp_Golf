#include "../../framework.h"
#include "../../framework/vn_environment.h"

#define DEDAULT_SLERP_RATE	(0.1f)
//初期化
bool SceneMyGame::initialize()
{
	pCollisionManager = new CollisionManager();

	//=== Box (x=-3) ===
	pBoxModel = new vnModel(L"data/model/primitive/", L"cube_soft.vnm");
	pBoxModel->setPosition(-3.0f, 0.0f, 0.0f);
	pBoxModel->setScale(1.0f, 1.0f, 1.0f);
	registerObject(pBoxModel);

	pBoxCollider = new BoxCollider(*pBoxModel->getPosition(), *pBoxModel->getScale() * 0.5f);
	pCollisionManager->Add(pBoxCollider);

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

	pSphereCollider = new SphereCollider(*pSphereModel->getPosition(), 0.5f);
	pCollisionManager->Add(pSphereCollider);

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
	pCollisionManager->Add(pMeshCollider);

	{
		XMVECTOR from = XMVectorSet(3.0f,1.0f, -2.0f, 1.0f);
		XMVECTOR to = XMVectorSet(3.0f, 1.0f, 2.0f, 1.0f);
		meshRay.fromPoints(&from, &to);
	}

	radius = 8.0f;
	theta = 0.0f; // Y軸回転(度数)
	phi = 20.0f;  // X軸回転(度数)
	updateCamera();
	return true;
}
//終了
void SceneMyGame::terminate()
{
	deleteObject(pBoxModel);
	deleteObject(pSphereModel);
	deleteObject(pMeshModel);
	delete pCollisionManager;
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

	updateCamera();

	vnDebugDraw::Grid();
	vnDebugDraw::Axis();

	vnFont::print(20.0f, 0, L"青色の線はRay");
	DebugDrawResult(L"Box", 20.0f, boxRay, boxResult, boxHit, boxNor);
	DebugDrawResult(L"Sphere", 40.0f, sphereRay, sphereResult, sphereHit, sphereNor);
	DebugDrawResult(L"Mesh", 60.0f, meshRay, meshResult, meshHit, meshNor);

	vnScene::execute();
}
//描画
void SceneMyGame::render()
{
	vnScene::render();
}

void SceneMyGame::updateCamera()
{
	//矢印キーでオービット回転(度数で加算)
	if (vnKeyboard::on(DIK_UP))
	{
		phi += 1.0f;
	}
	if (vnKeyboard::on(DIK_DOWN))
	{
		phi -= 1.0f;
	}
	if (vnKeyboard::on(DIK_LEFT))
	{
		theta += 1.0f;
	}
	if (vnKeyboard::on(DIK_RIGHT))
	{
		theta -= 1.0f;
	}

	//注視点(原点)からradius離れた位置を、theta/phiで回転させてカメラ座標を求める
	XMVECTOR v = XMVectorSet(0.0f, 0.0f, -radius, 0.0f);
	XMMATRIX rotate = MatrixMath::RotationX(phi) * MatrixMath::RotationY(theta);
	v = MatrixMath::MultiplyVector(rotate, v);

	vnCamera::setPosition(&v);

	XMVECTOR target = XMVectorZero();
	vnCamera::setTarget(&target);
}

void SceneMyGame::DebugDrawResult(const wchar_t* label, float y,
	const vnCollide::stSegment& ray, bool result, const XMVECTOR& hit, const XMVECTOR& nor)
{
	XMVECTOR rayEnd = ray.Pos + ray.Dir * ray.Length;
	vnDebugDraw::Line(&ray.Pos, &rayEnd, 0xffff0000); // 赤：Ray

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
