#pragma once
#include <map>
#include <string>

class PlayerScore;//前方宣言

/***************************************************************
 * クラス名：UIManager
 * 機能：ホールインテキスト・ショットゲージ・プレイヤースコア表示を管理するシングルトン
 * *************************************************************/
class UIManager
{
private:
	UIManager() {}
	UIManager(const UIManager&) = delete;
	UIManager& operator=(const UIManager&) = delete;

	bool isHoleInTextActive = false;
	bool isShotGaugeActive = false;
	float shotGaugeFillAmount = 0.0f;

	std::map<int, PlayerScore*> playerScores;

public:
	static UIManager& GetInstance()
	{
		static UIManager instance;
		return instance;
	}

	~UIManager();

	void ActiveHoleInText(bool isActive) { isHoleInTextActive = isActive; }
	void ActiveShotGauge(bool isActive) { isShotGaugeActive = isActive; }
	void UpdateShotGauge(float fillAmount) { shotGaugeFillAmount = fillAmount; }

	void AddPlayer(int id, const std::wstring& name = L"TEST");
	void UpdateScore(int id, int score);

	//毎フレーム呼び出す描画処理
	void Render();
};
