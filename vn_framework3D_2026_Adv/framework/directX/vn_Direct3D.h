//--------------------------------------------------------------//
//	"vn_Direct3D.h"												//
//		Direct3D管理											//
//													2026/01/01	//
//														Ichii	//
//--------------------------------------------------------------//
#pragma once

class vnDirect3D
{
public:
	static constexpr UINT frameCount = 2;

	enum ePostProcess
	{
		Copy,
		GrayScale,
		PostPorcessMax,
	};

private:
	static UINT rtvIndex;

	static IDXGIFactory4* pFactory;	//DXGIファクトリ
	static IDXGIAdapter3* pAdapter;	//アダプター
	static ID3D12Device* pDevice;	//D3D12デバイス
	static IDXGISwapChain3* pSwapChain;	//スワップチェイン
	static ID3D12CommandQueue* pCmdQueue;	//コマンドキュー
	static HANDLE						hFenceEvent;	//フェンスイベント
	static ID3D12Fence* pQueueFence;	//コマンドキュー用のフェンス
	static ID3D12DescriptorHeap* pDH_RTV;	//ディスクリプタヒープ(RenderTargetView)
	static ID3D12DescriptorHeap* pDH_DSV;	//ディスクリプタヒープ(DepthStencilView)
	static D3D12_CPU_DESCRIPTOR_HANDLE		hRTV[frameCount];	//ディスクリプタハンドル(RenderTargetView)
	static D3D12_CPU_DESCRIPTOR_HANDLE		hDSV;				//ディスクリプタハンドル(DepthStencilView)
	static ID3D12Resource* pRenderTarget[frameCount];	//レンダーターゲット
	static ID3D12Resource* pDepthBuffer;
	static ID3D12CommandAllocator* pCmdAllocator;	//コマンドアロケータ
	static ID3D12GraphicsCommandList* pCmdList;	//コマンドリスト
	static ID3D12RootSignature* pRootSignature;	//ルートシグネチャ

	static ID3D12RootSignature* pRootSignature2D;	//ルートシグネチャ

	static UINT fenceValue;

	static ID3D12Resource* pWhiteTex;
	static D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc;

	struct stSceneBuffer
	{
		XMFLOAT4X4	VP;			//View * Proj
		XMFLOAT4X4	LightVP;	//シャドウマップ用
		XMFLOAT4	LightDir;	//平行光源の(逆)方向
		XMFLOAT4	LightCol;	//平行光源の色
		XMFLOAT4	LightAmb;	//環境光の色
		XMFLOAT4	CameraPos;	//カメラのワールド座標
	};
	static ID3D12Resource* pSceneBuffer;		//シーン用の定数バッファ
	static ID3D12DescriptorHeap* pSceneHeap;
	static stSceneBuffer* pScene;

	//ポストプロセス用
	static D3D12_CLEAR_VALUE screenClearValue;

	static ID3D12Resource* pScreenResource;
	static D3D12_CPU_DESCRIPTOR_HANDLE		hPPRTV;				//ディスクリプタハンドル(RenderTargetView)
	static D3D12_CPU_DESCRIPTOR_HANDLE		hPPSRV;				//ディスクリプタハンドル(ShaderResourceView)

	static ID3D12DescriptorHeap* pPPRTVHeap;
	static ID3D12DescriptorHeap* pPPSRVHeap;

	static ID3D12Resource* pScreenVB;
	static D3D12_VERTEX_BUFFER_VIEW screenVBView;

	static ID3D12RootSignature* pRootSignatureScr;
	static ID3D12PipelineState* pPipelineStateScr[ePostProcess::PostPorcessMax];

	static int currentPostProcess;


	//初期化関数
	static void createFactory();
	static void createDevice();
	static void createCommandQueue();
	static void createSwapChain();
	static void createRenderTargetView();
	static void createDepthStencilBuffer();
	static void createCommandList();
	static void createRootSignature();
	static void createRootSignature2D();

	static void createWhiteTexture();

	//ポストプロセス用
	static void createSceneBuffer();
	static void createScreenVertex();
	static void createScreenPipeline();

	static void createPostProcessResource();

public:
	//フレームワーク管理
	static int initialize(void);
	static void terminate(void);

	//描画システムコール
	static void beginRender(void);
	static void beginRender2D(void);
	static void finishRender(void);
	static void present(void);

	//インターフェイス関連の取得
	static ID3D12Device* getDevice(void);
	static ID3D12CommandQueue* getCommandQueue(void);
	static ID3D12RootSignature* getRootSignature(void);
	static ID3D12RootSignature* getRootSignature2D(void);

	static ID3D12GraphicsCommandList* getCommandList(void);

	static ID3D12Resource* getRenderTarget(int id);

	static UINT getIndex(void);

	static ID3D12Resource* getWhiteTexture();
	static D3D12_SHADER_RESOURCE_VIEW_DESC* getWhiteTextueViewDesc();
};