#include "../../framework.h"
#include "../../framework/vn_environment.h"

#define DEDAULT_SLERP_RATE	(0.1f)

//初期化関数
bool SceneGroundTest::initialize()
{
	pSky = new vnModel(L"data/model/", L"skydome.vnm");
	pSky->setRenderFlag(vnObject::eRenderFlag::Lighting, false);	//ライティング無効

	//シーンの基底クラスへ登録
	registerObject(pSky);

	//カメラ情報
	radius = 5.0f;	//半径
	theta = XMConvertToRadians(0.0f);	//平面角
	phi = XMConvertToRadians(15.0f);	//仰角

	//物理関連
	velocity = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	gravity = XMVectorSet(0.0f, -0.01f, 0.0f, 0.0f);

	pPlayer = NULL;
	memset(pMotion, 0, sizeof(pMotion));

	//プレイヤーアクション関連
	pastMotionTime = 0.0f;
	jumpForce = 0.25f;
	air = true;

	return true;
}

//終了関数
void SceneGroundTest::terminate()
{
	//基底クラスの登録から削除
	deleteObject(pPlayer);
	deleteObject(pGround);
	deleteObject(pSky);

	for (int i = 0; i < ePlayerMotion::MotionMax; i++)
	{
		delete[] pMotion[i];
	}
}

//処理関数
void SceneGroundTest::execute()
{
	if (playerModel == ePlayerModel::PlayerModel_None)
	{
		vnFont::print(100.0f, 100.0f, L"Select Player Model");
		vnFont::print(100.0f, 120.0f, L"1 : Quinn");
		vnFont::print(100.0f, 140.0f, L"2 : UnityChan");
		vnFont::print(100.0f, 160.0f, L"3 : BoxUnityChan");

		if (vnKeyboard::trg(DIK_1))
		{
			playerModel = ePlayerModel::Quinn;
		}
		else if (vnKeyboard::trg(DIK_2))
		{
			playerModel = ePlayerModel::UnityChan;
		}
		else if (vnKeyboard::trg(DIK_3))
		{
			playerModel = ePlayerModel::BoxUnityChan;
		}
		return;
	}
	if (groundModel == eGroundModel::GroundModel_None)
	{
		vnFont::print(100.0f, 120.0f, playerModel == ePlayerModel::Quinn ? 0xff00ffff : 0xffffffff, L"1 : Quinn");
		vnFont::print(100.0f, 140.0f, playerModel == ePlayerModel::UnityChan ? 0xff00ffff : 0xffffffff, L"2 : UnityChan");
		vnFont::print(100.0f, 160.0f, playerModel == ePlayerModel::BoxUnityChan ? 0xff00ffff : 0xffffffff, L"3 : BoxUnityChan");

		vnFont::setColor(0xffffffff);
		vnFont::print(500.0f, 100.0f, L"Select Stage Model");
		vnFont::print(500.0f, 120.0f, L"1 : Flat");
		vnFont::print(500.0f, 140.0f, L"2 : Noised");
		vnFont::print(500.0f, 160.0f, L"3 : Overpass");
		vnFont::print(500.0f, 180.0f, L"4 : Test Field");

		if (vnKeyboard::trg(DIK_1))
		{
			groundModel = eGroundModel::Flat;
		}
		else if (vnKeyboard::trg(DIK_2))
		{
			groundModel = eGroundModel::Noised;
		}
		else if (vnKeyboard::trg(DIK_3))
		{
			groundModel = eGroundModel::Overpass;
		}
		else if (vnKeyboard::trg(DIK_4))
		{
			groundModel = eGroundModel::TestField;
		}
		return;
	}
	if (pPlayer==NULL)
	{
		switch (playerModel)
		{
		case ePlayerModel::Quinn:
			pPlayer = new vnCharacter(L"data/model/Quinn_nolod/", L"Quinn_nolod.bone");

			pMotion[ePlayerMotion::Idle] = vnCharacter::loadMotionFile(L"data/model/Quinn_nolod/motion/MF_Idle.mot");
			pMotion[ePlayerMotion::Run] = vnCharacter::loadMotionFile(L"data/model/Quinn_nolod/motion/MF_Run_Fwd.mot");
			pMotion[ePlayerMotion::Jump] = vnCharacter::loadMotionFile(L"data/model/Quinn_nolod/motion/MM_Jump.mot");
			pMotion[ePlayerMotion::Fall] = vnCharacter::loadMotionFile(L"data/model/Quinn_nolod/motion/MM_Fall_Loop.mot");

			break;
		case ePlayerModel::UnityChan:
			pPlayer = new vnCharacter(L"data/model/unitychan/", L"unitychan.bone");

			pMotion[ePlayerMotion::Idle] = vnCharacter::loadMotionFile(L"data/model/unitychan/motion/WAIT00.mot");
			pMotion[ePlayerMotion::Run] = vnCharacter::loadMotionFile(L"data/model/unitychan/motion/RUN00_F.mot");
			pMotion[ePlayerMotion::Jump] = vnCharacter::loadMotionFile(L"data/model/unitychan/motion/JUMP00B.mot");
			pMotion[ePlayerMotion::Fall] = vnCharacter::loadMotionFile(L"data/model/unitychan/motion/JUMP01B.mot");

			break;
		case ePlayerModel::BoxUnityChan:
			pPlayer = new vnCharacter(L"data/model/BoxUnityChan/", L"BoxUnityChan.bone");

			pMotion[ePlayerMotion::Idle] = vnCharacter::loadMotionFile(L"data/model/unitychan/motion/WAIT00.mot");
			pMotion[ePlayerMotion::Run] = vnCharacter::loadMotionFile(L"data/model/unitychan/motion/RUN00_F.mot");
			pMotion[ePlayerMotion::Jump] = vnCharacter::loadMotionFile(L"data/model/unitychan/motion/JUMP00B.mot");
			pMotion[ePlayerMotion::Fall] = vnCharacter::loadMotionFile(L"data/model/unitychan/motion/JUMP01B.mot");

			break;
		}

		/*
		vnCharacterクラスのオブジェクト基本設定
		*/

		//再生するモーションデータがクオータニオンで作られているため、全パーツの回転値をクオータニオン指定モードにする
		pPlayer->useQuaternion(true);
		pPlayer->vnObject::useQuaternion(false);	//pPlayer自身の回転はオイラー角で指定できるようにクオータニオン指定モードを解除
		pPlayer->registerObject();	//自身を含めた全パーツをシーンに登録する関数を呼ぶ

		//初期モーション
		pPlayer->setMotion(pMotion[ePlayerMotion::Fall]);
	}
	if (pGround == NULL)
	{
		switch (groundModel)
		{
		case eGroundModel::Flat:
			pGround = new vnModel(L"data/model/", L"ground.vnm");
			break;
		case eGroundModel::Noised:
			pGround = new vnModel(L"data/model/", L"noised_ground.vnm");
			pGround->setScaleX(2.0f);
			pGround->setScaleZ(2.0f);
			break;
		case eGroundModel::Overpass:
			pGround = new vnModel(L"data/model/", L"overpass.vnm");
			break;
		case eGroundModel::TestField:
			pGround = new vnModel(L"data/model/", L"test_field.vnm");
			break;
		}
		registerObject(pGround);
	}

	if (air == true)
	{
		float currentMotionTime = pPlayer->getTime();
		float nextMotionTime = currentMotionTime + pPlayer->getPlaySpeed();
		if(nextMotionTime > pPlayer->getMotionLength())
		{
			if (pPlayer->getMotion() == pMotion[ePlayerMotion::Jump])
			{
				pPlayer->setMotion(pMotion[ePlayerMotion::Fall]);
			}
		}
	}
	if (air == false && vnKeyboard::trg(DIK_SPACE))
	{
		air = true;
		velocity = XMVectorSetY(velocity, jumpForce);
		pPlayer->setMotion(pMotion[ePlayerMotion::Jump]);
	}
	pastMotionTime = pPlayer->getTime();

	//入力があるか示すフラグ
	bool input = false;
	//移動スピード
	float speed = 0.1f;
	//移動ベクトル
	XMVECTOR vMove = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	//移動入力
	if (vnKeyboard::on(DIK_W))
	{
		vMove = XMVectorSetZ(vMove, 1.0f);
		input = true;
	}
	if (vnKeyboard::on(DIK_S))
	{
		vMove = XMVectorSetZ(vMove, -1.0f);
		input = true;
	}
	if (vnKeyboard::on(DIK_A))
	{
		vMove = XMVectorSetX(vMove, -1.0f);
		input = true;
	}
	if (vnKeyboard::on(DIK_D))
	{
		vMove = XMVectorSetX(vMove, 1.0f);
		input = true;
	}
	//移動ベクトルにスピードを適用(長さを変える)
	vMove = XMVectorScale(vMove, speed);

	//視線ベクトルを計算
	XMVECTOR cam_pos = *vnCamera::getPosition();
	XMVECTOR cam_trg = *vnCamera::getTarget();
	XMVECTOR eye = XMVectorSubtract(cam_trg, cam_pos);

	//Y軸周りの回転値を計算
	float cam_rotY = atan2f(XMVectorGetX(eye), XMVectorGetZ(eye));

	//カメラのY軸周り(theta)の回転値
	XMMATRIX mRotY = XMMatrixRotationY(cam_rotY);
	//移動ベクトルを回転させる
	vMove = XMVector3TransformNormal(vMove, mRotY);

	if (input == true)
	{
		//進行方向(方向ベクトル:vMove)に向ける
		float rotY = atan2f(XMVectorGetX(vMove), XMVectorGetZ(vMove));
		{
			rotY = slerpRotY(pPlayer->getRotationY(), rotY, DEDAULT_SLERP_RATE);
		}

		pPlayer->setRotationY(rotY);

		if(!air)pPlayer->setMotion(pMotion[ePlayerMotion::Run]);
	}
	else
	{
		if (!air)pPlayer->setMotion(pMotion[ePlayerMotion::Idle]);
	}
	
	//カメラの回転
	if (vnKeyboard::on(DIK_UP))
	{
		phi += XMConvertToRadians(1.0f);
	}
	if (vnKeyboard::on(DIK_DOWN))
	{
		phi -= XMConvertToRadians(1.0f);
	}
	if (vnKeyboard::on(DIK_LEFT))
	{
		theta += XMConvertToRadians(1.0f);
	}
	if (vnKeyboard::on(DIK_RIGHT))
	{
		theta -= XMConvertToRadians(1.0f);
	}

	//カメラ座標の計算
	//回転していない基準となる軸のベクトル
	XMVECTOR v = XMVectorSet(0.0f, 0.0f, -radius, 0.0f);
	//回転マトリクスを作成
	XMMATRIX rotate = XMMatrixRotationRollPitchYaw(phi, theta, 0.0f);

	//基準軸(v)のベクトルを回転させる(回転マトリクスを乗算する)
	v = XMVector3TransformNormal(v, rotate);

	XMVECTOR CamPos = v;

	//カメラの座標にプレイヤーの座標を加算
	CamPos = XMVectorAdd(CamPos, *pPlayer->getPosition());

	//少しだけ上にあげる
	XMVECTOR CamBias = XMVectorSet(0.0f, 1.5f, 0.0f, 0.0f);
	CamPos = XMVectorAdd(CamPos, CamBias);

	//カメラの座標
	vnCamera::setPosition(&CamPos);

	//カメラの注視点をプレイヤーの座標に設定
	XMVECTOR CamTrg = *pPlayer->getPosition();
	CamTrg = XMVectorAdd(CamTrg, CamBias);
	vnCamera::setTarget(&CamTrg);

	//地形衝突
	XMVECTOR pos, nor;
	int ret = -1;

	//進行方向
	if (input)
	{
		vnCollide::stSegment front;

		front.Dir = XMVector3Normalize(vMove);
		front.Pos = *pPlayer->getPosition() + XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
		front.Pos += front.Dir * 1.0f;
		front.Length = 0.5f;

		ret = isCollide(&pos, &nor, &front, pGround);
		if (ret != -1)
		{
			//衝突があった際の壁ずり処理
				
			//http://marupeke296.com/COL_Basic_No5_WallVector.html
			vMove = vMove - XMVector3Dot(vMove, nor) * nor;
		}
		//移動ベクトルをプレイヤーの座標に加算
		pPlayer->addPosition(&vMove);
	}

	//物理挙動
	velocity += gravity;
	pPlayer->addPosition(&velocity);

	//リスポーン処理
	if (pPlayer->getPositionY() < -30.0f)
	{	//プレイヤーが一定以上落下すると原点に復帰させる
		pPlayer->setMotion(pMotion[ePlayerMotion::Fall]);
		pPlayer->setPosition(0.0f, 0.0f, 0.0f);
		velocity = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
		air = true;
	}

	//地面
	vnCollide::stSegment seg;

	seg.Pos = *pPlayer->getPosition() + XMVectorSet(0.0f, 0.5f, 0.0f, 0.0f);
	seg.Dir = XMVectorSet(0.0f, -1.0f, 0.0f, 0.0f);
	seg.Length = 0.6f;

	ret = isCollide(&pos, &nor, &seg, pGround);

	if (ret != -1)
	{
		float posY = XMVectorGetY(pos);
		pPlayer->setPositionY(posY);

		velocity = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
		air = false;
	}
	else if(air==false)
	{
		pPlayer->setMotion(pMotion[ePlayerMotion::Fall]);
		air = true;
	}



	vnDebugDraw::Grid();
	vnDebugDraw::Axis();

	//デバッグ情報
	vnFont::print(20.0f, 20.0f,	L"Player Pos :(%.3f, %.3f, %.3f)", pPlayer->getPositionX(),	pPlayer->getPositionY(), pPlayer->getPositionZ());
	vnFont::print(20.0f, 40.0, L"Player RotY : %.3f",	pPlayer->getRotationY() * 180.0f / 3.141592f);
	vnFont::print(20.0f, 60.0, L"Player Motion : %.0f / %.0f", pPlayer->getTime(), pPlayer->getMotionLength());
	vnFont::print(20.0f, 80.0, air ? L"Player Air : true" : L"Player Air : false");
	vnFont::print(20.0f, 100.0f, L"Camera Radius : %.3f", radius);
	vnFont::print(20.0f, 120.0f, L"Camera Theta : %.3f", theta * 180.0f / 3.141592f);
	vnFont::print(20.0f, 140.0f, L"Camera Phi : %.3f", phi * 180.0f / 3.141592f);

	vnScene::execute();
}

float SceneGroundTest::slerpRotY(float src, float dst, float rate)
{
	XMMATRIX mSrc, mDst, mAns;
	XMVECTOR qSrc, qDst, qAns;

	mSrc = XMMatrixRotationY(src);
	mDst = XMMatrixRotationY(dst);

	qSrc = XMQuaternionRotationRollPitchYaw(0.0f, src, 0.0f);
	qDst = XMQuaternionRotationRollPitchYaw(0.0f, dst, 0.0f);

	qAns = XMQuaternionSlerp(qSrc, qDst, rate);

	mAns = XMMatrixRotationQuaternion(qAns);

	return atan2f(-XMVectorGetZ(mAns.r[0]), XMVectorGetZ(mAns.r[2]));
}

int SceneGroundTest::isCollide(XMVECTOR* hit, XMVECTOR* nor, vnCollide::stSegment* seg, vnModel* p)
{
	vnCollide::stTriangle tri;

	int vnum = p->getVertexNum();
	unsigned short* idx = p->getIndex();
	vnVertex3D* vtx = p->getVertex();

	vnModel_MeshData* pMesh = p->getMesh();
	int meshNum = p->getMeshNum();
	int hitMeshID = -1;
	for (int i = 0; i < meshNum; i++)
	{
		int idxnum = pMesh[i].IndexNum;
		int s_idx = pMesh[i].StartIndex;
		for (int j = 0; j < idxnum; j += 3)
		{
			XMVECTOR v1 = XMVectorSet(vtx[idx[s_idx + j + 0]].x, vtx[idx[s_idx + j + 0]].y, vtx[idx[s_idx + j + 0]].z, 0.0f);
			XMVECTOR v2 = XMVectorSet(vtx[idx[s_idx + j + 1]].x, vtx[idx[s_idx + j + 1]].y, vtx[idx[s_idx + j + 1]].z, 0.0f);
			XMVECTOR v3 = XMVectorSet(vtx[idx[s_idx + j + 2]].x, vtx[idx[s_idx + j + 2]].y, vtx[idx[s_idx + j + 2]].z, 0.0f);

			v1 = XMVector3TransformCoord(v1, *p->getWorld());
			v2 = XMVector3TransformCoord(v2, *p->getWorld());
			v3 = XMVector3TransformCoord(v3, *p->getWorld());

			tri.fromPoints(&v1, &v2, &v3);
			bool result = vnCollide::isCollide(hit, seg, &tri);

			if (result == false)continue;

			if (nor)*nor = tri.plane.Normal;

			hitMeshID = i;
			return hitMeshID;
		}
	}
	return hitMeshID;
}

//描画関数
void SceneGroundTest::render()
{
	vnScene::render();
}
