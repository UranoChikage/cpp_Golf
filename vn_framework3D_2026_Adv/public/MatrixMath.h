#pragma once
/// <summary>
/// 行列の計算を行うライブラリクラス
/// </summary>
//     [行, 列] で要素にアクセスできる。
class MatrixMath
{
public:
	/*
	* XMMATRIXは行ベクトル規約(v * M)
	* [1  0  0  0]
	* [0  1  0  0]
	* [0  0  1  0]
	* [tx ty tz 1]
	*/
	static XMMATRIX Translation(const float tx, const float ty, const float tz);
	//Vector3オーバーロード版
	static XMMATRIX Translation(const XMVECTOR& v);
	// ──────────────────────────
	//  X軸回転行列 (Rotation around X-axis)
	//  「X軸を中心にangle度回転」する行列
	//
	//  数学的な形：
	//  | 1    0       0       0 |
	//  | 0  cos(θ)  -sin(θ) 0 |
	//  | 0  sin(θ)   cos(θ) 0 |
	//  | 0    0       0       1 |
	// ───────────────────────────
	static XMMATRIX RotationX(const float angleDeg);


	// ────────────────────────────
	//  Y軸回転行列 (Rotation around Y-axis)
	//  ゴルフでよく使う：ボールが横に転がるときの向き
	//
	//  数学的な形：
	//  |  cos(θ)  0  sin(θ)  0 |
	//  |    0     1    0       0 |
	//  | -sin(θ)  0  cos(θ)  0 |
	//  |    0     0    0       1 |
	// ─────────────────────────────
	static XMMATRIX RotationY(const float angleDeg);
	// ─────────────────────────────
	//  Z軸回転行列 (Rotation around Z-axis)
	//  でこぼこ道でボールが傾くとき
	// ─────────────────────────────

	static XMMATRIX RotationZ(const float angleDeg);

	// Vector3 で3軸まとめて渡す版
	//順番が関係するので注意
	static XMMATRIX Rotation(const XMVECTOR& anglesDeg);
	// ──────────────────────────────
	//  スケール行列 (Scale Matrix)
	//  「大きさを (sx, sy, sz) 倍にする」行列
	//
	//  数学的な形：
	//  | sx  0   0  0 |
	//  |  0  sy  0  0 |
	//  |  0  0  sz  0 |
	//  |  0  0   0  1 |
	// ──────────────────────────────
	static XMMATRIX Scale(const float sx, const float sy, const float sz);

	// Vector3 オーバーロード版
	static XMMATRIX Scale(const XMVECTOR& v);
	//均等にスケールする版
	static XMMATRIX Scale(float s);
	// ──────────────────────────────
	//
	//  |  1 sx  0  0 |
	//  |  0  1  0  0 |
	//  |  0  0  1  0 |
	//  |  0  0  0  1 |
	//他の要素を変えるとせん断ってのになるらしい(影とかに使えそう)
	// ──────────────────────────────


	// ──────────────────────────────────────────────
	//  任意軸回転行列 (Rodrigues' rotation formula)
	//  「地形の法線方向を軸にボールを傾ける」ときに使う
	//  普通の回転（X軸まわり）→ 軸が決まってるから cos/sin だけでいける
	//
	//  任意軸まわり（例：(1, 1, 0) を軸に回転）→ 軸が斜めなので、そのままでは計算できない
	//  → 「一度 X軸に合わせて → X軸回転して → 元に戻す」
	//  この「合わせて・回して・戻す」を1枚の行列にまとめた公式が"ロドリゲスの回転公式"
	//  UnityでQuaternion.AngleAxis()のやつ。
	// ──────────────────────────────────────────────
	static XMMATRIX RotationAxis(const XMVECTOR& axis, const float angleDeg);
	//行列をベクターに適用する関数
	/// <summary>
	/// world座標の位置を変換したいときはこっち(MultiplyPoint)
	/// </summary>
	static XMVECTOR MultiplyPoint(const XMMATRIX& m, const XMVECTOR& v);
	/// <summary>
	/// 方向を変換したいときはこっち(MultiplyVector)
	/// </summary>
	static XMVECTOR MultiplyVector(const XMMATRIX& m, const XMVECTOR& v);
	// ──────────────────────────────
	//  行列の情報を分解して position / rotation に反映する
	// ──────────────────────────────
	static void ApplyToTransform(XMMATRIX& m, vnObject* obj);

	static bool GetHitInfo(CollisionManager* manager, const float x, const float z, const float rayOriginHeight, const float rayMaxDistance, float* height, XMVECTOR* hitNormal);

	static XMVECTOR GetSlopeForce(XMVECTOR& vector, XMVECTOR& normal);
	// limitAngle: 回転可能な最大傾斜角度（例: 45度 を渡す）
	static bool IsWall(XMVECTOR groundNormal, float limitAngle);

	//転置行列を返す関数。列を行に、行を列に入れ替えるとなんかできるらしい。キーワード：「転置/直交行列」
	static XMMATRIX Transpose(const XMMATRIX& m);

	static float Dot(const XMVECTOR& a, const XMVECTOR& b);
};