//--------------------------------------------------------------//
//	"vn_mouse.h"												//
//		マウス入力(DirectInput)									//
//													2026/01/01	//
//														Ichii	//
//--------------------------------------------------------------//
#pragma once

class vnMouse
{
public:
	//マウスボタン
	enum eButton
	{
		LT	= 0x01,	//左ボタン
		MD	= 0x02,	//中ボタン
		RT	= 0x04,	//右ボタン
	};
	
	static int	getX(void);				//座標(X)
	static int	getY(void);				//座標(Y)
	static int	getR(void);				//ロール
	static int	getDX(void);			//移動量(X)
	static int	getDY(void);			//移動量(Y)
	static bool on(eButton btn=LT);		//オン状態(押している間)の取得
	static bool trg(eButton btn=LT);	//トリガー(押した瞬間)の取得
	static bool rel(eButton btn=LT);	//リリース(押した瞬間)の取得

	static bool onL() { return on(LT); }
	static bool onM() { return on(MD); }
	static bool onR() { return on(RT); }

	static bool trgL() { return trg(LT); }
	static bool trgM() { return trg(MD); }
	static bool trgR() { return trg(RT); }

	static bool relL() { return rel(LT); }
	static bool relM() { return rel(MD); }
	static bool relR() { return rel(RT); }

private:
	
	static int	x;
	static int	y;
	static int	r;
	static int	dx;
	static int	dy;
	static int	On;
	static int	Trg;
	static int	Rel;
	
public:
	//マウスの状態を設定
	static void setMouseStatus(void);
};
