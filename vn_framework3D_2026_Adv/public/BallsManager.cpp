#include "../framework.h"
#include "../framework/vn_environment.h"

BallsManager::~BallsManager()
{
	for (auto& pair : balls)
	{
		delete pair.second;
	}
}

Ball* BallsManager::AddBall(int id, float radius, CollisionManager* terrainManager, CollisionManager* obstacleManager)
{
	RemoveBall(id);//同じidが既にあれば作り直す

	Ball* ball = new Ball(radius, terrainManager, obstacleManager);
	balls[id] = ball;
	return ball;
}

Ball* BallsManager::GetBall(int id)
{
	auto it = balls.find(id);
	if (it == balls.end()) return nullptr;
	return it->second;
}

void BallsManager::RemoveBall(int id)
{
	auto it = balls.find(id);
	if (it == balls.end()) return;

	delete it->second;
	balls.erase(it);
}
