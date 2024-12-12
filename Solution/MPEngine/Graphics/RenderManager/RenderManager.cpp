#include "RenderManager.h"
#include "MPEngine/Math/Matrix4x4.h"
#include "MPEngine/Base/Manager/ListManager/ListManager.h"
#include "MPEngine/Base/Manager/ResourceManager/ResourceManager.h"
#include "MPEngine/Base/DetailSetting/SwapChain/SwapChain.h"
#include "ImGuiManager/ImGuiManager.h"

decltype(RenderManager::nowEffect)RenderManager::nowEffect = RenderManager::PostEffect::None;

void RenderManager::Initialize(SwapChain* swapchain) {
	spriteRender.Initialize();
	modelRender.Initialize();
	particleRender.Initialize();
	lineRender.Initialize();
	skyBoxRender.Initialize();

	camera = std::make_shared<Camera>();
	camera3d_ = Camera3d::GetInstance();
	camera3d_->Initialize(2000.0f);

	radialBlur_ = RadialBlur::GetInstance();
	radialBlur_->CreateRenderTexture(DeviceManager::GetInstance(), swapchain, ResourceManager::GetInstance());
	
	grayscale_ = Grayscale::GetInstance();
	grayscale_->CreateRenderTexture(DeviceManager::GetInstance(), swapchain, ResourceManager::GetInstance());

	hsvFilter_ = HSVFilter::GetInstance();
	hsvFilter_->CreateRenderTexture(DeviceManager::GetInstance(), swapchain, ResourceManager::GetInstance());


	for (auto& handle : intermediateRenderTarget_) {
		handle = std::make_unique<IntermediateRenderTarget>(DeviceManager::GetInstance(), swapchain, ResourceManager::GetInstance());
	}

}

void RenderManager::Draw() {
	auto list = ListManager::GetInstance()->GetList();
	auto rsManager = ResourceManager::GetInstance();
	auto srvHeap = rsManager->GetSRVHeap()->GetDescriptorHeap();
	Matrix4x4 projectionMatrix;
	camera3d_->DrawImGui();

	list->SetDescriptorHeaps(1, &srvHeap);

	projectionMatrix = camera3d_->GetViewProMat();
	skyBoxRender.DrawCommand(projectionMatrix);

	modelRender.DrawCommand(camera3d_);
	
	Matrix4x4 billboardMat = camera3d_->GetBillboardMat();
	particleRender.DrawCommand(projectionMatrix, billboardMat);

	lineRender.DrawCommand(projectionMatrix);


	grayscale_->PreProcess();
	radialBlur_->PreProcess();
	hsvFilter_->PreProcess();

#ifdef _DEBUG
	ImGui::Begin("HSV");
	hsvFilter_->SetUsed(true);
	ImGui::DragFloat("hue", &hsvFilter_->cParam_->hue, 0.01f);
	ImGui::DragFloat("saturation", &hsvFilter_->cParam_->saturation, 0.01f);
	ImGui::DragFloat("value", &hsvFilter_->cParam_->value, 0.01f);
	ImGui::End();

#endif // _DEBUG

}

void RenderManager::PostDraw(SwapChain* swapchain) {

	auto list = ListManager::GetInstance()->GetList();
	
	// 0枚目をRenderとして使用
	intermediateRenderTarget_.at(0)->PreProcess(list, intermediateRenderTarget_.at(0)->GetRTVHandle());
	uint32_t handleNum = 10u; // 最初はRenderのSRV
	
	grayscale_->DrawCommand(list, handleNum);
	
	// 0枚目をテクスチャとして使用し、1枚目に書き込み
	// 状態をテクスチャに
	handleNum = intermediateRenderTarget_.at(0)->PostProcess();
	// 描画先を1枚目に変更
	// handleNumは0番目を取得しなきゃいけない
	intermediateRenderTarget_.at(1)->PreProcess(list, intermediateRenderTarget_.at(1)->GetRTVHandle());

	radialBlur_->DrawCommand(list, handleNum);

	// 1枚目をtextureとして使用
	handleNum = intermediateRenderTarget_.at(1)->PostProcess();
	// swapchainに書き込み
	uint32_t index = swapchain->GetSwapChain()->GetCurrentBackBufferIndex();
	intermediateRenderTarget_.at(1)->PreProcess(list, index, false);

	hsvFilter_->DrawCommand(list, handleNum);
	
	//intermediateRenderTarget_.at(0)->PostProcess();
	//intermediateRenderTarget_.at(1)->PostProcess();

	// 1枚目をテクスチャとして使用し、SwapChainに書き込み
	//handleNum = intermediateRenderTarget_.at(1)->PreProcess(list, intermediateRenderTarget_.at(0)->GetRTVHandle());
	
	HudDraw();

}

void RenderManager::HudDraw() {
	Matrix4x4 projectionMatrix2D; 
	projectionMatrix2D = camera->GetViewProMat();
	spriteRender.DrawCommand(projectionMatrix2D);

}
