#include "CommandDirectX.h"
#include <cassert>

//	imguiのinclude
#include "externals/imgui/imgui.h"
#include "externals/imgui/imgui_impl_dx12.h"
#include "externals/imgui/imgui_impl_win32.h"
#include "MPEngine/Base/Log.h"

#include "MPEngine/Base/Manager/DeviceManager/DeviceManager.h"
#include "MPEngine/Base/Manager/ListManager/ListManager.h"
#include "MPEngine/Base/DetailSetting/SwapChain/SwapChain.h"
#include "MPEngine/Base/WinApp/WinApp.h"
#include "MPEngine/Base/Manager/ResourceManager/ResourceManager.h"

CommandDirectX* CommandDirectX::GetInstance() {
	static CommandDirectX instance;
	return &instance;
}

void CommandDirectX::Initialize(unsigned int bufferWidth, unsigned int bufferHeight) {
	
	device_ = DeviceManager::GetInstance();
	ListManager::Initialize();
	commandList_ = ListManager::GetInstance();
	swapChain_ = std::make_unique<SwapChain>();
	//rsManager_ = ResourceManager::GetInstance();
	
	CreateFactry();
	SelectAdapter();
	device_->CreateDevice(useAdapter_.Get());
	CreateCommandQueue();
	commandList_->CreateList();
	swapChain_->CreateSwapChain(dxgiFactory_.Get(), commandQueue_.Get());
	//rsManager_->Initialize();

	//ImGuiInitialize();

}

void CommandDirectX::PreDraw() {

}

void CommandDirectX::PostDraw() {

}

void CommandDirectX::Finalize() {
	////	ImGuiの解放
	//ImGui_ImplDX12_Shutdown();
	//ImGui_ImplWin32_Shutdown();
	//ImGui::DestroyContext();

	//rsManager_->Finalize();
	//rsManager_ = nullptr;
	//delete rsManager_;
	swapChain_->Finalize();
	ListManager::Finalize();
	commandList_ = nullptr;
	device_ = nullptr;
}

void CommandDirectX::CreateFactry() {
	//	DXGIファクトリーの生成
	dxgiFactory_ = nullptr;
	//	HRESULTはWindows系のエラーコードであり、
	//	関数が成功したかどうかをSUCCEEDEDマクロで判定できる
	HRESULT hr = CreateDXGIFactory(IID_PPV_ARGS(dxgiFactory_.GetAddressOf()));
	//	初期化の根本的な部分でエラーが出た場合はプログラムが間違っているか、
	//	どうにもできない場合が多いのでassertにしておく
	assert(SUCCEEDED(hr));
}

void CommandDirectX::SelectAdapter() {
	//	使用するアダプタ用の変数。最初にnullptrを入れておく
	useAdapter_ = nullptr;
	//	良い順にアダプタを頼む
	for (UINT i = 0; dxgiFactory_->EnumAdapterByGpuPreference(i, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(useAdapter_.GetAddressOf()))
		!= DXGI_ERROR_NOT_FOUND; ++i)
	{
		//	アダプタの情報を取得する
		DXGI_ADAPTER_DESC3 adapterDesc{};
		HRESULT hr = useAdapter_->GetDesc3(&adapterDesc);
		assert(SUCCEEDED(hr));	//取得できないのは一大事
		//	ソフトウェアアダプタでなければ採用
		if (!(adapterDesc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE))
		{
			//	採用したアダプタの情報をログに出力。wstringの方なので注意
			Log(ConvertString(std::format(L"Use Adapter:{}\n", adapterDesc.Description)));
			break;
		}
		//	ソフトウェアアダプタの場合は見なかったことにする
		useAdapter_ = nullptr;
	}
	//	適切なアダプタが見つからなかったので起動できない
	assert(useAdapter_ != nullptr);
}

void CommandDirectX::CreateCommandQueue() {
	//	コマンドキューを生成する
	commandQueue_ = nullptr;
	D3D12_COMMAND_QUEUE_DESC commandQueueDesc{};
	HRESULT hr = device_->GetDevice()->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(commandQueue_.GetAddressOf()));
	//	コマンドキューの生成がうまくいかないので起動できない
	assert(SUCCEEDED(hr));
}

void CommandDirectX::ImGuiInitialize() {
	//auto winApp = WinApp::GetInstance();
	////	swapChaineのbufferCountの取得
	//DXGI_SWAP_CHAIN_DESC1 SCD;
	////swapChain_->GetSwapChain()->GetDesc1(&SCD);
	//auto srvHeap = ResourceManager::GetInstance()->GetSRVHeap();

	////	ImGuiの初期化
	//IMGUI_CHECKVERSION();
	//ImGui::CreateContext();
	//ImGui::StyleColorsDark();
	//ImGui_ImplWin32_Init(winApp->GetHwnd());
	//ImGui_ImplDX12_Init(device_->GetDevice(),
	//	SCD.BufferCount,
	//	DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
	//	srvHeap,
	//	srvHeap->GetCPUDescriptorHandleForHeapStart(),
	//	srvHeap->GetGPUDescriptorHandleForHeapStart()
	//);
}
