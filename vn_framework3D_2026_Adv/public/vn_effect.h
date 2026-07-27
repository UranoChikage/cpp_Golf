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
	float		Size;	//サイズ(最終的な大きさ)
	bool		GrowSize;	//trueなら寿命にかけて徐々に大きくなる、falseなら最初からSizeでパッと出す
	bool		FadeAlpha;	//trueなら寿命にかけて徐々に透明になる、falseなら最初から不透明のまま
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

	//＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝
	//フレームワークに自分で機能追加しています。
	//Set〜で設定した内容を使って放出する(引数だらけになるのを避けるためSet方式にした)
	float burstSpeed = 0.1f;		//バースト放出時のパーティクルの初速
	bool burstGrowSize = false;	//バースト放出時、寿命にかけて徐々に大きくするか
	bool burstFadeAlpha = true;	//バースト放出時、寿命にかけて徐々に透明にするか

	//空いてるパーティクル枠を1つ探して初期設定する(見つからなければ何もしない)
	void SpawnParticle(const XMVECTOR& worldPos, float size, float speed, bool growSize, bool fadeAlpha);
	//＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝

public:
	vnEmitter(stEmitterDesc *desc);
	virtual ~vnEmitter();

	virtual void execute();

	virtual void render();

	void setEmit(bool flag);
	//＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝
	//フレームワークに自分で機能追加しています。
	//flagがtrueの時、count個のパーティクルをその場で即座にバースト生成する(見た目はSet〜で設定した内容を使う)
	void setEmit(bool flag, int count);
	//パーティクル1個の見た目の大きさの上限(継続的な放出/バースト放出どちらでも使うサイズ)を変更する
	void SetMaxSize(float size) { Desc.SizeMax = size; }
	//パーティクル1個の見た目の大きさの上限を取得する
	float GetMaxSize() const { return Desc.SizeMax; }
	//バースト放出時のパーティクルの初速を変更する
	void SetSpeed(float speed) { burstSpeed = speed; }
	//バースト放出時、寿命にかけて徐々に大きくする(true)か最初からパッと出す(false)かを変更する
	void SetGrowSize(bool flag) { burstGrowSize = flag; }
	//バースト放出時、寿命にかけて徐々に透明にする(true)か最初から不透明のまま(false)かを変更する
	void SetFadeAlpha(bool flag) { burstFadeAlpha = flag; }
	//＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝
	bool isEmit();
};