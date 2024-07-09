#include "GraphicsManager.h"
#include <cassert>

#include "MPEngine/Base/Log.h"
#include "MPEngine/Base/Manager/DeviceManager/DeviceManager.h"
#include "MPEngine/Base/Manager/ListManager/ListManager.h"
#include "MPEngine/Base/DetailSetting/SwapChain/SwapChain.h"
#include "MPEngine/Base/WindowSupervisor/WindowSupervisor.h"
#include "MPEngine/Base/Manager/ResourceManager/ResourceManager.h"
#include "MPEngine/Base/DetailSetting/DepthBuffer/DepthBuffer.h"
#include "MPEngine/Base/Manager/ImGuiManager/ImGuiManager.h"
#include "MPEngine/Base/DetailSetting/RenderTarget/RenderTarget.h"

GraphicsManager* GraphicsManager::GetInstance() {
	static GraphicsManager instance;
	return &instance;
}

void GraphicsManager::Initialize(unsigned int bufferWidth, unsigned int bufferHeight) {

#pragma region DirectXの初期化
	device_ = DeviceManager::GetInstance();
	commandList_ = ListManager::GetInstance();
	swapChain_ = std::make_unique<SwapChain>();
	rsManager_ = ResourceManager::GetInstance();
	depthBuffer_ = std::make_unique<DepthBuffer>();
	imguiManager_ = ImGuiManager::GetInstance();
	renderTarget_ = std::make_unique<RenderTarget>();

	CreateFactry();
	SelectAdapter();
	device_->CreateDevice(useAdapter_.Get());
	CreateCommandQueue();
	commandList_->CreateList();
	swapChain_->CreateSwapChain(dxgiFactory_.Get(), commandQueue_.Get());
	rsManager_->Initialize();
	depthBuffer_->Initialize(bufferWidth, bufferHeight);
	renderTarget_->CreateRenderTexture(device_, swapChain_.get(), rsManager_);
	CreateFence();
#pragma endregion

#ifdef _DEBUG
	ImGuiInitialize();
#endif // ImGuiの初期化


}

void GraphicsManager::PreDraw() {
#ifdef _DEBUG
	
	imguiManager_->Begin();

	// ImGuiデモの表示
	//ImGui::ShowDemoWindow();
#endif // _DEBUG

	auto index = renderTarget_->GetHandle();
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = swapChain_->GetRTVHeap()->GetCPUDescriptorHandle(index);
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = depthBuffer_->GetDSVDesc()->GetCPUDescriptorHandle(0);
	// 描画先のRTVとDSVを設定する
	commandList_->GetList()->OMSetRenderTargets(1, &rtvHandle, false, &dsvHandle);
	//	画面クリア
	renderTarget_->ClearRenderTarget(commandList_->GetList(), rtvHandle);
	//	指定した深度で画面全体をクリアする
	commandList_->GetList()->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

	ID3D12DescriptorHeap* descriptorHeap[] = { rsManager_->GetSRVHeap()->GetDescriptorHeap() };
	commandList_->GetList()->SetDescriptorHeaps(_countof(descriptorHeap), descriptorHeap);
}

void GraphicsManager::PostDrawProcess() {
	CreateBarrier(renderTarget_->GetResource(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

	CreateBarrier(swapChain_->GetBackBuffer(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);

	// SwapChainに対して書き込む
	auto index = swapChain_->GetSwapChain()->GetCurrentBackBufferIndex();
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = swapChain_->GetRTVHeap()->GetCPUDescriptorHandle(index);
	// 描画先のRTVを設定する
	commandList_->GetList()->OMSetRenderTargets(1, &rtvHandle, false, nullptr);
	// 画面クリア
	//renderTarget_->ClearRenderTarget(commandList_->GetList(), rtvHandle);
}

void GraphicsManager::PostDraw() {
	
	renderTarget_->DrawCommand(commandList_->GetList());

#ifdef _DEBUG
	imguiManager_->End();
	
	imguiManager_->Draw(commandList_->GetList());
#endif // DEBUG
	CreateBarrier(swapChain_->GetBackBuffer(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);

	CreateBarrier(renderTarget_->GetResource(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET);

	EndProcess();
}

void GraphicsManager::Finalize() {
	// ImGuiの解放
	ImGuiFinalize();

	depthBuffer_->Finalize();
	rsManager_->Finalize();
	swapChain_->Finalize();
}

void GraphicsManager::SetViewPort(uint32_t width, uint32_t height) {
	// ビューポート
	D3D12_VIEWPORT viewport{};
	// クライアント領域のサイズと一緒にして画面全体に表示
	viewport.Width = static_cast<FLOAT>(width);
	viewport.Height = static_cast<FLOAT>(height);
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	// Viewportを設定
	commandList_->GetList()->RSSetViewports(1, &viewport);

	// シザー矩形
	D3D12_RECT scissorRect{};
	// 基本的にビューポートと同じ矩形が構成されるようにする
	scissorRect.left = 0;
	scissorRect.right = width;
	scissorRect.top = 0;
	scissorRect.bottom = height;
	// Scirssorを設定
	commandList_->GetList()->RSSetScissorRects(1, &scissorRect);
}

void GraphicsManager::CreateFactry() {
	// DXGIファクトリーの生成
	dxgiFactory_ = nullptr;
	// HRESULTはWindows系のエラーコードであり、
	// 関数が成功したかどうかをSUCCEEDEDマクロで判定できる
	HRESULT hr = CreateDXGIFactory(IID_PPV_ARGS(dxgiFactory_.GetAddressOf()));
	// 初期化の根本的な部分でエラーが出た場合はプログラムが間違っているか、
	// どうにもできない場合が多いのでassertにしておく
	assert(SUCCEEDED(hr));
}

void GraphicsManager::SelectAdapter() {
	// 使用するアダプタ用の変数。最初にnullptrを入れておく
	useAdapter_ = nullptr;
	// 良い順にアダプタを頼む
	for (UINT i = 0; dxgiFactory_->EnumAdapterByGpuPreference(i, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(useAdapter_.GetAddressOf()))
		!= DXGI_ERROR_NOT_FOUND; ++i)
	{
		// アダプタの情報を取得する
		DXGI_ADAPTER_DESC3 adapterDesc{};
		HRESULT hr = useAdapter_->GetDesc3(&adapterDesc);
		assert(SUCCEEDED(hr));	//取得できないのは一大事
		// ソフトウェアアダプタでなければ採用
		if (!(adapterDesc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE))
		{
			//	採用したアダプタの情報をログに出力。wstringの方なので注意
			Log(ConvertString(std::format(L"Use Adapter:{}\n", adapterDesc.Description)));
			break;
		}
		// ソフトウェアアダプタの場合は見なかったことにする
		useAdapter_ = nullptr;
	}
	// 適切なアダプタが見つからなかったので起動できない
	assert(useAdapter_ != nullptr);
}

void GraphicsManager::CreateCommandQueue() {
	// コマンドキューを生成する
	commandQueue_ = nullptr;
	D3D12_COMMAND_QUEUE_DESC commandQueueDesc{};
	HRESULT hr = device_->GetDevice()->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(commandQueue_.GetAddressOf()));
	// コマンドキューの生成がうまくいかないので起動できない
	assert(SUCCEEDED(hr));
}

void GraphicsManager::CreateFence() {
	// 初期値0でFenceを作る
	fence_ = nullptr;
	fenceValue_ = 0u;
	HRESULT hr = device_->GetDevice()->CreateFence(fenceValue_, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(fence_.GetAddressOf()));
	assert(SUCCEEDED(hr));

	// FenceのSignalを持つためのイベントを作成する
	fenceEvent_ = nullptr;
	fenceEvent_ = CreateEvent(NULL, FALSE, FALSE, NULL);
	assert(fenceEvent_ != nullptr);
}

void GraphicsManager::ImGuiInitialize() {
#ifdef _DEBUG
	// swapChaineのbufferCountの取得
	DXGI_SWAP_CHAIN_DESC1 SCD;
	swapChain_->GetSwapChain()->GetDesc1(&SCD);
	
	// ImGuiの初期化
	imguiManager_->Initialize(SCD.BufferCount);

#endif // _DEBUG
}

void GraphicsManager::ImGuiFinalize() {
#ifdef _DEBUG
	imguiManager_->Finalize();
#endif // _DEBUG
}

void GraphicsManager::EndProcess() {

	// コマンドリストの内容を確定させる。すべてのコマンドを積んでからCloseする
	HRESULT hr = commandList_->GetList()->Close();
	assert(SUCCEEDED(hr));

	// GPUにコマンドリストの実行を行わせる
	ID3D12CommandList* commandLists[] = { commandList_->GetList() };
	commandQueue_->ExecuteCommandLists(1, commandLists);
	// GPUとOSに画面の交換を行うよう通知する
	swapChain_->Present();

	// Fenceの値を更新
	fenceValue_++;
	// GPUがここまでたどり着いた時に、Fenceの値を指定した値に代入するようにSignalを送る
	commandQueue_->Signal(fence_.Get(), fenceValue_);
	// Fenceの値が指定したSignal値にたどり着いているか確認する
	// GetCompletedValueの初期値はFence作成時に渡した初期値
	if (fence_->GetCompletedValue() < fenceValue_) {
		// 指定したSignalにたどり着いていないので、たどり着くまで待つようにイベントを設定する
		fence_->SetEventOnCompletion(fenceValue_, fenceEvent_);
		// イベント待つ
		WaitForSingleObject(fenceEvent_, INFINITE);
	}

	// 次のフレーム用のコマンドリストを用意
	commandList_->Reset();

}

void GraphicsManager::CreateBarrier(ID3D12Resource* resource, D3D12_RESOURCE_STATES stateBefore, D3D12_RESOURCE_STATES stateAfter,
	D3D12_RESOURCE_BARRIER_TYPE type, D3D12_RESOURCE_BARRIER_FLAGS flag) {
	D3D12_RESOURCE_BARRIER barrier{};
	barrier.Type = type;
	barrier.Flags = flag;
	barrier.Transition.pResource = resource;
	barrier.Transition.StateBefore = stateBefore;
	barrier.Transition.StateAfter = stateAfter;
	// TransitionBarrierを張る
	ListManager::GetInstance()->GetList()->ResourceBarrier(1, &barrier);
}
