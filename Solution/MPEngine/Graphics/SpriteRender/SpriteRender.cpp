#include "MPEngine/Base/DetailSetting/GraphicsPipeline/GraphicsPipeline.h"
#include "MPEngine/Base/Manager/ListManager/ListManager.h"
#include "MPEngine/Base/Manager/ShaderManager/ShaderManager.h"
#include "MPEngine/Graphics/Sprite/Sprite.h"
#include "SpriteRender.h"
#include <algorithm>
#include <string>
#include "MPEngine/Base/Manager/ResourceManager/ResourceManager.h"


void SpriteRender::Initialize() {
#pragma region Shader
	const std::string VSpath = "Texture2D.VS.hlsl";
	const std::string PSpath = "Texture2D.PS.hlsl";
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
	inputElementDesc[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	inputElementDesc[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	D3D12_INPUT_LAYOUT_DESC layoutDesc{};
	layoutDesc.pInputElementDescs = inputElementDesc;
	layoutDesc.NumElements = _countof(inputElementDesc);
	plDesc.layoutDesc_ = layoutDesc;

	plDesc.signature_ = rootSignature_->GetRootSignature().Get();
	plDesc.vertexShader_ = vertexShader.Get();
	plDesc.pixelShader_ = pixelShader.Get();

	plDesc.depthStencilDesc_.DepthEnable = false;
	// 裏面を表示しない
	plDesc.rasterizerDesc_.CullMode = D3D12_CULL_MODE_BACK;
	// 塗りつぶす
	plDesc.rasterizerDesc_.FillMode = D3D12_FILL_MODE_SOLID;
	plDesc.rasterizerDesc_.DepthClipEnable = true;

	for (uint8_t i = 0; i < static_cast<uint8_t>(BlendMode::BlendCount); i++) {
		graphicsPipeline_.at(i) = std::make_unique<GraphicsPipeline>();
		graphicsPipeline_.at(i)->CreatePipeline(plDesc, static_cast<BlendMode>(i));
	}

#pragma endregion

}

void SpriteRender::DrawCommand(const Matrix4x4& viewProjectionMat) {
	auto list = ListManager::GetInstance()->GetList();

	list->SetGraphicsRootSignature(rootSignature_->GetRootSignature().Get());
	list->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	
	auto& spriteList = Sprite::spriteLists_;
	// 描画順並べる
	spriteList.sort([](Sprite* a, Sprite* b) { return a->GetLayerNum() < b->GetLayerNum(); });
	
	for (auto sprite : spriteList) {
		if (!sprite->isActive_) { continue; }

		// 定数バッファ用の計算
		sprite->cMat->wvp = MakeAffineMatrix(
			{ sprite->scale_.x,sprite->scale_.y,1.0f },
			{ 0.0f,0.0f,sprite->rotate_ },
			{ sprite->translate_.x,sprite->translate_.y,0.5f }
		) * viewProjectionMat;
		sprite->cMaterial->uvMat = MakeAffineMatrix(
			{ sprite->uvScale_.x,sprite->uvScale_.y,1.0f },
			{ 0.0f,0.0f,sprite->uvRotate_ },
			{ sprite->uvTranslate_.x,sprite->uvTranslate_.y,0.5f }
		);
		//sprite->cMaterial->uvMat = Matrix3x3::MakeAffineMatrix(sprite->uvScale_, sprite->uvRotate_, sprite->uvTranslate_);
		sprite->cMaterial->color = sprite->color_;

		//auto pipeline = GraphicsPipeline::GetInstance()->GetSpritePipelineState(sprite->blendType_);
		list->SetPipelineState(graphicsPipeline_[static_cast<uint32_t>(sprite->blendType_)]->GetPipelineState());
		list->IASetVertexBuffers(0, 1, &sprite->vertexBufferView_);
		list->IASetIndexBuffer(&sprite->indexBufferView_);
		
		list->SetGraphicsRootDescriptorTable(0, sprite->texture_->GetHandle().GetGPU()); // Texture
		//list->SetGraphicsRootDescriptorTable(1, sprite->cMat.GetHandle().GetGPU()); // cMat
		list->SetGraphicsRootConstantBufferView(1, sprite->cMat.GetGPUVirtualAddress());
		//list->SetGraphicsRootDescriptorTable(2, sprite->cMaterial.GetHandle().GetGPU()); // cMaterial
		list->SetGraphicsRootConstantBufferView(2, sprite->cMaterial.GetGPUVirtualAddress());

		// 描画
		list->DrawIndexedInstanced(6, 1, 0, 0, 0);
	}
	
}
