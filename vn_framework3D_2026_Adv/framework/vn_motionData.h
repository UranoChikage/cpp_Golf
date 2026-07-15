//--------------------------------------------------------------//
//	"vn_motionData.h"											//
//		モーションデータ定義									//
//													2026/01/01	//
//														Ichii	//
//--------------------------------------------------------------//
#pragma once

//バージョン履歴
#define vnMOTION_DATA_LATEST_VER (0x20251201)

struct vnMotionData
{
	UINT Magic;
	UINT Date;
	UINT Version;
	UINT HeaderSize;

	float Length;
	UINT ChannelNum;
	UINT KeyFrameNum;
	float iFrameRate;

	UINT ChannelAccess;
	UINT KeyFrameAccess;
	UINT Reserve1;
	UINT Reserve2;

	UINT Align[4];
};

enum eMotionChannel
{
	PosX,
	PosY,
	PosZ,
	RotX,
	RotY,
	RotZ,
	SclX,
	SclY,
	SclZ,
	QutX,
	QutY,
	QutZ,
	QutW,
	ChannelMax,
};

struct vnMotionData_Channel
{
	WCHAR Name[32];
	int ChannelID;
	int KeyFrameNum;
	int StartIndex;
	int Reserve;
};

struct vnMotionData_KeyFrame
{
	float	Time;
	float	Value;
	float	InTangent;
	float	OutTangent;
};