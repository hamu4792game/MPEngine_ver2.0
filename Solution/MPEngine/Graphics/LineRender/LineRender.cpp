#include "LineRender.h"
#include <string>
#include "MPEngine/Base/Manager/ShaderManager/ShaderManager.h"
#include "MPEngine/Base/Manager/ListManager/ListManager.h"
#include "Graphics/Line/Line.h"
#include "MPEngine/Base/Manager/DeviceManager/DeviceManager.h"
#include "MPEngine/Base/Manager/ResourceManager/ResourceManager.h"

LineRender::~LineRender() {
	if (vertexResource_) {
		vertexResource_->Release();
		vertexResource_.Reset();
	}
	if (instancingResource_) {
		instancingResource_->Release();
		instancingResource_.Reset();
	}
}

void LineRender::Initialize() {
#pragma region Pipeline
#pragma region Shader
	const std::string VSpath = "Line.VS.hlsl";
	const std::string PSpath = "Line.PS.hlsl";
	auto shaderInstance = ShaderManager::GetInstance();
	vertexShader = shaderInstance->CompileShader(VSpath, ShaderManager::ShaderType::Vertex);
	pixelShader = shaderInstance->CompileShader(PSpath, ShaderManager::ShaderType::Pixel);
#pragma endregion

#pragma region RootSignature
	D3D12_DESCRIPTOR_RANGE insRange[1] = {};
	insRange[0].BaseShaderRegister = 0;
	insRange[0].NumDescriptors = 1;	//	必要な数
	insRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	insRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
	const uint8_t paramIndex = 1;
	D3D12_ROOT_PARAMETER rootParameter[paramIndex] = {};
	rootParameter[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameter[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	rootParameter[0].DescriptorTable.pDescriptorRanges = insRange;
	rootParameter[0].DescriptorTable.NumDescriptorRanges = _countof(insRange);

	rootSignature_ = std::make_unique<RootSignature>();
	rootSignature_->CreateRootSignature(rootParameter, paramIndex);
#pragma endregion

#pragma region GraphicsPipeline
	PipelineDesc plDesc{};

	// InputLayout
	D3D12_INPUT_ELEMENT_DESC inputElementDesc[2] = {};
	inputElementDesc[0].SemanticName = "POSITION";
	inputElementDesc[0].SemanticIndex = 0;
	inputElementDesc[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDesc[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputElementDesc[1].SemanticName = "COLOR";
	inputElementDesc[1].SemanticIndex = 0;
	inputElementDesc[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDesc[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	D3D12_INPUT_LAYOUT_DESC layoutDesc{};
	layoutDesc.pInputElementDescs = inputElementDesc;
	layoutDesc.NumElements = _countof(inputElementDesc);
	plDesc.layoutDesc_ = layoutDesc;

	plDesc.signature_ = rootSignature_->GetRootSignature().Get();
	plDesc.vertexShader_ = vertexShader.Get();
	plDesc.pixelShader_ = pixelShader.Get();

	plDesc.depthStencilDesc_.DepthEnable = true;
	plDesc.depthStencilDesc_.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	plDesc.depthStencilDesc_.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;

	plDesc.rasterizerDesc_.CullMode = D3D12_CULL_MODE_NONE;
	plDesc.rasterizerDesc_.FillMode = D3D12_FILL_MODE_WIREFRAME;
	plDesc.rasterizerDesc_.DepthClipEnable = true;

	plDesc.isLine_ = true;

	graphicsPipeline_ = std::make_unique<GraphicsPipeline>();
	graphicsPipeline_->CreatePipeline(plDesc, BlendMode::None);

#pragma endregion
#pragma endregion

	// リソースの生成
	CreateVertexResource();
	CreateInstancing();
}

void LineRender::DrawCommand(const Matrix4x4& viewProjectionMat) {
	auto list = ListManager::GetInstance()->GetList();
	auto& lineList = Line::lineLists_;
	// ラインリストが空じゃなければ
	if (!lineList.empty()) {
		list->SetGraphicsRootSignature(rootSignature_->GetRootSignature().Get());
		list->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);

		std::list<Line*> activeList;
		// lineListからactiveではないlineを除外
		for (auto line : lineList) {
			if (!line->isActive_) { continue; } // 早期リターン
			activeList.push_back(line);
		}

		UINT verticesSize = UploadVertexData(activeList);
		UploadInstancingData(activeList, viewProjectionMat);

		list->SetPipelineState(graphicsPipeline_->GetPipelineState());
		list->IASetVertexBuffers(0, 1, &vertexBufferView_);
		auto rsManager = ResourceManager::GetInstance();
		list->SetGraphicsRootDescriptorTable(0, instancingSrvHandle_.GetGPU()); // wvp・color

		// 描画
		list->DrawInstanced(2u, verticesSize, 0u, 0u);
	}
}

void LineRender::CreateVertexResource() {
	auto rsManager = ResourceManager::GetInstance();
	auto device = DeviceManager::GetInstance()->GetDevice();
	if (vertexResource_) {
		vertexResource_->Release();
		vertexResource_.Reset();
	}
	vertexResource_ = rsManager->CreateBufferResource(device, sizeof(Line::LineVertex) * kMaxNumInstance_);

	vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
	vertexBufferView_.SizeInBytes = UINT(sizeof(Line::LineVertex) * kMaxNumInstance_);
	vertexBufferView_.StrideInBytes = sizeof(Line::LineVertex);
}

UINT LineRender::UploadVertexData(std::list<Line*> lineList) {
	//auto& lineList = Line::lineLists_;
	std::vector<Line::LineVertex> vertices;
	// ライン頂点データを収集
	for (auto line : lineList) {
		if (!line->isActive_) { continue; } // 早期リターン
		vertices.push_back({ line->pos_.start, line->pos_.end });
	}
	const uint32_t vertexBufferSize = static_cast<uint32_t>(sizeof(Line::LineVertex) * vertices.size());
	// 送る
	Line::LineVertex* mapData = nullptr;
	vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&mapData));
	std::memcpy(mapData, vertices.data(), vertexBufferSize);
	vertexResource_->Unmap(0, nullptr);
	return static_cast<UINT>(vertices.size());
}

void LineRender::CreateInstancing() {
	auto rsManager = ResourceManager::GetInstance();
	auto device = DeviceManager::GetInstance()->GetDevice();
	// resourceの生成
	instancingResource_ = rsManager->CreateBufferResource(device, sizeof(InstancingStruct) * kMaxNumInstance_);
	
	// srvの生成
	// 設定
	D3D12_SHADER_RESOURCE_VIEW_DESC instancingSrvDesc{};
	instancingSrvDesc.Format = DXGI_FORMAT_UNKNOWN;
	instancingSrvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	instancingSrvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	instancingSrvDesc.Buffer.FirstElement = 0;
	instancingSrvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
	instancingSrvDesc.Buffer.NumElements = kMaxNumInstance_;
	instancingSrvDesc.Buffer.StructureByteStride = sizeof(kMaxNumInstance_);

	// ビューの生成
	instancingSrvHandle_.CreateView(rsManager->GetSRVHeap(), rsManager->GetCount());
	auto cpuHandle = instancingSrvHandle_.GetCPU();
	device->CreateShaderResourceView(instancingResource_.Get(), &instancingSrvDesc, cpuHandle);
}

void LineRender::UploadInstancingData(std::list<Line*> lineList, const Matrix4x4& viewProjectionMat) {
	//	書き込むためのアドレスを取得
	InstancingStruct* instancingData = nullptr;
	instancingResource_->Map(0, nullptr, reinterpret_cast<void**>(&instancingData));
	//	念のため単位行列を書き込んでおく
	uint32_t index = 0u;
	for (auto& line : lineList) {

		float scale = Length(line->pos_.end - line->pos_.start);
		Quaternion rotate = Quaternion::MakeFromTwoVector(Vector3(1.0f, 0.0f, 0.0f), (line->pos_.end - line->pos_.start).Normalize());
		Vector3 translate = line->pos_.start;
		Matrix4x4 affine = MakeAffineMatrix(Vector3(scale, 1.0f, 1.0f), rotate, translate);

		instancingData[index].wvp = affine * viewProjectionMat;
		instancingData[index].color = line->color_;
		index++;
	}
	instancingResource_->Unmap(0, nullptr);
}
