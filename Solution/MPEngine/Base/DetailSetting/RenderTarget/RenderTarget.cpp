#include "RenderTarget.h"
#include "MPEngine/Base/Manager/ResourceManager/ResourceManager.h"
#include "MPEngine/Base/DetailSetting/SwapChain/SwapChain.h"
#include "MPEngine/Base/Manager/DeviceManager/DeviceManager.h"
#include <ShaderManager/ShaderManager.h>
#include "MPEngine/Base/Manager/ShaderManager/ShaderManager.h"

RenderTarget::~RenderTarget() {
	if (renderTextureResource_) {
		renderTextureResource_->Release();
	}
	if (vertexResource_) {
		vertexResource_->Release();
	}
}

void RenderTarget::CreateRenderTexture(DeviceManager* device, SwapChain* swapChain, ResourceManager* rsManager) {
	renderTextureResource_ = rsManager->CreateRenderTextureResource(device->GetDevice(), swapChain->GetBackBuffer()->GetDesc(), DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, Vector4(clearColor_));

	// RTVの設定 swapChainと同じ
	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
	rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB; // 出力結果をSRGBに変換して書き込む
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D; // 2dテクスチャとして書き込む
	// RTVの生成
	device->GetDevice()->CreateRenderTargetView(renderTextureResource_.Get(), &rtvDesc, swapChain->GetRTVHeap()->GetCPUDescriptorHandle(2));

	// SRVの設定
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Format = rtvDesc.Format;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	handleNum_ = rsManager->GetCount();
	// SRVの生成
	device->GetDevice()->CreateShaderResourceView(renderTextureResource_.Get(), &srvDesc, rsManager->GetSRVHeap()->GetCPUDescriptorHandle(handleNum_));

	// 必要パラメーターの生成
	CreatePipelineState();
	CreateVertex(device, rsManager);
}

void RenderTarget::DrawCommand(ID3D12GraphicsCommandList* comList) {
	comList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	comList->IASetVertexBuffers(0, 1, &vertexBufferView_);
	comList->SetGraphicsRootSignature(rootSignature_->GetRootSignature().Get());
	comList->SetPipelineState(graphicsPipeline_[static_cast<uint32_t>(RenderManager::nowEffect)]->GetPipelineState());
	comList->SetGraphicsRootDescriptorTable(0, ResourceManager::GetInstance()->GetSRVHeap()->GetGPUDescriptorHandle(handleNum_));
	
	// 頂点3つ描画
	comList->DrawInstanced(3, 1, 0, 0);
}

void RenderTarget::ClearRenderTarget(ID3D12GraphicsCommandList* comList, D3D12_CPU_DESCRIPTOR_HANDLE rtvHeapPointer) const {
	// 指定した色で画面全体をクリアする
	comList->ClearRenderTargetView(rtvHeapPointer, clearColor_, 0, nullptr);
}

void RenderTarget::CreatePipelineState() {
#pragma region Shader
	Microsoft::WRL::ComPtr<IDxcBlob> vertexShader;
	Microsoft::WRL::ComPtr<IDxcBlob> pixelShader;
	const std::string VSpath = "Fullscreen.VS.hlsl";
	const std::string PSpath[] = {
		"GaussianFilter.PS.hlsl",
		"Grayscale.PS.hlsl",
		"Sepiatone.PS.hlsl",
		"Vignette.PS.hlsl",
	};
	auto shaderInstance = ShaderManager::GetInstance();
	vertexShader = shaderInstance->CompileShader(VSpath, ShaderManager::ShaderType::Vertex);
	//pixelShader = shaderInstance->CompileShader(PSpath, ShaderManager::ShaderType::Pixel);
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
	//plDesc.pixelShader_ = pixelShader.Get();

	plDesc.depthStencilDesc_.DepthEnable = false;
	// 裏面を表示しない
	plDesc.rasterizerDesc_.CullMode = D3D12_CULL_MODE_BACK;
	// 塗りつぶす
	plDesc.rasterizerDesc_.FillMode = D3D12_FILL_MODE_SOLID;
	plDesc.rasterizerDesc_.DepthClipEnable = true;

	for (uint8_t i = 0; i < static_cast<uint8_t>(RenderManager::PostEffect::kMaxNum); i++) {
		pixelShader = shaderInstance->CompileShader(PSpath[i], ShaderManager::ShaderType::Pixel);
		plDesc.pixelShader_ = pixelShader.Get();

		graphicsPipeline_.at(i) = std::make_unique<GraphicsPipeline>();
		graphicsPipeline_.at(i)->CreatePipeline(plDesc, BlendMode::None);
	}

#pragma endregion

}

void RenderTarget::CreateVertex(DeviceManager* device, ResourceManager* rsManager) {
	//	頂点データの作成
	struct PeraVertex {
		Vector4 pos;
		Vector2 uv;
	};

	PeraVertex pv[3] = {
		{{-1.0f, 1.0f, 0.0f, 1.0f},{0.0f,0.0f}}, // 左上
		{{3.0f, 1.0f, 0.0f, 1.0f},{2.0f,0.0f}}, // 右上
		{{-1.0f, -3.0f, 0.0f, 1.0f},{0.0f,2.0f}}, // 左下
	};

	// ペラポリゴンの頂点バッファの作成
	vertexResource_ = rsManager->CreateBufferResource(device->GetDevice(), sizeof(pv));

	// バッファービューの設定
	vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
	vertexBufferView_.SizeInBytes = sizeof(pv);
	vertexBufferView_.StrideInBytes = sizeof(PeraVertex);

	// マップでコピー
	PeraVertex* peramap = nullptr;
	vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&peramap));
	std::copy(std::begin(pv), std::end(pv), peramap);
	vertexResource_->Unmap(0, nullptr);
}
