//--------------------------------------------------------------//
//	"vn_scene.h"												//
//		シーンクラス											//
//													2024/04/01	//
//														Ichii	//
//--------------------------------------------------------------//
#pragma once

#define vnOBJECT2D_MAX (512)	//2Dオブジェクトを登録できる最大数
#define vnOBJECT3D_MAX (2048)	//3Dオブジェクトを登録できる最大数

class vnScene
{
private:
	static vnSprite *pObject2D_Array[vnOBJECT2D_MAX];	//2Dオブジェクトを登録しておく配列
	static vnObject* pObject3D_Array[vnOBJECT3D_MAX];	//3Dオブジェクトを登録しておく配列

	//2Dオブジェクトをソートする必要があるか
	static bool UpdateRenderPriority;

public:
	vnScene();
	virtual ~vnScene();

	virtual bool initialize() = 0;
	virtual void terminate() = 0;
	
	virtual void execute();
	virtual void render();
	virtual void render2D();

	//登録された2Dオブジェクトの描画優先順位が変更されたので、配列をソートをする必要がある
	static void setUpdateRenderPriority();

	//オブジェクトの登録(配列の空きに入れる)
	static bool registerObject(vnSprite *p);

	//オブジェクトの破棄(配列から削除/オブジェクト自体をdeleteする)
	static void deleteObject(vnSprite *p);

	//オブジェクトの登録(配列の空きに入れる)
	static bool registerObject(vnObject* p);

	//オブジェクトの破棄(配列から削除/オブジェクト自体をdeleteする)
	static void deleteObject(vnObject* p);
};
