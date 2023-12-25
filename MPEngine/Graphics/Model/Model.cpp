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

void Model::SetModel(const std::shared_ptr<Object3d>& model) {
	model_ = model;
	texture_ = model->GetTexture();
	Initialize();
}

void Model::CreateVertexResource() {
	auto rsManager = ResourceManager::GetInstance();
	auto device = DeviceManager::GetInstance()->GetDevice();
	vertexResource_ = rsManager->CreateBufferResource(device, sizeof(VertexData) * model_->GetModel().vertices.size());

	vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
	vertexBufferView_.SizeInBytes = UINT(sizeof(VertexData) * model_->GetModel().vertices.size());
	vertexBufferView_.StrideInBytes = sizeof(VertexData);
}

void Model::UploadVertexData() {
	VertexData* mapData = nullptr;
	vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&mapData));
	auto a = model_->GetModel().vertices.size();
	for (auto i = 0; i < a; i++) {
		mapData[i] = model_->GetModel().vertices[i];
	}
	vertexResource_->Unmap(0, nullptr);
}
