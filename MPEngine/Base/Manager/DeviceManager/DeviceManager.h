#pragma once
//	DirectX12
#include <d3d12.h>
#pragma comment(lib,"d3d12.lib")
#include <dxgi1_6.h>
#pragma comment(lib,"dxgi.lib")
#include <wrl.h>

class DeviceManager {
private:
	DeviceManager() = default;
	~DeviceManager() = default;
public:
	static DeviceManager* GetInstance();

	void CreateDevice(IDXGIAdapter4* useAdapter);
private:
	// デバイスの生成
	Microsoft::WRL::ComPtr<ID3D12Device> device_;

public: // ゲッター
	ID3D12Device* const GetDevice() { return device_.Get(); }

};

