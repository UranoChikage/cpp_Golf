#pragma once
class ShotInput
{
private:
	Ball* ball;
	DirectionController* dirCon;
	float maxAimPower;
	float meterSpeed;//Meterの往復速度

	//内部
	float meterValue = 0.0f; // 0~1
	bool meterGoingUp = true; // Meterの増加方向
	bool isAiming = false;
	bool isStopped = true;
	//ショットの方向のプレビュー
	XMVECTOR aimDirection = XMVectorSet(0, 0, 1, 0);//Forward


public:
	ShotInput(Ball* Ball, DirectionController* DirCon);
	void ShotInputUpdate(const float deltaTime);
	//IsAimingのプロパティ的なの
	bool GetIsAiming()const { return isAiming; }
	void SetIsAiming(bool value);
	//ショットパワーの割合(0~1)
	float GetMeterValue()const { return meterValue; }
};