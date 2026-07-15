//--------------------------------------------------------------//
//	"vn_model.h"												//
//		モデルデータ描画クラス									//
//													2026/01/01	//
//														Ichii	//
//--------------------------------------------------------------//
#pragma once

#include "vn_object.h"

class vnModel : public vnObject
{
protected:
	struct stMaterialBuffer
	{
		XMFLOAT4	Diffuse;		//拡散色
		XMFLOAT4	Ambient;		//環境色
		XMFLOAT4	Specular;		//鏡面反射色
		XMFLOAT4	TilingOffset;	//タイリングとオフセット
	};
	struct stMaterial
	{
		//カラー
		XMVECTOR	Diffuse;		//拡散色
		XMVECTOR	Ambient;		//環境色
		XMVECTOR	Specular;		//鏡面反射色
		XMVECTOR	TilingOffset;	//タイリングとオフセット
		//テクスチャ
		ID3D12Resource* texbuff;
		//定数バッファ
		ID3D12Resource* materialBuff;
		ID3D12DescriptorHeap* basicDescHeap;
		stMaterialBuffer* pMaterialBuffer;
	};
	
	//モデルデータ
	vnModelData		*pModelData;
	
	//マテリアル
	stMaterial		*pMaterials;

	//スキニング変換用マトリクス
	XMMATRIX* pSkinMatrix;

	//頂点バッファ
	ID3D12Resource* pVertexBuffer;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView;

	//インデックスバッファ
	ID3D12Resource* pIndexBuffer;
	D3D12_INDEX_BUFFER_VIEW indexBufferView;

	//コンスタントバッファ構造体
	ID3D12Resource* constBuff;
	//ID3D12DescriptorHeap* constDescHeap;
	stConstantBuffer* pConstBuffer;

	ID3D12Resource* bonesBuff;
	XMFLOAT4X4* bones;

public:
	//コンストラクタ
	vnModel(const WCHAR *folder, const WCHAR *file);
	//デストラクタ
	~vnModel();
	
	//システム関数
	virtual void execute();
	virtual void render();

	//スキンマトリクスの設定
	virtual void setSkinMatrix(XMMATRIX* m);

	//モデルデータ(vnmファイル)の情報の取得
	int getVertexNum();
	int getIndexNum();
	int getFaceNum();
	int getMaterialNum();
	int getMeshNum();
	int getBoneNum();
	const WCHAR *getTextureName(int id);

	vnModel_MeshData *getMesh();	//メッシュデータ(先頭アドレス)の取得
	vnVertex3D *getVertex();		//頂点データ(先頭アドレス)の取得
	LPWORD getIndex();				//インデックスデータ(先頭アドレス)の取得

	//拡散色の設定
	void setDiffuse(float r, float g, float b, float a, int id=-1);
	void setDiffuse(const XMVECTOR *v, int id=-1);
	void setDiffuseR(float value, int id=-1);
	void setDiffuseG(float value, int id=-1);
	void setDiffuseB(float value, int id=-1);
	void setDiffuseA(float value, int id=-1);
	void addDiffuseR(float value, int id=-1);
	void addDiffuseG(float value, int id=-1);
	void addDiffuseB(float value, int id=-1);
	void addDiffuseA(float value, int id=-1);

	//拡散色の取得
	XMVECTOR *getDiffuse(int id=0);
	float getDiffuseR(int id=0);
	float getDiffuseG(int id=0);
	float getDiffuseB(int id=0);
	float getDiffuseA(int id=0);

	//環境色の設定
	void setAmbient(float r, float g, float b, float a, int id=-1);
	void setAmbient(const XMVECTOR *v, int id=-1);
	void setAmbientR(float value, int id=-1);
	void setAmbientG(float value, int id=-1);
	void setAmbientB(float value, int id=-1);
	void setAmbientA(float value, int id=-1);
	void addAmbientR(float value, int id=-1);
	void addAmbientG(float value, int id=-1);
	void addAmbientB(float value, int id=-1);
	void addAmbientA(float value, int id=-1);

	//環境色の取得
	XMVECTOR *getAmbient(int id=0);
	float getAmbientR(int id=0);
	float getAmbientG(int id=0);
	float getAmbientB(int id=0);
	float getAmbientA(int id=0);

	//鏡面反射色の設定
	void setSpecular(float r, float g, float b, float p, int id = -1);
	void setSpecular(const XMVECTOR *v, int id = -1);
	void setSpecularR(float value, int id = -1);
	void setSpecularG(float value, int id = -1);
	void setSpecularB(float value, int id = -1);
	void setSpecularPower(float value, int id = -1);
	void addSpecularR(float value, int id = -1);
	void addSpecularG(float value, int id = -1);
	void addSpecularB(float value, int id = -1);
	void addSpecularPower(float value, int id = -1);

	//鏡面反射色の取得
	XMVECTOR *getSpecular(int id = 0);
	float getSpecularR(int id = 0);
	float getSpecularG(int id = 0);
	float getSpecularB(int id = 0);
	float getSpecularPower(int id = 0);

	//タイリングとオフセットの取得
	const XMVECTOR* getTilingOffset(int id = 0);
};
