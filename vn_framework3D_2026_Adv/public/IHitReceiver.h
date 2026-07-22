#pragma once

class IBall;//前方宣言

class IHitReceiver
{
public:
	virtual void OnHit(IBall* ball) = 0;
	virtual void OnHit(const XMVECTOR t) = 0;
	virtual void OnHit(IBall* ball, const XMVECTOR t)
	{
		OnHit(ball);
		OnHit(t);
	}
};