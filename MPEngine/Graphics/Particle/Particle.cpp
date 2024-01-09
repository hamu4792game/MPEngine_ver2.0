#include "Particle.h"
decltype(Particle::particleLists_) Particle::particleLists_;


Particle::Particle() {
	particleLists_.emplace_back(this);
}

Particle::~Particle() {
	particleLists_.remove(this);
	if (vertexResource_) {
		vertexResource_->Release();
		vertexResource_.Reset();
	}
	if (instancingResource_) {
		instancingResource_->Release();
		instancingResource_.Reset();
	}
}

void Particle::SetModel(const std::shared_ptr<Object3d>& model, const uint32_t& count) {
	model_ = model;
	texture_ = model->GetTexture();
	kNumInstance = count;
	transform_.resize(kNumInstance);
	color_.resize(kNumInstance);
	Initialize();
}

void Particle::Initialize() {
	CreateVertexResource();
	UploadVertexData();
	CreateInstancingResource();
	CreateInstancingSRV();
	UploadInstancingData(MakeIdentity4x4());
}

void Particle::CreateVertexResource() {
	auto rsManager = ResourceManager::GetInstance();
	auto device = DeviceManager::GetInstance()->GetDevice();
	vertexResource_ = rsManager->CreateBufferResource(device, sizeof(VertexData) * model_->GetModel().vertices.size());

	vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
	vertexBufferView_.SizeInBytes = UINT(sizeof(VertexData) * model_->GetModel().vertices.size());
	vertexBufferView_.StrideInBytes = sizeof(VertexData);
}

void Particle::CreateInstancingResource() {
	auto rsManager = ResourceManager::GetInstance();
	auto device = DeviceManager::GetInstance()->GetDevice();
	instancingResource_ = rsManager->CreateBufferResource(device, sizeof(InstancingStruct) * kNumInstance);
}

void Particle::CreateInstancingSRV() {
	auto rsManager = ResourceManager::GetInstance();
	auto device = DeviceManager::GetInstance()->GetDevice();

	// 設定
	D3D12_SHADER_RESOURCE_VIEW_DESC instancingSrvDesc{};
	instancingSrvDesc.Format = DXGI_FORMAT_UNKNOWN;
	instancingSrvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	instancingSrvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	instancingSrvDesc.Buffer.FirstElement = 0;
	instancingSrvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
	instancingSrvDesc.Buffer.NumElements = kNumInstance;
	instancingSrvDesc.Buffer.StructureByteStride = sizeof(InstancingStruct);

	// ビューの生成
	instancingSrvHandle_.CreateView(rsManager->GetSRVHeap(), rsManager->GetCount());
	auto cpuHandle = instancingSrvHandle_.GetCPU();
	device->CreateShaderResourceView(instancingResource_.Get(), &instancingSrvDesc, cpuHandle);
}

void Particle::UploadVertexData() {
	VertexData* mapData = nullptr;
	vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&mapData));
	auto a = model_->GetModel().vertices.size();
	for (auto i = 0; i < a; i++) {
		mapData[i] = model_->GetModel().vertices[i];
	}
	vertexResource_->Unmap(0, nullptr);
}

void Particle::UploadInstancingData(const Matrix4x4& viewProjectionMat, WorldTransform* transform, Vector4* color) {
	//	書き込むためのアドレスを取得
	InstancingStruct* instancingData = nullptr;
	instancingResource_->Map(0, nullptr, reinterpret_cast<void**>(&instancingData));
	//	念のため単位行列を書き込んでおく
	for (uint32_t i = 0u; i < kNumInstance; i++) {
		Matrix4x4 wMat = MakeIdentity4x4();
		if (transform) {
			wMat = transform[i].UpdateMatrix();
		}
		Matrix4x4 wvpMat = wMat * viewProjectionMat;
		instancingData[i].wvp = wvpMat;
		Vector4 lColor = Vector4::one;
		if (color) {
			lColor = color[i];
		}
		instancingData[i].color = lColor;
	}
	instancingResource_->Unmap(0, nullptr);
}
