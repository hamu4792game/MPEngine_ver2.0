#pragma once
//	DirectX12
#include <d3d12.h>
#pragma comment(lib,"d3d12.lib")
#include <dxgi1_6.h>
#pragma comment(lib,"dxgi.lib")
#include <wrl.h>

class ListManager {
private:
	ListManager() = default;
	~ListManager() = default;
public:
	static ListManager* GetInstance();

public:
	void CreateList();

	// 次のフレーム用のコマンドリストを用意
	void Reset();

private:
	// 命令保存用メモリ管理機構の生成
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator_;
	// GPUの命令群の生成
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList_;

public: // ゲッター
	ID3D12CommandAllocator* const GetAllocator() { return commandAllocator_.Get(); }
	ID3D12GraphicsCommandList* const GetList() { return commandList_.Get(); }

};
