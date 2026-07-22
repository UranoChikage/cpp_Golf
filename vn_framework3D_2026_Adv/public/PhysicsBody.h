#pragma once
#include <functional>

class IBall;//前方宣言

/***************************************************************
 * 2026/07/17
 * クラス名：PhysicsBody
 * 機能：重力・地形/障害物との衝突・転がり・跳ね返りを扱う物理挙動クラス
 * *************************************************************/

//AddForceで力の加え方を切り替える種類
enum class ForceMode
{
	Force,
	Impulse,
	VelocityChange,
};
//物理パラメーターはこのクラスでのみ変更可能
class PhysicsBody : public vnObject
{
private:
	//物理パラメーター
	float gravity = 9.8f; // 重力加速度
	float rollingFriction = 0.015f; // 転がり抵抗係数
	float bounciness = 0.15f; // 反発係数

	//停止判定パラメーター
	float timeOut = 3;
	float minMagnitude = 0.06f; // 動きが止まったとみなす速度の閾値
	float velocityChangeThreshold = 0.1f; // 速度の変化がこの値以下なら停止とみなす

	ICollisionShape* collisionShape = nullptr;

	IBall* owner = nullptr;//このPhysicsBodyを持っているBall

	// 内部状態
	XMVECTOR velocity = XMVectorZero(); // 現在の速度
	bool isMoving = false; // 動いているかどうか
	bool isGrounded = false; // 地面に接しているかどうか
	bool isObstaclehit = false;
	float velocityChange = 0.0f;

	float timeOutTimer = 0.0f;
	XMVECTOR prevVelocity = XMVectorZero();

	void ApplyBounce(const XMVECTOR& normal);

public:
	PhysicsBody();

	//衝突判定対象の形状を設定
	void SetCollisionShape(ICollisionShape* shape);

	//このPhysicsBodyを持っているBallを設定
	void SetOwner(IBall* value) { owner = value; }

	//状態
	bool GetIsMoving() const { return isMoving; }
	void SetIsMoving(bool value);

	float GetVelocityChange() const { return velocityChange; }

	//停止/再開が切り替わったときに呼ばれるコールバック(引数は「止まったかどうか」)
	std::function<void(bool)> OnIsStoppedChanged;

	//毎フレーム呼び出す物理更新処理(deltaTimeは呼び出し側から渡す)
	void Step(float deltaTime);

	void AddForce(const XMVECTOR& launchDir, ForceMode forceMode);
};
