//--------------------------------------------------------------//
//	"scene_table.h"												//
//		シーンテーブル											//
//													2026/01/01	//
//														Ichii	//
//--------------------------------------------------------------//
#pragma once

//シーンの種類
enum eSceneTable
{
	Boot,
	FieldTest,
	GroundTest,
	SpriteTest,
	JoystickTest,
	SeTest,
	FontTest,
	MyGame,
	SceneMax,
};

//シーンクラス定義ファイル
#include "vn_scene.h"
#include "scene_boot.h"
#include "scene_field_test.h"
#include "scene_ground_test.h"
#include "scene_sprite_test.h"
#include "scene_joystick_test.h"
#include "scene_se_test.h"
#include "scene_font_test.h"
#include "scene_MyGame.h"

//シーン切り替え関数(予約)
void switchScene(eSceneTable scene);
//現在のシーン取得(実処理)
void switchScene();
