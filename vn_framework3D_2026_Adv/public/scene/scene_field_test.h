#pragma once
//scene_field_test.h

#define FENCE_NUM	(32)		//フェンスモデルの数

#define FENCE_RADIUS (37.5f)	//フェンスを配置する円周の半径
#define FIELD_RADIUS (36.5f)	//プレイヤーが移動できる円周の半径

class SceneFieldTest : public vnScene
{
private:
	vnCharacter *pPlayer;
	vnModel* pShadow;

	vnModel* pFence[FENCE_NUM];

	vnModel* pGround;
	vnModel* pSky;

	vnCharacter* pNPC;
	vnModel* pNPCShadow;
	vnModel* pDebugFan;

	vnBillboard* pBillboard;

	vnModel* pCrown;

	vnEmitter* pEmitter;
	vnEmitter* pEmitterNPC;

	vnMotionData* pMotionIdle;
	vnMotionData* pMotionWalk;

	//極座標の情報
	float radius;	//半径
	float theta;	//角度(平面角 / 経度)
	float phi;		//角度(仰角 / 緯度)


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
