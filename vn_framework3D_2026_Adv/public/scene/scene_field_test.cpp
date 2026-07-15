#include "../../framework.h"
#include "../../framework/vn_environment.h"

//extern stMotion motion_idle;
//extern stMotion motion_walk;

//初期化関数
bool SceneFieldTest::initialize()
{
	//モデルデータの作成
	pPlayer = new vnCharacter(L"data/model/bear_part/", L"bear_part.bone");
	pShadow = new vnModel(L"data/model/", L"shadow.vnm");
	pShadow->setRenderFlag(vnObject::eRenderFlag::Transparent, true);	//半透明

	pGround = new vnModel(L"data/model/", L"ground.vnm");

	pSky = new vnModel(L"data/model/", L"skydome.vnm");
	pSky->setRenderFlag(vnObject::eRenderFlag::Lighting, false);	//ライティング無し

	pNPC = new vnCharacter(L"data/model/bear_part/", L"bear_part.bone");
	pNPCShadow = new vnModel(L"data/model/", L"shadow.vnm");
	pNPCShadow->setRenderFlag(vnObject::eRenderFlag::Transparent, true);	//半透明

	pDebugFan = new vnModel(L"data/model/", L"debug_fan.vnm");
	pDebugFan->setRenderFlag(vnObject::eRenderFlag::Transparent, true);
	
	//適当な位置に初期配置
	pNPC->setPosition(5.0f, 0.0f, 5.0f);

	pCrown = new vnModel(L"data/model/", L"crown.vnm");
	//階層構造の設定
	pCrown->setParent(pPlayer->getParts(L"head"));
	//位置調整(親からの)
	pCrown->setPositionY(1.3f);
	pCrown->setScale(0.2f, 0.2f, 0.2f);

	//フェンスの配置
	for (int i = 0; i < FENCE_NUM; i++)
	{
		//フェンスモデルを作成
		pFence[i] = new vnModel(L"data/model/", L"fence.vnm");
		//配置角度
		float degree = (float)i * 360.0f / (float)FENCE_NUM;
		//Degree -> Radian変換
		float radian = degree * 3.141592f / 180.0f;
		//極座標 -> 直交座標
		//※角度(rotY)と配置(posXZ)が一致するようにx=sin,z=cosにする
		float x = sinf(radian) * FENCE_RADIUS;
		float z = cosf(radian) * FENCE_RADIUS;

		//フェンスに姿勢情報を設定
		pFence[i]->setPosition(x, 0.0f, z);
		pFence[i]->setRotationY(radian);
		//フェンスをシーンに登録
		registerObject(pFence[i]);
	}

	//ビルボードの作成(幅、高さ、画像パス)
	pBillboard = new vnBillboard(1.0f, 1.0f, L"data/image/icon_exc.png");
	pBillboard->setPositionY(3.5f);

	pDebugFan->setParent(pNPC);
	pBillboard->setParent(pNPC);
	//エミッターの作成
//設定構造体
	vnEmitter::stEmitterDesc desc;
	swprintf_s(desc.Texture, L"%s", L"data/image/particle/particle002.png");
	desc.ColorMax = XMVectorSet(0.9f, 0.5f, 0.4f, 1.0f);
	pEmitter = new vnEmitter(&desc);


	swprintf_s(desc.Texture, L"%s", L"data/image/particle/particle004.png");
	desc.ColorMax = XMVectorSet(0.0f, 0.0f, 1.0f, 1.0f);
	pEmitterNPC = new vnEmitter(&desc);

	pEmitter->setParent(pPlayer);
	pEmitterNPC->setParent(pNPC);
	pEmitter->setEmit(false);
	pEmitterNPC->setEmit(false);

	//オブジェクトの登録(不透明→半透明)
	registerObject(pGround);
	registerObject(pSky);

	pPlayer->registerObject();	//オブジェクト内に含まれるパーツもすべて登録するためクラスに用意された関数を呼び出す
	pNPC->registerObject();		//オブジェクト内に含まれるパーツもすべて登録するためクラスに用意された関数を呼び出す

	registerObject(pCrown);
	//半透明のオブジェクトは不透明オブジェクトの後に描画
	registerObject(pShadow);
	registerObject(pNPCShadow);
	registerObject(pDebugFan);
	registerObject(pBillboard);
	registerObject(pEmitter);
	registerObject(pEmitterNPC);

	//三人称視点プレイヤーカメラの初期値
	radius = 10.0f;
	theta = 0.0f;
	phi = 0.0f;

	//モーションファイルの読み込み
	pMotionIdle = vnCharacter::loadMotionFile(L"data/model/bear_part/motion/motion_idle.mot");
	pMotionWalk = vnCharacter::loadMotionFile(L"data/model/bear_part/motion/motion_walk.mot");

	//初期モーション
	pPlayer->setMotion(pMotionIdle);
	pNPC->setMotion(pMotionIdle);
	
	return true;
}


//終了関数
void SceneFieldTest::terminate()
{
	//シーンの登録から削除
	for (int i = 0; i < pPlayer->getPartsNum(); i++)
	{
		deleteObject(pPlayer->getParts(i));
	}
	deleteObject(pPlayer);
	for (int i = 0; i < pNPC->getPartsNum(); i++)
	{
		deleteObject(pNPC->getParts(i));
	}
	deleteObject(pNPC);
	deleteObject(pShadow);
	deleteObject(pNPCShadow);
	deleteObject(pGround);
	deleteObject(pSky);
	deleteObject(pCrown);
	for (int i = 0; i < FENCE_NUM; i++)
	{
		deleteObject(pFence[i]);
	}
	deleteObject(pDebugFan);
	deleteObject(pBillboard);
	deleteObject(pEmitter);
	deleteObject(pEmitterNPC);

	delete[] pMotionIdle;
	delete[] pMotionWalk;

}

//処理関数
void SceneFieldTest::execute()
{
	//プレイヤーの移動量
	bool input = false;
	XMVECTOR vMove = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	//プレイヤーの移動
	if (vnKeyboard::on(DIK_W))
	{
		vMove = XMVectorSetZ(vMove, 0.1f);
		input = true;
	}
	if (vnKeyboard::on(DIK_S))
	{
		vMove = XMVectorSetZ(vMove, -0.1f);
		input = true;
	}
	if (vnKeyboard::on(DIK_A))
	{
		vMove = XMVectorSetX(vMove, -0.1f);
		input = true;
	}
	if (vnKeyboard::on(DIK_D))
	{
		vMove = XMVectorSetX(vMove, 0.1f);
		input = true;
	}

	//プレイヤーの移動ベクトルをカメラの視点に応じて回転
	XMMATRIX rotate = XMMatrixRotationY(theta);
	//移動ベクトルに回転マトリクスを適用
	vMove = XMVector3TransformNormal(vMove, rotate);

	//プレイヤーの移動
	pPlayer->addPosition(&vMove);
	//影の座標にプレイヤーの座標を代入
	pShadow->setPosition(pPlayer->getPosition());
	//Z-Fightingを回避するため、影のポリゴンを少し浮かす
	pShadow->setPositionY(0.01f);
	
	//プレイヤーを進行方向(vMove)に向ける
	if (input == true)	//入力があるときのみ実行
	{
		float moveX = XMVectorGetX(vMove);	//ベクトルのx成分
		float moveZ = XMVectorGetZ(vMove);	//ベクトルのz成分
		float rotY = atan2f(moveX, moveZ);	//目的の角度

		//現在の角度(source)
		//目的の角度(destination)
		XMMATRIX /*mSrc, mDst, */mAns;
		XMVECTOR qSrc, qDst, qAns;
		
		//オイラー角をマトリクスに変換
		//mSrc = XMMatrixRotationY(pPlayer->getRotationY());
		//mDst = XMMatrixRotationY(rotY);
		
		//マトリクスをクオータニオンに変換
		//qSrc = XMQuaternionRotationMatrix(mSrc);
		//qDst = XMQuaternionRotationMatrix(mDst);

		//オイラー角→クオータニオンに変換する関数
		qSrc = XMQuaternionRotationRollPitchYaw(0.0f, pPlayer->getRotationY(), 0.0f);
		qDst = XMQuaternionRotationRollPitchYaw(0.0f, rotY, 0.0f);

		//球面線形補間で中間の姿勢を計算
		qAns = XMQuaternionSlerp(qSrc, qDst, 0.1f);

		//クオータニオンからマトリクスに変換
		mAns = XMMatrixRotationQuaternion(qAns);

		//マトリクスからY軸回転値を計算
		float ry = atan2f(-XMVectorGetZ(mAns.r[0]), XMVectorGetZ(mAns.r[2]));

		pPlayer->setRotationY(ry);

		pPlayer->setMotion(pMotionWalk);

		pEmitter->setEmit(true);
	}
	else
	{
		pPlayer->setMotion(pMotionIdle);

		pEmitter->setEmit(false);
	}


	//カメラの回転
	if (vnKeyboard::on(DIK_UP))
	{
		phi += 1.0f * 3.141592f / 180.0f;
	}
	if (vnKeyboard::on(DIK_DOWN))
	{
		phi -= 1.0f * 3.141592f / 180.0f;
	}
	if (vnKeyboard::on(DIK_LEFT))
	{
		theta += 1.0f * 3.141592f / 180.0f;
	}
	if (vnKeyboard::on(DIK_RIGHT))
	{
		theta -= 1.0f * 3.141592f / 180.0f;
	}

	XMVECTOR camPos = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	XMVECTOR camTrg = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	XMVECTOR camBias = XMVectorSet(0.0f, 1.5f, 0.0f, 0.0f);

	//カメラの座標の計算
	//回転していない基準となるベクトル
	XMVECTOR eye = XMVectorSet(0.0f, 0.0f, -radius, 0.0f);
	//回転マトリクスを作成
	XMMATRIX camRotate = XMMatrixRotationRollPitchYaw(phi, theta, 0.0f);
	//基準となるベクトルを回転させる
	eye = XMVector3TransformNormal(eye, camRotate);

	//float px = radius * cosf(theta) * cos(phi);
	//float py = radius * sinf(phi);
	//float pz = radius * sinf(theta) * cos(phi);

	//回転の中心をプレイヤーにする
	//XMVECTOR v = XMVectorSet(px, py, pz, 0.0f);
	camPos = XMVectorAdd(eye, *pPlayer->getPosition());
	camPos = XMVectorAdd(camPos, camBias);

	//カメラの位置を極座標(r,θ,φ)で表した座標にする
	vnCamera::setPosition(&camPos);

	//カメラの注視点をプレイヤーに合わせる
	camTrg = XMVectorAdd(*pPlayer->getPosition(), camBias);
	vnCamera::setTarget(&camTrg);


	//vnCamera::setPosition(&camTrg);
	//vnCamera::setTarget(&camPos);

	//XMVECTOR, XMMATRIX
	// SIMD : Single Instructure Multi Decimal



	//・プレイヤーの原点からの距離を調べる
	//プレイヤーの座標 : pPlayer->getPosition()
	//ベクトルの長さを調べる
	XMVECTOR v = *pPlayer->getPosition();
	XMVECTOR vLength = XMVector3Length(v);
	float length = XMVectorGetX(vLength);
	//・距離(length)とフェンスの半径(FENCE_RADIUS)を比較
	if (length > FIELD_RADIUS)
	{
		//・プレイヤーがフェンスの外にいる場合
		//・プレイヤーの原点からの距離をフェンスの半径になる位置を計算
		//ベクトルの長さを自分で設定する
		//正規化:ベクトルの長さを1にする
		XMVECTOR nv = XMVector3Normalize(v);
		//設定したい長さの数値を掛ける
		nv = XMVectorScale(nv, FIELD_RADIUS);
		//→プレイヤーの位置を設定
		pPlayer->setPosition(&nv);
	}


	//NPCの認識範囲にプレイヤーが入っているか調べる

	//①NPCとプレイヤーの距離が10ｍ以内

	//NPCからプレイヤーへ向かうベクトルを作る
	XMVECTOR vNpcToPlayer = 
		*pPlayer->getPosition() - *pNPC->getPosition();
	//NPCからプレイヤーへ向かうベクトルの長さを求める
	XMVECTOR vDist = XMVector3Length(vNpcToPlayer);
	float dist = XMVectorGetX(vDist);

	//②NPCの前方向からプレイヤーの角度が±45度以内
	// 
	//XMVECTOR vFront = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	//XMMATRIX mNPC = XMMatrixRotationY(pNPC->getRotationY());
	//vFront = XMVector3TransformNormal(vFront, mNPC);
	XMMATRIX mNpc = *pNPC->getWorld();	//ワールドマトリクスを取得
	XMVECTOR vFront = mNpc.r[2];	//前方向を表すベクトル

	//2本のベクトルの内積を計算
	//2本のベクトルの長さが1のとき、
	//内積値はcosθと一致する

	//正規化を行い、ベクトルの長さを1にする
	vNpcToPlayer = XMVector3Normalize(vNpcToPlayer);
	//vFront = XMVector3Normalize(vFront);
	//内積値(cosθ)を求める
	XMVECTOR vDot = XMVector3Dot(vNpcToPlayer, vFront);
	float dot = XMVectorGetX(vDot);
	//float型変数のノイズ除去
	if (dot > 1.0f)
	{
		dot = 1.0f;
	}
	else if (dot < -1.0f)
	{
		dot = -1.0f;
	}
	//θを求める
	float angle = acosf(dot);	//dot = 1.00000012(float型の精度のノイズ)
	//-nan(ind)
	if (dist < 10.0f && angle < 45.0f*3.141592f/180.0f)
	{
		vnFont::print(250.0f, 100.0f, L"認識範囲に入っている");

		//プレイヤーを追いかける
		vNpcToPlayer = XMVector3Normalize(vNpcToPlayer);
		vNpcToPlayer = XMVectorScale(vNpcToPlayer, 0.05f);
		pNPC->addPosition(&vNpcToPlayer);
		//プレイヤーの方を向く
		float x = XMVectorGetX(vNpcToPlayer);
		float z = XMVectorGetZ(vNpcToPlayer);
		float npcY = atan2f(x, z);
		pNPC->setRotationY(npcY);

		pNPC->setMotion(pMotionWalk);

		pBillboard->setRenderEnable(true);
		pEmitterNPC->setEmit(true);
	}
	else
	{
		pNPC->setMotion(pMotionIdle);
		pBillboard->setRenderEnable(false);
		pEmitterNPC->setEmit(false);
	}
	pNPCShadow->setPosition(pNPC->getPosition());
	pNPCShadow->setPositionY(0.01f);



	//デバッグ表示
	vnFont::setColor(0xff000000);
	vnFont::print(10.0f, 10.0f, L"PlayerPos : %.3f, %.3f, %.3f",
		pPlayer->getPositionX(),
		pPlayer->getPositionY(), 
		pPlayer->getPositionZ());
	vnFont::print(10.0f, 30.0f, L"CrownLocalPos : %.3f, %.3f, %.3f",
		pCrown->getPositionX(),
		pCrown->getPositionY(),
		pCrown->getPositionZ());

	XMVECTOR wld;
	pCrown->getWorldPosition(&wld);
	vnFont::print(10.0f, 50.0f, L"CrownWorldPos : %.3f, %.3f, %.3f",
		XMVectorGetX(wld),
		XMVectorGetY(wld),
		XMVectorGetZ(wld));


	vnFont::print(10.0f, 70.0f, L"Player RotY : %.3f", 
		pPlayer->getRotationY() * 180.0f / 3.1415192f);
	vnFont::print(10.0f, 90.0f, L"radius : %.3f", radius);
	vnFont::print(10.0f, 110.0f, L"theta : %.3f", theta * 180.0f / 3.1415192f);
	vnFont::print(10.0f, 130.0f, L"phi : %.3f", phi * 180.0f / 3.1415192f);

	vnFont::print(10.0f, 150.0f,
		L"プレイヤーの原点からの距離 : %.3f", length);

	vnFont::print(10.0f, 190.0f,
		L"NPCとプレイヤーの距離 : %.3f", dist);
	vnFont::print(10.0f, 210.0f,
		L"NPCとプレイヤーの角度 : %.3f", angle * 180.0f / 3.141592f);
	vnFont::setColor(0xffffffff);


	vnDebugDraw::Grid();
	vnDebugDraw::Axis();

	vnScene::execute();
}

//描画関数
void SceneFieldTest::render()
{
	vnScene::render();
}
