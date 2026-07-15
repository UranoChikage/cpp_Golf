//--------------------------------------------------------------//
//	"vn_character.cpp"											//
//		階層構造 & モーション再生オブジェクトクラス				//
//													2026/01/01	//
//														Ichii	//
//--------------------------------------------------------------//
#include "../framework.h"
#include "../framework/vn_environment.h"

vnCharacter::vnCharacter(const WCHAR* folder, const WCHAR* file)
{
	//boneファイルのロード
	WCHAR path[256];
	swprintf_s(path, L"%s%s", folder, file);

	FILE* fp = NULL;
	long size = 0;

	if ((_wfopen_s(&fp, path, L"rb")) != 0)
	{
		vnFont::output(L"cannot open file \"%s\"\n", path);
		assert(false);
		return;
	}
	fseek(fp, 0, SEEK_END);
	size = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	pBoneData = (vnModel_BoneData*)new BYTE[size];
	fread(pBoneData, size, 1, fp);
	fclose(fp);

	//パーツ数
	PartsNum = size / sizeof(vnModel_BoneData);

	//パーツの作成
	pParts = new vnObject * [PartsNum];

	bool skin = false;
	for (int i = 0; i < PartsNum; i++)
	{
		WCHAR partsfile[256];
		swprintf_s(partsfile, L"%s.vnm", pBoneData[i].Name);
		swprintf_s(path, L"%s%s", folder, partsfile);

		if ((_wfopen_s(&fp, path, L"rb")) != 0)
		{
			pParts[i] = new vnObject();
		}
		else
		{
			vnModel* pModel = new vnModel(folder, partsfile);
			pParts[i] = pModel;

			skin |= (pModel->getBoneNum() > 0);
		}

		if (pBoneData[i].ParentID >= 0)
		{
			pParts[i]->setParent(pParts[pBoneData[i].ParentID]);
		}
		else
		{
			pParts[i]->setParent(this);
		}
		pParts[i]->setPosition(pBoneData[i].pos[0], pBoneData[i].pos[1], pBoneData[i].pos[2]);
		pParts[i]->setRotation(pBoneData[i].rot[0], pBoneData[i].rot[1], pBoneData[i].rot[2]);
		pParts[i]->setScale(pBoneData[i].scl[0], pBoneData[i].scl[1], pBoneData[i].scl[2]);

		XMVECTOR quaternion = XMQuaternionRotationRollPitchYaw(pBoneData[i].rot[0], pBoneData[i].rot[1], pBoneData[i].rot[2]);
		pParts[i]->setQuaternion(&quaternion);

		//vnFont::output(L"parts[%d] : %s\n", i, partsfile);
	}

	bindPose();

	pPartsMatrix = NULL;
	if (skin)
	{
		pPartsMatrix = new XMMATRIX[PartsNum];
	}

	Play = true;
	Time = 0.0f;
	Speed = 1.0f;

	pMotion = NULL;
	pBoneChannel = new stBoneMotion[PartsNum * eMotionChannel::ChannelMax];
	memset(pBoneChannel, 0, sizeof(stBoneMotion) * PartsNum * eMotionChannel::ChannelMax);

	updateMotion = false;
}


vnCharacter::~vnCharacter()
{
	pMotion = NULL;
	delete[] pBoneChannel;

	if (pPartsMatrix)delete[] pPartsMatrix;

	for (int i = 0; i < PartsNum; i++)
	{
		if (pParts[i] != NULL)vnScene::deleteObject(pParts[i]);
	}
	delete[] pParts;
	delete[](BYTE*)pBoneData;
}

void vnCharacter::execute()
{
	applyMotion();
}

void vnCharacter::render()
{
	vnObject::render();

	if (pPartsMatrix == NULL)return;

	for (int i = 0; i < PartsNum; i++)
	{
		pPartsMatrix[i] = *pParts[i]->getWorld();
	}

	for (int i = 0; i < PartsNum; i++)
	{
		pParts[i]->setSkinMatrix(pPartsMatrix);
	}
}

void vnCharacter::setRenderEnable(bool flag)
{
	vnObject::setRenderEnable(flag);
	for (int i = 0; i < PartsNum; i++)
	{
		pParts[i]->setRenderEnable(flag);
	}

}

void vnCharacter::useQuaternion(bool flag)
{
	vnObject::useQuaternion(flag);
	for (int i = 0; i < PartsNum; i++)
	{
		pParts[i]->useQuaternion(flag);
	}
}


void vnCharacter::registerObject()
{
	vnScene::registerObject(this);
	for (int i = 0; i < PartsNum; i++)
	{
		vnScene::registerObject(pParts[i]);
	}
}

vnObject* vnCharacter::getParts(int id)
{
	if (id < 0 || id >= PartsNum)return NULL;
	return pParts[id];
}

vnObject* vnCharacter::getParts(const WCHAR* name)
{
	if (!name)return NULL;
	for (int i = 0; i < PartsNum; i++)
	{
		if (wcscmp(name, pBoneData[i].Name) == 0)return pParts[i];
	}
	return NULL;
}

const WCHAR* vnCharacter::getPartsName(int id)
{
	if (id < 0 || id >= PartsNum)return NULL;
	return pBoneData[id].Name;
}

int vnCharacter::getPartsNum()
{
	return PartsNum; 
}

void vnCharacter::bindPose()
{
	for (int i = 0; i < PartsNum; i++)
	{
		bindPose(i);
	}
}

void vnCharacter::bindPose(int id)
{
	if (id < 0 || id >= PartsNum)return;
	pParts[id]->setPosition(pBoneData[id].pos[0], pBoneData[id].pos[1], pBoneData[id].pos[2]);
	pParts[id]->setRotation(pBoneData[id].rot[0], pBoneData[id].rot[1], pBoneData[id].rot[2]);
	pParts[id]->setScale(pBoneData[id].scl[0], pBoneData[id].scl[1], pBoneData[id].scl[2]);

	XMVECTOR quaternion = XMQuaternionRotationRollPitchYaw(pBoneData[id].rot[0], pBoneData[id].rot[1], pBoneData[id].rot[2]);
	pParts[id]->setQuaternion(&quaternion);
}

void vnCharacter::applyMotion()
{
	if (pMotion == NULL)return;

	if (Play == false && updateMotion == false)return;

	if (Play == true)
	{
		Time += Speed;
		if (Time >= pMotion->Length)
		{
			Time = 0;
		}
		else if (Time <= 0)
		{
			Time = pMotion->Length;
		}
	}

	vnMotionData_KeyFrame* key = reinterpret_cast<vnMotionData_KeyFrame*>(reinterpret_cast<__int64>(pMotion) + pMotion->KeyFrameAccess);

	for (int i = 0, b = 0; i < PartsNum; i++)
	{
		for (int j = 0; j < eMotionChannel::ChannelMax; j++, b++)
		{
			if (!pBoneChannel[b].pBone)continue;
			if (!pBoneChannel[b].pChannel)continue;
			vnMotionData_KeyFrame* keys = pBoneChannel[b].pKey;
			int n = pBoneChannel[b].pChannel->KeyFrameNum;

			float v = keys[n-1].Value;
			for (int k = 0; k < n; k++)
			{
				if (Time == keys[k].Time)
				{
					v = keys[k].Value;
					break;
				}
				else if (Time < keys[k].Time)
				{
					float rate = (Time - keys[k - 1].Time) / (keys[k].Time - keys[k - 1].Time);
					float d = keys[k].Value - keys[k - 1].Value;
					v = d * rate + keys[k - 1].Value;
					break;
				}
			}
			switch (pBoneChannel[b].pChannel->ChannelID)
			{
			case eMotionChannel::PosX:	pBoneChannel[b].pBone->setPositionX(v);	break;
			case eMotionChannel::PosY:	pBoneChannel[b].pBone->setPositionY(v);	break;
			case eMotionChannel::PosZ:	pBoneChannel[b].pBone->setPositionZ(v);	break;
			case eMotionChannel::RotX:	pBoneChannel[b].pBone->setRotationX(v);	break;
			case eMotionChannel::RotY:	pBoneChannel[b].pBone->setRotationY(v);	break;
			case eMotionChannel::RotZ:	pBoneChannel[b].pBone->setRotationZ(v);	break;
			case eMotionChannel::SclX:	pBoneChannel[b].pBone->setScaleX(v);	break;
			case eMotionChannel::SclY:	pBoneChannel[b].pBone->setScaleY(v);	break;
			case eMotionChannel::SclZ:	pBoneChannel[b].pBone->setScaleZ(v);	break;
			case eMotionChannel::QutX:	pBoneChannel[b].pBone->setQuaternionX(v);	break;
			case eMotionChannel::QutY:	pBoneChannel[b].pBone->setQuaternionY(v);	break;
			case eMotionChannel::QutZ:	pBoneChannel[b].pBone->setQuaternionZ(v);	break;
			case eMotionChannel::QutW:	pBoneChannel[b].pBone->setQuaternionW(v);	break;
			}
		}
	}
	updateMotion = false;
}

vnMotionData_Channel* vnCharacter::getMotionChannel(const vnMotionData* motion, eMotionChannel channel, const WCHAR* name)
{
	if (!motion || !name)return NULL;

	vnMotionData_Channel* p = reinterpret_cast<vnMotionData_Channel*>(reinterpret_cast<__int64>(motion) + motion->ChannelAccess);

	for (UINT i = 0; i < motion->ChannelNum; i++)
	{
		if (wcscmp(name, p[i].Name) != 0)continue;
		if (channel != p[i].ChannelID)continue;
		return &p[i];
	}
	return NULL;
}

int vnCharacter::getKeyFrames(const vnModel_BoneData* bone, const vnMotionData* motion, eMotionChannel channel, vnMotionData_KeyFrame** out)
{
	if (!bone || !motion || !out) return 0;

	vnMotionData_KeyFrame* key = reinterpret_cast<vnMotionData_KeyFrame*>(reinterpret_cast<__int64>(pMotion) + pMotion->KeyFrameAccess);

	vnMotionData_Channel* p = reinterpret_cast<vnMotionData_Channel*>(reinterpret_cast<__int64>(pMotion) + pMotion->ChannelAccess);

	for (UINT i = 0; i < pMotion->ChannelNum; i++)
	{
		if (wcscmp(bone->Name, p[i].Name) != 0)continue;
		if (p[i].KeyFrameNum == 0)return 0;
		if (p[i].ChannelID != channel)continue;
		*out = &key[p[i].StartIndex];
		return p[i].KeyFrameNum;
	}
	return 0;
}

void vnCharacter::setMotion(vnMotionData* motion)
{
	if (motion == pMotion)
	{
		return;
	}
	Play = true;
	Time = 0;
	if (motion == NULL)
	{
		pMotion = NULL;
		return;
	}
	pMotion = motion;

	memset(pBoneChannel, 0, sizeof(stBoneMotion) * PartsNum * eMotionChannel::ChannelMax);

	vnMotionData_KeyFrame* key = reinterpret_cast<vnMotionData_KeyFrame*>(reinterpret_cast<__int64>(pMotion) + pMotion->KeyFrameAccess);
	for (int i = 0, n = 0; i < PartsNum; i++)
	{
		for (int j = 0; j < eMotionChannel::ChannelMax; j++)
		{
			vnMotionData_Channel* p = getMotionChannel(pMotion, (eMotionChannel)j, pBoneData[i].Name);
			if (!p)continue;
			pBoneChannel[n].pBone = pParts[i];
			pBoneChannel[n].pChannel = p;
			pBoneChannel[n].pKey = &key[p->StartIndex];

			n++;
		}
	}

	bindPose();
}

bool vnCharacter::hasMotion()
{
	return pMotion != NULL;
}

vnMotionData* vnCharacter::getMotion()
{
	return pMotion;
}

float vnCharacter::getMotionLength()
{
	return hasMotion() ? pMotion->Length : 0;
}

void vnCharacter::playMotion(bool play)
{
	Play = play;
}

void vnCharacter::pauseMotion()
{
	Play = false;
}

bool vnCharacter::isPlayMotion()
{
	return Play;
}

float vnCharacter::getTime()
{
	return Time;
}

void vnCharacter::setTime(float time)
{
	Time = time;
	if (pMotion)
	{
		updateMotion = true;
		if (Time < 0)Time = pMotion->Length;
		if (Time > pMotion->Length)Time = 0;
	}
	else
	{
		if (Time < 0)Time = 0;
	}
}

float vnCharacter::getPlaySpeed()
{
	return Speed;
}

void vnCharacter::setPlaySpeed(float speed)
{
	Speed = speed;
}

vnMotionData* vnCharacter::loadMotionFile(const WCHAR* path)
{
	vnMotionData* ret = NULL;

	FILE* fp = NULL;
	long size = 0;

	if ((_wfopen_s(&fp, path, L"rb")) != 0)
	{
		vnFont::output(L"cannot open file \"%s\"\n", path);
		assert(false);
		return ret;
	}
	fseek(fp, 0, SEEK_END);
	size = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	ret = (vnMotionData*)new BYTE[size];
	fread(ret, size, 1, fp);
	fclose(fp);

	return ret;
}
