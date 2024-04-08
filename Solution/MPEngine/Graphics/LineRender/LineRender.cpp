#include "LineRender.h"
#include <string>
#include "MPEngine/Base/Manager/ShaderManager/ShaderManager.h"
#include "MPEngine/Base/Manager/ListManager/ListManager.h"
#include "Graphics/Line/Line.h"

void LineRender::Initialize() {
#pragma region Shader
	const std::string VSpath = "Line.VS.hlsl";
	const std::string PSpath = "Line.PS.hlsl";
	auto shaderInstance = ShaderManager::GetInstance();
	vertexShader = shaderInstance->CompileShader(VSpath, ShaderManager::ShaderType::Vertex);
	pixelShader = shaderInstance->CompileShader(PSpath, ShaderManager::ShaderType::Pixel);
#pragma endregion

#pragma region RootSignature
	const uint8_t paramIndex = 2;
	D3D12_ROOT_PARAMETER rootParameter[paramIndex] = {};
	rootParameter[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameter[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
	rootParameter[0].Descriptor.ShaderRegister = 0;
	rootParameter[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameter[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameter[1].Descriptor.ShaderRegister = 1;

	rootSignature_ = std::make_unique<RootSignature>();
	rootSignature_->CreateRootSignature(rootParameter, paramIndex);
#pragma endregion

#pragma region GraphicsPipeline
	PipelineDesc plDesc{};

	// InputLayout
	D3D12_INPUT_ELEMENT_DESC inputElementDesc[1] = {};
	inputElementDesc[0].SemanticName = "POSITION";
	inputElementDesc[0].SemanticIndex = 0;
	inputElementDesc[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDesc[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
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
	graphicsPipeline_->CreatePipeline(plDesc, BlendMode::Normal);

#pragma endregion
}

void LineRender::DrawCommand(const Matrix4x4& viewProjectionMat) {
	auto list = ListManager::GetInstance()->GetList();

	list->SetGraphicsRootSignature(rootSignature_->GetRootSignature().Get());
	list->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);

	auto& lineList = Line::lineLists_;

	for (auto line : lineList) {
		if (!line->isActive_) { continue; }

		// 定数バッファ用の計算
		line->UploadVertexData();
		line->cMat->wvp = viewProjectionMat;
		line->cMaterial->color = line->color_;


		list->SetPipelineState(graphicsPipeline_->GetPipelineState());
		list->IASetVertexBuffers(0, 1, &line->vertexBufferView_);
		auto rsManager = ResourceManager::GetInstance();
		list->SetGraphicsRootConstantBufferView(0, line->cMat.GetGPUVirtualAddress()); // cMat
		list->SetGraphicsRootConstantBufferView(1, line->cMaterial.GetGPUVirtualAddress()); // cMaterial

		// 描画
		list->DrawInstanced(2u, 1u, 0u, 0u);
	}
}
