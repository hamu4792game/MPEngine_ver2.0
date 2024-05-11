#include "ModelRender.h"
#include <string>
#include "MPEngine/Base/Manager/ShaderManager/ShaderManager.h"
#include "MPEngine/Base/Manager/ListManager/ListManager.h"
#include "MPEngine/Graphics/Model/Model.h"
#include "Utils/Camera/Camera3d.h"

void ModelRender::Initialize() {
	// Object
	if (true) {

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

		const uint8_t paramIndex = 5;
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

		// カメラの情報
		rootParameter[4].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
		rootParameter[4].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
		rootParameter[4].Descriptor.ShaderRegister = 3;

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

	// Skinning
	if (true) {
#pragma region Shader
		const std::string VSpath = "SkinningObject3d.VS.hlsl";
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

		const uint8_t paramIndex = 6;
		D3D12_ROOT_PARAMETER rootParameter[paramIndex] = {};
		rootParameter[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
		rootParameter[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
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

		// カメラの情報
		rootParameter[4].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
		rootParameter[4].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
		rootParameter[4].Descriptor.ShaderRegister = 3;
		
		// skinning
		rootParameter[5].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
		rootParameter[5].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
		rootParameter[5].DescriptorTable.pDescriptorRanges = range;
		rootParameter[5].DescriptorTable.NumDescriptorRanges = _countof(range);

		skinningRootSignature_ = std::make_unique<RootSignature>();
		skinningRootSignature_->CreateRootSignature(rootParameter, paramIndex);
#pragma endregion

#pragma region GraphicsPipeline
		PipelineDesc plDesc{};

		// InputLayout
		const uint8_t inputIndex = 5;
		std::array<D3D12_INPUT_ELEMENT_DESC, inputIndex> inputElementDesc{};
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
		inputElementDesc[3].SemanticName = "WEIGHT";
		inputElementDesc[3].SemanticIndex = 0;
		inputElementDesc[3].Format = DXGI_FORMAT_R32G32B32_FLOAT;
		inputElementDesc[3].InputSlot = 1; // 1番目のslotのvbvのことだと伝える
		inputElementDesc[3].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
		inputElementDesc[4].SemanticName = "INDEX";
		inputElementDesc[4].SemanticIndex = 0;
		inputElementDesc[4].Format = DXGI_FORMAT_R32G32B32_SINT;
		inputElementDesc[4].InputSlot = 1; // 1番目のslotのvbvのことだと伝える
		inputElementDesc[4].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;


		D3D12_INPUT_LAYOUT_DESC layoutDesc{};
		layoutDesc.pInputElementDescs = inputElementDesc.data();
		layoutDesc.NumElements = UINT(inputElementDesc.max_size());
		plDesc.layoutDesc_ = layoutDesc;

		plDesc.signature_ = skinningRootSignature_->GetRootSignature().Get();
		plDesc.vertexShader_ = vertexShader.Get();
		plDesc.pixelShader_ = pixelShader.Get();

		// Depthの機能を有効化する
		plDesc.depthStencilDesc_.DepthEnable = true;
		// 書き込みします
		plDesc.depthStencilDesc_.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
		// 比較関数はLessEqual。つまり、近ければ描画される
		plDesc.depthStencilDesc_.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;

		// 裏面を表示しない
		plDesc.rasterizerDesc_.CullMode = D3D12_CULL_MODE_BACK;
		// 塗りつぶす
		plDesc.rasterizerDesc_.FillMode = D3D12_FILL_MODE_SOLID;
		plDesc.rasterizerDesc_.DepthClipEnable = true;

		for (uint8_t i = 0; i < static_cast<uint8_t>(BlendMode::BlendCount); i++) {
			skinningGraphicsPipeline_.at(i) = std::make_unique<GraphicsPipeline>();
			skinningGraphicsPipeline_.at(i)->CreatePipeline(plDesc, static_cast<BlendMode>(i));
		}

#pragma endregion
	}

	// ライトの生成
	directionalLight_ = std::make_unique<DirectionalLight>();

}

void ModelRender::DrawCommand(Camera3d* cameraPtr) {
	auto list = ListManager::GetInstance()->GetList();
	auto rsManager = ResourceManager::GetInstance();

	list->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	directionalLight_->Update();
	auto camera = Camera3d::GetInstance();
	Matrix4x4 viewProjectionMat = camera->GetViewProMat();
	camera->cCamera->worldPosition = camera->GetTransform().GetPosition();

	auto& modelList = Model::modelLists_;

	for (auto model : modelList) {
		if (!model->isActive_) { continue; }
		/*Matrix4x4 nodeMatrix = MakeIdentity4x4();
		if (model->animation_) {
			nodeMatrix = model->animation_->GetMatrix();
		}*/
		// 定数バッファ用の計算
		model->cMat->world = model->transform_.UpdateMatrix();
		model->cMat->worldInverseTranspose = Inverse(model->cMat->world);
		model->cMat->wvp = model->cMat->world * viewProjectionMat;
		model->cMaterial->color = model->color_;
		model->cMaterial->enableLighting = true;
		model->cMaterial->shininess = 5.0f;
		model->cMaterial->phongLighing = false;

		if (model->animation_) {
			D3D12_VERTEX_BUFFER_VIEW vbvs[2] = {
				model->vertexBufferView_, // VertexのVBV
				model->animation_->skinCluster_.influenceBufferView, // InfluenceのVBV
			};
			list->SetGraphicsRootSignature(skinningRootSignature_->GetRootSignature().Get());
			list->SetPipelineState(skinningGraphicsPipeline_[static_cast<uint32_t>(model->blendType_)]->GetPipelineState());
			list->IASetVertexBuffers(0, 2, vbvs);
		}
		else {
			list->SetGraphicsRootSignature(rootSignature_->GetRootSignature().Get());
			list->SetPipelineState(graphicsPipeline_[static_cast<uint32_t>(model->blendType_)]->GetPipelineState());
			list->IASetVertexBuffers(0, 1, &model->vertexBufferView_);
		}

		list->IASetIndexBuffer(&model->indexBufferView_);
		list->SetGraphicsRootDescriptorTable(0, model->texture_->GetHandle().GetGPU()); // Texture
		list->SetGraphicsRootConstantBufferView(1, model->cMat.GetGPUVirtualAddress()); // cMat
		list->SetGraphicsRootConstantBufferView(2, model->cMaterial.GetGPUVirtualAddress()); // cMaterial
		list->SetGraphicsRootConstantBufferView(3, directionalLight_->cDirectionLight_.GetGPUVirtualAddress()); // cDirectinalLight
		list->SetGraphicsRootConstantBufferView(4, camera->cCamera.GetGPUVirtualAddress()); // cCamera
		if (model->animation_) {
			list->SetGraphicsRootDescriptorTable(5, model->animation_->skinCluster_.paletteSrvHandle.GetGPU()); // skinning
		}
		// 描画
		//list->DrawInstanced(UINT(model->model_->GetModel().vertices.size()), 1, 0, 0);
		list->DrawIndexedInstanced(UINT(model->model_->GetModel().indices.size()), 1, 0, 0, 0);
	}

}
