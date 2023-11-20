#include "ListManager.h"
#include <cassert>
#include "MPEngine/Base/Manager/DeviceManager/DeviceManager.h"

ListManager* ListManager::instance = nullptr;

ListManager* ListManager::GetInstance() {
	return instance;
}

void ListManager::Initialize() {
	instance = new ListManager;
}

void ListManager::Finalize() {
	delete instance;
	instance = nullptr;
}

void ListManager::CreateList() {
	auto device = DeviceManager::GetInstance();

	//	コマンドアロケーターを生成する
	commandAllocator_ = nullptr;
	HRESULT hr = device->GetDevice()->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(commandAllocator_.GetAddressOf()));
	//	コマンドアロケーターの生成がうまくいかないので起動できない
	assert(SUCCEEDED(hr));

	//	コマンドリストを生成する
	commandList_ = nullptr;
	hr = device->GetDevice()->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator_.Get(), nullptr, IID_PPV_ARGS(commandList_.GetAddressOf()));
	//	コマンドリストの生成がうまくいかないので起動できない
	assert(SUCCEEDED(hr));
}

void ListManager::Reset() {
	HRESULT hr = commandAllocator_->Reset();
	assert(SUCCEEDED(hr));
	hr = commandList_->Reset(commandAllocator_.Get(), nullptr);
	assert(SUCCEEDED(hr));
}
