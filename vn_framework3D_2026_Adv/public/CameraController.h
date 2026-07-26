#pragma once
class CameraController
{
private:
	DirectionController *dirCon;
	XMVECTOR* target = nullptr; // カメラが追従するターゲット（プレイヤーの位置）
public:
	CameraController(DirectionController* DirCon);
	void SetTarget(XMVECTOR* targetPos);
	void CameraUpdate(const float deltaTime);
};