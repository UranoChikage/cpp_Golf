#include "../framework.h"
#include "../framework/vn_environment.h"

namespace
{
	constexpr float PLAYER_SCORE_LINE_HEIGHT = 20.0f;
}

UIManager::~UIManager()
{
	for (auto& pair : playerScores)
	{
		delete pair.second;
	}
}

void UIManager::AddPlayer(int id, const std::wstring& name)
{
	float posY = PLAYER_SCORE_LINE_HEIGHT * playerScores.size();
	playerScores[id] = new PlayerScore(name, 0.0f, posY);
}

void UIManager::UpdateScore(int id, int score)
{
	auto it = playerScores.find(id);
	if (it == playerScores.end()) return;
	it->second->ChangeScore(score);
}

void UIManager::Render()
{
	if (isHoleInTextActive)
	{
		vnFont::print(0, 0, L"ホールイン！");
	}
	if (isShotGaugeActive)
	{
		//本物のゲージ画像はまだ無いので%表示で仮実装(将来vnDirect2Dに矩形塗りつぶしAPIを追加して差し替える)
		vnFont::print(0, PLAYER_SCORE_LINE_HEIGHT, L"SHOT: %.0f%%", shotGaugeFillAmount * 100.0f);
	}

	for (auto& pair : playerScores)
	{
		pair.second->Render();
	}
}
