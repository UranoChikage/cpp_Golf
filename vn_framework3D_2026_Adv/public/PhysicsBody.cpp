#include "../framework.h"
#include "../framework/vn_environment.h"

PhysicsBody::PhysicsBody()
{
}

void PhysicsBody::SetCollisionShape(ICollisionShape* shape)
{
	collisionShape = shape;
	if (collisionShape == nullptr)
	{
		OutputDebugString(L"CollisionShapeが設定されていません！\n");
	}
}

void PhysicsBody::SetIsMoving(bool value)
{
	if (isMoving == value) return;
	isMoving = value;
	if (OnIsStoppedChanged) OnIsStoppedChanged(!isMoving);
}

void PhysicsBody::Step(float deltaTime)
{
	if (collisionShape == nullptr) return;

	XMVECTOR pos = *getPosition();

	if (!isGrounded) velocity = XMVectorSetY(velocity, XMVectorGetY(velocity) - gravity * deltaTime);

	//地形衝突判定
	XMFLOAT3 posF3;
	XMStoreFloat3(&posF3, pos);
	Contact c = collisionShape->QueryTerrain(&posF3);
	bool wasGrounded = isGrounded; // 前フレームの接地状態を保存
	XMVECTOR cNormal = XMVectorZero();
	if (c.isHit)
	{
		pos = XMVectorSetY(pos, XMVectorGetY(pos) + c.penetration);
		isGrounded = true;
		cNormal = XMLoadFloat3(&c.normal);
	}
	else
	{
		isGrounded = false; // 空中にいる
	}
	//跳ね返り処理＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝

	//障害物
	XMStoreFloat3(&posF3, pos);
	std::vector<Contact> obstacle;
	collisionShape->QueryObstacle(&posF3, obstacle);
	bool wasObstaclrHit = isObstaclehit;
	if (!obstacle.empty())
	{
		//転がり抵抗をかける
		for (auto& o : obstacle)
		{
			XMVECTOR oNormal = XMLoadFloat3(&o.normal);
			if (!MatrixMath::IsWall(oNormal, 30.0f)) //ひとまず30°以上が跳ねっかえる障害物
			{
				if (!o.isTigger)
				{
					isGrounded |= wasObstaclrHit; //障害物に当たってもほぼ床みたいなもんなので接地ということに
					pos += oNormal * o.penetration;
				}
			}
		}
		isObstaclehit = true;
	}
	else
	{
		isObstaclehit = false;
	}

	bool terrainNew = c.isHit && !wasGrounded;
	bool obstacleNew = !obstacle.empty() && !wasObstaclrHit;
	if (terrainNew && obstacleNew)
	{
		//イテレーションてので
		//繰り返しBounceやめり込み防止をはさむことでオブジェクト同士の押し合いをなんやかんやできるらしい
		//いつか実装してみたい。
		for (auto& o : obstacle)
		{
			XMVECTOR oNormal = XMLoadFloat3(&o.normal);
			XMVECTOR combined = XMVector3Normalize(cNormal + oNormal);
			ApplyBounce(combined);
		}
	}
	else if (terrainNew)
	{
		ApplyBounce(cNormal);
	}
	else if (obstacleNew)
	{
		for (auto& o : obstacle)
		{
			if (o.hit != nullptr) o.hit->OnHit(owner, pos);

			if (!o.isTigger)
			{
				XMVECTOR oNormal = XMLoadFloat3(&o.normal);
				ApplyBounce(oNormal);
			}
		}
	}

	//転がり＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝

	if (c.isHit)
	{
		//転がり抵抗をかける
		if (MatrixMath::IsWall(cNormal, 60.0f)) //ひとまず60°までが床
		{
			ApplyBounce(cNormal);
		}
		velocity *= (1.0f - rollingFriction);
		XMVECTOR down = XMVectorSet(0.0f, -gravity, 0.0f, 0.0f);
		XMVECTOR slopeForce = MatrixMath::GetSlopeForce(down, cNormal);
		velocity += slopeForce * deltaTime; // 傾斜に沿って加速
	}

	if (isMoving)
	{
		XMVECTOR deltapos = velocity * deltaTime;
		XMFLOAT3 deltaF3, normalF3;
		XMStoreFloat3(&deltaF3, deltapos);
		XMStoreFloat3(&normalF3, cNormal);
		collisionShape->UpdateOrientation(&deltaF3, &normalF3);
	}

	pos += velocity * deltaTime;
	setPosition(&pos);

	if (isGrounded)
	{
		velocityChange = XMVectorGetX(XMVector3Length(velocity - prevVelocity));
		if (velocityChange < velocityChangeThreshold) // 速度がほぼ変化していない
		{
			timeOutTimer += deltaTime;
			if (timeOutTimer >= timeOut)
			{
				velocity = XMVectorZero();
				SetIsMoving(false);
			}
		}
		else
		{
			timeOutTimer = 0.0f;
		}
	}

	prevVelocity = velocity;
	if (isGrounded && XMVectorGetX(XMVector3Length(velocity)) < minMagnitude)
	{
		velocity = XMVectorZero();
		SetIsMoving(false);
	}
}

void PhysicsBody::ApplyBounce(const XMVECTOR& normal)
{
	// 速度を法線方向と接線方向に分解
	float normalSpeed = MatrixMath::Dot(velocity, normal); // 法線方向の速度
	if (normalSpeed < 0.0f)
	{
		XMVECTOR normalVelocity = normal * normalSpeed;
		XMVECTOR tangentialVelocity = velocity - normalVelocity; // 接線方向の速度

		velocity = tangentialVelocity - normalVelocity * bounciness; // 反発係数をかける
		if (fabsf(XMVectorGetY(velocity)) < 0.5f) // 小さいときは転がりに切り替える
		{
			// Vector3.ProjectOnPlane相当：速度から法線方向成分を取り除く
			float d = MatrixMath::Dot(velocity, normal);
			velocity = velocity - normal * d;
		}
	}
}

void PhysicsBody::AddForce(const XMVECTOR& launchDir, ForceMode forceMode)
{
	switch (forceMode)
	{
	case ForceMode::Force:

		break;
	case ForceMode::Impulse:
		velocity += launchDir;
		break;
	case ForceMode::VelocityChange:
		velocity = launchDir;
		break;
	}

	isMoving = true;
	isGrounded = false;
}
