#pragma once
#include <wrl.h>
#include <d3d12.h>
#pragma comment(lib,"d3d12.lib")
#include <dxgi1_6.h>
#pragma comment(lib,"dxgi.lib")
#include <dxgidebug.h>
#pragma comment(lib,"dxguid.lib")

// 前方宣言
class GraphicsManager;
class WindowSupervisor;
class Input;

class MPEngine {
private:
	MPEngine() = default;
	~MPEngine() = default;
public:
	static MPEngine* GetInstance();
	void Run();
private:
	void Initialize(const char* title, int width, int height);
	void Update();
	void Finalize();
public:
	int windowWidth_ = 1280;
	int windowHeight_ = 720;

private: // メンバ変数
	WindowSupervisor* winSv_ = nullptr;
	GraphicsManager* graphics_ = nullptr;
	Input* input_ = nullptr;
#ifdef _DEBUG
private: // デバッグレイヤー
	class D3DResourceLeakChecker {
	public:
		D3DResourceLeakChecker() = default;
		~D3DResourceLeakChecker();
		void EnableDebugLayer();
		void ErrorStoped(ID3D12Device* device_);

	};
	static D3DResourceLeakChecker debugLayer_;
#endif // DEBUG
};

