#include "../framework.h"
#include "../framework/vn_environment.h"

XMMATRIX MatrixMath::Translation(const float tx, const float ty, const float tz)
{
	XMMATRIX m = XMMatrixIdentity();
	m.r[3] = XMVectorSet(tx, ty, tz, 0);
	return m;
}
//Vector3オーバーロード版
XMMATRIX MatrixMath::Translation(const XMVECTOR& v)
{
	return Translation(XMVectorGetX(v), XMVectorGetY(v), XMVectorGetZ(v));
}
XMMATRIX MatrixMath::RotationX(const float angleDeg)
{
	float rad = XMConvertToRadians(angleDeg); // 角度をラジアンに変換
	float cos = XMScalarCos(rad);
	float sin = XMScalarSin(rad);

	XMMATRIX m = XMMatrixIdentity();

	m.r[1] = XMVectorSet(0.0f, cos, -sin, 0.0f);
	m.r[2] = XMVectorSet(0.0f, sin, cos, 0.0f);

	return m;
}
XMMATRIX MatrixMath::RotationY(const float angleDeg)
{
	float rad = XMConvertToRadians(angleDeg); // 角度をラジアンに変換
	float cos = XMScalarCos(rad);
	float sin = XMScalarSin(rad);
	XMMATRIX m = XMMatrixIdentity();
	m.r[0] = XMVectorSet(cos, 0.0f, sin, 0.0f);
	m.r[2] = XMVectorSet(-sin, 0.0f, cos, 0.0f);
	return m;
}

XMMATRIX MatrixMath::RotationZ(const float angleDeg)
{
	float rad = XMConvertToRadians(angleDeg); // 角度をラジアンに変換
	float cos = XMScalarCos(rad);
	float sin = XMScalarSin(rad);
	XMMATRIX m = XMMatrixIdentity();
	m.r[0] = XMVectorSet(cos, -sin, 0.0f, 0.0f);
	m.r[1] = XMVectorSet(sin, cos, 0.0f, 0.0f);
	return m;
}

// Vector3 で3軸まとめて渡す版
//順番が関係するので注意
XMMATRIX MatrixMath::Rotation(const XMVECTOR& anglesDeg)
{
	XMMATRIX rotX = RotationX(XMVectorGetX(anglesDeg));
	XMMATRIX rotY = RotationY(XMVectorGetY(anglesDeg));
	XMMATRIX rotZ = RotationZ(XMVectorGetZ(anglesDeg));
	// 回転の順番は Z → Y → X とする（一般的なオイラー角の順番）
	return rotZ * rotY * rotX;
}
XMMATRIX MatrixMath::Scale(const float sx, const float sy, const float sz)
{
	XMMATRIX m = XMMatrixIdentity();
	m.r[0] = XMVectorSet(sx, 0.0f, 0.0f, 0.0f);
	m.r[1] = XMVectorSet(0.0f, sy, 0.0f, 0.0f);
	m.r[2] = XMVectorSet(0.0f, 0.0f, sz, 0.0f);
	return m;
}

// Vector3 オーバーロード版
XMMATRIX MatrixMath::Scale(const XMVECTOR& v)
{
	return Scale(XMVectorGetX(v), XMVectorGetY(v), XMVectorGetZ(v));
}
//均等にスケールする版
XMMATRIX MatrixMath::Scale(float s)
{
	return Scale(s, s, s);
}
XMMATRIX MatrixMath::RotationAxis(const XMVECTOR& axis, const float angleDeg)
{
	float rad = XMConvertToRadians(angleDeg);
	float cosv = XMScalarCos(rad);
	float sinv = XMScalarSin(rad);
	float t = 1.0f - cosv;

	// 正規化してから演算用にfloatとして取り出す
	XMVECTOR v = XMVector3Normalize(axis);
	float x = XMVectorGetX(v);
	float y = XMVectorGetY(v);
	float z = XMVectorGetZ(v);

	XMMATRIX m = XMMatrixIdentity();

	m.r[0] = XMVectorSet(t * x * x + cosv, t * x * y - sinv * z, t * x * z + sinv * y, 0.0f);
	m.r[1] = XMVectorSet(t * y * x + sinv * z, t * y * y + cosv, t * y * z - sinv * x, 0.0f);
	m.r[2] = XMVectorSet(t * z * x - sinv * y, t * z * y + sinv * x, t * z * z + cosv, 0.0f);
	// m.r[3] は単位行列のまま (0,0,0,1)

	return m;
}
XMVECTOR MatrixMath::MultiplyPoint(const XMMATRIX& m, const XMVECTOR& v)
{
	//w = 1で掛け算(位置の変換)
	XMVECTOR v4 = XMVectorSetW(v, 1.0f);
	return XMVector3Transform(v4, m);
}
XMVECTOR MatrixMath::MultiplyVector(const XMMATRIX& m, const XMVECTOR& v)
{
	//w = 0で掛け算(方向の変換：平行移動の影響を受けない )
	XMVECTOR v4 = XMVectorSetW(v, 0.0f);
	return XMVector3Transform(v4, m);
}
void MatrixMath::ApplyToTransform(XMMATRIX& m, vnObject* obj)
{
	//平行移動成分をposにセット
	obj->setPosition(&m.r[3]);
	XMVECTOR q = XMQuaternionRotationMatrix(m);
	obj->setQuaternion(&q);
}

bool MatrixMath::GetHitInfo(CollisionManager* manager, const float x, const float z, const float rayOriginHeight, const float rayMaxDistance, float* height, XMVECTOR* hitNormal)
{
	vnCollide::stSegment ray;
	XMVECTOR p1 = XMVectorSet(x, rayOriginHeight, z, 0);
	XMVECTOR p2 = XMVectorSet(x, rayOriginHeight - rayMaxDistance, z, 0);// ray(真下)の終点を設定

	ray.fromPoints(&p1, &p2);

	XMVECTOR hit, normal;
	int id = manager->Raycast(ray, &hit, &normal);
	if (id != -1)
	{
		*height = XMVectorGetY(hit);
		*hitNormal = normal;
	}
	return id != -1;
}

XMVECTOR MatrixMath::GetSlopeForce(XMVECTOR& vector, XMVECTOR& normal)
{
	float dot = Dot(vector, normal); // 法線方向の重力成分の大きさ
	XMVECTOR slopeForce = vector - normal * dot; // 法線方向の重力成分を打ち消すベクトル
	return slopeForce;
}
bool MatrixMath::IsWall(XMVECTOR groundNormal, float limitAngle)
{
	XMVECTOR up = XMVectorSet(0, 1, 0, 0);
	float dot = Dot(groundNormal, up);
	float angleRad = acosf(dot);
	float angleDeg = XMConvertToDegrees(angleRad);
	return angleDeg > limitAngle;
}

float MatrixMath::Dot(const XMVECTOR& a, const XMVECTOR& b)
{
	return XMVectorGetX(XMVector3Dot(a, b));
}

XMMATRIX MatrixMath::Transpose(const XMMATRIX& m)
{
	XMMATRIX result;
	result.r[0] = XMVectorSet(XMVectorGetX(m.r[0]), XMVectorGetX(m.r[1]), XMVectorGetX(m.r[2]), XMVectorGetX(m.r[3]));
	result.r[1] = XMVectorSet(XMVectorGetY(m.r[0]), XMVectorGetY(m.r[1]), XMVectorGetY(m.r[2]), XMVectorGetY(m.r[3]));
	result.r[2] = XMVectorSet(XMVectorGetZ(m.r[0]), XMVectorGetZ(m.r[1]), XMVectorGetZ(m.r[2]), XMVectorGetZ(m.r[3]));
	result.r[3] = XMVectorSet(XMVectorGetW(m.r[0]), XMVectorGetW(m.r[1]), XMVectorGetW(m.r[2]), XMVectorGetW(m.r[3]));
	return result;
}

