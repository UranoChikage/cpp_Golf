//--------------------------------------------------------------//
//	"vn_camera.cpp"												//
//		ÉJÉÅÉâ													//
//													2026/01/01	//
//														Ichii	//
//--------------------------------------------------------------//
#include "../framework.h"
#include "vn_environment.h"

XMVECTOR		vnCamera::Position;
XMVECTOR		vnCamera::Target;
XMVECTOR		vnCamera::Up;

float			vnCamera::FovY;
float			vnCamera::Aspect;
float			vnCamera::zNear;
float			vnCamera::zFar;

XMMATRIX		vnCamera::View;
XMMATRIX		vnCamera::Proj;
XMMATRIX		vnCamera::Screen;

D3D12_VIEWPORT	vnCamera::Viewport;


bool vnCamera::initialize()
{
	Position = XMVectorSet(5.0f, 5.0f, -10.0f, 0.0f);

	Target = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);

	Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	FovY = XMConvertToRadians(45.0f);
	Aspect = (float)vnMainFrame::screenWidth / (float)vnMainFrame::screenHeight;
	zNear = 1.0f;
	zFar = 1000.0f;
	
	Viewport.TopLeftX = 0;
	Viewport.TopLeftY = 0;
	Viewport.Width = (float)vnMainFrame::screenWidth;
	Viewport.Height = (float)vnMainFrame::screenHeight;
	Viewport.MinDepth = 0.0f;
	Viewport.MaxDepth = 1.0f;

	return true;
}

void vnCamera::terminate()
{

}

XMMATRIX *vnCamera::getView()
{
	View = XMMatrixLookAtLH(Position, Target, Up);
	return &View;
}

XMMATRIX *vnCamera::getProj()
{
	Proj = XMMatrixPerspectiveFovLH(FovY, Aspect, zNear, zFar);
	return &Proj;
}

XMMATRIX *vnCamera::getScreen()
{
	Screen = XMMatrixMultiply(*getView(), *getProj());
	return &Screen;
}

D3D12_VIEWPORT * vnCamera::getViewport()
{
	return &Viewport;
}


//éÊìæ
const XMVECTOR *vnCamera::getPosition()
{
	return &Position;
}

float vnCamera::getPositionX()
{
	return XMVectorGetX(Position);
}

float vnCamera::getPositionY()
{
	return XMVectorGetY(Position);
}

float vnCamera::getPositionZ()
{
	return XMVectorGetZ(Position);
}

const XMVECTOR *vnCamera::getTarget()
{
	return &Target;
}

float vnCamera::getTargetX()
{
	return XMVectorGetX(Target);
}

float vnCamera::getTargetY()
{
	return XMVectorGetY(Target);
}

float vnCamera::getTargetZ()
{
	return XMVectorGetZ(Target);
}

const XMVECTOR *vnCamera::getUp()
{
	return &Up;
}

float vnCamera::getUpX()
{
	return XMVectorGetX(Up);
}

float vnCamera::getUpY()
{
	return XMVectorGetY(Up);
}

float vnCamera::getUpZ()
{
	return XMVectorGetZ(Up);
}

float vnCamera::getFovY()
{
	return FovY;
}

float vnCamera::getAspect()
{
	return Aspect;
}

//ê›íË
void vnCamera::setPosition(float x, float y, float z)
{
	Position = XMVectorSet(x, y, z, 0.0f);
}

void vnCamera::setPosition(const XMVECTOR *v)
{
	Position = *v;
}

void vnCamera::setPositionX(float value)
{
	Position = XMVectorSetX(Position, value);
}

void vnCamera::setPositionY(float value)
{
	Position = XMVectorSetY(Position, value);
}

void vnCamera::setPositionZ(float value)
{
	Position = XMVectorSetZ(Position, value);
}

void vnCamera::setTarget(float x, float y, float z)
{
	Target = XMVectorSet(x, y, z, 0.0f);
}

void vnCamera::setTarget(const XMVECTOR *v)
{
	Target = *v;
}

void vnCamera::setTargetX(float value)
{
	Target = XMVectorSetX(Target, value);
}

void vnCamera::setTargetY(float value)
{
	Target = XMVectorSetY(Target, value);
}

void vnCamera::setTargetZ(float value)
{
	Target = XMVectorSetZ(Target, value);
}

void vnCamera::setUp(float x, float y, float z)
{
	Up = XMVectorSet(x, y, z, 0.0f);
}

void vnCamera::setUp(const XMVECTOR *v)
{
	Up = *v;
}

void vnCamera::setUpX(float value)
{
	Up = XMVectorSetX(Up, value);
}

void vnCamera::setUpY(float value)
{
	Up = XMVectorSetY(Up, value);
}

void vnCamera::setUpZ(float value)
{
	Up = XMVectorSetZ(Up, value);
}


void vnCamera::setFovY(float value)
{
	FovY = value;
}

void vnCamera::setAspect(float value)
{
	Aspect = value;
}

//â¡éZ
void vnCamera::addPosition(float x, float y, float z)
{
	Position = XMVectorAdd(Position, XMVectorSet(x, y, z, 0.0f));
}

void vnCamera::addPosition(const XMVECTOR *v)
{
	Position += *v;
}

void vnCamera::addPositionX(float value)
{
	Position = XMVectorAdd(Position, XMVectorSet(value, 0.0f, 0.0f, 0.0f));
}

void vnCamera::addPositionY(float value)
{
	Position = XMVectorAdd(Position, XMVectorSet(0.0f, value, 0.0f, 0.0f));
}

void vnCamera::addPositionZ(float value)
{
	Position = XMVectorAdd(Position, XMVectorSet(0.0f, 0.0f, value, 0.0f));
}

void vnCamera::addTarget(float x, float y, float z)
{
	Target = XMVectorAdd(Target, XMVectorSet(x, y, z, 0.0f));
}

void vnCamera::addTarget(const XMVECTOR *v)
{
	Target += *v;
}

void vnCamera::addTargetX(float value)
{
	Target = XMVectorAdd(Target, XMVectorSet(value, 0.0f, 0.0f, 0.0f));
}

void vnCamera::addTargetY(float value)
{
	Target = XMVectorAdd(Target, XMVectorSet(0.0f, value, 0.0f, 0.0f));
}

void vnCamera::addTargetZ(float value)
{
	Target = XMVectorAdd(Target, XMVectorSet(0.0f, 0.0f, value, 0.0f));
}


void vnCamera::addFovY(float value)
{
	FovY += value;
}

void vnCamera::addAspect(float value)
{
	Aspect += value;
}
