#pragma once

class SceneGroundTest : public vnScene
{
private:

	enum ePlayerModel
	{
		PlayerModel_None,
		Quinn,
		UnityChan,
		BoxUnityChan,
	};
	ePlayerModel playerModel;

	enum eGroundModel
	{
		GroundModel_None,
		Flat,
		Noised,
		Overpass,
		TestField,
	};
	eGroundModel groundModel;

	enum ePlayerMotion
	{
		Idle,
		Run,
		Jump,
		Fall,
		MotionMax,
	};

	vnCharacter* pPlayer;
	vnMotionData* pMotion[ePlayerMotion::MotionMax];

	//プレイヤーアクション関連
	float pastMotionTime;
	float jumpForce;
	bool air;

	//環境関連
	vnModel* pGround;
	vnModel* pSky;

	//物理関連
	XMVECTOR velocity;
	XMVECTOR gravity;

	//極座標の情報
	float radius;	//半径
	float theta;	//平面角
	float phi;		//仰角

	float slerpRotY(float src, float dst, float rate);

	int isCollide(XMVECTOR* hit, XMVECTOR* nor, vnCollide::stSegment* seg, vnModel* p);

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
