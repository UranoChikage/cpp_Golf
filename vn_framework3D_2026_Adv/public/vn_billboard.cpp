//--------------------------------------------------------------//
//	"vn_billboard.cpp"											//
//		ビルボードポリゴン										//
//													2026/01/01	//
//														Ichii	//
//--------------------------------------------------------------//
#include "../framework.h"
#include "../framework/vn_environment.h"

vnBillboard::vnBillboard(float width, float height, const WCHAR* texture_file, float left_u, float right_u, float top_v, float bottom_v) : vnObject()
{
	HRESULT hr = S_OK;

	float w = width * 0.5f;
	float h = height * 0.5f;
	BasePos[0] = XMVectorSet(-w, h, 0.0f, 0.0f);
	BasePos[1] = XMVectorSet(w, h, 0.0f, 0.0f);
	BasePos[2] = XMVectorSet(-w, -h, 0.0f, 0.0f);
	BasePos[3] = XMVectorSet(w, -h, 0.0f, 0.0f);

	TexMetadata metadata = {};
	ScratchImage scratchImg = {};
	DXGI_FORMAT format = DXGI_FORMAT_UNKNOWN;

	//マテリアルの作成
	pMaterials = new stMaterial[1];

	pMaterials[0].texbuff = NULL;
	pMaterials[0].basicDescHeap = NULL;

	//テクスチャの拡張子
	const WCHAR* ext = wcsrchr(texture_file, L'.');

	//テクスチャの読み込み
	if (ext != NULL)
	{
		if (wcscmp(ext, L".tga") == 0 || wcscmp(ext, L".TGA") == 0)
		{
			hr = LoadFromTGAFile(texture_file, TGA_FLAGS_NONE, &metadata, scratchImg);
		}
		else
		{
			hr = LoadFromWICFile(texture_file, WIC_FLAGS_NONE, &metadata, scratchImg);
		}
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
				nullptr,
				IID_PPV_ARGS(&pMaterials[0].texbuff)
			);
			pMaterials[0].texbuff->SetName(L"vnBillboard::texbuff");

			hr = pMaterials[0].texbuff->WriteToSubresource(0,
				NULL,
				img->pixels,
				(UINT)img->rowPitch,
				(UINT)img->slicePitch
			);
		}
	}

	{
		D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc = {};
		descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		descHeapDesc.NodeMask = 0;
		descHeapDesc.NumDescriptors = 2;
		descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		hr = vnDirect3D::getDevice()->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(&pMaterials[0].basicDescHeap));
		if (hr != S_OK)
		{
			assert(hr == S_OK);
		}
		//定数バッファ
		D3D12_HEAP_PROPERTIES constHeapProp = {};
		constHeapProp.Type = D3D12_HEAP_TYPE_UPLOAD;
		constHeapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		constHeapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		constHeapProp.CreationNodeMask = 1;
		constHeapProp.VisibleNodeMask = 1;

		D3D12_RESOURCE_DESC constDesc = {};
		constDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		constDesc.Width = ((sizeof(stMaterialBuffer) + 0xff) & ~0xff);
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
			nullptr,
			IID_PPV_ARGS(&pMaterials[0].materialBuff)
		);
		if (hr != S_OK)
		{
			assert(hr == S_OK);
		}
		pMaterials[0].materialBuff->SetName(L"vnBillboard::materialBuff");

		D3D12_CPU_DESCRIPTOR_HANDLE basicHeapHandle = pMaterials[0].basicDescHeap->GetCPUDescriptorHandleForHeapStart();
		//シェーダリソースビューの作成
		if (pMaterials[0].texbuff != NULL)
		{
			//テクスチャビュー作成
			D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
			srvDesc.Format = metadata.format;
			srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
			srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
			srvDesc.Texture2D.MipLevels = 1;

			vnDirect3D::getDevice()->CreateShaderResourceView(pMaterials[0].texbuff, &srvDesc, basicHeapHandle);
		}
		else
		{
			vnDirect3D::getDevice()->CreateShaderResourceView(vnDirect3D::getWhiteTexture(), vnDirect3D::getWhiteTextueViewDesc(), basicHeapHandle);
		}
		basicHeapHandle.ptr += vnDirect3D::getDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

		D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
		cbvDesc.BufferLocation = pMaterials[0].materialBuff->GetGPUVirtualAddress();
		cbvDesc.SizeInBytes = (UINT)pMaterials[0].materialBuff->GetDesc().Width;
		//定数バッファビューの作成
		vnDirect3D::getDevice()->CreateConstantBufferView(&cbvDesc, basicHeapHandle);

		hr = pMaterials[0].materialBuff->Map(0, NULL, (void**)&pMaterials[0].pMaterialBuffer);
	}
	//マテリアルカラー
	pMaterials[0].Diffuse = XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);
	pMaterials[0].Ambient = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	pMaterials[0].Specular = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
	pMaterials[0].TilingOffset = XMVectorSet(1.0f, 1.0f, 0.0f, 0.0f);

	{	//コンスタントバッファ(マトリクス)
		D3D12_HEAP_PROPERTIES heapProp = {};
		heapProp.Type = D3D12_HEAP_TYPE_UPLOAD;
		heapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		heapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		heapProp.CreationNodeMask = 1;
		heapProp.VisibleNodeMask = 1;

		D3D12_RESOURCE_DESC resDesc = {};
		resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		resDesc.Width = ((sizeof(stConstantBuffer) + 0xff) & ~0xff);
		resDesc.Height = 1;
		resDesc.DepthOrArraySize = 1;
		resDesc.MipLevels = 1;
		resDesc.Format = DXGI_FORMAT_UNKNOWN;
		resDesc.SampleDesc.Count = 1;
		resDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
		resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		hr = vnDirect3D::getDevice()->CreateCommittedResource(
			&heapProp,
			D3D12_HEAP_FLAG_NONE,
			&resDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&constBuff)
		);
		assert(hr == S_OK);
		constBuff->SetName(L"vnBillboard::constBuff");

		hr = constBuff->Map(0, NULL, (void**)&pConstBuffer);
		assert(hr == S_OK);
	}

	//頂点バッファ
	const int vnum = 4;

	const UINT vertexBufferSize = sizeof(vnVertex3D) * vnum;

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
	vertBuff->SetName(L"vnBillboard::vertBuff");

	hr = vertBuff->Map(0, NULL, reinterpret_cast<void**>(&vtx));

	vertexBufferView.BufferLocation = vertBuff->GetGPUVirtualAddress();
	vertexBufferView.StrideInBytes = sizeof(vnVertex3D);
	vertexBufferView.SizeInBytes = vertexBufferSize;

	//頂点メモリの初期化
	memset(vtx, 0, sizeof(vnVertex3D) * vnum);

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

	setRenderFlag(eRenderFlag::Transparent, true);

	setRenderFlag(eRenderFlag::Lighting, false);

}

vnBillboard::~vnBillboard()
{
	SAFE_RELEASE(pMaterials[0].materialBuff);
	SAFE_RELEASE(pMaterials[0].texbuff);
	SAFE_RELEASE(pMaterials[0].basicDescHeap);
	SAFE_RELEASE(constBuff);
	SAFE_RELEASE(vertBuff);

	delete[] pMaterials;
}

void vnBillboard::execute()
{
}

void vnBillboard::render()
{
	vnObject::render();

	XMMATRIX W = XMMatrixIdentity();
	XMMATRIX WVP = *vnCamera::getScreen();

	//頂点座標の設定
	setVertexPosition();

#if 1
	//コンスタントバッファに情報を設定
	XMStoreFloat4x4(&pConstBuffer->WVP, XMMatrixTranspose(WVP));
	XMStoreFloat4x4(&pConstBuffer->World, XMMatrixTranspose(W));
	vnDirect3D::getCommandList()->SetGraphicsRootConstantBufferView(1, constBuff->GetGPUVirtualAddress());

	vnDirect3D::getCommandList()->SetPipelineState(pPipelineState[renderFlag]);

	XMStoreFloat4(&pMaterials[0].pMaterialBuffer->Diffuse, pMaterials[0].Diffuse);
	XMStoreFloat4(&pMaterials[0].pMaterialBuffer->Ambient, pMaterials[0].Ambient);
	XMStoreFloat4(&pMaterials[0].pMaterialBuffer->Specular, pMaterials[0].Specular);
	XMStoreFloat4(&pMaterials[0].pMaterialBuffer->TilingOffset, pMaterials[0].TilingOffset);

	vnDirect3D::getCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	vnDirect3D::getCommandList()->SetDescriptorHeaps(1, &pMaterials[0].basicDescHeap);
	vnDirect3D::getCommandList()->SetGraphicsRootDescriptorTable(2, pMaterials[0].basicDescHeap->GetGPUDescriptorHandleForHeapStart());
	vnDirect3D::getCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView);

	vnDirect3D::getCommandList()->DrawInstanced(4, 1, 0, 0);
#endif
}

//頂点座標の設定
void vnBillboard::setVertexPosition()
{
	//カメラのビューマトリクスを取得
	XMMATRIX mBillboard = *vnCamera::getView();
	//移動成分をなくす(※w成分は1)
	mBillboard.r[3] = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
	//逆行列を計算(※アフィン変換のみのため、転置行列と同じ)
	mBillboard = XMMatrixTranspose(mBillboard);

	//ワールド座標
	XMVECTOR w;
	getWorldPosition(&w);

	XMVECTOR v[4];
	for (int i = 0; i < 4; i++)
	{
		v[i] = XMVector3TransformNormal(BasePos[i], mBillboard);
		v[i] = XMVectorAdd(v[i], w);
		vtx[i].x = XMVectorGetX(v[i]);
		vtx[i].y = XMVectorGetY(v[i]);
		vtx[i].z = XMVectorGetZ(v[i]);
	}

	//vnDebugDraw::Line(&v[0], &v[1]);
	//vnDebugDraw::Line(&v[1], &v[3]);
	//vnDebugDraw::Line(&v[3], &v[2]);
	//vnDebugDraw::Line(&v[2], &v[0]);
}

void vnBillboard::setDiffuse(float r, float g, float b, float a)
{
	if(pMaterials)pMaterials[0].Diffuse = XMVectorSet(r, g, b, a);
}
