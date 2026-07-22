//--------------------------------------------------------------//
//	"vn_environment.h"											//
//		�t���[�����[�N���ʊ��w�b�_�[							//
//													2026/01/01	//
//														Ichii	//
//--------------------------------------------------------------//
#pragma once

//���C�u����
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "dwrite.lib")
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "xinput.lib")
#pragma comment(lib, "winmm.lib")

#if (_MSC_VER>=1930)

//Visual Studio 2022
#ifdef _DEBUG
#pragma comment(lib, "DirectXTex/Bin/Desktop_2022_Win10/x64/Debug/DirectXTex.lib")
#else
#pragma comment(lib, "DirectXTex/Bin/Desktop_2022_Win10/x64/Release/DirectXTex.lib")
#endif

#else

//Visual Studio 2019
#ifdef _DEBUG
#pragma comment(lib, "DirectXTex/Bin/Desktop_2019_Win10/x64/Debug/DirectXTex.lib")
#else
#pragma comment(lib, "DirectXTex/Bin/Desktop_2019_Win10/x64/Release/DirectXTex.lib")
#endif

#endif

//���ʃw�b�_�t�@�C���̃C���N���[�h
#include <stdio.h>
#include <assert.h>
#include <mmsystem.h>
#include <locale.h>
#include <time.h>

//DirectX�w�b�_�[
#include <d2d1_3.h>
#include <dwrite.h>
#include <d3d11on12.h>
#include <d3d12.h>
#include <dxgi1_6.h>

#define	DIRECTINPUT_VERSION	0x0800
#include <dinput.h>
#include <Xinput.h>
#include <xaudio2.h>

#include <DirectXMath.h>

#include "../DirectXTex/DirectXTex.h"

//�l�[���X�y�[�X
using namespace DirectX;

//��ʃ}�N��
#define SCREEN_WIDTH	(1280)	//�N���C�A���g�̈�̕�(�s�N�Z��)
#define SCREEN_HEIGHT	(720)	//�N���C�A���g�̈�̍���(�s�N�Z��)

//�����[�X
#define SAFE_RELEASE(p) {if(p){(p)->Release();(p)=NULL;}}

//�t���[�����[�N�w�b�_�[
#include "directX/vn_shader.h"
#include "directX/vn_Direct3D.h"
#include "directX/vn_Direct2D.h"
#include "directX/vn_DirectInput.h"
#include "directX/vn_XInput.h"
#include "directX/vn_XAudio2.h"

#include "vn_mouse.h"
#include "vn_keyboard.h"
#include "vn_joystick.h"
#include "vn_sound.h"

#include "vn_font.h"

#include "vn_camera.h"
#include "vn_light.h"
#include "vn_debugDraw.h"

#include "../public/vn_object.h"
#include "../public/vn_model.h"
#include "../public/vn_billboard.h"
#include "../public/vn_effect.h"
#include "../public/vn_character.h"
#include "../public/vn_sprite.h"
#include "../public/vn_collide.h"

#include "../public/IHitReceiver.h"
#include "../public/IBall.h"
#include "../public/Contact.h"
#include "../public/ICollisionShape.h"
#include "../public/Collider.h"
#include "../public/CollisionManager.h"
#include "../public/MatrixMath.h"
#include "../public/PhysicsBody.h"
#include "../public/Ball.h"
#include "../public/SphereShape.h"



#include "../public/scene/vn_scene.h"
#include "../public/scene/scene_table.h"


#include "vn_mainframe.h"
