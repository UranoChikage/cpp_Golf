#include "../framework.h"
#include "../framework/vn_environment.h"

ShotInput::ShotInput(Ball* Ball, DirectionController* DirCon)
{
    ball = Ball;
    dirCon = DirCon;

    maxAimPower = 15.0f;
    meterSpeed = 1.0f;

    aimDirection = dirCon->GetDirection(); // 初期のショット方向を設定
    
    PhysicsBody *pb = ball->GetPhysicsBody();
    pb->OnIsStoppedChanged = [this](bool stopped) { isStopped = stopped; };

}
void ShotInput::ShotInputUpdate(const float deltaTime)
{
    if (vnMouse::trgL())
    {
        if (!GetIsAiming() && isStopped)
        {
            SetIsAiming(true) ; // 1回目クリック → メーター開始
        }
        else
        {
            // 2回目クリック → ショット
            float shotPower = meterValue * maxAimPower;
            //ballController.Shot(aimDirection, shotPower);
            ball->Shot(aimDirection, shotPower);

            meterValue = 0.0f;
            meterGoingUp = true;
            SetIsAiming(false);
            isStopped = false; // ボールが飛んでる間は入力を受け付けない
        }
    }
    if (GetIsAiming())
    {
        if (meterGoingUp)
        {

            meterValue += meterSpeed * deltaTime;

            if (meterValue >= 1.0f)
            {
                meterValue = 1.0f;
                meterGoingUp = false;
            }
        }

        else
        {
            meterValue -= meterSpeed * deltaTime;
            if (meterValue <= 0.0f)
            {
                meterValue = 0.0f;
                meterGoingUp = true;
            }

        }
        UIManager::GetInstance().UpdateShotGauge(meterValue);
    }
    if (GetIsAiming())
    {
        aimDirection = dirCon->GetDirection(); // 毎フレームショット方向を更新
    }
}
void ShotInput::SetIsAiming(bool value)
{
    isAiming = value;
    UIManager::GetInstance().ActiveShotGauge(value);
}