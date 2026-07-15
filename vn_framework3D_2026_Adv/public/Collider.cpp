#include "../framework.h"
#include "../framework/vn_environment.h"

bool BoxCollider::IsCollide(const vnCollide::stSegment& ray, XMVECTOR* hit, XMVECTOR* nor)
{
	//stSegmentはPos,Dir(Normalize), Lengthを持つ
	// ここにBoxColliderの衝突判定ロジックを実装する

	bool isAxisAligned = IsAxisAligned();
	if (isAxisAligned)
	{
		float tEnter = 0.0f; // 線分がボックスに入るパラメータ
		float tExit = ray.Length; // 線分がボックスから出るパラメータ
		// AABBの衝突判定を行う
		for (int i = 0; i < 3; ++i)
		{
			//DirectXにたまたま便利な関数があったので使う(x,y,zの順番で0,1,2)
			//https://learn.microsoft.com/ja-jp/windows/win32/api/directxmath/nf-directxmath-xmvectorgetbyindex

			float rayOrigin = XMVectorGetByIndex(ray.Pos, i);
			float rayDir = XMVectorGetByIndex(ray.Dir, i);
			float boxMin = XMVectorGetByIndex(min, i);
			float boxMax = XMVectorGetByIndex(max, i);
			if (rayDir == 0.0f)
			{
				// 線分がボックスの面に平行な場合、線分の始点がボックス内にあるかを確認
				if (rayOrigin < boxMin || rayOrigin > boxMax)
				{
					return false; // 衝突なし
				}
			}
			else
			{
				// 線分がボックスの面に垂直な場合、交点を計算
				float t1 = (boxMin - rayOrigin) / rayDir;
				float t2 = (boxMax - rayOrigin) / rayDir;
				if (t1 > t2) std::swap(t1, t2);
				if (t1 > tEnter) tEnter = t1;
				if (t2 < tExit) tExit = t2;
				if (tEnter > tExit)
				{
					return false; // 衝突なし
				}
			}
		}
	}
	else
	{
		// OBBの衝突判定を行う

	}


	return true;
}
bool BoxCollider::IsAxisAligned()
{

	constexpr float epsilon = 0.001f; // 許容誤差の閾値  
	auto isRowAxisAligned = [&](XMVECTOR row) -> bool
		{
			float x = fabsf(XMVectorGetX(row));
			float y = fabsf(XMVectorGetY(row));
			float z = fabsf(XMVectorGetZ(row));
			// どれか1つでも1に近ければ、その行は軸整列
			return (x > 1.0f - epsilon) || (y > 1.0f - epsilon) || (z > 1.0f - epsilon);
		};
	return isRowAxisAligned(rotate.r[0])
		&& isRowAxisAligned(rotate.r[1])
		&& isRowAxisAligned(rotate.r[2]);
}
void BoxCollider::SetRotate(const XMVECTOR& eulerRotate)
{
	rotate = XMMatrixRotationRollPitchYawFromVector(eulerRotate);
}

bool SphereCollider::IsCollide(const vnCollide::stSegment& ray, XMVECTOR* hit, XMVECTOR* nor)
{
	//stSegmentはPos,Dir(Normalize), Lengthを持つ

	vnCollide::stSphere sphere{ center, radius };
	XMVECTOR hits[2];//球体と線分は最大2点交わるから(判別式Dで習ったよね！)
	int count = vnCollide::isCollide(hits, &ray, &sphere);
	if (count == 0)return false;//交わらんかったから早期リターン
	//交点が線分の範囲内(0<=t<=Length)にあるかを確認する
	//tは距離
	float bestT = -1.0f;
	XMVECTOR bestHit;

	for (int i = 0; i < count; i++)
	{
		XMVECTOR hitPoint = hits[i];
		float t = XMVectorGetX(XMVector3Length(hitPoint - ray.Pos));
		if (t >= 0 && t <= ray.Length)
		{
			if (bestT < 0 || t < bestT)
			{
				bestT = t;
				bestHit = hitPoint;
			}
		}
	}
	if (bestT < 0)return false;//線分の範囲内に交点がなかった

	*hit = bestHit;
	*nor = XMVector3Normalize(bestHit - center);
	return true;
}
bool MeshCollider::IsCollide(const vnCollide::stSegment& ray, XMVECTOR* hit, XMVECTOR* nor)
{
	//stSegmentはPos,Dir(Normalize), Lengthを持つ
	// ここにMeshColliderの衝突判定ロジックを実装する
	return false;

}