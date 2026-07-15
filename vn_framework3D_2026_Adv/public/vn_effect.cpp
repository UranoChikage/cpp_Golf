//--------------------------------------------------------------//
//	"vn_effect.cpp"												//
//		エフェクト(パーティクル)クラス							//
//													2026/01/01	//
//														Ichii	//
//--------------------------------------------------------------//
#include "../framework.h"
#include "../framework/vn_environment.h"

#define vnPARTICLE_MAX	(1024)

vnEmitter::vnEmitter(stEmitterDesc* desc)
{
	//パーティクルの作成 & 初期化
	pParticle = new vnParticle[vnPARTICLE_MAX];
	for (int i = 0; i < vnPARTICLE_MAX; i++)
	{
		pParticle[i].Life = 0.0f;
		pParticle[i].StartLife = 0.0f;
		pParticle[i].Pos = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
		pParticle[i].Vel = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
		pParticle[i].Col = XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);
		pParticle[i].Size = 1.0f;
	}

	//描画情報の初期化
	HRESULT hr = S_OK;

	TexMetadata metadata = {};
	ScratchImage scratchImg = {};
	DXGI_FORMAT format = DXGI_FORMAT_UNKNOWN;

	//マテリアルの作成
	pMaterials = new stMaterial[1];

	pMaterials[0].texbuff = NULL;
	pMaterials[0].basicDescHeap = NULL;

	//テクスチャの拡張子
	WCHAR texture_file[64] = L"";// L"data/image/particle/particle001.png";
	if (desc != NULL)
	{
		swprintf_s(texture_file, L"%s", desc->Texture);
	}

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
			pMaterials[0].texbuff->SetName(L"vnEffect::texbuff");

			hr = pMaterials[0].texbuff->WriteToSubresource(0, NULL, img->pixels, (UINT)img->rowPitch, (UINT)img->slicePitch);
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
		pMaterials[0].materialBuff->SetName(L"vnEffect::materialBuff");

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
	//マテリアル関連
	XMStoreFloat4(&pMaterials[0].pMaterialBuffer->Diffuse, XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f));
	XMStoreFloat4(&pMaterials[0].pMaterialBuffer->Ambient, XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f));
	XMStoreFloat4(&pMaterials[0].pMaterialBuffer->Specular, XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f));
	XMStoreFloat4(&pMaterials[0].pMaterialBuffer->TilingOffset, XMVectorSet(1.0f, 1.0f, 0.0f, 0.0f));

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
		constBuff->SetName(L"vnEffect::constBuff");

		hr = constBuff->Map(0, NULL, (void**)&pConstBuffer);
		assert(hr == S_OK);
	}

	//頂点バッファ
	const int vnum = vnPARTICLE_MAX * 4;

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
	vertBuff->SetName(L"vnEffect::vertBuff");

	hr = vertBuff->Map(0, NULL, reinterpret_cast<void**>(&vtx));

	vertexBufferView.BufferLocation = vertBuff->GetGPUVirtualAddress();
	vertexBufferView.StrideInBytes = sizeof(vnVertex3D);
	vertexBufferView.SizeInBytes = vertexBufferSize;

	//インデックスバッファ
	const int inum = vnPARTICLE_MAX * 6;
	{
		const UINT indexBufferSize = sizeof(WORD) * inum;

		D3D12_HEAP_PROPERTIES heapprop = {};
		heapprop.Type = D3D12_HEAP_TYPE_UPLOAD;
		heapprop.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		heapprop.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;

		D3D12_RESOURCE_DESC resdesc = {};
		resdesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		resdesc.Width = indexBufferSize;
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
			nullptr,
			IID_PPV_ARGS(&idxBuffer)
		);
		idxBuffer->SetName(L"vnEffect::idxBuffer");
		idxBuffer->Map(0, nullptr, (void**)&idx);

		//インデックスバッファビューを作成
		indexBufferView.BufferLocation = idxBuffer->GetGPUVirtualAddress();
		indexBufferView.Format = DXGI_FORMAT_R16_UINT;
		indexBufferView.SizeInBytes = indexBufferSize;

		//インデックスデータの初期化
		memset(idx, 0, sizeof(WORD) * inum);
	}

	//頂点データの初期化
	for (int i = 0; i < vnPARTICLE_MAX; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			vtx[i * 4 + j].x = 0.0f;
			vtx[i * 4 + j].y = 0.0f;
			vtx[i * 4 + j].z = 0.0f;
			vtx[i * 4 + j].nx = 0.0f;
			vtx[i * 4 + j].ny = 0.0f;
			vtx[i * 4 + j].nz = 0.0f;
			vtx[i * 4 + j].r = 1.0f;
			vtx[i * 4 + j].g = 1.0f;
			vtx[i * 4 + j].b = 1.0f;
			vtx[i * 4 + j].a = 1.0f;
		}
		vtx[i * 4 + 0].u = 0.0f;	vtx[i * 4 + 0].v = 0.0f;
		vtx[i * 4 + 1].u = 1.0f;	vtx[i * 4 + 1].v = 0.0f;
		vtx[i * 4 + 2].u = 0.0f;	vtx[i * 4 + 2].v = 1.0f;
		vtx[i * 4 + 3].u = 1.0f;	vtx[i * 4 + 3].v = 1.0f;
	}

	setRenderFlag(eRenderFlag::Transparent, true);	//半透明

	setRenderFlag(eRenderFlag::Lighting, false);		//ライティング無し

	setRenderFlag(eRenderFlag::ZWrite, false);	//深度書き込み無し

	//描画されるインデックス数
	IndexNum = 0;

	//描画されるパーティクルの数
	renderParticleNum = 0;

	//各種初期化
	if (desc)
	{
		memcpy(&Desc, desc, sizeof(stEmitterDesc));
	}

	emit = true;

}

vnEmitter::~vnEmitter()
{
	SAFE_RELEASE(pMaterials[0].materialBuff);
	SAFE_RELEASE(pMaterials[0].texbuff);
	SAFE_RELEASE(pMaterials[0].basicDescHeap);
	SAFE_RELEASE(constBuff);
	SAFE_RELEASE(vertBuff);
	SAFE_RELEASE(idxBuffer);
}
void vnEmitter::execute()
{
	XMVECTOR world;
	getWorldPosition(&world);

	//パーティクルの放出
	for (int i = 0; i < vnPARTICLE_MAX && emit == true; i++)
	{
		if (pParticle[i].Life > 0.0f)continue;
		//放出可能なパーティクルに初期設定
		pParticle[i].Life = (float)(rand() % 30) + 30.0f;	//30~60
		pParticle[i].StartLife = pParticle[i].Life;
		pParticle[i].Pos = world;
		pParticle[i].Vel = XMVectorSet(
			(float)(rand() % 2000) / 1000.0f - 1.0f,	//-1~+1
			(float)(rand() % 1000) / 1000.0f,			//0~+1
			(float)(rand() % 2000) / 1000.0f - 1.0f,	//-1~+1
			0.0f
		);
		pParticle[i].Vel *= 0.1f;
		/*pParticle[i].Col = XMVectorSet(
			(float)(rand() % 1000) / 1000.0f,
			(float)(rand() % 1000) / 1000.0f,
			(float)(rand() % 1000) / 1000.0f,
			(float)(rand() % 1000) / 1000.0f
		);*/
		pParticle[i].Col = Desc.ColorMax;
		pParticle[i].Size = 1.0f;
		break;
	}

	//カメラのビューマトリクスを取得
	XMMATRIX mBillboard = *vnCamera::getView();
	//移動成分をなくす(※w成分は1)
	mBillboard.r[3] = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
	//逆行列を計算(※アフィン変換のみのため、転置行列と同じ)
	mBillboard = XMMatrixTranspose(mBillboard);

	//パーティクルの動作
	renderParticleNum = 0;
	IndexNum = 0;
	for (int i = 0; i < vnPARTICLE_MAX; i++)
	{
		if (pParticle[i].Life <= 0.0f)continue;

		//Lifeの減少に応じた割合
		float overLifeTime = pParticle[i].Life / pParticle[i].StartLife;

		pParticle[i].Life -= 1.0f;
		pParticle[i].Pos += pParticle[i].Vel;

		//vnDebugDraw::Line(&pParticle[i].Pos, &Position, 0xffffff00);

		//描画用頂点座標(ビルボード)
		XMVECTOR v[4];	//パーティクル周りの基準となる座標
		float w = pParticle[i].Size;// *overLifeTime;
		float h = pParticle[i].Size;// *overLifeTime;
		v[0] = XMVectorSet(-w, h, 0.0f, 0.0f);
		v[1] = XMVectorSet(w, h, 0.0f, 0.0f);
		v[2] = XMVectorSet(-w, -h, 0.0f, 0.0f);
		v[3] = XMVectorSet(w, -h, 0.0f, 0.0f);
		/* 4頂点の座標をビルボード化 */
		for (int j = 0; j < 4; j++)
		{
			v[j] = XMVector3TransformNormal(v[j], mBillboard);
			v[j] += pParticle[i].Pos;
		}
		//vnDebugDraw::Line(&v[0], &v[1]);
		//vnDebugDraw::Line(&v[1], &v[3]);
		//vnDebugDraw::Line(&v[3], &v[2]);
		//vnDebugDraw::Line(&v[2], &v[0]);

		//描画用の頂点データに設定
		for (int j = 0; j < 4; j++)
		{
			vtx[renderParticleNum * 4 + j].x = XMVectorGetX(v[j]);
			vtx[renderParticleNum * 4 + j].y = XMVectorGetY(v[j]);
			vtx[renderParticleNum * 4 + j].z = XMVectorGetZ(v[j]);
			vtx[renderParticleNum * 4 + j].r = XMVectorGetX(pParticle[i].Col);
			vtx[renderParticleNum * 4 + j].g = XMVectorGetY(pParticle[i].Col);
			vtx[renderParticleNum * 4 + j].b = XMVectorGetZ(pParticle[i].Col);
			vtx[renderParticleNum * 4 + j].a = XMVectorGetW(pParticle[i].Col) * overLifeTime;
		}
		//インデックスデータの設定
		idx[IndexNum++] = renderParticleNum * 4 + 0;
		idx[IndexNum++] = renderParticleNum * 4 + 1;
		idx[IndexNum++] = renderParticleNum * 4 + 2;

		idx[IndexNum++] = renderParticleNum * 4 + 1;
		idx[IndexNum++] = renderParticleNum * 4 + 3;
		idx[IndexNum++] = renderParticleNum * 4 + 2;

		renderParticleNum++;
	}
}

void vnEmitter::render()
{
	vnObject::render();

	XMMATRIX W = XMMatrixIdentity();
	XMMATRIX WVP = XMMatrixMultiply(XMMatrixIdentity(), *vnCamera::getScreen());

	//頂点座標の設定
	setVertexPosition();

	//コンスタントバッファに情報を設定
	XMStoreFloat4x4(&pConstBuffer->WVP, XMMatrixTranspose(WVP));
	XMStoreFloat4x4(&pConstBuffer->World, XMMatrixTranspose(W));
	vnDirect3D::getCommandList()->SetGraphicsRootConstantBufferView(1, constBuff->GetGPUVirtualAddress());

	vnDirect3D::getCommandList()->SetPipelineState(pPipelineState[renderFlag]);

	vnDirect3D::getCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	vnDirect3D::getCommandList()->SetDescriptorHeaps(1, &pMaterials[0].basicDescHeap);
	vnDirect3D::getCommandList()->SetGraphicsRootDescriptorTable(2, pMaterials[0].basicDescHeap->GetGPUDescriptorHandleForHeapStart());
	vnDirect3D::getCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView);
	vnDirect3D::getCommandList()->IASetIndexBuffer(&indexBufferView);
	
	vnDirect3D::getCommandList()->DrawIndexedInstanced(IndexNum, 1, 0, 0, 0);
}

void vnEmitter::setVertexPosition()
{
}

void vnEmitter::setEmit(bool flag)
{
	emit = flag;
}

bool vnEmitter::isEmit()
{
	return emit;
}
