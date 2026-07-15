#pragma once
class IHitReceiver
{
public:
	virtual void OnHit(const XMVECTOR t) = 0;
};