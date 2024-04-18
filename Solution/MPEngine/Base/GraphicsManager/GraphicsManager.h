#pragma once

//	DirectX12
#include <d3d12.h>
#pragma comment(lib,"d3d12.lib")
#include <dxgi1_6.h>
#pragma comment(lib,"dxgi.lib")

#include <Windows.h>
#include <wrl.h>
#include <memory>

class WindowSupervisor;
class DeviceManager;
class ListManager;
class SwapChain;
class ResourceManager;
class DepthBuffer;
class ImGuiManager;
class RenderTarget;

class GraphicsManager {
private:
	GraphicsManager() = default;
	GraphicsManager(GraphicsManager&&) = delete;
	~GraphicsManager() = default;

public:
	static GraphicsManager* GetInstance();

	// 初期化
	void Initialize(unsigned int bufferWidth, unsigned int bufferHeight);

	// 描画前処理
	void PreDraw();

	// 描画後処理
	void PostDraw();

	// 解放処理
	void Finalize();

	// windowSize
	void SetViewPort(uint32_t width, uint32_t height);

private:

	// DXGIファクトリーの生成
	void CreateFactry();
	// 使用するアダプタを決める
	void SelectAdapter();
	// コマンドキューの生成
	void CreateCommandQueue();
	// フェンスの生成
	void CreateFence();
	// ImGuiの初期化・解放
	void ImGuiInitialize();
	void ImGuiFinalize();

	// 終了処理
	void EndProcess();

	// バリアの作成
	void CreateBarrier(ID3D12Resource* pResource, D3D12_RESOURCE_STATES stateBefore, D3D12_RESOURCE_STATES stateAfter,
		D3D12_RESOURCE_BARRIER_TYPE type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION, D3D12_RESOURCE_BARRIER_FLAGS flag = D3D12_RESOURCE_BARRIER_FLAG_NONE);
	// 指定色でクリア
	void ClearRenderTarget(D3D12_CPU_DESCRIPTOR_HANDLE rtvHeapPointer);

private:
	const WindowSupervisor* winSv_ = nullptr;

	// DXGIファクトリの生成
	Microsoft::WRL::ComPtr<IDXGIFactory7> dxgiFactory_;
	// 使用するアダプタ用の変数
	Microsoft::WRL::ComPtr<IDXGIAdapter4> useAdapter_;
	// デバイスの生成
	DeviceManager* device_ = nullptr;
	// GPUに命令を投げるキューの生成
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue_;
	// GPUの命令群の生成
	ListManager* commandList_ = nullptr;
	// 
	std::unique_ptr<SwapChain> swapChain_;
	// 
	ResourceManager* rsManager_ = nullptr;
	//
	std::unique_ptr<DepthBuffer> depthBuffer_;
	// imgui用
	ImGuiManager* imguiManager_ = nullptr;
	// RenderTexture用
	std::unique_ptr<RenderTarget> renderTexture_;

	// フェンス関係
	Microsoft::WRL::ComPtr<ID3D12Fence> fence_;
	uint32_t fenceValue_ = 0u;
	HANDLE fenceEvent_{};

	// レンダーターゲット用クリアカラー
	const float clearColor_[4] = { 0.1f,0.25f,0.5f,1.0f };	//	青っぽい色、RGBA

public: // ゲッター

};
