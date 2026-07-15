//--------------------------------------------------------------//
//	"vn_debugDraw.cpp"											//
//		デバッグ描画											//
//													2026/01/01	//
//														Ichii	//
//--------------------------------------------------------------//
#include "../framework.h"
#include "../framework/vn_environment.h"

//頂点要素
D3D12_INPUT_ELEMENT_DESC vnDebugDraw::inputElementDescs[] =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT , D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	{ "COLOR",    0, DXGI_FORMAT_R8G8B8A8_UNORM,     0, D3D11_APPEND_ALIGNED_ELEMENT , D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
};

int						vnDebugDraw::VertexNum = 0;
vnVertex3D_Line			*vnDebugDraw::vtx = NULL;
XMFLOAT4X4				*vnDebugDraw::VP = NULL;

ID3D12Resource* vnDebugDraw::vertBuff = NULL;
D3D12_VERTEX_BUFFER_VIEW vnDebugDraw::vertexBufferView;

ID3D12PipelineState* vnDebugDraw::pPipelineState;


bool vnDebugDraw::initialize()
{
	VertexNum = 0;

	HRESULT hr = S_OK;

	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
	psoDesc.InputLayout = { inputElementDescs, _countof(inputElementDescs) };
	psoDesc.pRootSignature = vnDirect3D::getRootSignature();

	psoDesc.VS.pShaderBytecode = vnShader::getVShader(vnShader::eVertexShader::VS_3D_Line)->getCode();
	psoDesc.VS.BytecodeLength = vnShader::getVShader(vnShader::eVertexShader::VS_3D_Line)->getLength();
	psoDesc.PS.pShaderBytecode = vnShader::getPShader(vnShader::ePixelShader::PS_3D_Line)->getCode();
	psoDesc.PS.BytecodeLength = vnShader::getPShader(vnShader::ePixelShader::PS_3D_Line)->getLength();

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

	psoDesc.DepthStencilState.DepthEnable = true;
	psoDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
	psoDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	psoDesc.DepthStencilState.StencilEnable = false;
	psoDesc.DepthStencilState.StencilReadMask = D3D12_DEFAULT_STENCIL_READ_MASK;
	psoDesc.DepthStencilState.StencilWriteMask = D3D12_DEFAULT_STENCIL_WRITE_MASK;
	psoDesc.DepthStencilState.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	psoDesc.DepthStencilState.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	psoDesc.DepthStencilState.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	psoDesc.DepthStencilState.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;
	psoDesc.DepthStencilState.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	psoDesc.DepthStencilState.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	psoDesc.DepthStencilState.BackFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	psoDesc.DepthStencilState.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;
	psoDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;

	psoDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
	psoDesc.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
	psoDesc.NumRenderTargets = 1;
	psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	psoDesc.SampleDesc.Count = 1;
	psoDesc.SampleDesc.Quality = 0;
	hr = vnDirect3D::getDevice()->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&pPipelineState));
	pPipelineState->SetName(L"vnDebugDraw::pPipelineState");

	//頂点バッファ
	const UINT vertexBufferSize = sizeof(vnVertex3D_Line) * vnDebugDraw_VertexMax;

	D3D12_HEAP_PROPERTIES heapprop = {};
	heapprop.Type = D3D12_HEAP_TYPE_UPLOAD;
	heapprop.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	heapprop.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;

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
	if (hr != S_OK)
	{
		assert(hr == S_OK);
	}
	vertBuff->SetName(L"vnDebugDraw::vertBuff");

	//頂点バッファのマップ
	hr = vertBuff->Map(0, NULL, reinterpret_cast<void**>(&vtx));

	vertexBufferView.BufferLocation = vertBuff->GetGPUVirtualAddress();
	vertexBufferView.StrideInBytes = sizeof(vnVertex3D_Line);
	vertexBufferView.SizeInBytes = vertexBufferSize;

	return true;
}

void vnDebugDraw::terminate()
{
	SAFE_RELEASE(vertBuff);
	SAFE_RELEASE(pPipelineState);
}

void vnDebugDraw::render()
{
	if(VertexNum==0)return;

	vnDirect3D::getCommandList()->SetPipelineState(pPipelineState);

	vnDirect3D::getCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);
	vnDirect3D::getCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView);
	vnDirect3D::getCommandList()->DrawInstanced(VertexNum, 1, 0, 0);

	VertexNum = 0;
}

void vnDebugDraw::Line(const XMVECTOR *start, const XMVECTOR *end, DWORD color)
{
	if(VertexNum>=vnDebugDraw_VertexMax-2)return;
	
	vtx[VertexNum].x = XMVectorGetX(*start);	vtx[VertexNum].y = XMVectorGetY(*start);	vtx[VertexNum].z = XMVectorGetZ(*start);	vtx[VertexNum].color = color;	VertexNum++;
	vtx[VertexNum].x = XMVectorGetX(*end);		vtx[VertexNum].y = XMVectorGetY(*end);		vtx[VertexNum].z = XMVectorGetZ(*end);		vtx[VertexNum].color = color;	VertexNum++;
}

void vnDebugDraw::Line(float start_x, float start_y, float start_z, float end_x, float end_y, float end_z, DWORD color)
{
	if (VertexNum >= vnDebugDraw_VertexMax - 2)return;

	vtx[VertexNum].x = start_x;		vtx[VertexNum].y = start_y;		vtx[VertexNum].z = start_z;		vtx[VertexNum].color = color;	VertexNum++;
	vtx[VertexNum].x = end_x;		vtx[VertexNum].y = end_y;		vtx[VertexNum].z = end_z;		vtx[VertexNum].color = color;	VertexNum++;
}

void vnDebugDraw::Grid(int num, float interval, DWORD color)
{
	for (int i = 0; i <= num; i++)
	{
		vtx[VertexNum].x = -(float)(num / 2) * interval;	vtx[VertexNum].y = 0.0f;	vtx[VertexNum].z = (float)(-num / 2 + i) * interval;	vtx[VertexNum].color = color;	VertexNum++;
		vtx[VertexNum].x = (float)(num / 2) * interval;	vtx[VertexNum].y = 0.0f;	vtx[VertexNum].z = (float)(-num / 2 + i) * interval;	vtx[VertexNum].color = color;	VertexNum++;
	}
	for (int i = 0; i <= num; i++)
	{
		vtx[VertexNum].x = (float)(-num / 2 + i) * interval;	vtx[VertexNum].y = 0.0f;	vtx[VertexNum].z = -(float)(num / 2) * interval;	vtx[VertexNum].color = color;	VertexNum++;
		vtx[VertexNum].x = (float)(-num / 2 + i) * interval;	vtx[VertexNum].y = 0.0f;	vtx[VertexNum].z = (float)(num / 2) * interval;	vtx[VertexNum].color = color;	VertexNum++;
	}
}

void vnDebugDraw::Axis(float length)
{
	//X軸
	vtx[VertexNum].x = 0.0f;	vtx[VertexNum].y = 0.0f;	vtx[VertexNum].z = 0.0f;	vtx[VertexNum].color = 0xff0000ff;	VertexNum++;
	vtx[VertexNum].x = length;	vtx[VertexNum].y = 0.0f;	vtx[VertexNum].z = 0.0f;	vtx[VertexNum].color = 0xff0000ff;	VertexNum++;
	//Y軸
	vtx[VertexNum].x = 0.0f;	vtx[VertexNum].y = 0.0f;	vtx[VertexNum].z = 0.0f;	vtx[VertexNum].color = 0xff00ff00;	VertexNum++;
	vtx[VertexNum].x = 0.0f;	vtx[VertexNum].y = length;	vtx[VertexNum].z = 0.0f;	vtx[VertexNum].color = 0xff00ff00;	VertexNum++;
	//Z軸
	vtx[VertexNum].x = 0.0f;	vtx[VertexNum].y = 0.0f;	vtx[VertexNum].z = 0.0f;	vtx[VertexNum].color = 0xffff0000;	VertexNum++;
	vtx[VertexNum].x = 0.0f;	vtx[VertexNum].y = 0.0f;	vtx[VertexNum].z = length;	vtx[VertexNum].color = 0xffff0000;	VertexNum++;
}

