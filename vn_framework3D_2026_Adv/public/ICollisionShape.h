#pragma once

class ICollisionShape
{
private:
	virtual Contact QueryTerrain(const XMFLOAT3 pos) = 0;
	virtual bool QueryObstacle(const XMFLOAT3 pos, std::vector<Contact>& outContact) = 0;
	virtual void UpdateOrientation(const XMFLOAT3 deltapos, const XMFLOAT3 contactNomal) = 0;
};