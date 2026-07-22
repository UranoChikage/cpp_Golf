#pragma once
#include <functional>

class SphereShape; //前方宣言：ポインタとして使うだけなら先に宣言しちゃって
class BallsManager;//コンパイラを通す

/***************************************************************
 * クラス名：Ball
 * 機能：PhysicsBodyとSphereShapeを組み合わせたボールの実体
 * *************************************************************/
class Ball : public IBall
{
private:
	PhysicsBody* pb = nullptr;
	SphereShape* shape = nullptr;

	bool isActive = false;

public:
	Ball(float radius,CollisionManager* terrainManager,
		CollisionManager* obstacleManager);
	~Ball();

	XMVECTOR GetPosition() const;

	PhysicsBody* GetPhysicsBody() const { return pb; }

	bool GetIsActive() const override { return isActive; }

	void Shot(const XMVECTOR& dir, float pow);

	void Deactivate() override;
};
