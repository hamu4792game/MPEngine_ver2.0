#include "Object3d.h"
#include "MPEngine/Base/Manager/DeviceManager/DeviceManager.h"
#include "MPEngine/Graphics/Texture/Texture.h"
#include <filesystem>

Object3d::~Object3d() {
	for (auto& handle : vertexResource_) {
		if (handle) {
			handle->Release();
			handle.Reset();
		}
	}
	for (auto& handle : indexResource_) {
		if (handle) {
			handle->Release();
			handle.Reset();
		}
	}
}

void Object3d::Load(const std::string& name, const std::string& filePath) {
	name_ = name;
	auto rsManager = ResourceManager::GetInstance();
	// モデル読み込み
	modelDatas_ = rsManager->LoadModelFile(filePath);

	uint32_t index = 0u;
	texture_.resize(modelDatas_.size());
	for (auto& model : modelDatas_) {
		std::string handle = "white2x2";
		if (model.material.textureFilePath != "") {
			// textureのpathを取得
			std::string fileName = std::filesystem::path(model.material.textureFilePath).stem().string();
			handle = name_ + "_" + fileName;
			rsManager->AddTexture(handle, model.material.textureFilePath);
		}
		texture_.at(index++) = rsManager->FindTexture(handle);
	}
	Initialize();
}

void Object3d::Initialize() {
	vertexResource_.resize(modelDatas_.size());
	vertexBufferView_.resize(modelDatas_.size());
	indexResource_.resize(modelDatas_.size());
	indexBufferView_.resize(modelDatas_.size());

	CreateVertexResource();
	UploadVertexData();
}

void Object3d::CreateVertexResource() {
	auto rsManager = ResourceManager::GetInstance();
	auto device = DeviceManager::GetInstance()->GetDevice();
	for (uint32_t index = 0u; index < modelDatas_.size(); index++) {
		if (vertexResource_.at(index)) {
			vertexResource_.at(index)->Release();
			vertexResource_.at(index).Reset();
		}
		vertexResource_.at(index) = rsManager->CreateBufferResource(device, sizeof(VertexData) * modelDatas_.at(index).vertices.size());

		vertexBufferView_.at(index).BufferLocation = vertexResource_.at(index)->GetGPUVirtualAddress();
		vertexBufferView_.at(index).SizeInBytes = UINT(sizeof(VertexData) * modelDatas_.at(index).vertices.size());
		vertexBufferView_.at(index).StrideInBytes = sizeof(VertexData);

		// indexResourceの生成
		if (indexResource_.at(index)) {
			indexResource_.at(index)->Release();
			indexResource_.at(index).Reset();
		}
		indexResource_.at(index) = rsManager->CreateBufferResource(device, sizeof(uint32_t) * modelDatas_.at(index).indices.size());

		indexBufferView_.at(index).BufferLocation = indexResource_.at(index)->GetGPUVirtualAddress();
		indexBufferView_.at(index).SizeInBytes = UINT(sizeof(uint32_t) * modelDatas_.at(index).indices.size());
		indexBufferView_.at(index).Format = DXGI_FORMAT_R32_UINT;
	}
}

void Object3d::UploadVertexData() {
	for (uint32_t index = 0u; index < modelDatas_.size(); index++) {
		VertexData* mapData = nullptr;
		vertexResource_.at(index)->Map(0, nullptr, reinterpret_cast<void**>(&mapData));
		std::memcpy(mapData, modelDatas_.at(index).vertices.data(), sizeof(VertexData) * modelDatas_.at(index).vertices.size());
		vertexResource_.at(index)->Unmap(0, nullptr);

		uint32_t* mappedIndex = nullptr;
		indexResource_.at(index)->Map(0, nullptr, reinterpret_cast<void**>(&mappedIndex));
		std::memcpy(mappedIndex, modelDatas_.at(index).indices.data(), sizeof(uint32_t) * modelDatas_.at(index).indices.size());
		indexResource_.at(index)->Unmap(0, nullptr);
	}
}
