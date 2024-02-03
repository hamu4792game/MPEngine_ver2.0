#include "ModelRender.h"
#include <string>
#include "MPEngine/Base/Manager/ShaderManager/ShaderManager.h"
#include "MPEngine/Base/Manager/ListManager/ListManager.h"
#include "MPEngine/Graphics/Model/Model.h"

void ModelRender::Initialize() {
#pragma region Shader
	const std::string VSpath = "Object3D.VS.hlsl";
	const std::string PSpath = "Object3D.PS.hlsl";
	auto shaderInstance = ShaderManager::GetInstance();
	vertexShader = shaderInstance->CompileShader(VSpath, ShaderManager::ShaderType::Vertex);
	pixelShader = shaderInstance->CompileShader(PSpath, ShaderManager::ShaderType::Pixel);
#pragma endregion

#pragma region RootSignature
	D3D12_DESCRIPTOR_RANGE range[1] = {};
	range[0].BaseShaderRegister = 0;
	range[0].NumDescriptors = 1; // 必要な数
	range[0].RegisterSpace = 0;
	range[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	range[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	const uint8_t paramIndex = 4;
	D3D12_ROOT_PARAMETER rootParameter[paramIndex] = {};
	rootParameter[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameter[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	rootParameter[0].DescriptorTable.pDescriptorRanges = range;
	rootParameter[0].DescriptorTable.NumDescriptorRanges = _countof(range);

	// WVP
	rootParameter[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameter[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
	rootParameter[1].Descriptor.ShaderRegister = 0;

	// 色などのマテリアル系
	rootParameter[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameter[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameter[2].Descriptor.ShaderRegister = 1;

	// ライトの情報
	rootParameter[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameter[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameter[3].Descriptor.ShaderRegister = 2;

	rootSignature_ = std::make_unique<RootSignature>();
	rootSignature_->CreateRootSignature(rootParameter, paramIndex);
#pragma endregion

#pragma region GraphicsPipeline
	PipelineDesc plDesc{};

	// InputLayout
	const uint8_t inputIndex = 3;
	D3D12_INPUT_ELEMENT_DESC inputElementDesc[inputIndex] = {};
	inputElementDesc[0].SemanticName = "POSITION";
	inputElementDesc[0].SemanticIndex = 0;
	inputElementDesc[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDesc[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputElementDesc[1].SemanticName = "TEXCOORD";
	inputElementDesc[1].SemanticIndex = 0;
	inputElementDesc[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	inputElementDesc[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputElementDesc[2].SemanticName = "NORMAL";
	inputElementDesc[2].SemanticIndex = 0;
	inputElementDesc[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	inputElementDesc[2].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	D3D12_INPUT_LAYOUT_DESC layoutDesc{};
	layoutDesc.pInputElementDescs = inputElementDesc;
	layoutDesc.NumElements = _countof(inputElementDesc);
	plDesc.layoutDesc_ = layoutDesc;

	plDesc.signature_ = rootSignature_->GetRootSignature().Get();
	plDesc.vertexShader_ = vertexShader.Get();
	plDesc.pixelShader_ = pixelShader.Get();

	// Depthの機能を有効化する
	plDesc.depthStencilDesc_.DepthEnable = true;
	// 書き込みします
	plDesc.depthStencilDesc_.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	// 比較関数はLessEqual。つまり、近ければ描画される
	plDesc.depthStencilDesc_.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;

	for (uint8_t i = 0; i < static_cast<uint8_t>(BlendMode::BlendCount); i++) {
		graphicsPipeline_.at(i) = std::make_unique<GraphicsPipeline>();
		graphicsPipeline_.at(i)->CreatePipeline(plDesc, static_cast<BlendMode>(i));
	}

#pragma endregion

	// ライトの生成
	directionalLight_ = std::make_unique<DirectionalLight>();

}

void ModelRender::DrawCommand(const Matrix4x4& viewProjectionMat) {
	auto list = ListManager::GetInstance()->GetList();

	list->SetGraphicsRootSignature(rootSignature_->GetRootSignature().Get());
	list->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	directionalLight_->Update();

	auto& modelList = Model::modelLists_;

	for (auto model : modelList) {
		if (!model->isActive_) { continue; }

		// 定数バッファ用の計算
		model->cMat->world = model->transform_.UpdateMatrix();
		model->cMat->wvp = model->cMat->world * viewProjectionMat;
		model->cMaterial->color = model->color_;
		model->cMaterial->enableLighting = true;

		list->SetPipelineState(graphicsPipeline_[static_cast<uint32_t>(model->blendType_)]->GetPipelineState());
		list->IASetVertexBuffers(0, 1, &model->vertexBufferView_);
		auto rsManager = ResourceManager::GetInstance();
		list->SetGraphicsRootDescriptorTable(0, model->texture_->GetHandle().GetGPU()); // Texture
		list->SetGraphicsRootConstantBufferView(1, model->cMat.GetGPUVirtualAddress()); // cMat
		list->SetGraphicsRootConstantBufferView(2, model->cMaterial.GetGPUVirtualAddress()); // cMaterial
		list->SetGraphicsRootConstantBufferView(3, directionalLight_->cDirectionLight_.GetGPUVirtualAddress()); // cDirectinalLight

		// 描画
		list->DrawInstanced(UINT(model->model_->GetModel().vertices.size()), 1, 0, 0);
	}

}
