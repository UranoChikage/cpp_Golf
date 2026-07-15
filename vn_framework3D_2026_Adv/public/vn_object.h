//--------------------------------------------------------------//
//	"vn_object.h"												//
//		オブジェクト基底クラス									//
//													2026/01/01	//
//														Ichii	//
//--------------------------------------------------------------//
#pragma once

#include "../framework/vn_modelData.h"

//ポリゴン頂点構造体
struct vnVertex3D
{
	float x,y,z;		//座標
	float nx,ny,nz;		//法線
	float r,g,b,a;		//カラー
	float u,v;			//UV
};

class vnObject
{
public:
	//レンダーフラグ
	enum eRenderFlag
	{
		Transparent		= 0x00000001,	//半透明の描画
		Lighting		= 0x00000002,	//ライティング
		DoubleSided		= 0x00000004,	//両面描画
		Addition		= 0x00000008,	//加算合成
		ZWrite			= 0x00000010,	//深度書き込み無効
		Skin			= 0x00000020,	//スキニング頂点変換
		Last			= 0x00000040,
		MaxNum			= Last,
	};

protected:
	//移動値
	XMVECTOR	Position;
	
	//回転値
	XMVECTOR	Rotation;
	
	//拡大値
	XMVECTOR	Scale;

	//クオータニオン
	XMVECTOR	Quaternion;

	//変換行列
	XMMATRIX	Local;
	XMMATRIX	World;

	//親子関係
	vnObject* pParent;

	//行列の計算
	void calculateLocalMatrix();
	void calculateWorldMatrix();

	//実行状態の管理
	bool executeEnable;

	//描画状態の管理
	bool renderEnable;

	//回転にクオータニオンを使用
	bool rotateQuaternion;

	UINT32 renderFlag;

	//静的共通データ
	static D3D12_INPUT_ELEMENT_DESC inputElementDescs[];	//頂点要素
	static D3D12_INPUT_ELEMENT_DESC inputElementDescs_Skin[];	//頂点要素
	static ID3D12PipelineState		*pPipelineState[eRenderFlag::MaxNum];		//パイプラインステート

	//コンスタントバッファ構造体
	struct stConstantBuffer
	{
		XMFLOAT4X4	WVP;		//World*View*Proj
		XMFLOAT4X4	World;		//World
		XMFLOAT4X4	Bones[256];
	};

public:
	//静的共通データ初期化
	static bool initializeCommon();

	//静的共通データ削除
	static void terminateCommon();

	//コンストラクタ
	vnObject();

	//デストラクタ
	virtual ~vnObject();
	
	//システム関数
	virtual void execute();
	virtual void postExecute();
	virtual void render();

	//移動値の設定
	void setPosition(float x, float y, float z);
	void setPosition(const XMVECTOR *v);
	void setPositionX(float value);
	void setPositionY(float value);
	void setPositionZ(float value);
	void addPosition(float x, float y, float z);
	void addPosition(const XMVECTOR *v);
	void addPositionX(float value);
	void addPositionY(float value);
	void addPositionZ(float value);

	//移動値の取得
	XMVECTOR *getPosition();
	float getPositionX(void);
	float getPositionY(void);
	float getPositionZ(void);


	//回転値の設定
	void setRotation(float x, float y, float z);
	void setRotation(const XMVECTOR *v);
	void setRotationX(float radian);
	void setRotationY(float radian);
	void setRotationZ(float radian);
	void addRotation(float x, float y, float z);
	void addRotation(const XMVECTOR *v);
	void addRotationX(float radian);
	void addRotationY(float radian);
	void addRotationZ(float radian);

	//回転値の取得
	XMVECTOR *getRotation();
	float getRotationX(void);
	float getRotationY(void);
	float getRotationZ(void);
	
	//拡大値の設定
	void setScale(float x, float y, float z);
	void setScale(const XMVECTOR *v);
	void setScaleX(float value);
	void setScaleY(float value);
	void setScaleZ(float value);
	void addScale(float x, float y, float z);
	void addScale(const XMVECTOR *v);
	void addScaleX(float value);
	void addScaleY(float value);
	void addScaleZ(float value);

	//拡大値の取得
	XMVECTOR *getScale();
	float getScaleX(void);
	float getScaleY(void);
	float getScaleZ(void);

	//クオータニオンの設定
	void setQuaternion(const XMVECTOR* q);
	void setQuaternionX(float value);
	void setQuaternionY(float value);
	void setQuaternionZ(float value);
	void setQuaternionW(float value);

	//クオータニオンの取得
	XMVECTOR* getQuaternion();


	//ワールド行列の取得
	XMMATRIX *getWorld();

	//ワールド座標の取得
	void getWorldPosition(XMVECTOR *v);
	float getWorldPositionX();
	float getWorldPositionY();
	float getWorldPositionZ();

	//ワールド行列の設定
	void setWorld(XMMATRIX *m);


	//実行状態の設定
	void setExecuteEnable(bool flag);

	//描画状態の設定
	void setRenderEnable(bool flag);

	//実行状態の取得
	bool isExecuteEnable();

	//描画状態の取得
	bool isRenderEnable();

	//親子関係の設定
	void setParent(vnObject* p);

	//親子関係の取得
	vnObject* getParent();

	void setRenderFlag(eRenderFlag flag, bool value);

	bool getRenderFlag(eRenderFlag flag);

	//回転にクオータニオンを使用
	void useQuaternion(bool flag);

	//スキンマトリクスの設定
	virtual void setSkinMatrix(XMMATRIX * m){}

};
