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
//2026/07/18　AIに手伝ってもらって追加
namespace
{
	float ClampF(float v, float lo, float hi)
	{
		return (v < lo) ? lo : (v > hi ? hi : v);
	}
}

//2026/07/18　AIに手伝ってもらって追加(QueryObstacle用のPhysics.OverlapSphere相当が無かったため)
bool BoxCollider::Overlap(const XMVECTOR& sphereCenter, float sphereRadius,
	XMVECTOR* outClosestPoint, XMVECTOR* outNormal, float* outPenetration)
{
	XMVECTOR closest;
	if (IsAxisAligned())
	{
		float cx = ClampF(XMVectorGetX(sphereCenter), XMVectorGetX(min), XMVectorGetX(max));
		float cy = ClampF(XMVectorGetY(sphereCenter), XMVectorGetY(min), XMVectorGetY(max));
		float cz = ClampF(XMVectorGetZ(sphereCenter), XMVectorGetZ(min), XMVectorGetZ(max));
		closest = XMVectorSet(cx, cy, cz, 0.0f);
	}
	else
	{
		//ワールド→ローカルに変換してクランプしてから、ワールドに戻す
		XMMATRIX localMatrix = GetLocalMatrix();
		XMVECTOR localCenter = MatrixMath::MultiplyPoint(localMatrix, sphereCenter);

		float lx = ClampF(XMVectorGetX(localCenter), -XMVectorGetX(half), XMVectorGetX(half));
		float ly = ClampF(XMVectorGetY(localCenter), -XMVectorGetY(half), XMVectorGetY(half));
		float lz = ClampF(XMVectorGetZ(localCenter), -XMVectorGetZ(half), XMVectorGetZ(half));
		XMVECTOR localClosest = XMVectorSet(lx, ly, lz, 0.0f);

		//GetLocalMatrix()は「ワールド→ローカル」なので、その逆(ローカル→ワールド)は回転してから平行移動
		XMMATRIX worldMatrix = rotate * MatrixMath::Translation(center);
		closest = MatrixMath::MultiplyPoint(worldMatrix, localClosest);
	}

	XMVECTOR diff = sphereCenter - closest;
	float dist = XMVectorGetX(XMVector3Length(diff));
	if (dist > sphereRadius) return false; // 重なっていない

	XMVECTOR normal = (dist > 0.0001f) ? diff / dist : XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	if (outClosestPoint) *outClosestPoint = closest;
	if (outNormal) *outNormal = normal;
	if (outPenetration) *outPenetration = sphereRadius - dist;
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

//AI
bool SphereCollider::Overlap(const XMVECTOR& sphereCenter, float sphereRadius,
	XMVECTOR* outClosestPoint, XMVECTOR* outNormal, float* outPenetration)
{
	XMVECTOR diff = sphereCenter - center;
	float dist = XMVectorGetX(XMVector3Length(diff));
	float sumRadius = sphereRadius + radius;
	if (dist > sumRadius) return false; // 重なっていない

	XMVECTOR normal = (dist > 0.0001f) ? diff / dist : XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	XMVECTOR closest = center + normal * radius;

	if (outClosestPoint) *outClosestPoint = closest;
	if (outNormal) *outNormal = normal;
	if (outPenetration) *outPenetration = sumRadius - dist;
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

//AI 点から三角形への最近接点を求める処理。エッジや頂点にはみ出すケースも含めて場合分けする。
// (ボロノイ領域分割法というやつ)
namespace
{
	XMVECTOR ClosestPointOnTriangle(const XMVECTOR& p, const XMVECTOR& a, const XMVECTOR& b, const XMVECTOR& c)
	{
		XMVECTOR ab = b - a;
		XMVECTOR ac = c - a;
		XMVECTOR ap = p - a;

		float d1 = MatrixMath::Dot(ab, ap);
		float d2 = MatrixMath::Dot(ac, ap);
		if (d1 <= 0.0f && d2 <= 0.0f) return a; // aが最近接

		XMVECTOR bp = p - b;
		float d3 = MatrixMath::Dot(ab, bp);
		float d4 = MatrixMath::Dot(ac, bp);
		if (d3 >= 0.0f && d4 <= d3) return b; // bが最近接

		float vc = d1 * d4 - d3 * d2;
		if (vc <= 0.0f && d1 >= 0.0f && d3 <= 0.0f)
		{
			float v = d1 / (d1 - d3);
			return a + ab * v; // 辺ab上
		}

		XMVECTOR cp = p - c;
		float d5 = MatrixMath::Dot(ab, cp);
		float d6 = MatrixMath::Dot(ac, cp);
		if (d6 >= 0.0f && d5 <= d6) return c; // cが最近接

		float vb = d5 * d2 - d1 * d6;
		if (vb <= 0.0f && d2 >= 0.0f && d6 <= 0.0f)
		{
			float w = d2 / (d2 - d6);
			return a + ac * w; // 辺ac上
		}

		float va = d3 * d6 - d5 * d4;
		if (va <= 0.0f && (d4 - d3) >= 0.0f && (d5 - d6) >= 0.0f)
		{
			float w = (d4 - d3) / ((d4 - d3) + (d5 - d6));
			return b + (c - b) * w; // 辺bc上
		}

		//面の内側
		float denom = 1.0f / (va + vb + vc);
		float v = vb * denom;
		float w = vc * denom;
		return a + ab * v + ac * w;
	}
}

//AI 三角形を全部見て一番近い点との距離で重なりを判定してる
bool MeshCollider::Overlap(const XMVECTOR& sphereCenter, float sphereRadius,
	XMVECTOR* outClosestPoint, XMVECTOR* outNormal, float* outPenetration)
{
	float bestDist = FLT_MAX;
	XMVECTOR bestPoint = XMVectorZero();
	XMVECTOR bestNormal = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	bool found = false;

	for (const auto& tri : Triangles)
	{
		XMVECTOR closest = ClosestPointOnTriangle(sphereCenter, tri.v[0], tri.v[1], tri.v[2]);
		float dist = XMVectorGetX(XMVector3Length(sphereCenter - closest));
		if (dist < bestDist)
		{
			bestDist = dist;
			bestPoint = closest;
			bestNormal = tri.plane.Normal;
			found = true;
		}
	}

	if (!found || bestDist > sphereRadius) return false; // 重なっている三角形がなかった

	XMVECTOR diff = sphereCenter - bestPoint;
	XMVECTOR normal = (bestDist > 0.0001f) ? diff / bestDist : bestNormal;

	if (outClosestPoint) *outClosestPoint = bestPoint;
	if (outNormal) *outNormal = normal;
	if (outPenetration) *outPenetration = sphereRadius - bestDist;
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

			//このモデルの頂点の並び順だと法線が内側を向いてしまうので、v2とv3を入れ替えて巻き順を逆にする
			tri.fromPoints(&v1, &v3, &v2);
			triangles.push_back(tri);
		}
	}
	return triangles;
}