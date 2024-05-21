#include "SkyBoxRender.h"
#include "MPEngine/Base/DetailSetting/GraphicsPipeline/GraphicsPipeline.h"
#include "MPEngine/Base/Manager/ListManager/ListManager.h"
#include "MPEngine/Base/Manager/ShaderManager/ShaderManager.h"
#include "MPEngine/Graphics/SkyBox/SkyBox.h"
#include <algorithm>
#include <string>
#include "MPEngine/Base/Manager/ResourceManager/ResourceManager.h"


void SkyBoxRender::Initialize() {
#pragma region Shader
	const std::string VSpath = "Skybox.VS.hlsl";
	const std::string PSpath = "Skybox.PS.hlsl";
	auto shaderInstance = ShaderManager::GetInstance();
	vertexShader = shaderInstance->CompileShader(VSpath, ShaderManager::ShaderType::Vertex);
	pixelShader = shaderInstance->CompileShader(PSpath, ShaderManager::ShaderType::Pixel);
#pragma endregion

#pragma region RootSignature
	D3D12_DESCRIPTOR_RANGE range[1] = {};
	range[0].BaseShaderRegister = 0;
	range[0].NumDescriptors = 1;	//	必要な数
	range[0].RegisterSpace = 0;
	range[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	range[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
	const uint8_t paramIndex = 3;
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
	rootSignature_ = std::make_unique<RootSignature>();
	rootSignature_->CreateRootSignature(rootParameter, paramIndex);
#pragma endregion

#pragma region GraphicsPipeline
	PipelineDesc plDesc;

	// InputLayout
	D3D12_INPUT_ELEMENT_DESC inputElementDesc[2] = {};
	inputElementDesc[0].SemanticName = "POSITION";
	inputElementDesc[0].SemanticIndex = 0;
	inputElementDesc[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDesc[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputElementDesc[1].SemanticName = "TEXCOORD";
	inputElementDesc[1].SemanticIndex = 0;
	inputElementDesc[1].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	inputElementDesc[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	D3D12_INPUT_LAYOUT_DESC layoutDesc{};
	layoutDesc.pInputElementDescs = inputElementDesc;
	layoutDesc.NumElements = _countof(inputElementDesc);
	plDesc.layoutDesc_ = layoutDesc;

	plDesc.signature_ = rootSignature_->GetRootSignature().Get();
	plDesc.vertexShader_ = vertexShader.Get();
	plDesc.pixelShader_ = pixelShader.Get();

	plDesc.depthStencilDesc_.DepthEnable = true; // 比較するので有効化
	plDesc.depthStencilDesc_.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO; // 全ピクセルがz=1に出力されるので、わざわざ書き込む必要がない
	plDesc.depthStencilDesc_.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL; // 今までと同様に比較
	// 裏面を表示しない
	plDesc.rasterizerDesc_.CullMode = D3D12_CULL_MODE_BACK;
	// 塗りつぶす
	plDesc.rasterizerDesc_.FillMode = D3D12_FILL_MODE_SOLID;
	plDesc.rasterizerDesc_.DepthClipEnable = true;

	graphicsPipeline_ = std::make_unique<GraphicsPipeline>();
	graphicsPipeline_->CreatePipeline(plDesc, BlendMode::None);

#pragma endregion

}

void SkyBoxRender::DrawCommand(const Matrix4x4& viewProjectionMat) {
	auto list = ListManager::GetInstance()->GetList();

	list->SetGraphicsRootSignature(rootSignature_->GetRootSignature().Get());
	list->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	
	auto& SkyBoxList = SkyBox::skyBoxLists_;
	// 描画順並べる
	SkyBoxList.sort([](SkyBox* a, SkyBox* b) { return a->GetLayerNum() < b->GetLayerNum(); });
	
	for (auto skybox : SkyBoxList) {
		if (!skybox->isActive_) { continue; }

		// 定数バッファ用の計算
		skybox->cMat->wvp = MakeAffineMatrix(
			{ skybox->scale_.x,skybox->scale_.y,skybox->scale_.z },
			{ 0.0f,0.0f,skybox->rotate_ },
			{ skybox->translate_.x,skybox->translate_.y,skybox->translate_.z }
		) * viewProjectionMat;
		skybox->cMaterial->uvMat = MakeAffineMatrix(
			{ skybox->uvScale_.x,skybox->uvScale_.y,1.0f },
			{ 0.0f,0.0f,skybox->uvRotate_ },
			{ skybox->uvTranslate_.x,skybox->uvTranslate_.y,0.5f }
		);
		//SkyBox->cMaterial->uvMat = Matrix3x3::MakeAffineMatrix(SkyBox->uvScale_, SkyBox->uvRotate_, SkyBox->uvTranslate_);
		skybox->cMaterial->color = skybox->color_;

		//auto pipeline = GraphicsPipeline::GetInstance()->GetSkyBoxPipelineState(SkyBox->blendType_);
		list->SetPipelineState(graphicsPipeline_->GetPipelineState());
		list->IASetVertexBuffers(0, 1, &skybox->vertexBufferView_);
		list->IASetIndexBuffer(&skybox->indexBufferView_);
		auto rsManager = ResourceManager::GetInstance();
		list->SetGraphicsRootDescriptorTable(0, skybox->texture_->GetHandle().GetGPU()); // Texture
		list->SetGraphicsRootConstantBufferView(1, skybox->cMat.GetGPUVirtualAddress()); // cMat
		list->SetGraphicsRootConstantBufferView(2, skybox->cMaterial.GetGPUVirtualAddress()); // cMaterial

		// 描画
		list->DrawIndexedInstanced(36, 1, 0, 0, 0);
	}
	
}
