#include "CommandDirectX.h"
#include <cassert>

//	imguiのinclude
#include "externals/imgui/imgui.h"
#include "externals/imgui/imgui_impl_dx12.h"
#include "externals/imgui/imgui_impl_win32.h"
#include "MPEngine/Base/Log.h"

CommandDirectX* CommandDirectX::GetInstance() {
	static CommandDirectX instance;
	return &instance;
}

void CommandDirectX::Initialize(const WinApp* winApp, unsigned int bufferWidth, unsigned int bufferHeight) {
	winApp_ = winApp;

	CreateFactry();
	SelectAdapter();
	CreateDevice();
	CreateCommandQueue();
	CreateCommandList();

}

void CommandDirectX::PreDraw() {

}

void CommandDirectX::PostDraw() {

}

void CommandDirectX::Finalize() {

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

void CommandDirectX::CreateDevice() {
	device_ = nullptr;
	//	機能レベルとログ出力用の文字列
	D3D_FEATURE_LEVEL featureLevels[] = {
		D3D_FEATURE_LEVEL_12_2,D3D_FEATURE_LEVEL_12_1,D3D_FEATURE_LEVEL_12_0
	};
	const char* featureLevelStrings[] = { "12.2","12.1","12.0" };
	//	高い順に生成できるか試していく
	for (size_t i = 0; i < _countof(featureLevels); ++i)
	{
		//	採用したアダプタでデバイスを生成
		HRESULT hr = D3D12CreateDevice(useAdapter_.Get(), featureLevels[i], IID_PPV_ARGS(device_.GetAddressOf()));
		//	指定した機能レベルでデバイスが生成できたかを確認
		if (SUCCEEDED(hr))
		{
			//	生成できたのでログ出力を行ってループを抜ける
			Log(std::format("FeatureLevel : {}\n", featureLevelStrings[i]));
			break;
		}
	}
	//	デバイスの生成がうまくいかなかったので機能できない
	assert(device_ != nullptr);
	Log("Complete create D3D12Device!!!\n");	//初期化完了のログを出す
}

void CommandDirectX::CreateCommandQueue() {
	//	コマンドキューを生成する
	commandQueue_ = nullptr;
	D3D12_COMMAND_QUEUE_DESC commandQueueDesc{};
	HRESULT hr = device_->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(commandQueue_.GetAddressOf()));
	//	コマンドキューの生成がうまくいかないので起動できない
	assert(SUCCEEDED(hr));
}

void CommandDirectX::CreateCommandList() {
	//	コマンドアロケーターを生成する
	commandAllocator_ = nullptr;
	HRESULT hr = device_->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(commandAllocator_.GetAddressOf()));
	//	コマンドアロケーターの生成がうまくいかないので起動できない
	assert(SUCCEEDED(hr));

	//	コマンドリストを生成する
	commandList_ = nullptr;
	hr = device_->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator_.Get(), nullptr, IID_PPV_ARGS(commandList_.GetAddressOf()));
	//	コマンドリストの生成がうまくいかないので起動できない
	assert(SUCCEEDED(hr));
}
