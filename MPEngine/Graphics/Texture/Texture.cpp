#include "Texture.h"
#include "MPEngine/Base/Log.h"
#include "MPEngine/Base/Manager/ResourceManager/ResourceManager.h"
#include "MPEngine/Base/Manager/DeviceManager/DeviceManager.h"

Texture::~Texture() {
	if (resource_) {
		resource_->Release();
		resource_.Reset();
	}
}

void Texture::Load(const std::string& filePath) {
	auto rsManager = ResourceManager::GetInstance();
	auto device = DeviceManager::GetInstance()->GetDevice();

	DirectX::ScratchImage mipImages = rsManager->LoadTexture(filePath);
	const DirectX::TexMetadata& metaData = mipImages.GetMetadata();
	resource_ = rsManager->CreateTextureResource(device, metaData);
	rsManager->UploadTextureData(resource_.Get(), mipImages);

	// 画像サイズの代入
	textureSize_.x = static_cast<float>(metaData.width);
	textureSize_.y = static_cast<float>(metaData.height);

	srvHandle_.CreateView(rsManager->GetSRVHeap(), rsManager->GetCount());
	auto cpuHandle = srvHandle_.GetCPU();

	// 設定
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Format = metaData.format;
	srvDesc.Texture2D.MipLevels = static_cast<UINT>(metaData.mipLevels);
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	// ビューの生成
	device->CreateShaderResourceView(resource_.Get(), &srvDesc, /*rsManager->GetSRVHeap()->GetCPUDescriptorHandle(0)*/cpuHandle);

}
