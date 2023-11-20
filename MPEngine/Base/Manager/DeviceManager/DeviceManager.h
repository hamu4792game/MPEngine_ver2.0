#pragma once
//	DirectX12
#include <d3d12.h>
#pragma comment(lib,"d3d12.lib")
#include <dxgi1_6.h>
#pragma comment(lib,"dxgi.lib")
#include <wrl.h>

class DeviceManager
{
public:
	DeviceManager() = default;
	~DeviceManager() = default;

	static DeviceManager* GetInstance();

	static void Initialize();
	static void Finalize();

private:
	// デバイスの生成
	Microsoft::WRL::ComPtr<ID3D12Device> device_;
	static DeviceManager* instance;

public:
	void CreateDevice(IDXGIAdapter4* useAdapter);

public: // ゲッター
	ID3D12Device* const GetDevice() { return device_.Get(); }

};

