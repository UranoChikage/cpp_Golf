//--------------------------------------------------------------//
//	"vn_debugDraw.h"											//
//		デバッグ描画											//
//													2026/01/01	//
//														Ichii	//
//--------------------------------------------------------------//
#pragma once

//ポリゴン頂点構造体
struct vnVertex3D_Line
{
	float x,y,z;
	DWORD color;
};

//頂点数
#define vnDebugDraw_VertexMax	(1024)

class vnDebugDraw
{
protected:
	//頂点数
	static int						VertexNum;
	//頂点データ
	static vnVertex3D_Line			*vtx;
	//マトリクス
	static XMFLOAT4X4				*VP;

	//頂点バッファ
	static ID3D12Resource* vertBuff;
	static D3D12_VERTEX_BUFFER_VIEW vertexBufferView;

	static D3D12_INPUT_ELEMENT_DESC inputElementDescs[];	//頂点要素
	static ID3D12PipelineState* pPipelineState;				//パイプラインステート

public:
	//[システム管理関数
	static bool initialize();
	static void terminate();
	static void render();
	//]
	
	static void Line(const XMVECTOR *start, const XMVECTOR *end, DWORD color=0xffffffff);
	static void Line(float start_x, float start_y, float start_z, float end_x, float end_y, float end_z, DWORD color = 0xffffffff);

	static void Grid(int num=10, float interval=1.0f, DWORD color=0x7fffffff);
	static void Axis(float length = 5.0f);
};
