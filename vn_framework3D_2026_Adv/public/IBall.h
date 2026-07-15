#pragma once
#include <functional>
class IBall
{
public:
	bool GetIsActive() const
	{
		return IsActive;
	}
	std::function<IBall()> OnShotAdded;
private:
	virtual void Deactivate() = 0;
	bool IsActive;
};