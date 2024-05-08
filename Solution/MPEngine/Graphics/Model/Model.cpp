#include "Model.h"
decltype(Model::modelLists_) Model::modelLists_;

Model::Model() {
	modelLists_.emplace_back(this);
}

Model::~Model() {
	modelLists_.remove(this);
	if (vertexResource_) {
		vertexResource_->Release();
		vertexResource_.Reset();
	}
	if (indexResource_) {
		indexResource_->Release();
		indexResource_.Reset();
	}
}

void Model::Initialize() {
	CreateVertexResource();
	UploadVertexData();
}

void Model::SetModel(Object3d* model) {
	model_ = model;
	texture_ = model->GetTexture();
	Initialize();
}

void Model::CreateVertexResource() {
	auto rsManager = ResourceManager::GetInstance();
	auto device = DeviceManager::GetInstance()->GetDevice();
	if (vertexResource_) {
		vertexResource_->Release();
		vertexResource_.Reset();
	}
	vertexResource_ = rsManager->CreateBufferResource(device, sizeof(VertexData) * model_->GetModel().vertices.size());

	vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
	vertexBufferView_.SizeInBytes = UINT(sizeof(VertexData) * model_->GetModel().vertices.size());
	vertexBufferView_.StrideInBytes = sizeof(VertexData);
	
	// indexResourceの生成
	if (indexResource_) {
		indexResource_->Release();
		indexResource_.Reset();
	}
	indexResource_ = rsManager->CreateBufferResource(device, sizeof(uint32_t) * model_->GetModel().indices.size());

	indexBufferView_.BufferLocation = indexResource_->GetGPUVirtualAddress();
	indexBufferView_.SizeInBytes = UINT(sizeof(uint32_t) * model_->GetModel().indices.size());
	indexBufferView_.Format = DXGI_FORMAT_R32_UINT;
}

void Model::UploadVertexData() {
	VertexData* mapData = nullptr;
	vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&mapData));
	std::memcpy(mapData, model_->GetModel().vertices.data(), sizeof(VertexData) * model_->GetModel().vertices.size());
	vertexResource_->Unmap(0, nullptr);

	uint32_t* mappedIndex = nullptr;
	indexResource_->Map(0, nullptr, reinterpret_cast<void**>(&mappedIndex));
	std::memcpy(mappedIndex, model_->GetModel().indices.data(), sizeof(uint32_t) * model_->GetModel().indices.size());
	indexResource_->Unmap(0, nullptr);
}
