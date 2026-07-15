//--------------------------------------------------------------//
//	"vn_sprite.cpp"												//
//		スプライトクラス										//
//													2026/01/01	//
//														Ichii	//
//--------------------------------------------------------------//
#include "../framework.h"
#include "../framework/vn_environment.h"

#define USE_RESOURCE_LOADER (1)

//頂点要素
D3D12_INPUT_ELEMENT_DESC vnSprite::inputElementDescs[] =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT,       0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,       0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
};

//静的共通データ
ID3D12PipelineState *vnSprite::pPipelineState = NULL;	//パイプラインステート
ID3D12PipelineState* vnSprite::pPipelineState_Add = NULL;	//パイプラインステート(加算合成)
ID3D12Resource* vnSprite::constBuff = NULL;	//定数バッファ
float *vnSprite::scr = NULL;

//静的共通データ初期化
bool vnSprite::initializeCommon()
{
	HRESULT hr = S_OK;

	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
	psoDesc.InputLayout = { inputElementDescs, _countof(inputElementDescs) };
	psoDesc.pRootSignature = vnDirect3D::getRootSignature2D();

	psoDesc.VS.pShaderBytecode = vnShader::getVShader(vnShader::eVertexShader::VS_2D)->getCode();
	psoDesc.VS.BytecodeLength = vnShader::getVShader(vnShader::eVertexShader::VS_2D)->getLength();
	psoDesc.PS.pShaderBytecode = vnShader::getPShader(vnShader::ePixelShader::PS_2D)->getCode();
	psoDesc.PS.BytecodeLength = vnShader::getPShader(vnShader::ePixelShader::PS_2D)->getLength();

	psoDesc.RasterizerState.MultisampleEnable = false;
	psoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
	psoDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
	psoDesc.RasterizerState.DepthClipEnable = true;
	psoDesc.RasterizerState.FrontCounterClockwise = false;
	psoDesc.RasterizerState.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
	psoDesc.RasterizerState.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
	psoDesc.RasterizerState.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
	psoDesc.RasterizerState.AntialiasedLineEnable = false;
	psoDesc.RasterizerState.ForcedSampleCount = 0;
	psoDesc.RasterizerState.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

	psoDesc.BlendState.AlphaToCoverageEnable = false;
	psoDesc.BlendState.IndependentBlendEnable = false;
	psoDesc.BlendState.RenderTarget[0].BlendEnable = true;
	psoDesc.BlendState.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	psoDesc.BlendState.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	psoDesc.BlendState.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	psoDesc.BlendState.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_SRC_ALPHA;
	psoDesc.BlendState.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_DEST_ALPHA;
	psoDesc.BlendState.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	psoDesc.BlendState.RenderTarget[0].LogicOpEnable = false;
	psoDesc.BlendState.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
	psoDesc.BlendState.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	psoDesc.DepthStencilState.DepthEnable = FALSE;
	psoDesc.DepthStencilState.StencilEnable = FALSE;

	psoDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
	psoDesc.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	psoDesc.NumRenderTargets = 1;
	psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	psoDesc.SampleDesc.Count = 1;
	psoDesc.SampleDesc.Quality = 0;
	hr = vnDirect3D::getDevice()->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&pPipelineState));
	pPipelineState->SetName(L"vnSprite::pPipelineState");
	
	//加算合成用パイプラインステートオブジェクトの作成
	psoDesc.BlendState.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
	hr = vnDirect3D::getDevice()->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&pPipelineState_Add));
	pPipelineState_Add->SetName(L"vnSprite::pPipelineState_Add");

	//定数バッファ
	D3D12_HEAP_PROPERTIES constHeapProp = {};
	constHeapProp.Type = D3D12_HEAP_TYPE_UPLOAD;
	constHeapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	constHeapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	constHeapProp.CreationNodeMask = 1;
	constHeapProp.VisibleNodeMask = 1;
	D3D12_RESOURCE_DESC constDesc = {};
	constDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	constDesc.Width = ((sizeof(float)*4 + 0xff) & ~0xff);
	constDesc.Height = 1;
	constDesc.DepthOrArraySize = 1;
	constDesc.MipLevels = 1;
	constDesc.Format = DXGI_FORMAT_UNKNOWN;
	constDesc.SampleDesc.Count = 1;
	constDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
	constDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	hr = vnDirect3D::getDevice()->CreateCommittedResource(
		&constHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&constDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		NULL,
		IID_PPV_ARGS(&constBuff)
	);
	assert(hr == S_OK);
	hr = constBuff->SetName(L"vnSprite::constBuff");
	assert(hr == S_OK);

	//定数バッファへ値をセット
	hr = constBuff->Map(0, NULL, (void**)&scr);
	assert(hr == S_OK);
	//スクリーン座標からビューポート座標へ変換するための係数
	float w = (float)vnMainFrame::screenWidth;
	float h = (float)vnMainFrame::screenHeight;
	scr[0] = 2.0f / w;
	scr[1] = -2.0f / h;
	scr[2] = -w * 0.5f;
	scr[3] = -h * 0.5f;
	constBuff->Unmap(0, NULL);

	return true;
}

//静的共通データ削除
void vnSprite::terminateCommon()
{
	SAFE_RELEASE(constBuff);
	SAFE_RELEASE(pPipelineState);
	SAFE_RELEASE(pPipelineState_Add);
}

//コンストラクタ
vnSprite::vnSprite(float x, float y, float width, float height, const WCHAR* texture_file, float left_u, float right_u, float top_v, float bottom_v)
{
	//基本情報を保存
	posX = x;
	posY = y;
	sizeX = width;
	sizeY = height;
	scaleX = 1.0f;
	scaleY = 1.0f;
	rot = 0.0f;

	HRESULT hr = S_OK;

	texbuff = NULL;
	basicDescHeap = NULL;

	//WICテクスチャのロード
	TexMetadata metadata = {};
	ScratchImage scratchImg = {};
	hr = LoadFromWICFile(texture_file, WIC_FLAGS_NONE, &metadata, scratchImg);
	if (hr == S_OK)
	{
		const Image* img = scratchImg.GetImage(0, 0, 0);

		D3D12_HEAP_PROPERTIES texHeapProp = {};
		texHeapProp.Type = D3D12_HEAP_TYPE_CUSTOM;
		texHeapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
		texHeapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;
		texHeapProp.CreationNodeMask = 0;
		texHeapProp.VisibleNodeMask = 0;

		D3D12_RESOURCE_DESC resDesc = {};
		resDesc.Format = metadata.format;
		resDesc.Width = metadata.width;
		resDesc.Height = (UINT)metadata.height;
		resDesc.DepthOrArraySize = (UINT16)metadata.arraySize;
		resDesc.SampleDesc.Count = 1;
		resDesc.SampleDesc.Quality = 0;
		resDesc.MipLevels = (UINT16)metadata.mipLevels;
		resDesc.Dimension = static_cast<D3D12_RESOURCE_DIMENSION>(metadata.dimension);
		resDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		resDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

		hr = vnDirect3D::getDevice()->CreateCommittedResource(
			&texHeapProp,
			D3D12_HEAP_FLAG_NONE,
			&resDesc,
			D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
			NULL,
			IID_PPV_ARGS(&texbuff)
		);
		texbuff->SetName(L"vnSprite::texbuff");

		hr = texbuff->WriteToSubresource(0,
			NULL,
			img->pixels,
			(UINT)img->rowPitch,
			(UINT)img->slicePitch
		);
	}

	{
		//デスクリプタヒープ
		D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc = {};
		descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		descHeapDesc.NodeMask = 0;
		descHeapDesc.NumDescriptors = 1;// 2;
		descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		hr = vnDirect3D::getDevice()->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(&basicDescHeap));

		//テクスチャビュー作成
		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = metadata.format;
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = 1;

		D3D12_CPU_DESCRIPTOR_HANDLE basicHeapHandle = basicDescHeap->GetCPUDescriptorHandleForHeapStart();
		//シェーダリソースビューの作成
		if (texbuff != NULL)
		{
			vnDirect3D::getDevice()->CreateShaderResourceView(texbuff,
				&srvDesc,
				basicHeapHandle
			);
		}
		else
		{
			vnDirect3D::getDevice()->CreateShaderResourceView(vnDirect3D::getWhiteTexture(),
				vnDirect3D::getWhiteTextueViewDesc(),
				basicHeapHandle
			);
		}
		/*basicHeapHandle.ptr += vnDirect3D::getDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);


		D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
		cbvDesc.BufferLocation = constBuff->GetGPUVirtualAddress();
		cbvDesc.SizeInBytes = (UINT)constBuff->GetDesc().Width;
		//定数バッファビューの作成
		vnDirect3D::getDevice()->CreateConstantBufferView(&cbvDesc, basicHeapHandle);*/
	}

	//頂点バッファ
	const UINT vertexBufferSize = sizeof(vnVertex2D) * VertexNum;

	D3D12_HEAP_PROPERTIES heapprop = {};
	heapprop.Type = D3D12_HEAP_TYPE_UPLOAD;
	heapprop.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	heapprop.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	heapprop.CreationNodeMask = 1;
	heapprop.VisibleNodeMask = 1;


	D3D12_RESOURCE_DESC resdesc = {};
	resdesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resdesc.Width = vertexBufferSize;
	resdesc.Height = 1;
	resdesc.DepthOrArraySize = 1;
	resdesc.MipLevels = 1;
	resdesc.Format = DXGI_FORMAT_UNKNOWN;
	resdesc.SampleDesc.Count = 1;
	resdesc.Flags = D3D12_RESOURCE_FLAG_NONE;
	resdesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	hr = vnDirect3D::getDevice()->CreateCommittedResource(
		&heapprop,
		D3D12_HEAP_FLAG_NONE,
		&resdesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		NULL,
		IID_PPV_ARGS(&vertBuff)
	);
	assert(hr == S_OK);
	vertBuff->SetName(L"vnSprite::vertBuff");

	hr = vertBuff->Map(0, NULL, reinterpret_cast<void**>(&vtx));
	assert(hr == S_OK);

	vertexBufferView.BufferLocation = vertBuff->GetGPUVirtualAddress();
	vertexBufferView.StrideInBytes = sizeof(vnVertex2D);
	vertexBufferView.SizeInBytes = vertexBufferSize;

	//頂点座標を設定
	setVertexPosition();

	//頂点カラーを設定
	vtx[0].r = 1.0f;	vtx[0].g = 1.0f;	vtx[0].b = 1.0f;	vtx[0].a = 1.0f;
	vtx[1].r = 1.0f;	vtx[1].g = 1.0f;	vtx[1].b = 1.0f;	vtx[1].a = 1.0f;
	vtx[2].r = 1.0f;	vtx[2].g = 1.0f;	vtx[2].b = 1.0f;	vtx[2].a = 1.0f;
	vtx[3].r = 1.0f;	vtx[3].g = 1.0f;	vtx[3].b = 1.0f;	vtx[3].a = 1.0f;

	//uvを設定
	vtx[0].u = left_u;	vtx[0].v = top_v;
	vtx[1].u = right_u;	vtx[1].v = top_v;
	vtx[2].u = left_u;	vtx[2].v = bottom_v;
	vtx[3].u = right_u;	vtx[3].v = bottom_v;

	//実行状態の管理
	executeEnable = true;

	//描画状態の管理
	renderEnable = true;

	//描画優先順位
	renderPriority = 0;

}

//デストラクタ
vnSprite::~vnSprite()
{
	SAFE_RELEASE(basicDescHeap);
	SAFE_RELEASE(texbuff);
	SAFE_RELEASE(vertBuff);
	texbuff = NULL;
}

//処理関数
void vnSprite::execute()
{
}

//描画関数
void vnSprite::render()
{
	//頂点座標の設定
	setVertexPosition();

	if (addition)
	{
		vnDirect3D::getCommandList()->SetPipelineState(pPipelineState_Add);
	}
	else
	{
		vnDirect3D::getCommandList()->SetPipelineState(pPipelineState);
	}
	vnDirect3D::getCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	vnDirect3D::getCommandList()->SetGraphicsRootConstantBufferView(0, constBuff->GetGPUVirtualAddress());
	vnDirect3D::getCommandList()->SetDescriptorHeaps(1, &basicDescHeap);
	vnDirect3D::getCommandList()->SetGraphicsRootDescriptorTable(1, basicDescHeap->GetGPUDescriptorHandleForHeapStart());
	vnDirect3D::getCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView);
	//vnDirect3D::getCommandList()->SetGraphicsRootShaderResourceView(1, texbuff->GetGPUVirtualAddress());

	vnDirect3D::getCommandList()->DrawInstanced(VertexNum, 1, 0, 0);
}

//頂点座標の設定
void vnSprite::setVertexPosition()
{
	//標準状態での各頂点の座標
	float v0x = (-sizeX * 0.5f) * scaleX, v0y = (-sizeY * 0.5f) * scaleY;
	float v1x = (+sizeX * 0.5f) * scaleX, v1y = (-sizeY * 0.5f) * scaleY;
	float v2x = (-sizeX * 0.5f) * scaleX, v2y = (+sizeY * 0.5f) * scaleY;
	float v3x = (+sizeX * 0.5f) * scaleX, v3y = (+sizeY * 0.5f) * scaleY;

	//回転を適用
	float v0x_r = v0x * cosf(rot) - (v0y * sinf(rot));	//頂点0の回転後のx座標
	float v0y_r = v0y * cosf(rot) + (v0x * sinf(rot));	//頂点0の回転後のy座標
	float v1x_r = v1x * cosf(rot) - (v1y * sinf(rot));	//頂点1の回転後のx座標
	float v1y_r = v1y * cosf(rot) + (v1x * sinf(rot));	//頂点1の回転後のy座標
	float v2x_r = v2x * cosf(rot) - (v2y * sinf(rot));	//頂点2の回転後のx座標
	float v2y_r = v2y * cosf(rot) + (v2x * sinf(rot));	//頂点2の回転後のy座標
	float v3x_r = v3x * cosf(rot) - (v3y * sinf(rot));	//頂点3の回転後のx座標
	float v3y_r = v3y * cosf(rot) + (v3x * sinf(rot));	//頂点3の回転後のy座標

	//各頂点に中心座標を適用し、絶対座標を設定
	vtx[0].x = v0x_r + posX;	vtx[0].y = v0y_r + posY;
	vtx[1].x = v1x_r + posX;	vtx[1].y = v1y_r + posY;
	vtx[2].x = v2x_r + posX;	vtx[2].y = v2y_r + posY;
	vtx[3].x = v3x_r + posX;	vtx[3].y = v3y_r + posY;
}

//---- setter関数 ----

//実行状態の設定
void vnSprite::setExecuteEnable(bool flag)
{
	executeEnable = flag;
}

//描画状態の設定
void vnSprite::setRenderEnable(bool flag)
{
	renderEnable = flag;
}

//描画優先順位の設定
void vnSprite::setRenderPriority(int value)
{
	if (renderPriority == value)return;
	renderPriority = value;
	vnScene::setUpdateRenderPriority();
}

//加算合成の設定
void vnSprite::setAddition(bool flag)
{
	addition = flag;
}

//---- getter関数 ----

//実行状態の取得
bool vnSprite::isExecuteEnable()
{
	return executeEnable;
}

//描画状態の取得
bool vnSprite::isRenderEnable()
{
	return renderEnable;
}

//描画優先順位の取得
int vnSprite::getRenderPriority(void)
{
	return renderPriority;
}

//加算合成の設定
bool vnSprite::getAddition()
{
	return addition;
}
