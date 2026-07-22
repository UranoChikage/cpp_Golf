#pragma once
#include <functional>

/***************************************************************
 * クラス名：HoleBase
 * 機能：ホール(穴)に何かが当たったことを検知する
 * *************************************************************/
class HoleBase : public IHitReceiver
{
public:
	std::function<void(IBall*)> OnHoleIn;

	void OnHit(IBall* ball) override;
	void OnHit(const XMVECTOR t) override {}
};
