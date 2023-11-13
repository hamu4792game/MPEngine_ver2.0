#pragma once

//	DirectX12
#include <d3d12.h>
#pragma comment(lib,"d3d12.lib")
#include <dxgi1_6.h>
#pragma comment(lib,"dxgi.lib")

#include <Windows.h>
#include <wrl.h>

class WinApp;

class CommandDirectX
{
public:
	CommandDirectX() = default;
	~CommandDirectX() = default;

	static CommandDirectX* GetInstance();

	// 初期化
	void Initialize(const WinApp* winApp, unsigned int bufferWidth, unsigned int bufferHeight);

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
	// デバイスの生成
	void CreateDevice();
	// コマンドキューの生成
	void CreateCommandQueue();
	// コマンドキューの生成
	void CreateCommandList();

private:
	const WinApp* winApp_ = nullptr;

	// DXGIファクトリの生成
	Microsoft::WRL::ComPtr<IDXGIFactory7> dxgiFactory_;
	// 使用するアダプタ用の変数
	Microsoft::WRL::ComPtr<IDXGIAdapter4> useAdapter_;
	// デバイスの生成
	Microsoft::WRL::ComPtr<ID3D12Device> device_;
	// GPUに命令を投げるキューの生成
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue_;
	// 命令保存用メモリ管理機構の生成
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator_;
	// GPUの命令群の生成
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList_;

public: // ゲッター
	ID3D12Device* GetDevice() const { return device_.Get(); }
	ID3D12GraphicsCommandList* GetList() const { return commandList_.Get(); }

};
