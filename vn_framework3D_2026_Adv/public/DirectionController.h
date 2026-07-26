#pragma once
/***************************************************************
 * クラス名：DirectionController
 * 機能：マウスX入力から向き(Dir)を作る入力・状態クラス
 * 　　　他にもいろいろ使い方はあるヨ！
 * *************************************************************/
class DirectionController
{
private:
	//感度
	float sensitivity;


	XMVECTOR dir = XMVectorSet(0, 0, 0, 0);

	float yaw; //累積の回転量
public:
	void DirUpdate();
	// 外部からdirを読み取るため
	XMVECTOR GetDirection()const { return dir; }
	DirectionController();
};