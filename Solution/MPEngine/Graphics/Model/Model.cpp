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
}

void Model::UploadVertexData() {
	VertexData* mapData = nullptr;
	vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&mapData));
	std::memcpy(mapData, model_->GetModel().vertices.data(), sizeof(VertexData) * model_->GetModel().vertices.size());
	vertexResource_->Unmap(0, nullptr);
}
