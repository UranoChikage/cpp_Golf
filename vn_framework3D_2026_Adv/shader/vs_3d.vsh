struct vertexIn
{
	float4 pos : POSITION0;
	float4 nor : NORMAL0;
	float4 col : COLOR0;
	float2 tx0 : TEXCOORD0;
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

vertexOut main(vertexIn IN)
{
	vertexOut OUT = (vertexOut)0;

	//頂点座標をビューポート座標に変換
	OUT.pos = mul(IN.pos, WVP);

	//ワールド座標
	float4 wpos = mul(IN.pos, World);

	//法線ベクトルをワールド行列で変換
	OUT.nor = mul(float4(IN.nor.xyz, 0), World).xyz;

	//頂点カラー
	OUT.col = IN.col;

	//テクスチャ座標
    OUT.tx0 = IN.tx0 * TilingOffset.xy + TilingOffset.zw;;

	//視線ベクトル
	OUT.eye = CameraPos.xyz - wpos.xyz;

	return OUT;
}