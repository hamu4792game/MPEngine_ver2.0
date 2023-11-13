#include "MPEngine.h"

MPEngine* MPEngine::GetInstance() {
	static MPEngine instance;
	return &instance;
}

void MPEngine::Initialize(const char* title, int width, int height) {
	windowWidth_ = width;
	windowHeight_ = height;

#ifdef _DEBUG
	debugLayer_.EnableDebugLayer();
#endif

#ifdef _DEBUG
	//debugLayer_.ErrorStoped();
#endif


}

#ifdef _DEBUG
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
	if (SUCCEEDED(device_->QueryInterface(IID_PPV_ARGS(&infoQueue))))
	{
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
		infoQueue->Release();
	}
}

#endif