#pragma once
#include <functional>

/***************************************************************
 * インターフェイス名：IBall
 *
 * *************************************************************/
class IBall
{
public:
	virtual ~IBall() = default;

	virtual bool GetIsActive() const = 0;
	virtual void Deactivate() = 0;

	std::function<void(IBall*)> OnShotAdded;
};