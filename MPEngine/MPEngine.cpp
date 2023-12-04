#include "MPEngine.h"
#include "MPEngine/Base/WindowSupervisor/WindowSupervisor.h"
#include "MPEngine/Base/GraphicsManager/GraphicsManager.h"
#include "MPEngine/Base/Log.h"
#include "MPEngine/Base/Manager/DeviceManager/DeviceManager.h"
#include "MPEngine/Base/Manager/ResourceManager/ResourceManager.h"
#include "Input/Input.h"
#include "TimeBaseLoopExecuter/TimeBaseLoopExecuter.h"

MPEngine* MPEngine::GetInstance() {
	static MPEngine instance;
	return &instance;
}

void MPEngine::Run() {
	int32_t windowWidth = 1280; int32_t windowHeight = 720;
	Initialize("えとせとら", windowWidth, windowHeight);

	//	ウィンドウの×ボタンが押されるまでループ
	while (!winSv_->ProcessMessage()) {
		//	フレームの開始
		TimeBaseLoopExecuter fpsManager(60);
		Update();
		fpsManager.TimeAdjustment();
		//break;
	}

	Finalize();

}

void MPEngine::Initialize(const char* title, int width, int height) {
	windowWidth_ = width;
	windowHeight_ = height;

	// ゲームウィンドウの生成
	winSv_ = WindowSupervisor::GetInstance();
	winSv_->CreateGameWindow(ConvertString(title).c_str(), windowWidth_, windowHeight_);

#ifdef _DEBUG
	debugLayer_.EnableDebugLayer();
#endif // debugLayerの有効化
	
	// DirectXの初期化
	graphics_ = GraphicsManager::GetInstance();
	graphics_->Initialize(windowWidth_, windowHeight_);

#ifdef _DEBUG
	debugLayer_.ErrorStoped(DeviceManager::GetInstance()->GetDevice());
#endif

	// 入力処理の初期化
	input_ = Input::GetInstance();
	input_->Initialize();

}

void MPEngine::Update() {
	graphics_->PreDraw();
	graphics_->SetViewPort(windowWidth_, windowHeight_);
	input_->Update();
	
	// ゲームシーン処理

	graphics_->PostDraw();

}

void MPEngine::Finalize() {
	graphics_->Finalize();
	winSv_->DeleteGameWindow();
}


#ifdef _DEBUG
MPEngine::D3DResourceLeakChecker MPEngine::debugLayer_;

MPEngine::D3DResourceLeakChecker::~D3DResourceLeakChecker() {
	//	リソースリークチェック
	Microsoft::WRL::ComPtr<IDXGIDebug1>debug;
	if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(debug.GetAddressOf()))))
	{
		debug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);
		debug->ReportLiveObjects(DXGI_DEBUG_APP, DXGI_DEBUG_RLO_ALL);
		debug->ReportLiveObjects(DXGI_DEBUG_D3D12, DXGI_DEBUG_RLO_ALL);
	};
}

void MPEngine::D3DResourceLeakChecker::EnableDebugLayer() {
	Microsoft::WRL::ComPtr<ID3D12Debug1> debugController = nullptr;
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
	{
		//	デバッグレイヤーを有効可する
		debugController->EnableDebugLayer();
		//	さらにGPU側でもチェックを行うようにする
		debugController->SetEnableGPUBasedValidation(true);
	}
}

void MPEngine::D3DResourceLeakChecker::ErrorStoped(ID3D12Device* device_) {
	Microsoft::WRL::ComPtr<ID3D12InfoQueue> infoQueue = nullptr;
	if (SUCCEEDED(device_->QueryInterface(IID_PPV_ARGS(infoQueue.GetAddressOf())))) {
		//	やばいエラー時に止まる
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
		//	エラー時に止まる
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
		//	警告時に止まる
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);

		//	抑制するメッセージのID
		D3D12_MESSAGE_ID denyIds[] = {
			//	Windows11でのDXGIデバッグレイヤーとDX12デバッグレイヤーの相互作用バグによるエラーメッセージ
			//	https://stackoverflow.com/questions/69805245/directx-12-application-is-crashing-in-windows-11
			D3D12_MESSAGE_ID_RESOURCE_BARRIER_MISMATCHING_COMMAND_LIST_TYPE
		};
		//	抑制するレベル
		D3D12_MESSAGE_SEVERITY severities[] = { D3D12_MESSAGE_SEVERITY_INFO };
		D3D12_INFO_QUEUE_FILTER filter{};
		filter.DenyList.NumIDs = _countof(denyIds);
		filter.DenyList.pIDList = denyIds;
		filter.DenyList.NumSeverities = _countof(severities);
		filter.DenyList.pSeverityList = severities;
		//	指定したメッセージの表示を抑制する
		infoQueue->PushStorageFilter(&filter);

		//	解放
		//infoQueue->Release();
	}
}

#endif