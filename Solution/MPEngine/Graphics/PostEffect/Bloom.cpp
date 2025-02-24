#include "Bloom.h"
#include "Base/GraphicsManager/GraphicsManager.h"
#include "MPEngine/Base/Manager/DeviceManager/DeviceManager.h"
#include "MPEngine/Base/Manager/ListManager/ListManager.h"
#include "MPEngine/Base/Manager/ResourceManager/ResourceManager.h"
#include "MPEngine/Base/DetailSetting/SwapChain/SwapChain.h"

#include "Graphics/PostEffect/HighLumi.h"
#include "Graphics/PostEffect/GaussianBlur.h"

Bloom::Bloom() {
	render_ = std::make_unique<IntermediateRenderTarget>();
}

Bloom* Bloom::GetInstance() {
	static Bloom instance;
	return &instance;
}

void Bloom::Initialize(SwapChain* swap) {
	auto device = DeviceManager::GetInstance();
	auto rsManager = ResourceManager::GetInstance();

	// レンダーの作成
	render_->CreateRenderTexture(device, swap, rsManager);
	BaseEffect::CreateRenderTexture(device, swap, rsManager);

}

void Bloom::CreatePipelineState() {
#pragma region Shader
	Microsoft::WRL::ComPtr<IDxcBlob> vertexShader;
	Microsoft::WRL::ComPtr<IDxcBlob> pixelShader;
	const std::string VSpath = "Fullscreen.VS.hlsl";
	const std::string PSpath = "BloomFilter.PS.hlsl";
	auto shaderInstance = ShaderManager::GetInstance();
	vertexShader = shaderInstance->CompileShader(VSpath, ShaderManager::ShaderType::Vertex);
	pixelShader = shaderInstance->CompileShader(PSpath, ShaderManager::ShaderType::Pixel);
#pragma endregion

#pragma region RootSignature
	D3D12_DESCRIPTOR_RANGE range[2] = {};
	// 元のテクスチャ
	range[0].BaseShaderRegister = 0;
	range[0].NumDescriptors = 1; // 必要な数
	range[0].RegisterSpace = 0;
	range[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	range[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
	// ぼかしテクスチャ
	range[1].BaseShaderRegister = 1;
	range[1].NumDescriptors = 1; // 必要な数
	range[1].RegisterSpace = 0;
	range[1].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	range[1].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;


	const uint8_t paramIndex = 3;
	D3D12_ROOT_PARAMETER rootParameter[paramIndex] = {};
	rootParameter[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameter[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	rootParameter[0].DescriptorTable.pDescriptorRanges = &range[0];
	rootParameter[0].DescriptorTable.NumDescriptorRanges = 1u;

	rootParameter[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameter[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	rootParameter[1].DescriptorTable.pDescriptorRanges = &range[1];
	rootParameter[1].DescriptorTable.NumDescriptorRanges = 1u;

	// 定数バッファに送るパラメーター
	rootParameter[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameter[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameter[2].Descriptor.ShaderRegister = 0;

	rootSignature_ = std::make_unique<RootSignature>();
	rootSignature_->CreateRootSignature(rootParameter, paramIndex);
#pragma endregion

#pragma region GraphicsPipeline
	PipelineDesc plDesc;

	// 頂点データには何も入力しないので利用しない。ドライバやGUPが軽くなるかもしれないらしい
	D3D12_INPUT_LAYOUT_DESC layoutDesc{};
	layoutDesc.pInputElementDescs = nullptr;
	layoutDesc.NumElements = 0;
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

void Bloom::DrawCommand(ID3D12GraphicsCommandList* comList, const uint32_t& handleNum) {

	auto lumi = HighLumi::GetInstance();
	auto gaus = GaussianBlur::GetInstance();
	// 今まで描画されていたものを読み込んで中間レンダーに書き込む
	// lumiに高輝度出力を書かせてからtextureにする
	lumi->DrawCommand(comList, handleNum);
	// その後、gausに出力し、textureとして利用、bloomにgausと元画像を合成

	// 中間レンダーから読み取って中間レンダー2に書き込む
	gaus->DrawCommand(comList, handleNum);
	// 中間レンダー2と今までのを合成したものを出力する

	GraphicsManager::CreateBarrier(renderTextureResource_.Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
	
	BaseEffect::PreDraw(comList, handleNum);
	// ぼかしテクスチャの設定
	comList->SetGraphicsRootDescriptorTable(1, ResourceManager::GetInstance()->GetSRVHeap()->GetGPUDescriptorHandle(handleNum));
	if (isUsed) {
		comList->SetGraphicsRootConstantBufferView(2, cParam_.GetGPUVirtualAddress());
	}
	// 描画コマンド
	BaseEffect::DrawCommand(comList);
}
