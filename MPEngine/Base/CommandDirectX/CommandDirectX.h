#pragma once

//	DirectX12
#include <d3d12.h>
#pragma comment(lib,"d3d12.lib")
#include <dxgi1_6.h>
#pragma comment(lib,"dxgi.lib")

#include <Windows.h>
#include <wrl.h>
#include <memory>

class WinApp;
class DeviceManager;
class ListManager;
class SwapChain;
class ResourceManager;

class CommandDirectX
{
public:
	CommandDirectX() = default;
	~CommandDirectX() = default;

	static CommandDirectX* GetInstance();

	// 初期化
	void Initialize(unsigned int bufferWidth, unsigned int bufferHeight);

	// 描画前処理
	void PreDraw();

	// 描画後処理
	void PostDraw();

	// 解放処理
	void Finalize();

private:

	// DXGIファクトリーの生成
	void CreateFactry();
	// 使用するアダプタを決める
	void SelectAdapter();
	// コマンドキューの生成
	void CreateCommandQueue();
	// ImGuiの初期化
	void ImGuiInitialize();

private:
	const WinApp* winApp_ = nullptr;

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
	//ResourceManager* rsManager_ = nullptr;
public: // ゲッター
	
};
