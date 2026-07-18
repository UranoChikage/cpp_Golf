#include "../framework.h"
#include "../framework/vn_environment.h"

void CollisionManager::Add(Collider* c)
{
	colliders.push_back(c);
}
int CollisionManager::Raycast(const vnCollide::stSegment& ray, XMVECTOR* hit, XMVECTOR* normal)
{
	float closestLength = FLT_MAX;
	//そのままhit,norを使うと、最も近いコライダーの情報が上書きされてしまうから
	XMVECTOR tmpHit, tmpNormal;
	//colliderのリストの中で最も近いコライダーのインデックス
	int closestID = -1;
	
	//コライダーのリストを順番にチェックして、衝突判定を行う
	for (size_t i = 0; i < colliders.size(); ++i)//size_tは-の値にするとヤバイので注意（ネットでみた)
	{
		Collider* c = colliders[i];
		if (c->IsCollide(ray, &tmpHit, &tmpNormal))
		{
			float length;
			XMVECTOR subtractV;
			subtractV = XMVectorSubtract(ray.Pos, tmpHit);
			length = XMVectorGetX(XMVector3Length(subtractV));
			if (length < closestLength)
			{
				closestLength = length;
				closestID = static_cast<int>(i);// 最も近いコライダーのインデックスを更新
				*hit = tmpHit;
				*normal = tmpNormal;
			}
		}

	}
	return closestID;
}

//全コライダーに対してOverlapを聞いて、重なってたやつを片っ端からContactに詰めてる
bool CollisionManager::OverlapSphere(const XMVECTOR& center, float radius, std::vector<Contact>& outContacts)
{
	for (Collider* c : colliders)
	{
		XMVECTOR closest, normal;
		float penetration;
		if (c->Overlap(center, radius, &closest, &normal, &penetration))
		{
			Contact contact{};
			contact.isHit = true;
			XMStoreFloat3(&contact.point, closest);
			XMStoreFloat3(&contact.normal, normal);
			contact.penetration = penetration;
			contact.hit = nullptr;
			contact.isTigger = false;
			outContacts.push_back(contact);
		}
	}
	return !outContacts.empty();
}

CollisionManager::~CollisionManager()
{
	for (Collider* c : colliders) 
	{
		delete c; // コライダーのメモリを解放
	}
	colliders.clear(); // リストをクリア
}