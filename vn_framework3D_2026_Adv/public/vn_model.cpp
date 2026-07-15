//--------------------------------------------------------------//
//	"vn_model.cpp"												//
//		モデルデータ描画クラス									//
//													2026/01/01	//
//														Ichii	//
//--------------------------------------------------------------//
#include "../framework.h"
#include "../framework/vn_environment.h"

extern HWND hWnd;

vnModel::vnModel(const WCHAR* folder, const WCHAR* file) : vnObject()
{
	HRESULT hr;

	pModelData = NULL;
	pMaterials = NULL;
	pVertexBuffer = NULL;
	pIndexBuffer = NULL;
	bones = NULL;
	bonesBuff = NULL;

	//vnmファイルのロード
	WCHAR path[256];
	swprintf_s(path, L"%s%s", folder, file);

	FILE* fp = NULL;
	long size = 0;

	if ((_wfopen_s(&fp, path, L"rb")) != 0)
	{
		WCHAR text[256];
		swprintf_s(text, L"cannot open file \"%s\"", path);
		vnFont::output(text);
		MessageBox(hWnd, text, L"vnModel Construct Error", MB_OK);
		assert(false);
		return;
	}
	fseek(fp, 0, SEEK_END);
	size = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	pModelData = (vnModelData*)new BYTE[size];
	fread(pModelData, size, 1, fp);
	fclose(fp);

	//バージョンチェック
	if (pModelData->Version < vnMODEL_DATA_LATEST_VER)
	{
		WCHAR text[256];
		swprintf_s(text, L"this file version is obsolete : \"%s\"", path);
		vnFont::output(text);
		MessageBox(hWnd, text, L"vnModel Construct Error", MB_OK);
		assert(false);
		return;
	}

	//マテリアルの作成
	vnModel_MaterialData* pMaterialData = reinterpret_cast<vnModel_MaterialData*>(reinterpret_cast<__int64>(pModelData) + pModelData->MaterialAccess);
	pMaterials = new stMaterial[pModelData->MaterialNum];

	for (int i = 0; i < (int)pModelData->MaterialNum; i++)
	{
		TexMetadata metadata = {};
		ScratchImage scratchImg = {};

		pMaterials[i].texbuff = NULL;
		pMaterials[i].basicDescHeap = NULL;
		if (pMaterialData[i].Texture[0])
		{
			swprintf_s(path, L"%s%s", folder, pMaterialData[i].Texture);

			//テクスチャの拡張子
			const WCHAR* ext = wcschr(pMaterialData[i].Texture, '.');

			//テクスチャの読み込み
			if (ext != NULL)
			{
				if (wcscmp(ext, L".tga") == 0 || wcscmp(ext, L".TGA") == 0)
				{
					hr = LoadFromTGAFile(path, TGA_FLAGS_NONE, &metadata, scratchImg);
				}
				else
				{
					hr = LoadFromWICFile(path, WIC_FLAGS_NONE, &metadata, scratchImg);
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
						IID_PPV_ARGS(&pMaterials[i].texbuff)
					);
					pMaterials[i].texbuff->SetName(L"vnModel::texbuff");

					hr = pMaterials[i].texbuff->WriteToSubresource(0,
						NULL,
						img->pixels,
						(UINT)img->rowPitch,
						(UINT)img->slicePitch
					);
				}
			}
		}

		{
			D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc = {};
			descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
			descHeapDesc.NodeMask = 0;
			descHeapDesc.NumDescriptors = 2;
			descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
			hr = vnDirect3D::getDevice()->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(&pMaterials[i].basicDescHeap));
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
				IID_PPV_ARGS(&pMaterials[i].materialBuff)
			);
			if (hr != S_OK)
			{
				assert(hr == S_OK);
			}
			pMaterials[i].materialBuff->SetName(L"vnModel::materialBuff");

			D3D12_CPU_DESCRIPTOR_HANDLE basicHeapHandle = pMaterials[i].basicDescHeap->GetCPUDescriptorHandleForHeapStart();
			//シェーダリソースビューの作成
			if (pMaterials[i].texbuff != NULL)
			{
				//テクスチャビュー作成
				D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
				srvDesc.Format = metadata.format;
				srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
				srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
				srvDesc.Texture2D.MipLevels = 1;

				vnDirect3D::getDevice()->CreateShaderResourceView(pMaterials[i].texbuff,
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
			basicHeapHandle.ptr += vnDirect3D::getDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);


			D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
			cbvDesc.BufferLocation = pMaterials[i].materialBuff->GetGPUVirtualAddress();
			cbvDesc.SizeInBytes = (UINT)pMaterials[i].materialBuff->GetDesc().Width;
			//定数バッファビューの作成
			vnDirect3D::getDevice()->CreateConstantBufferView(&cbvDesc, basicHeapHandle);

			hr = pMaterials[i].materialBuff->Map(0, NULL, (void**)&pMaterials[i].pMaterialBuffer);
		}
		//マテリアルカラー
		pMaterials[i].Diffuse = XMVectorSet(pMaterialData[i].Diffuse[0], pMaterialData[i].Diffuse[1], pMaterialData[i].Diffuse[2], pMaterialData[i].Diffuse[3]);
		pMaterials[i].Ambient = XMVectorSet(pMaterialData[i].Ambient[0], pMaterialData[i].Ambient[1], pMaterialData[i].Ambient[2], pMaterialData[i].Ambient[3]);
		pMaterials[i].Specular = XMVectorSet(pMaterialData[i].Specular[0], pMaterialData[i].Specular[1], pMaterialData[i].Specular[2], pMaterialData[i].Specular[3]);
		pMaterials[i].TilingOffset = XMVectorSet(pMaterialData[i].TilingOffset[0], pMaterialData[i].TilingOffset[1], pMaterialData[i].TilingOffset[2], pMaterialData[i].TilingOffset[3]);
	}

	{	//頂点バッファ
		const UINT vertexStride = (pModelData->VertexStride > 0) ? pModelData->VertexStride : sizeof(vnVertex3D);
		const UINT vertexBufferSize = vertexStride * pModelData->VertexNum;

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
			IID_PPV_ARGS(&pVertexBuffer)
		);
		assert(hr == S_OK);

		pVertexBuffer->SetName(L"vnModel::pVertexBuffer");

		//頂点データのコピー
		vnVertex3D* pVertexDataBegin = reinterpret_cast<vnVertex3D*>(reinterpret_cast<__int64>(pModelData) + pModelData->VertexAccess);
		vnVertex3D* pMappedMem = NULL;
		hr = pVertexBuffer->Map(0, NULL, (void**)&pMappedMem);
		memcpy(pMappedMem, pVertexDataBegin, vertexBufferSize);
		pVertexBuffer->Unmap(0, nullptr);

		//頂点バッファビューを作成
		vertexBufferView.BufferLocation = pVertexBuffer->GetGPUVirtualAddress();
		vertexBufferView.StrideInBytes = vertexStride;
		vertexBufferView.SizeInBytes = vertexBufferSize;
	}


	{	//インデックスバッファ
		const UINT indexBufferSize = ((pModelData->VertexNum <= 0xffff) ? sizeof(WORD) : sizeof(DWORD)) * pModelData->IndexNum;

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
			IID_PPV_ARGS(&pIndexBuffer)
		);
		if (hr != S_OK)
		{
			assert(hr == S_OK);
		}
		pIndexBuffer->SetName(L"vnModel::pIndexBuffer");

		//インデックスデータのコピー
		void* pIndexDataBegin = reinterpret_cast<void*>(reinterpret_cast<__int64>(pModelData) + pModelData->IndexAccess);
		void* pMappedMem = NULL;
		pIndexBuffer->Map(0, nullptr, (void**)&pMappedMem);
		memcpy(pMappedMem, pIndexDataBegin, indexBufferSize);
		pIndexBuffer->Unmap(0, nullptr);

		//インデックスバッファビューを作成
		indexBufferView.BufferLocation = pIndexBuffer->GetGPUVirtualAddress();
		indexBufferView.Format = (pModelData->VertexNum <= 0xffff) ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;
		indexBufferView.SizeInBytes = indexBufferSize;
	}

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
		constBuff->SetName(L"vnModel::constBuff");

		hr = constBuff->Map(0, NULL, (void**)&pConstBuffer);
		assert(hr == S_OK);
	}

	pSkinMatrix = NULL;
	if (pModelData->BoneNum > 0)
	{
		setRenderFlag(eRenderFlag::Skin, true);

		XMMATRIX* iBind = reinterpret_cast<XMMATRIX*>(reinterpret_cast<__int64>(pModelData) + pModelData->iBindAccess);

		pSkinMatrix = new XMMATRIX[pModelData->BoneNum];
		for (UINT i = 0; i < pModelData->BoneNum; i++)
		{
			pSkinMatrix[i] = XMMatrixIdentity();
			//pSkinMatrix[i] = iBind[i];
		}

		//コンスタントバッファ(マトリクス)

		D3D12_HEAP_PROPERTIES heapProp = {};
		heapProp.Type = D3D12_HEAP_TYPE_UPLOAD;
		heapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		heapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		heapProp.CreationNodeMask = 1;
		heapProp.VisibleNodeMask = 1;

		D3D12_RESOURCE_DESC resDesc = {};
		resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		resDesc.Width = ((sizeof(XMFLOAT4X4)* pModelData->BoneNum + 0xff) & ~0xff);
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
			IID_PPV_ARGS(&bonesBuff)
		);
		assert(hr == S_OK);
		bonesBuff->SetName(L"vnModel::bonesBuff");

		hr = bonesBuff->Map(0, NULL, (void**)&bones);
		assert(hr == S_OK);
	}
}

vnModel::~vnModel()
{
	if(pSkinMatrix)delete[] pSkinMatrix;
	SAFE_RELEASE(bonesBuff);
	SAFE_RELEASE(constBuff);
	SAFE_RELEASE(pIndexBuffer);
	SAFE_RELEASE(pVertexBuffer);
	if (pModelData)
	{
		for(int i=0; i<(int)pModelData->MaterialNum; i++)
		{
			SAFE_RELEASE(pMaterials[i].materialBuff);
			SAFE_RELEASE(pMaterials[i].texbuff);
			SAFE_RELEASE(pMaterials[i].basicDescHeap);
		}
		delete[] pMaterials;

		delete[](BYTE*)pModelData;
	}
}

void vnModel::execute()
{
}

void vnModel::render()
{
	vnObject::render();

	//ビューポート変換マトリクスの計算
	XMMATRIX WVP = pModelData->BoneNum ? (*vnCamera::getScreen()) : XMMatrixMultiply(World, *vnCamera::getScreen());

	//コンスタントバッファに情報を設定
	XMStoreFloat4x4(&pConstBuffer->WVP, XMMatrixTranspose(WVP));
	XMStoreFloat4x4(&pConstBuffer->World, XMMatrixTranspose(World));
	vnDirect3D::getCommandList()->SetGraphicsRootConstantBufferView(1, constBuff->GetGPUVirtualAddress());


	if (pSkinMatrix)
	{
		for (DWORD j = 0; j < pModelData->BoneNum; j++)
		{
			XMStoreFloat4x4(&bones[j], pSkinMatrix[j]);
		}
		vnDirect3D::getCommandList()->SetGraphicsRootConstantBufferView(3, bonesBuff->GetGPUVirtualAddress());
	}

	vnDirect3D::getCommandList()->SetPipelineState(pPipelineState[renderFlag]);

	vnDirect3D::getCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	vnDirect3D::getCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView);
	vnDirect3D::getCommandList()->IASetIndexBuffer(&indexBufferView);

	//メッシュ単位の描画
	vnModel_MeshData *pMesh = reinterpret_cast<vnModel_MeshData*>(reinterpret_cast<__int64>(pModelData) + pModelData->MeshAccess);
	for (int i = 0; i < (int)pModelData->MeshNum; i++)
	{
		int mid = pMesh[i].MaterialID;

		//コンスタントバッファに情報を設定
		XMStoreFloat4(&pMaterials[mid].pMaterialBuffer->Diffuse, *getDiffuse(mid));
		XMStoreFloat4(&pMaterials[mid].pMaterialBuffer->Ambient, *getAmbient(mid));
		XMStoreFloat4(&pMaterials[mid].pMaterialBuffer->Specular, *getSpecular(mid));
		XMStoreFloat4(&pMaterials[mid].pMaterialBuffer->TilingOffset, *getTilingOffset(mid));

		vnDirect3D::getCommandList()->SetDescriptorHeaps(1, &pMaterials[mid].basicDescHeap);

		D3D12_GPU_DESCRIPTOR_HANDLE basicHeapHandle = pMaterials[mid].basicDescHeap->GetGPUDescriptorHandleForHeapStart();
		vnDirect3D::getCommandList()->SetGraphicsRootDescriptorTable(2, basicHeapHandle);

		vnDirect3D::getCommandList()->DrawIndexedInstanced(pMesh[i].IndexNum, 1, pMesh[i].StartIndex, pMesh[i].StartVertex, 0);
	}
}

//スキンマトリクスの設定
void vnModel::setSkinMatrix(XMMATRIX* m)
{
	if (pSkinMatrix == NULL)return;

	WORD* table = reinterpret_cast<WORD*>(reinterpret_cast<__int64>(pModelData) + pModelData->BoneTableAccess);

	XMMATRIX* iBind = reinterpret_cast<XMMATRIX*>(reinterpret_cast<__int64>(pModelData) + pModelData->iBindAccess);

	for (DWORD i = 0; i < pModelData->BoneNum; i++)
	{
		pSkinMatrix[i] = iBind[i] * m[table[i]];
	}
}


int vnModel::getVertexNum()
{
	return pModelData ? pModelData->VertexNum : 0;
}

int vnModel::getIndexNum()
{
	return pModelData ? pModelData->IndexNum : 0;
}

int vnModel::getFaceNum()
{
	return pModelData ? pModelData->IndexNum / 3 : 0;
}

int vnModel::getMaterialNum()
{
	return pModelData ? pModelData->MaterialNum : 0;
}

int vnModel::getMeshNum()
{
	return pModelData ? pModelData->MeshNum : 0;
}

int vnModel::getBoneNum()
{
	return pModelData ? pModelData->BoneNum : 0;
}

const WCHAR*vnModel::getTextureName(int id)
{
	if (id < 0 || id >= getMaterialNum())return NULL;
	vnModel_MaterialData *pMaterialData = reinterpret_cast<vnModel_MaterialData*>(reinterpret_cast<__int64>(pModelData) + pModelData->MaterialAccess);
	return pMaterialData[id].Texture;
}

//メッシュデータ(先頭アドレス)の取得
vnModel_MeshData *vnModel::getMesh()
{
	if (!pModelData)return NULL;
	vnModel_MeshData *ret = reinterpret_cast<vnModel_MeshData*>(reinterpret_cast<__int64>(pModelData) + pModelData->MeshAccess);
	return ret;
}

//頂点データ(先頭アドレス)の取得
vnVertex3D *vnModel::getVertex()
{
	if (!pModelData)return NULL;
	vnVertex3D *ret = reinterpret_cast<vnVertex3D*>(reinterpret_cast<__int64>(pModelData) + pModelData->VertexAccess);
	return ret;
}

//インデックスデータ(先頭アドレス)の取得
LPWORD vnModel::getIndex()
{
	if (!pModelData)return NULL;
	LPWORD ret = reinterpret_cast<LPWORD>(reinterpret_cast<__int64>(pModelData) + pModelData->IndexAccess);
	return ret;
}



//拡散色の設定
void vnModel::setDiffuse(float r, float g, float b, float a, int id)
{
	if(pMaterials==NULL || getMaterialNum()<=0)return;
	if(id==-1)
	{
		for(int i=0; i<getMaterialNum(); i++)
		{
			pMaterials[i].Diffuse = XMVectorSet(r, g, b, a);
		}
	}
	else if(id>=0 && id<getMaterialNum())
	{
		pMaterials[id].Diffuse = XMVectorSet(r, g, b, a);
	}
}

void vnModel::setDiffuse(const XMVECTOR *v, int id)
{
	if (pMaterials == NULL || getMaterialNum() <= 0)return;
	if(id==-1)
	{
		for(int i=0; i<getMaterialNum(); i++)
		{
			pMaterials[i].Diffuse = *v;
		}
	}
	else if(id>=0 && id< getMaterialNum())
	{
		pMaterials[id].Diffuse = *v;
	}
}

void vnModel::setDiffuseR(float value, int id)
{
	if (pMaterials == NULL || getMaterialNum() <= 0)return;
	if(id==-1)
	{
		for(int i=0; i< getMaterialNum(); i++)
		{
			pMaterials[i].Diffuse = XMVectorSetX(pMaterials[i].Diffuse, value);
		}
	}
	else if(id>=0 && id< getMaterialNum())
	{
		pMaterials[id].Diffuse = XMVectorSetX(pMaterials[id].Diffuse, value);
	}
}

void vnModel::setDiffuseG(float value, int id)
{
	if (pMaterials == NULL || getMaterialNum() <= 0)return;
	if(id==-1)
	{
		for(int i=0; i<getMaterialNum(); i++)
		{
			pMaterials[i].Diffuse = XMVectorSetY(pMaterials[i].Diffuse, value);
		}
	}
	else if(id>=0 && id<getMaterialNum())
	{
		pMaterials[id].Diffuse = XMVectorSetY(pMaterials[id].Diffuse, value);
	}
}

void vnModel::setDiffuseB(float value, int id)
{
	if (pMaterials == NULL || getMaterialNum() <= 0)return;
	if(id==-1)
	{
		for(int i=0; i<getMaterialNum(); i++)
		{
			pMaterials[i].Diffuse = XMVectorSetZ(pMaterials[i].Diffuse, value);
		}
	}
	else if(id>=0 && id<getMaterialNum())
	{
		pMaterials[id].Diffuse = XMVectorSetZ(pMaterials[id].Diffuse, value);
	}
}

void vnModel::setDiffuseA(float value, int id)
{
	if (pMaterials == NULL || getMaterialNum() <= 0)return;
	if(id==-1)
	{
		for(int i=0; i<getMaterialNum(); i++)
		{
			pMaterials[i].Diffuse = XMVectorSetW(pMaterials[i].Diffuse, value);
		}
	}
	else if(id>=0 && id<getMaterialNum())
	{
		pMaterials[id].Diffuse = XMVectorSetW(pMaterials[id].Diffuse, value);
	}
}

void vnModel::addDiffuseR(float value, int id)
{
	if (pMaterials == NULL || getMaterialNum() <= 0)return;
	if(id==-1)
	{
		for(int i=0; i<getMaterialNum(); i++)
		{
			pMaterials[i].Diffuse = XMVectorAdd(pMaterials[i].Diffuse, XMVectorSet(value, 0.0f, 0.0f, 0.0f));
		}
	}
	else if(id>=0 && id<getMaterialNum())
	{
		pMaterials[id].Diffuse = XMVectorAdd(pMaterials[id].Diffuse, XMVectorSet(value, 0.0f, 0.0f, 0.0f));
	}
}

void vnModel::addDiffuseG(float value, int id)
{
	if (pMaterials == NULL || getMaterialNum() <= 0)return;
	if (id == -1)
	{
		for (int i = 0; i < getMaterialNum(); i++)
		{
			pMaterials[i].Diffuse = XMVectorAdd(pMaterials[i].Diffuse, XMVectorSet(0.0f, value, 0.0f, 0.0f));
		}
	}
	else if (id >= 0 && id < getMaterialNum())
	{
		pMaterials[id].Diffuse = XMVectorAdd(pMaterials[id].Diffuse, XMVectorSet(0.0f, value, 0.0f, 0.0f));
	}
}

void vnModel::addDiffuseB(float value, int id)
{
	if (pMaterials == NULL || getMaterialNum() <= 0)return;
	if(id==-1)
	{
		for(int i=0; i<getMaterialNum(); i++)
		{
			pMaterials[i].Diffuse = XMVectorAdd(pMaterials[i].Diffuse, XMVectorSet(0.0f, 0.0f, value, 0.0f));
		}
	}
	else if(id>=0 && id<getMaterialNum())
	{
		pMaterials[id].Diffuse = XMVectorAdd(pMaterials[id].Diffuse, XMVectorSet(0.0f, 0.0f, value, 0.0f));
	}
}

void vnModel::addDiffuseA(float value, int id)
{
	if (pMaterials == NULL || getMaterialNum() <= 0)return;
	if (id == -1)
	{
		for (int i = 0; i < getMaterialNum(); i++)
		{
			pMaterials[i].Diffuse = XMVectorAdd(pMaterials[i].Diffuse, XMVectorSet(0.0f, 0.0f, 0.0f, value));
		}
	}
	else if (id >= 0 && id < getMaterialNum())
	{
		pMaterials[id].Diffuse = XMVectorAdd(pMaterials[id].Diffuse, XMVectorSet(0.0f, 0.0f, 0.0f, value));
	}
}

//拡散色の取得
XMVECTOR *vnModel::getDiffuse(int id)
{
	if (pMaterials == NULL || getMaterialNum() <= 0)return NULL;
	return &pMaterials[id].Diffuse;
}

float vnModel::getDiffuseR(int id)
{
	if (pMaterials == NULL || getMaterialNum() <= 0)return 1.0f;
	return XMVectorGetX(pMaterials[id].Diffuse);
}

float vnModel::getDiffuseG(int id)
{
	if (pMaterials == NULL || getMaterialNum() <= 0)return 1.0f;
	return XMVectorGetY(pMaterials[id].Diffuse);
}

float vnModel::getDiffuseB(int id)
{
	if (pMaterials == NULL || getMaterialNum() <= 0)return 1.0f;
	return XMVectorGetZ(pMaterials[id].Diffuse);
}

float vnModel::getDiffuseA(int id)
{
	if (pMaterials == NULL || getMaterialNum() <= 0)return 1.0f;
	return XMVectorGetW(pMaterials[id].Diffuse);
}


//環境色の設定
void vnModel::setAmbient(float r, float g, float b, float a, int id)
{
	if (pMaterials == NULL || getMaterialNum() <= 0)return;
	if(id==-1)
	{
		for(int i=0; i<getMaterialNum(); i++)
		{
			pMaterials[i].Ambient = XMVectorSet(r, g, b, a);
		}
	}
	else if(id>=0 && id<getMaterialNum())
	{
		pMaterials[id].Ambient = XMVectorSet(r, g, b, a);
	}
}

void vnModel::setAmbient(const XMVECTOR *v, int id)
{
	if (pMaterials == NULL || getMaterialNum() <= 0)return;
	if(id==-1)
	{
		for(int i=0; i<getMaterialNum(); i++)
		{
			pMaterials[i].Ambient = *v;
		}
	}
	else if(id>=0 && id<getMaterialNum())
	{
		pMaterials[id].Ambient = *v;
	}
}

void vnModel::setAmbientR(float value, int id)
{
	if (pMaterials == NULL || getMaterialNum() <= 0)return;
	if(id==-1)
	{
		for(int i=0; i<getMaterialNum(); i++)
		{
			pMaterials[i].Ambient = XMVectorSetX(pMaterials[i].Ambient, value);
		}
	}
	else if(id>=0 && id<getMaterialNum())
	{
		pMaterials[id].Ambient = XMVectorSetX(pMaterials[id].Ambient, value);
	}
}

void vnModel::setAmbientG(float value, int id)
{
	if (pMaterials == NULL || getMaterialNum() <= 0)return;
	if (id == -1)
	{
		for (int i = 0; i < getMaterialNum(); i++)
		{
			pMaterials[i].Ambient = XMVectorSetY(pMaterials[i].Ambient, value);
		}
	}
	else if (id >= 0 && id < getMaterialNum())
	{
		pMaterials[id].Ambient = XMVectorSetY(pMaterials[id].Ambient, value);
	}
}

void vnModel::setAmbientB(float value, int id)
{
	if (pMaterials == NULL || getMaterialNum() <= 0)return;
	if (id == -1)
	{
		for (int i = 0; i < getMaterialNum(); i++)
		{
			pMaterials[i].Ambient = XMVectorSetZ(pMaterials[i].Ambient, value);
		}
	}
	else if (id >= 0 && id < getMaterialNum())
	{
		pMaterials[id].Ambient = XMVectorSetZ(pMaterials[id].Ambient, value);
	}
}

void vnModel::setAmbientA(float value, int id)
{
	if (pMaterials == NULL || getMaterialNum() <= 0)return;
	if (id == -1)
	{
		for (int i = 0; i < getMaterialNum(); i++)
		{
			pMaterials[i].Ambient = XMVectorSetW(pMaterials[i].Ambient, value);
		}
	}
	else if (id >= 0 && id < getMaterialNum())
	{
		pMaterials[id].Ambient = XMVectorSetW(pMaterials[id].Ambient, value);
	}
}

void vnModel::addAmbientR(float value, int id)
{
	if (pMaterials == NULL || getMaterialNum() <= 0)return;
	if (id == -1)
	{
		for (int i = 0; i < getMaterialNum(); i++)
		{
			pMaterials[i].Ambient = XMVectorAdd(pMaterials[i].Ambient, XMVectorSet(value, 0.0f, 0.0f, 0.0f));
		}
	}
	else if (id >= 0 && id < getMaterialNum())
	{
		pMaterials[id].Ambient = XMVectorAdd(pMaterials[id].Ambient, XMVectorSet(value, 0.0f, 0.0f, 0.0f));
	}
}

void vnModel::addAmbientG(float value, int id)
{
	if (pMaterials == NULL || getMaterialNum() <= 0)return;
	if (id == -1)
	{
		for (int i = 0; i < getMaterialNum(); i++)
		{
			pMaterials[i].Ambient = XMVectorAdd(pMaterials[i].Ambient, XMVectorSet(0.0f, value, 0.0f, 0.0f));
		}
	}
	else if (id >= 0 && id < getMaterialNum())
	{
		pMaterials[id].Ambient = XMVectorAdd(pMaterials[id].Ambient, XMVectorSet(0.0f, value, 0.0f, 0.0f));
	}
}

void vnModel::addAmbientB(float value, int id)
{
	if (pMaterials == NULL || getMaterialNum() <= 0)return;
	if (id == -1)
	{
		for (int i = 0; i < getMaterialNum(); i++)
		{
			pMaterials[i].Ambient = XMVectorAdd(pMaterials[i].Ambient, XMVectorSet(0.0f, 0.0f, value, 0.0f));
		}
	}
	else if (id >= 0 && id < getMaterialNum())
	{
		pMaterials[id].Ambient = XMVectorAdd(pMaterials[id].Ambient, XMVectorSet(0.0f, 0.0f, value, 0.0f));
	}
}

void vnModel::addAmbientA(float value, int id)
{
	if (pMaterials == NULL || getMaterialNum() <= 0)return;
	if (id == -1)
	{
		for (int i = 0; i < getMaterialNum(); i++)
		{
			pMaterials[i].Ambient = XMVectorAdd(pMaterials[i].Ambient, XMVectorSet(0.0f, 0.0f, 0.0f, value));
		}
	}
	else if (id >= 0 && id < getMaterialNum())
	{
		pMaterials[id].Ambient = XMVectorAdd(pMaterials[id].Ambient, XMVectorSet(0.0f, 0.0f, 0.0f, value));
	}
}

//環境色の取得
XMVECTOR *vnModel::getAmbient(int id)
{
	if (pMaterials == NULL || getMaterialNum() <= 0)return NULL;
	return &pMaterials[id].Ambient;
}

float vnModel::getAmbientR(int id)
{
	if (pMaterials == NULL || getMaterialNum() <= 0)return 0.0f;
	return XMVectorGetX(pMaterials[id].Ambient);
}

float vnModel::getAmbientG(int id)
{
	if (pMaterials == NULL || getMaterialNum() <= 0)return 0.0f;
	return XMVectorGetY(pMaterials[id].Ambient);
}

float vnModel::getAmbientB(int id)
{
	if (pMaterials == NULL || getMaterialNum() <= 0)return 0.0f;
	return XMVectorGetZ(pMaterials[id].Ambient);
}

float vnModel::getAmbientA(int id)
{
	if (pMaterials == NULL || getMaterialNum() <= 0)return 0.0f;
	return XMVectorGetW(pMaterials[id].Ambient);
}


//鏡面反射色の設定
void vnModel::setSpecular(float r, float g, float b, float p, int id)
{
	if (pMaterials == NULL || getMaterialNum() <= 0)return;
	if (id == -1)
	{
		for (int i = 0; i < getMaterialNum(); i++)
		{
			pMaterials[i].Specular = XMVectorSet(r, g, b, p);
		}
	}
	else if (id >= 0 && id < getMaterialNum())
	{
		pMaterials[id].Specular = XMVectorSet(r, g, b, p);
	}
}

void vnModel::setSpecular(const XMVECTOR *v, int id)
{
	if (pMaterials == NULL || getMaterialNum() <= 0)return;
	if (id == -1)
	{
		for (int i = 0; i < getMaterialNum(); i++)
		{
			pMaterials[i].Specular = *v;
		}
	}
	else if (id >= 0 && id < getMaterialNum())
	{
		pMaterials[id].Specular = *v;
	}
}

void vnModel::setSpecularR(float value, int id)
{
	if (pMaterials == NULL || getMaterialNum() <= 0)return;
	if (id == -1)
	{
		for (int i = 0; i < getMaterialNum(); i++)
		{
			pMaterials[i].Specular = XMVectorSetX(pMaterials[i].Specular, value);
		}
	}
	else if (id >= 0 && id < getMaterialNum())
	{
		pMaterials[id].Specular = XMVectorSetX(pMaterials[id].Specular, value);
	}
}

void vnModel::setSpecularG(float value, int id)
{
	if (pMaterials == NULL || getMaterialNum() <= 0)return;
	if (id == -1)
	{
		for (int i = 0; i < getMaterialNum(); i++)
		{
			pMaterials[i].Specular = XMVectorSetY(pMaterials[i].Specular, value);
		}
	}
	else if (id >= 0 && id < getMaterialNum())
	{
		pMaterials[id].Specular = XMVectorSetY(pMaterials[id].Specular, value);
	}
}

void vnModel::setSpecularB(float value, int id)
{
	if (pMaterials == NULL || getMaterialNum() <= 0)return;
	if (id == -1)
	{
		for (int i = 0; i < getMaterialNum(); i++)
		{
			pMaterials[i].Specular = XMVectorSetZ(pMaterials[i].Specular, value);
		}
	}
	else if (id >= 0 && id < getMaterialNum())
	{
		pMaterials[id].Specular = XMVectorSetZ(pMaterials[id].Specular, value);
	}
}

void vnModel::setSpecularPower(float value, int id)
{
	if (pMaterials == NULL || getMaterialNum() <= 0)return;
	if (id == -1)
	{
		for (int i = 0; i < getMaterialNum(); i++)
		{
			pMaterials[i].Specular = XMVectorSetW(pMaterials[i].Specular, value);
		}
	}
	else if (id >= 0 && id < getMaterialNum())
	{
		pMaterials[id].Specular = XMVectorSetW(pMaterials[id].Specular, value);
	}
}

void vnModel::addSpecularR(float value, int id)
{
	if (pMaterials == NULL || getMaterialNum() <= 0)return;
	if (id == -1)
	{
		for (int i = 0; i < getMaterialNum(); i++)
		{
			pMaterials[i].Specular = XMVectorAdd(pMaterials[i].Specular, XMVectorSet(value, 0.0f, 0.0f, 0.0f));
		}
	}
	else if (id >= 0 && id < getMaterialNum())
	{
		pMaterials[id].Specular = XMVectorAdd(pMaterials[id].Specular, XMVectorSet(value, 0.0f, 0.0f, 0.0f));
	}
}

void vnModel::addSpecularG(float value, int id)
{
	if (pMaterials == NULL || getMaterialNum() <= 0)return;
	if (id == -1)
	{
		for (int i = 0; i < getMaterialNum(); i++)
		{
			pMaterials[i].Specular = XMVectorAdd(pMaterials[i].Specular, XMVectorSet(0.0f, value, 0.0f, 0.0f));
		}
	}
	else if (id >= 0 && id < getMaterialNum())
	{
		pMaterials[id].Specular = XMVectorAdd(pMaterials[id].Specular, XMVectorSet(0.0f, value, 0.0f, 0.0f));
	}
}

void vnModel::addSpecularB(float value, int id)
{
	if (pMaterials == NULL || getMaterialNum() <= 0)return;
	if (id == -1)
	{
		for (int i = 0; i < getMaterialNum(); i++)
		{
			pMaterials[i].Specular = XMVectorAdd(pMaterials[i].Specular, XMVectorSet(0.0f, 0.0f, value, 0.0f));
		}
	}
	else if (id >= 0 && id < getMaterialNum())
	{
		pMaterials[id].Specular = XMVectorAdd(pMaterials[id].Specular, XMVectorSet(0.0f, 0.0f, value, 0.0f));
	}
}

void vnModel::addSpecularPower(float value, int id)
{
	if (pMaterials == NULL || getMaterialNum() <= 0)return;
	if (id == -1)
	{
		for (int i = 0; i < getMaterialNum(); i++)
		{
			pMaterials[i].Specular = XMVectorAdd(pMaterials[i].Specular, XMVectorSet(0.0f, 0.0f, 0.0f, value));
		}
	}
	else if (id >= 0 && id < getMaterialNum())
	{
		pMaterials[id].Specular = XMVectorAdd(pMaterials[id].Specular, XMVectorSet(0.0f, 0.0f, 0.0f, value));
	}
}

//鏡面反射色の取得
XMVECTOR *vnModel::getSpecular(int id)
{
	if (pMaterials == NULL || getMaterialNum() <= 0)return NULL;
	return &pMaterials[id].Specular;
}

float vnModel::getSpecularR(int id)
{
	if (pMaterials == NULL || getMaterialNum() <= 0)return 0.0f;
	return XMVectorGetX(pMaterials[id].Specular);
}

float vnModel::getSpecularG(int id)
{
	if (pMaterials == NULL || getMaterialNum() <= 0)return 0.0f;
	return XMVectorGetY(pMaterials[id].Specular);
}

float vnModel::getSpecularB(int id)
{
	if (pMaterials == NULL || getMaterialNum() <= 0)return 0.0f;
	return XMVectorGetZ(pMaterials[id].Specular);
}

float vnModel::getSpecularPower(int id)
{
	if (pMaterials == NULL || getMaterialNum() <= 0)return 1.0f;
	return XMVectorGetW(pMaterials[id].Specular);
}

//タイリングとオフセットの取得
const XMVECTOR* vnModel::getTilingOffset(int id)
{
	if (pMaterials == NULL || getMaterialNum() <= 0)return NULL;
	return &pMaterials[id].TilingOffset;
}