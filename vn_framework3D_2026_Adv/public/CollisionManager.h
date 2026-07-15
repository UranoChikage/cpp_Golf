#pragma once
class CollisionManager
{
	std::vector<Collider*> colliders; // 衝突判定対象のコライダーのリスト
public:
	void Add(Collider* c);
	int Raycast(const vnCollide::stSegment& ray, XMVECTOR* hit, XMVECTOR* normal);
	~CollisionManager();
};

