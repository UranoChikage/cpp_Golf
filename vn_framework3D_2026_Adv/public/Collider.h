#pragma once

class Collider
{
protected:
	XMVECTOR center;
public:

	virtual bool IsCollide(const vnCollide::stSegment& ray,
		XMVECTOR* hit, XMVECTOR* nor) = 0; // 中身は各形状が実装する
	virtual ~Collider();
};

class BoxCollider : public Collider
{
private:
	XMVECTOR half;
	XMVECTOR min, max;
	XMMATRIX rotate;//回転行列(単位行列ならAABBで計算可能)※scaleは考慮しない

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
};
class MeshCollider : public Collider
{
private:
	std::vector<vnCollide::stTriangle> Triangles; // 頂点のリスト
public:
	bool IsCollide(const vnCollide::stSegment& ray,
		XMVECTOR* hit, XMVECTOR* nor) override;
};
