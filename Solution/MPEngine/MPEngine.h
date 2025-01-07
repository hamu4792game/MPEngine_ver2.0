#pragma once
#include <wrl.h>
#include <d3d12.h>
#pragma comment(lib,"d3d12.lib")

// 前方宣言
class GraphicsManager;
class WindowSupervisor;
class Input;
class ShaderManager;
#include "MPEngine/Graphics/RenderManager/RenderManager.h"
#include "Framework/GameFrame.h"

class MPEngine {
private:
	MPEngine() = default;
	~MPEngine() = default;
public:
	static MPEngine* GetInstance();
	void Run(GameFrame* game = nullptr);
private:
	void Initialize(const char* title, int width, int height);
	bool Update();
	void Finalize();
public:
	int windowWidth_ = 1280;
	int windowHeight_ = 720;

private: // メンバ変数
	WindowSupervisor* winSv_ = nullptr;
	GraphicsManager* graphics_ = nullptr;
	Input* input_ = nullptr;
	// シェーダーマネージャー
	ShaderManager* shaderManager_ = nullptr;
	// 描画のためのインスタンス
	RenderManager render;


	// ゲームシーン
	GameFrame* game_ = nullptr;


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

