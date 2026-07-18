#include "../framework.h"
#include "../framework/vn_environment.h"

Collider::~Collider()
{
}

bool BoxCollider::IsCollide(const vnCollide::stSegment& ray, XMVECTOR* hit, XMVECTOR* nor)
{
	//stSegmentはPos,Dir(Normalize), Lengthを持つ
	// ここにBoxColliderの衝突判定ロジックを実装する

	float tEnter = 0.0f; // 線分がボックスに入るパラメータ
	float tExit = ray.Length; // 線分がボックスから出るパラメータ
	int hitAxis = -1; // 衝突した軸のインデックス
	bool hitFace = false; // 当たった面がMinかMaxかを判定するためのフラグ
	bool isAxisAligned = IsAxisAligned();
	if (isAxisAligned)
	{
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
				if (t1 > tEnter)
				{
					hitAxis = i; // 衝突した軸を記録
					tEnter = t1;
					hitFace = (rayDir > 0) ? false : true; // 当たった面を記録
				}
				if (t2 < tExit) tExit = t2;
				if (tEnter > tExit)
				{
					return false; // 衝突なし
				}
			}
		}

		// 衝突点と法線を計算する
		if (hit)
		{
			*hit = ray.Pos + ray.Dir * tEnter;
		}
		if (nor)
		{
			XMVECTOR normal = XMVectorZero();
			if (hitAxis != -1)
			{
				// 衝突した軸に応じて法線を設定
				switch (hitAxis)
				{
				case 0: // X軸
					normal = hitFace ? XMVectorSet(1, 0, 0, 0) : XMVectorSet(-1, 0, 0, 0);
					break;
				case 1: // Y軸
					normal = hitFace ? XMVectorSet(0, 1, 0, 0) : XMVectorSet(0, -1, 0, 0);
					break;
				case 2: // Z軸
					normal = hitFace ? XMVectorSet(0, 0, 1, 0) : XMVectorSet(0, 0, -1, 0);
					break;
				default:
					break;
				}
			}
			*nor = normal;
		}
	}
	else
	{
		// OBBの衝突判定を行う
		//先生がヒントをくれた！world座標からlocal座標に変換してからAABBの衝突判定を行うようにする！
		XMMATRIX localMatrix = GetLocalMatrix();
		//ray.posは回転と平行移動の影響を受けるので、w=1
		XMVECTOR localPos = MatrixMath::MultiplyPoint(localMatrix, ray.Pos);
		//ray.dirは回転の影響を受けるので、w=0
		XMVECTOR localDir = MatrixMath::MultiplyVector(localMatrix, ray.Dir);
		// AABBの衝突判定を行う
		for (int i = 0; i < 3; ++i)
		{
			//DirectXにたまたま便利な関数があったので使う(x,y,zの順番で0,1,2)
			//https://learn.microsoft.com/ja-jp/windows/win32/api/directxmath/nf-directxmath-xmvectorgetbyindex

			float rayOrigin = XMVectorGetByIndex(localPos, i);
			float rayDir = XMVectorGetByIndex(localDir, i);
			float boxMin = -XMVectorGetByIndex(half, i);
			float boxMax = XMVectorGetByIndex(half, i);
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

				if (t1 > tEnter)
				{
					hitAxis = i; // 衝突した軸を記録
					tEnter = t1;
					hitFace = (rayDir > 0) ? false : true; // 当たった面を記録
				}
				if (t2 < tExit) tExit = t2;
				if (tEnter > tExit)
				{
					return false; // 衝突なし
				}
			}

		}
		// 衝突点と法線を計算する
		if (hit)
		{
			*hit = ray.Pos + ray.Dir * tEnter;
		}
		if (nor)
		{
			XMVECTOR normal = XMVectorZero();
			if (hitAxis != -1)
			{
				// 衝突した軸に応じて法線を設定
				switch (hitAxis)
				{
				case 0: // X軸
					normal = hitFace ? XMVectorSet(1, 0, 0, 0) : XMVectorSet(-1, 0, 0, 0);
					break;
				case 1: // Y軸
					normal = hitFace ? XMVectorSet(0, 1, 0, 0) : XMVectorSet(0, -1, 0, 0);
					break;
				case 2: // Z軸
					normal = hitFace ? XMVectorSet(0, 0, 1, 0) : XMVectorSet(0, 0, -1, 0);
					break;
				default:
					break;
				}
			}
			normal = MatrixMath::MultiplyVector(rotate, normal);
			*nor = normal;
		}
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
XMMATRIX BoxCollider::GetLocalMatrix()
{
	XMMATRIX invRotate = MatrixMath::Transpose(rotate);

	//平行移動の逆行列は平行移動の逆方向への移動なので、-centerを掛ける
	//自作の逆行列はむずかったので、DirectXMathの関数を使うことにした
	XMMATRIX invTranslate = XMMatrixTranslationFromVector(-center);

	return invTranslate * invRotate;
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

	float bestT = -1.0f;
	XMVECTOR bestHit;
	XMVECTOR bestNormal;

	for (const auto& tri : Triangles)
	{
		XMVECTOR hitPoint;
		if (!vnCollide::isCollide(&hitPoint, &ray, &tri))
		{
			continue; // この三角形とは衝突しなかった
		}

		//交点が線分の範囲内(0<=t<=Length)にあるかを確認する
		float t = XMVectorGetX(XMVector3Length(hitPoint - ray.Pos));
		if (t < 0 || t > ray.Length)
		{
			continue; // 線分の範囲外
		}

		if (bestT < 0 || t < bestT)
		{
			bestT = t;
			bestHit = hitPoint;
			bestNormal = tri.plane.Normal; // 三角形が既に法線を持っているのでそのまま使う
		}
	}

	if (bestT < 0)return false;//どの三角形とも衝突しなかった

	*hit = bestHit;
	*nor = bestNormal;
	return true;
}
std::vector<vnCollide::stTriangle> MeshCollider::BuildTriangles(vnModel* p)
{
	vnCollide::stTriangle tri;
	std::vector<vnCollide::stTriangle> triangles;

	int vnum = p->getVertexNum();
	unsigned short* idx = p->getIndex();
	vnVertex3D* vtx = p->getVertex();

	vnModel_MeshData* pMesh = p->getMesh();
	int meshNum = p->getMeshNum();
	for (int i = 0; i < meshNum; i++)
	{
		int idxnum = pMesh[i].IndexNum;
		int s_idx = pMesh[i].StartIndex;
		for (int j = 0; j < idxnum; j += 3)
		{
			XMVECTOR v1 = XMVectorSet(vtx[idx[s_idx + j + 0]].x, vtx[idx[s_idx + j + 0]].y, vtx[idx[s_idx + j + 0]].z, 0.0f);
			XMVECTOR v2 = XMVectorSet(vtx[idx[s_idx + j + 1]].x, vtx[idx[s_idx + j + 1]].y, vtx[idx[s_idx + j + 1]].z, 0.0f);
			XMVECTOR v3 = XMVectorSet(vtx[idx[s_idx + j + 2]].x, vtx[idx[s_idx + j + 2]].y, vtx[idx[s_idx + j + 2]].z, 0.0f);

			v1 = XMVector3TransformCoord(v1, *p->getWorld());
			v2 = XMVector3TransformCoord(v2, *p->getWorld());
			v3 = XMVector3TransformCoord(v3, *p->getWorld());

			tri.fromPoints(&v1, &v2, &v3);
			triangles.push_back(tri);
		}
	}
	return triangles;
}