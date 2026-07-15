//--------------------------------------------------------------//
//	"vn_effect.h"												//
//		エフェクト(パーティクル)クラス							//
//													2026/01/01	//
//														Ichii	//
//--------------------------------------------------------------//
#pragma once

//パーティクルクラス
class vnParticle
{
public:
	float		Life;	//寿命(フレーム)
	float		StartLife;//寿命(エミット時の初期値)
	XMVECTOR	Pos;	//位置
	XMVECTOR	Vel;	//速度
	XMVECTOR	Col;	//色
	float		Size;	//サイズ
};

//エミッタークラス
class vnEmitter : public vnObject
{
public:
	//パーティクルを放出する際の設定
	struct stEmitterDesc
	{
		WCHAR Texture[64] = L"";

		float LifeMin = 30.0f;
		float LifeMax = 60.0f;

		XMVECTOR ColorMin = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
		XMVECTOR ColorMax = XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);

		float SizeMin = 0.5f;
		float SizeMax = 1.0f;

		float SpeedMin = 0.1f;
		float SpeedMax = 0.2f;
	};
private:

	//放出するかのフラグ
	bool emit;

	stEmitterDesc Desc;

	//パーティクル配列
	vnParticle* pParticle;

	//描画されるパーティクル数
	int renderParticleNum;

	//描画されるインデックス数
	int IndexNum;

	//頂点データ
	vnVertex3D* vtx;

	//インデックスデータ
	WORD* idx;

	struct stMaterialBuffer
	{
		XMFLOAT4	Diffuse;		//拡散色
		XMFLOAT4	Ambient;		//環境色
		XMFLOAT4	Specular;		//鏡面反射色
		XMFLOAT4	TilingOffset;	//タイリングとオフセット
	};
	struct stMaterial
	{
		//テクスチャ
		ID3D12Resource* texbuff;
		//定数バッファ
		ID3D12Resource* materialBuff;
		ID3D12DescriptorHeap* basicDescHeap;
		stMaterialBuffer* pMaterialBuffer;
	};

	//頂点バッファ
	ID3D12Resource* vertBuff;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView;

	//インデックスバッファ
	ID3D12Resource* idxBuffer;
	D3D12_INDEX_BUFFER_VIEW indexBufferView;

	//マテリアル
	stMaterial* pMaterials;
	//定数バッファ
	ID3D12Resource* constBuff;
	stConstantBuffer* pConstBuffer;

	//頂点データへの各種情報の設定
	virtual void setVertexPosition();

public:
	vnEmitter(stEmitterDesc *desc);
	virtual ~vnEmitter();

	virtual void execute();

	virtual void render();

	void setEmit(bool flag);

	bool isEmit();
};