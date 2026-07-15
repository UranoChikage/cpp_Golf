//--------------------------------------------------------------//
//	"vn_collide.h"												//
//		衝突演算のインターフェイス								//
//													2026/01/01	//
//														Ichii	//
//--------------------------------------------------------------//
#pragma once

//微小値
#define EPSILON (1.0e-6)

//微小値を考慮したゼロ比較
#define ISZERO(x) (fabsf(x)<EPSILON)

namespace vnCollide
{
	//形状 : 直線
	struct stLine
	{
		XMVECTOR Pos;	//始点
		XMVECTOR Dir;	//方向(正規化)

		//2点から直線の情報を作成
		void fromPoints(const XMVECTOR* p1, const XMVECTOR* p2);
	};

	//形状：線分
	struct stSegment : public stLine
	{
		float Length;	//線分の長さ

		//2点から線分の情報を作成
		void fromPoints(const XMVECTOR* p1, const XMVECTOR* p2);
	};

	//形状 : 平面
	struct stPlane
	{
		XMVECTOR Normal;	//平面の法線
		float Distance;		//原点からの距離

		//3点から平面の情報を作成
		void fromPoints(const XMVECTOR* p1, const XMVECTOR* p2, const XMVECTOR* p3);
		//1点と法線から平面の情報を作成
		void fromPointNormal(const XMVECTOR* p, const XMVECTOR* n);
	};

	//形状 : 三角形
	struct stTriangle
	{
		stPlane plane;	//内包(Including)
		XMVECTOR v[3];	//頂点座標

		//3点から三角形の情報を作成
		void fromPoints(const XMVECTOR* p1, const XMVECTOR* p2, const XMVECTOR* p3);
	};

	//形状 : 球
	struct stSphere
	{
		XMVECTOR Pos;	//中心座標
		float Radius;	//半径
	};

	//直線と点の最近傍距離を示す直線上の点を求める
	//[return] void
	//[Argument]
	//[out]XMVECTOR *ret : 衝突座標を格納するアドレス
	//[in]const stLine* line : 直線の情報
	//[in]const XMVECTOR* tri : 点の座標
	void closestPoint(XMVECTOR* ret, const stLine* line, const XMVECTOR* pos);

	//直線と平面の交点を調べる
	//[return] bool true : 衝突している, false : 衝突していない
	//[Argument]
	//[out]XMVECTOR *hit : 衝突座標を格納するアドレス
	//[in]const stLine* line : 直線の情報
	//[in]const stPlane* plane : 三角形の情報
	bool isCollide(XMVECTOR* hit, const stLine* line, const stPlane* plane);

	//直線と三角形の交点を調べる
	//[return] bool true : 衝突している, false : 衝突していない
	//[Argument]
	//[out]XMVECTOR *hit : 衝突座標を格納するアドレス
	//[in]const stLine* line : 直線の情報
	//[in]const stTriangle* tri : 三角形の情報
	bool isCollide(XMVECTOR* hit, const stLine* line, const stTriangle* tri);

	//直線と球の交点を調べる
	//[return] int 交点の数
	//[Argument]
	//[out]XMVECTOR *hit : 衝突座標を格納する配列の先頭アドレス
	//[in]const stLine* line : 直線の情報
	//[in]const stSphere* tri : 球の情報
	int isCollide(XMVECTOR* hit, const stLine* line, const stSphere* sphere);

	//線分と平面の交点を調べる
	//[return] bool true : 衝突している, false : 衝突していない
	//[Argument]
	//[out]XMVECTOR *hit : 衝突座標を格納するアドレス
	//[in]const stSegment* seg : 線分の情報
	//[in]const stPlane* plane : 三角形の情報
	bool isCollide(XMVECTOR* hit, const stSegment* seg, const stPlane* plane);

	//線分と三角形の交点を調べる
	//[return] bool true : 衝突している, false : 衝突していない
	//[Argument]
	//[out]XMVECTOR *hit : 衝突座標を格納するアドレス
	//[in]const stSegment* seg : 線分の情報
	//[in]const stTriangle* tri : 三角形の情報
	bool isCollide(XMVECTOR* hit, const stSegment* seg, const stTriangle* tri);

}