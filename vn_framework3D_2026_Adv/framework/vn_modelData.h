//--------------------------------------------------------------//
//	"vn_modelData.h"											//
//		モデルデータ定義(*.vnm / バイナリファイル)				//
//													2026/01/01	//
//														Ichii	//
//--------------------------------------------------------------//
#pragma once

//バージョン履歴
#define vnMODEL_DATA_LATEST_VER (0x20260101)

struct vnModelData
{
	UINT Magic;
	UINT Date;
	UINT Version;
	UINT HeaderSize;

	UINT MaterialNum;
	UINT MeshNum;
	UINT VertexNum;
	UINT IndexNum;

	UINT MaterialAccess;
	UINT MeshAccess;
	UINT VertexAccess;
	UINT IndexAccess;

	//UINT Align[4];
	UINT BoneNum;
	UINT BoneTableAccess;
	UINT iBindAccess;
	UINT VertexStride;
};

struct vnModel_MaterialData
{
	UINT StructSize;
	UINT TextureNum;
	UINT FloatNum;
	UINT Flag;
	float Diffuse[4];
	float Ambient[4];
	float Specular[4];
	float TilingOffset[4];
	WCHAR Texture[32];
};

struct vnModel_MeshData
{
	UINT MaterialID;
	UINT IndexNum;
	UINT StartIndex;
	UINT StartVertex;
};


//.boneファイルの中身
struct vnModel_BoneData
{
	WCHAR Name[32];	//ボーンの名前
	int ParentID;	//親の番号(-1は親がない)
	int Generation;	//世代
	int StructSize;	//この構造体のサイズ(Byte)
	float pos[3];	//標準状態でのPosition(x,y,z)
	float rot[3];	//標準状態でのRotation(x,y,z)
	float scl[3];	//標準状態でのScale(x,y,z)
};
