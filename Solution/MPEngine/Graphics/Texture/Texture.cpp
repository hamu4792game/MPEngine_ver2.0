#include "Texture.h"
#include "MPEngine/Base/Log.h"
#include "MPEngine/Base/Manager/ResourceManager/ResourceManager.h"
#include "MPEngine/Base/Manager/DeviceManager/DeviceManager.h"
#include "MPEngine/Base/Manager/ListManager/ListManager.h"

Texture::~Texture() {
	if (resource_) {
		resource_->Release();
		resource_.Reset();
	}
	if (val) {
		val->Release();
		val.Reset();
	}
}

void Texture::Load(const std::string& name, const std::string& filePath) {
	name_ = name;
	auto rsManager = ResourceManager::GetInstance();
	auto device = DeviceManager::GetInstance()->GetDevice();
	auto list = ListManager::GetInstance()->GetList();

	DirectX::ScratchImage mipImages = rsManager->LoadTexture(filePath);
	const DirectX::TexMetadata& metaData = mipImages.GetMetadata();
	resource_ = rsManager->CreateTextureResource(device, metaData);
	//rsManager->UploadTextureData(resource_.Get(), mipImages);
	val = rsManager->UploadTextureData(resource_.Get(), mipImages, device, list);

	// 画像サイズの代入
	textureSize_.x = static_cast<float>(metaData.width);
	textureSize_.y = static_cast<float>(metaData.height);

	srvHandle_.CreateView(rsManager->GetSRVHeap(), rsManager->GetCount());
	auto cpuHandle = srvHandle_.GetCPU();

	// 設定
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = metaData.format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	// metaDataからcubemapかどうかを取得できるので利用して分離
	if (metaData.IsCubemap()) {
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
		srvDesc.TextureCube.MostDetailedMip = 0; // unionがTextureCubeになったが、内部パラメータの意味はTexture2dと変わらない
		srvDesc.TextureCube.MipLevels = UINT_MAX;
		srvDesc.TextureCube.ResourceMinLODClamp = 0.0f;
	}
	else {

		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = static_cast<UINT>(metaData.mipLevels);
	}
	// ビューの生成
	device->CreateShaderResourceView(resource_.Get(), &srvDesc, /*rsManager->GetSRVHeap()->GetCPUDescriptorHandle(0)*/cpuHandle); 
}
