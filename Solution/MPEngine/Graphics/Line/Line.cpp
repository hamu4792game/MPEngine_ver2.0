#include "Line.h"
decltype(Line::lineLists_) Line::lineLists_;

Line::Line() {
	lineLists_.emplace_back(this);
	Initialize();
}

Line::~Line() {
	lineLists_.remove(this);
	if (vertexResource_) {
		vertexResource_->Release();
		vertexResource_.Reset();
	}
}

void Line::SetLine(const Vector3& start, const Vector3& end) {
	start_ = start;
	end_ = end;
	UploadVertexData();
}

void Line::Initialize() {
	CreateVertexResource();
	UploadVertexData();
}

void Line::CreateVertexResource() {
	auto rsManager = ResourceManager::GetInstance();
	auto device = DeviceManager::GetInstance()->GetDevice();
	if (vertexResource_) {
		vertexResource_->Release();
		vertexResource_.Reset();
	}
	vertexResource_ = rsManager->CreateBufferResource(device, sizeof(VertexData) * 2);

	vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
	vertexBufferView_.SizeInBytes = UINT(sizeof(VertexData) * 2);
	vertexBufferView_.StrideInBytes = sizeof(VertexData);
}

void Line::UploadVertexData() {
	// 頂点データ
	VertexData vertex[2] = {
		{{start_.x,start_.y,start_.z,1.0f}},
		{{end_.x,end_.y,end_.z,1.0f}},
	};

	VertexData* mapData = nullptr;
	vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&mapData));
	for (auto i = 0; i < _countof(vertex); i++) {
		mapData[i] = vertex[i];
	}
	vertexResource_->Unmap(0, nullptr);
}
