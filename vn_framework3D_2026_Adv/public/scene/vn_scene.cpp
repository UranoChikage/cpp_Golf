//--------------------------------------------------------------//
//	"vn_scene.cpp"												//
//		シーンクラス											//
//													2024/04/01	//
//														Ichii	//
//--------------------------------------------------------------//
#include "../../framework.h"
#include "../../framework/vn_environment.h"

bool vnScene::UpdateRenderPriority = false;
vnSprite* vnScene::pObject2D_Array[vnOBJECT2D_MAX];	//2Dオブジェクトを登録しておく配列
vnObject* vnScene::pObject3D_Array[vnOBJECT3D_MAX];	//3Dオブジェクトを登録しておく配列

//コンストラクタ
vnScene::vnScene()
{
	for (int i = 0; i < vnOBJECT3D_MAX; i++)
	{
		pObject3D_Array[i] = NULL;
	}
	for (int i = 0; i < vnOBJECT2D_MAX; i++)
	{
		pObject2D_Array[i] = NULL;
	}
	UpdateRenderPriority = false;
}

//デストラクタ
vnScene::~vnScene()
{
	for (int i = 0; i < vnOBJECT3D_MAX; i++)
	{
		if (pObject3D_Array[i] == NULL)continue;
		delete pObject3D_Array[i];
		pObject3D_Array[i] = NULL;
	}
	for (int i = 0; i < vnOBJECT2D_MAX; i++)
	{
		if (pObject2D_Array[i] == NULL)continue;
		delete pObject2D_Array[i];
		pObject2D_Array[i] = NULL;
	}
	UpdateRenderPriority = false;
}

//処理関数
void vnScene::execute()
{
	//3Dオブジェクトのexecuteを実行
	for (int i = 0; i < vnOBJECT3D_MAX; i++)
	{
		if (pObject3D_Array[i] == NULL)continue;
		if (pObject3D_Array[i]->isExecuteEnable() == false)continue;
		pObject3D_Array[i]->execute();
	}
	//3DオブジェクトのpostExecuteを実行
	for (int i = 0; i < vnOBJECT3D_MAX; i++)
	{
		if (pObject3D_Array[i] == NULL)continue;
		if (pObject3D_Array[i]->isExecuteEnable() == false)continue;
		pObject3D_Array[i]->postExecute();
	}
	//2Dオブジェクトのexecuteを実行
	for (int i = 0; i < vnOBJECT2D_MAX; i++)
	{
		if (pObject2D_Array[i] == NULL)continue;
		if (pObject2D_Array[i]->isExecuteEnable() == false)continue;
		pObject2D_Array[i]->execute();
	}
}

//描画関数
void vnScene::render()
{
	//3Dオブジェクトのrenderを実行
	for (int i = 0; i < vnOBJECT3D_MAX; i++)
	{
		if (pObject3D_Array[i] == NULL)continue;
		if (pObject3D_Array[i]->isRenderEnable() == false)continue;
		pObject3D_Array[i]->render();
	}
}

void vnScene::render2D()
{
	if (UpdateRenderPriority == true)	//ソートの必要がある時のみソートする
	{
		//描画優先順位に従って配列をソート
		for (int i = 0; i < vnOBJECT2D_MAX - 1; i++)
		{
			if (pObject2D_Array[i] == NULL)continue;
			for (int j = i + 1; j < vnOBJECT2D_MAX; j++)
			{
				if (pObject2D_Array[j] == NULL)continue;
				if (pObject2D_Array[i]->getRenderPriority() > pObject2D_Array[j]->getRenderPriority())
				{
					vnSprite* tmp = pObject2D_Array[i];
					pObject2D_Array[i] = pObject2D_Array[j];
					pObject2D_Array[j] = tmp;
				}
			}
		}
		UpdateRenderPriority = false;	//ソートが完了したら、次にソートが必要になるまでソートしないようにする
	}

	//2Dオブジェクトのrenderを実行
	for (int i = 0; i < vnOBJECT2D_MAX; i++)
	{
		if (pObject2D_Array[i] == NULL)continue;
		if (pObject2D_Array[i]->isRenderEnable() == false)continue;
		pObject2D_Array[i]->render();
	}
}

//登録された2Dオブジェクトの描画優先順位が変更されたので、配列をソートをする必要がある
void vnScene::setUpdateRenderPriority()
{
	UpdateRenderPriority = true;
}

//オブジェクトの登録(配列の空きに入れる)
bool vnScene::registerObject(vnSprite *p)
{
	if (p == NULL)
	{
		return false;
	}

	for (int i = 0; i < vnOBJECT2D_MAX; i++)
	{
		if (pObject2D_Array[i] != NULL)continue;
		pObject2D_Array[i] = p;
		return true;
	}
	return false;
}

//オブジェクトの破棄(配列から削除/オブジェクト自体をdeleteする)
void vnScene::deleteObject(vnSprite *p)
{
	if (p == NULL)
	{
		return;
	}

	for (int i = 0; i < vnOBJECT2D_MAX; i++)
	{
		if (pObject2D_Array[i] == NULL)continue;
		if (pObject2D_Array[i] == p)
		{
			delete pObject2D_Array[i];
			pObject2D_Array[i] = NULL;
		}
	}
}

//オブジェクトの登録(配列の空きに入れる)
bool vnScene::registerObject(vnObject* p)
{
	if (p == NULL)
	{
		return false;
	}

	for (int i = 0; i < vnOBJECT3D_MAX; i++)
	{
		if (pObject3D_Array[i] != NULL)continue;
		pObject3D_Array[i] = p;
		return true;
	}
	return false;
}

//オブジェクトの破棄(配列から削除/オブジェクト自体をdeleteする)
void vnScene::deleteObject(vnObject* p)
{
	if (p == NULL)
	{
		return;
	}

	for (int i = 0; i < vnOBJECT3D_MAX; i++)
	{
		if (pObject3D_Array[i] == NULL)continue;
		if (pObject3D_Array[i] == p)
		{
			delete pObject3D_Array[i];
			pObject3D_Array[i] = NULL;
		}
	}
}
