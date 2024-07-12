#include "BaseEffect.h"
#include "MPEngine/Base/Manager/DeviceManager/DeviceManager.h"
#include "MPEngine/Base/Manager/ShaderManager/ShaderManager.h"
#include "ResourceManager/ResourceManager.h"
#include "MPEngine/Base/DetailSetting/SwapChain/SwapChain.h"
#include "Base/GraphicsManager/GraphicsManager.h"

decltype(BaseEffect::isUsed) BaseEffect::isUsed = false;
decltype(BaseEffect::copyRootSignature_) BaseEffect::copyRootSignature_;
decltype(BaseEffect::copyGraphicsPipeline_) BaseEffect::copyGraphicsPipeline_;

BaseEffect::BaseEffect() {
	BaseEffect::CreateCopyPipelineState();
}

BaseEffect::~BaseEffect() {
	if (renderTextureResource_) {
		renderTextureResource_->Release();
	}
}

void BaseEffect::CreateRenderTexture(DeviceManager* device, SwapChain* swapChain, ResourceManager* rsManager) {
	renderTextureResource_ = rsManager->CreateRenderTextureResource(device->GetDevice(), swapChain->GetBackBuffer()->GetDesc(), DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, Vector4(clearColor_));

	// RTVの設定 swapChainと同じ
	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
	rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB; // 出力結果をSRGBに変換して書き込む
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D; // 2dテクスチャとして書き込む
	rtvHandleNum_ = swapChain->GetCount();
	// RTVの生成
	device->GetDevice()->CreateRenderTargetView(renderTextureResource_.Get(), &rtvDesc, swapChain->GetRTVHeap()->GetCPUDescriptorHandle(rtvHandleNum_));

	// SRVの設定
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Format = rtvDesc.Format;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvHandleNum_ = rsManager->GetCount();
	// SRVの生成
	device->GetDevice()->CreateShaderResourceView(renderTextureResource_.Get(), &srvDesc, rsManager->GetSRVHeap()->GetCPUDescriptorHandle(srvHandleNum_));

	// 必要パラメーターの生成
	CreatePipelineState();

	GraphicsManager::CreateBarrier(renderTextureResource_.Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
}

void BaseEffect::PreProcess() {
	GraphicsManager::CreateBarrier(renderTextureResource_.Get(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET);
}

void BaseEffect::PreDraw(ID3D12GraphicsCommandList* comList, const uint32_t& handleNum) {
	comList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	if (isUsed) {
		comList->SetGraphicsRootSignature(rootSignature_->GetRootSignature().Get());
		comList->SetPipelineState(graphicsPipeline_[static_cast<uint32_t>(blendType_)]->GetPipelineState());
	}
	else {
		comList->SetGraphicsRootSignature(copyRootSignature_->GetRootSignature().Get());
		comList->SetPipelineState(copyGraphicsPipeline_->GetPipelineState());
	}
	// 読み込むテクスチャの設定
	comList->SetGraphicsRootDescriptorTable(0, ResourceManager::GetInstance()->GetSRVHeap()->GetGPUDescriptorHandle(handleNum));
}

void BaseEffect::DrawCommand(ID3D12GraphicsCommandList* comList) {	
	comList->DrawInstanced(3, 1, 0, 0);
}

void BaseEffect::CreateCopyPipelineState() {
	if (copyRootSignature_) {
		return;
	}
	if (copyGraphicsPipeline_) {
		return;
	}

#pragma region Shader
	Microsoft::WRL::ComPtr<IDxcBlob> vertexShader;
	Microsoft::WRL::ComPtr<IDxcBlob> pixelShader;
	const std::string VSpath = "Fullscreen.VS.hlsl";
	const std::string PSpath = "CopyImage.PS.hlsl";
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
	const uint8_t paramIndex = 1;
	D3D12_ROOT_PARAMETER rootParameter[paramIndex] = {};
	rootParameter[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameter[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	rootParameter[0].DescriptorTable.pDescriptorRanges = range;
	rootParameter[0].DescriptorTable.NumDescriptorRanges = _countof(range);

	copyRootSignature_ = std::make_unique<RootSignature>();
	copyRootSignature_->CreateRootSignature(rootParameter, paramIndex);
#pragma endregion

#pragma region GraphicsPipeline
	PipelineDesc plDesc;

	// 頂点データには何も入力しないので利用しない。ドライバやGUPが軽くなるかもしれないらしい
	D3D12_INPUT_LAYOUT_DESC layoutDesc{};
	layoutDesc.pInputElementDescs = nullptr;
	layoutDesc.NumElements = 0;
	plDesc.layoutDesc_ = layoutDesc;

	plDesc.signature_ = copyRootSignature_->GetRootSignature().Get();
	plDesc.vertexShader_ = vertexShader.Get();
	plDesc.pixelShader_ = pixelShader.Get();

	plDesc.depthStencilDesc_.DepthEnable = false;
	// 裏面を表示しない
	plDesc.rasterizerDesc_.CullMode = D3D12_CULL_MODE_BACK;
	// 塗りつぶす
	plDesc.rasterizerDesc_.FillMode = D3D12_FILL_MODE_SOLID;
	plDesc.rasterizerDesc_.DepthClipEnable = true;

	copyGraphicsPipeline_ = std::make_unique<GraphicsPipeline>();
	copyGraphicsPipeline_->CreatePipeline(plDesc, BlendMode::None);

#pragma endregion
}

void BaseEffect::ClearRenderTarget(ID3D12GraphicsCommandList* comList, D3D12_CPU_DESCRIPTOR_HANDLE rtvHeapPointer) const {
	// 指定した色で画面全体をクリアする
	comList->ClearRenderTargetView(rtvHeapPointer, clearColor_, 0, nullptr);
}
