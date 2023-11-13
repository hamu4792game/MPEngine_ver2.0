#pragma once
#include <wrl.h>
#include <d3d12.h>
#pragma comment(lib,"d3d12.lib")
#include <dxgi1_6.h>
#pragma comment(lib,"dxgi.lib")
#include <dxgidebug.h>
#pragma comment(lib,"dxguid.lib")

// 前方宣言
class CommandDirectX;
class WinApp;


class MPEngine {
public:
	static MPEngine* GetInstance();

	void Initialize(const char* title, int width, int height);

public:
	int windowWidth_;
	int windowHeight_;

private:
	class D3DResourceLeakChecker {
	public:
		D3DResourceLeakChecker() = default;
		~D3DResourceLeakChecker();
		void EnableDebugLayer();
		void ErrorStoped(ID3D12Device* device_);

	};
	static D3DResourceLeakChecker debugLayer_;
};

