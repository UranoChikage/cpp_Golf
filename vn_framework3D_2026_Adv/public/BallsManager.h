#pragma once
#include <string>
#include <map>

class Ball;//前方宣言
class CollisionManager;//前方宣言

/***************************************************************
 * クラス名：BallsManager
 * 機能：Ballの登録/管理を行うシングルトン
 * *************************************************************/
class BallsManager
{
private:
	BallsManager() {};//コンストラクタ
	BallsManager(const BallsManager&) = delete;//コピー禁止
	BallsManager& operator=(const BallsManager&) = delete;
	class PlayerInfo
	{
	public:
		int Score;
		int ID;
		std::string	Name;
		PlayerInfo(int id);
	};

	std::map<int, Ball*> balls;

public:
	static BallsManager& GetInstance()
	{
		static BallsManager instance;
		return instance;
	}

	~BallsManager();

	//idでBallを新規登録して返す(既に同じidがあれば作り直す)
	Ball* AddBall(int id, float radius, CollisionManager* terrainManager, CollisionManager* obstacleManager);
	//idからBallを取得(無ければnullptr)
	Ball* GetBall(int id);
	//idのBallを削除
	void RemoveBall(int id);
};