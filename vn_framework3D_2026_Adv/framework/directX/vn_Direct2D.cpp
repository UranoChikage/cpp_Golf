//--------------------------------------------------------------//
//	"vn_Direct2D.cpp"											//
//		Direct2D管理											//
//													2026/01/01	//
//														Ichii	//
//--------------------------------------------------------------//
#include "../../framework.h"
#include "../vn_environment.h"

extern HWND hWnd;	// ウィンドウのハンドル

ID3D11On12Device* vnDirect2D::pDevice11On12 = NULL;
ID3D11DeviceContext* vnDirect2D::pDeviceContext11 = NULL;

ID2D1Factory3* vnDirect2D::pD2dFactory = NULL;
ID2D1Device2* vnDirect2D::pD2dDevice = NULL;
ID2D1DeviceContext2* vnDirect2D::pD2dDeviceContext = NULL;
IDWriteFactory* vnDirect2D::pDWFactory = NULL;

ID3D11Resource* vnDirect2D::pWrappedBackBaffers[vnDirect3D::frameCount];
ID2D1Bitmap1* vnDirect2D::pD2dRenderTargets[vnDirect3D::frameCount];

int vnDirect2D::initialize()
{
	HRESULT hr = S_OK;
	UINT d3d11DeviceFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
	D2D1_FACTORY_OPTIONS d2dFactoryOptions = {};

#if defined(_DEBUG)
	d3d11DeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
	d2dFactoryOptions.debugLevel = D2D1_DEBUG_LEVEL_INFORMATION;

	ID3D12InfoQueue* infoQueue = NULL;
	hr = vnDirect3D::getDevice()->QueryInterface(IID_PPV_ARGS(&infoQueue));
	if (hr == S_OK)
	{
		D3D12_MESSAGE_SEVERITY severities[] =
		{
			D3D12_MESSAGE_SEVERITY_INFO,
		};

		D3D12_MESSAGE_ID denyIds[] =
		{
			D3D12_MESSAGE_ID_INVALID_DESCRIPTOR_HANDLE,
		};

		D3D12_INFO_QUEUE_FILTER filter = {};
		//filter.DenyList.NumCategories = _countof(categories);
		//filter.DenyList.pCategoryList = categories;
		filter.DenyList.NumSeverities = _countof(severities);
		filter.DenyList.pSeverityList = severities;
		filter.DenyList.NumIDs = _countof(denyIds);
		filter.DenyList.pIDList = denyIds;

		hr = infoQueue->PushStorageFilter(&filter);

		infoQueue->Release();
	}
#endif

	ID3D11Device* pDevice11 = NULL;
	ID3D12CommandQueue* cmdQueue = vnDirect3D::getCommandQueue();
	hr = D3D11On12CreateDevice(
		vnDirect3D::getDevice(),
		d3d11DeviceFlags,
		NULL,
		0,
		reinterpret_cast<IUnknown* const*>(&cmdQueue),
		1,
		0,
		&pDevice11,
		&pDeviceContext11,
		NULL
	);
	assert(hr == S_OK);

	hr = pDevice11->QueryInterface(IID_PPV_ARGS(&pDevice11On12));
	assert(hr == S_OK);
	pDevice11->Release();

	{
		D2D1_DEVICE_CONTEXT_OPTIONS deviceOptions = D2D1_DEVICE_CONTEXT_OPTIONS_NONE;// D2D1_DEVICE_CONTEXT_OPTIONS_ENABLE_MULTITHREADED_OPTIMIZATIONS;
		//D2D1_DEBUG_LEVEL debugLevel = D2D1_DEBUG_LEVEL_INFORMATION;
		hr = D2D1CreateFactory(/*D2D1_FACTORY_TYPE_MULTI_THREADED */D2D1_FACTORY_TYPE_SINGLE_THREADED, _uuidof(ID2D1Factory3), /*&debugLevel*/&d2dFactoryOptions, (void**)&pD2dFactory);
		assert(hr == S_OK);
		IDXGIDevice* dxgiDevice = NULL;

		hr = pDevice11On12->QueryInterface(IID_PPV_ARGS(&dxgiDevice));
		assert(hr == S_OK);

		hr = pD2dFactory->CreateDevice(dxgiDevice, &pD2dDevice);
		assert(hr == S_OK);
		hr = pD2dDevice->CreateDeviceContext(deviceOptions, &pD2dDeviceContext);
		assert(hr == S_OK);
		hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown**>(&pDWFactory));
		assert(hr == S_OK);

		dxgiDevice->Release();
	}

	//float dpiX = 96.0f;
	//float dpiY = 96.0f;
#pragma warning(push)
#pragma warning(disable : 4996) // GetDesktopDpi is deprecated.
	//pD2dFactory->GetDesktopDpi(&dpiX, &dpiY);
	UINT dpi = GetDpiForWindow(hWnd);
	float dpiX = (float)dpi;
	float dpiY = (float)dpi;
#pragma warning(pop)
	D2D1_BITMAP_PROPERTIES1 bitmapProperties = D2D1::BitmapProperties1(
		D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
		D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED),
		dpiX,
		dpiY
	);

	//レンダーターゲットを作成する処理
	for (UINT i = 0; i < vnDirect3D::frameCount; i++)
	{
		D3D11_RESOURCE_FLAGS d3d11Flas = { D3D11_BIND_RENDER_TARGET };
		hr = pDevice11On12->CreateWrappedResource(
			vnDirect3D::getRenderTarget(i),
			&d3d11Flas,
			D3D12_RESOURCE_STATE_RENDER_TARGET,
			D3D12_RESOURCE_STATE_PRESENT,
			IID_PPV_ARGS(&pWrappedBackBaffers[i])
		);
		assert(hr == S_OK);

		IDXGISurface* surface = NULL;
		hr = pWrappedBackBaffers[i]->QueryInterface(IID_PPV_ARGS(&surface));
		assert(hr == S_OK);
		hr = pD2dDeviceContext->CreateBitmapFromDxgiSurface(surface, &bitmapProperties, &pD2dRenderTargets[i]);
		assert(hr == S_OK);

		surface->Release();
	}

	return 1;
}

void vnDirect2D::terminate()
{
	SAFE_RELEASE(pDWFactory);
	SAFE_RELEASE(pD2dDeviceContext);
	SAFE_RELEASE(pD2dDevice);
	SAFE_RELEASE(pD2dFactory);
	SAFE_RELEASE(pDeviceContext11);
	SAFE_RELEASE(pDevice11On12);

	for (int i = 0; i < vnDirect3D::frameCount; i++)
	{
		SAFE_RELEASE(pD2dRenderTargets[i]);
		SAFE_RELEASE(pWrappedBackBaffers[i]);
	}
}

void vnDirect2D::render()
{
	HRESULT hr = S_OK;
	UINT idx = vnDirect3D::getIndex();
	D2D1_SIZE_F rtSize = pD2dRenderTargets[idx]->GetSize();
	D2D1_RECT_F textRect = D2D1::RectF(0.0f, 0.0f, rtSize.width, rtSize.height);

	pDevice11On12->AcquireWrappedResources(&pWrappedBackBaffers[idx], 1);

	pD2dDeviceContext->SetTarget(pD2dRenderTargets[idx]);
	pD2dDeviceContext->BeginDraw();
	pD2dDeviceContext->SetTransform(D2D1::Matrix3x2F::Identity());
	
	//フォントの描画
	vnFont::renderString();

	hr = pD2dDeviceContext->EndDraw();
	assert(hr == S_OK);

	pDevice11On12->ReleaseWrappedResources(&pWrappedBackBaffers[idx], 1);

	pDeviceContext11->Flush();
}

ID2D1DeviceContext2* vnDirect2D::getDeviceContext()
{
	return pD2dDeviceContext;
}

IDWriteFactory* vnDirect2D::getDWFactory()
{
	return pDWFactory;
}
