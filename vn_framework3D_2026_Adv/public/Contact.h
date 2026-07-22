#pragma once

/***************************************************************
 * 2026/07/02
 * 構造体名：Contact
 * 接触してるかをはかるために定義
 * *************************************************************/

 /*https://bookstack.yz-learning.com/books/windows/page/xmvectorxmfloat
 * XMFLOAT
 * 構造体やクラスのメンバ変数: オブジェクトの位置、回転、スケールなど、永続的に保持したいデータ。
 * 頂点バッファのレイアウト: 頂点データ（位置、法線、UVなど）を定義する場合。
 * ファイルへの保存や読み込み: データをファイルに書き込んだり、ファイルから読み込んだりする場合。
 */
struct Contact
{
public:
	bool isHit = false;
	XMFLOAT3 point{};
	XMFLOAT3 normal{};
	float penetration = 0.0f; // めり込み量
	IHitReceiver* hit = nullptr;
	bool isTigger = false;
};