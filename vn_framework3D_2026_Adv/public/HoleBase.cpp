#include "../framework.h"
#include "../framework/vn_environment.h"

void HoleBase::OnHit(IBall* ball)
{
	if (ball == nullptr) return;

	if (OnHoleIn) OnHoleIn(ball);
	OutputDebugString(L"ホールイン！\n");
}
