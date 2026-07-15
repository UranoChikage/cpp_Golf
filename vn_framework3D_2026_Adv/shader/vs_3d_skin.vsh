#define BONE_MAX	(512)

struct vertexIn
{
	float4 pos : POSITION0;
	float4 nor : NORMAL0;
	float4 col : COLOR0;
	float2 tx0 : TEXCOORD0;
	uint4  idx : SKIN_INDEX;
	float4 wet : SKIN_WEIGHT;
};

struct vertexOut
{
	float4 pos : SV_POSITION;
	float4 col : COLOR0;
	float2 tx0 : TEXCOORD0;
	float3 nor : TEXCOORD1;
	float3 eye : TEXCOORD2;
	float4 sdw : TEXCOORD3;
};

//シーン(カメラ & ライト)
cbuffer SceneBuffer : register(b0)
{
	float4x4	VP;			//View * Proj
	float4x4	LightVP;	//シャドウマップ用
	float4		LightDir;	//平行光源の(逆)方向
	float4		LightCol;	//平行光源の色
	float4		LightAmb;	//環境光の色
	float4		CameraPos;	//カメラのワールド座標
}

//マテリアル
cbuffer MaterialBuffer : register(b1)
{
    float4 Diffuse;		//マテリアルの拡散色
    float4 Ambient;		//マテリアルの環境色
    float4 Specular;	//マテリアルの鏡面反射光
    float4 TilingOffset;
}

//マトリクス
cbuffer ConstantBuffer : register(b2)
{
	float4x4	WVP;		//World*View*Proj
	float4x4	World;		//ワールド行列
}

cbuffer BoneBuffer : register(b3)
{
	float4x4 Bones[BONE_MAX];
}

vertexOut main(vertexIn IN)
{
	vertexOut OUT = (vertexOut)0;

	//スキニング変換(Worldマトリクスは単位行列としておく)
	float4x4 mtx;
	mtx  = Bones[IN.idx.x] * IN.wet.x;
	mtx += Bones[IN.idx.y] * IN.wet.y;
	mtx += Bones[IN.idx.z] * IN.wet.z;
	mtx += Bones[IN.idx.w] * IN.wet.w;
	float4 wpos = mul(mtx, IN.pos);

	//頂点座標をビューポート座標に変換
	OUT.pos = mul(wpos, WVP);

	//法線ベクトルをワールド行列で変換
	OUT.nor = mul(float4(IN.nor.xyz,0), transpose(mtx)).xyz;

	//頂点カラー
	OUT.col = IN.col;

	//テクスチャ座標
	OUT.tx0 = IN.tx0 * TilingOffset.xy + TilingOffset.zw;

	//視線ベクトル
	OUT.eye = CameraPos.xyz - wpos.xyz;

	return OUT;
}