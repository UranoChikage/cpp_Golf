#pragma once

class Collider
{
protected:
	XMVECTOR center;
public:

	virtual bool IsCollide(const vnCollide::stSegment& ray,
		XMVECTOR* hit, XMVECTOR* nor) = 0; // 中身は各形状が実装する

	//2026/07/18　AIに手伝ってもらって追加(QueryObstacle用のPhysics.OverlapSphere相当が無かったため)
	//球との重なり判定(Physics.OverlapSphere相当)
	//[return] bool true : 重なっている, false : 重なっていない
	//[out]outClosestPoint : 形状上の最近接点
	//[out]outNormal : sphereCenterから見た押し出し方向の法線
	//[out]outPenetration : めり込み量
	virtual bool Overlap(const XMVECTOR& sphereCenter, float sphereRadius,
		XMVECTOR* outClosestPoint, XMVECTOR* outNormal, float* outPenetration) = 0;

	virtual ~Collider();
};

class BoxCollider : public Collider
{
private:
	XMVECTOR half;
	XMVECTOR min, max;
	XMMATRIX rotate;//回転行列(単位行列ならAABBで計算可能)※scaleは考慮しない

	XMMATRIX GetLocalMatrix();
	//2026/07/02　一旦回転考慮せずいくぜ！
	//2026/07/13　world座標からlocal座標に変換してからAABBの衝突判定を行うようにする！先生がヒントをくれた！
	//AABB
public:
	BoxCollider(const XMVECTOR& rCenter, const XMVECTOR& rHalf) : half(rHalf)
	{
		rotate = XMMatrixIdentity();
		center = rCenter;
		min = center - half;
		max = center + half;
	}
	void SetRotate(const XMVECTOR& eulerRotate);

	bool IsAxisAligned();
	bool IsCollide(const vnCollide::stSegment& ray,
		XMVECTOR* hit, XMVECTOR* nor) override;
	//2026/07/18　AIに手伝ってもらって追加
	bool Overlap(const XMVECTOR& sphereCenter, float sphereRadius,
		XMVECTOR* outClosestPoint, XMVECTOR* outNormal, float* outPenetration) override;
};
class SphereCollider : public Collider
{
private:
	float radius;

public:
	SphereCollider(const XMVECTOR& c, float r) : radius(r)
	{
		SetCenter(c);
	}

	void SetCenter(const XMVECTOR& c)
	{
		center = c;
	}

	XMVECTOR GetCenter() const
	{
		return center;
	}
	bool IsCollide(const vnCollide::stSegment& ray,
		XMVECTOR* hit, XMVECTOR* nor) override;
	//2026/07/18　AIに手伝ってもらって追加
	bool Overlap(const XMVECTOR& sphereCenter, float sphereRadius,
		XMVECTOR* outClosestPoint, XMVECTOR* outNormal, float* outPenetration) override;
};
class MeshCollider : public Collider
{
private:
	std::vector<vnCollide::stTriangle> Triangles; // 頂点のリスト
public:
	MeshCollider(const std::vector<vnCollide::stTriangle>& triangles)
		: Triangles(triangles) {
	}
	static std::vector<vnCollide::stTriangle> BuildTriangles(vnModel* p);
	bool IsCollide(const vnCollide::stSegment& ray,
		XMVECTOR* hit, XMVECTOR* nor) override;
	//2026/07/18　AIに手伝ってもらって追加
	bool Overlap(const XMVECTOR& sphereCenter, float sphereRadius,
		XMVECTOR* outClosestPoint, XMVECTOR* outNormal, float* outPenetration) override;
};
