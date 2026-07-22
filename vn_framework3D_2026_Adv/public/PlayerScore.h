#pragma once
#include <string>

/***************************************************************
 * クラス名：PlayerScore
 * 機能：プレイヤー名とスコアを保持し、画面に描画する
 * *************************************************************/
class PlayerScore
{
private:
	std::wstring name;
	int score = 0;
	float posX;
	float posY;
	bool isActive = true;

public:
	PlayerScore(const std::wstring& rName, float rPosX, float rPosY)
		: name(rName), posX(rPosX), posY(rPosY) {
	}

	void SetActive(bool value) { isActive = value; }

	void ChangeScore(int value) { score = value; }
	void ChangeName(const std::wstring& value) { name = value; }

	void Render()
	{
		if (!isActive) return;
		vnFont::print(posX, posY, L"%s : %d", name.c_str(), score);
	}
};
