#include "RenderManager.h"
#include "MPEngine/Math/Matrix4x4.h"
#include "MPEngine/Base/Manager/ListManager/ListManager.h"
#include "MPEngine/Base/Manager/ResourceManager/ResourceManager.h"
#include "MPEngine/Base/DetailSetting/SwapChain/SwapChain.h"

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

	radialBlur_ = std::make_unique<RadialBlur>();
	radialBlur_->CreateRenderTexture(DeviceManager::GetInstance(), swapchain, ResourceManager::GetInstance());
	
	grayscale_ = std::make_unique<Grayscale>();
	grayscale_->CreateRenderTexture(DeviceManager::GetInstance(), swapchain, ResourceManager::GetInstance());

	for (auto& handle : intermediateRenderTarget_) {
		handle = std::make_unique<IntermediateRenderTarget>(DeviceManager::GetInstance(), swapchain, ResourceManager::GetInstance());
	}

}

void RenderManager::Draw(SwapChain* swapchain) {
	auto list = ListManager::GetInstance()->GetList();
	auto rsManager = ResourceManager::GetInstance();
	auto srvHeap = rsManager->GetSRVHeap()->GetDescriptorHeap();
	Matrix4x4 projectionMatrix;
	Matrix4x4 projectionMatrix2D;

	list->SetDescriptorHeaps(1, &srvHeap);

	projectionMatrix = camera3d_->GetViewProMat();
	modelRender.DrawCommand(camera3d_);
	
	Matrix4x4 billboardMat = camera3d_->GetBillboardMat();
	particleRender.DrawCommand(projectionMatrix, billboardMat);

	lineRender.DrawCommand(projectionMatrix);

	skyBoxRender.DrawCommand(projectionMatrix);

	projectionMatrix2D = camera->GetViewProMat();
	spriteRender.DrawCommand(projectionMatrix2D);

	grayscale_->PreProcess();
	radialBlur_->PreProcess();
}

void RenderManager::PostDraw(SwapChain* swapchain) {
	auto list = ListManager::GetInstance()->GetList();
	
	// 0枚目をRenderとして使用
	intermediateRenderTarget_.at(0)->PreProcess(list, intermediateRenderTarget_.at(0)->GetRTVHandle());
	uint8_t handleNum = 10u; // 最初はRenderのSRV
	
	grayscale_->DrawCommand(list, handleNum);
	
	// 0枚目をテクスチャとして使用し、SwapChainに書き込み
	// 状態をテクスチャに
	intermediateRenderTarget_.at(0)->PostProcess();
	auto index = swapchain->GetSwapChain()->GetCurrentBackBufferIndex();
	// 状態を変更せず、描画先をswapchainに変更
	handleNum = intermediateRenderTarget_.at(0)->PreProcess(list, index, false);

	radialBlur_->DrawCommand(list, handleNum);
	
	//intermediateRenderTarget_.at(0)->PostProcess();
	//intermediateRenderTarget_.at(1)->PostProcess();

	// 1枚目をテクスチャとして使用し、SwapChainに書き込み
	//handleNum = intermediateRenderTarget_.at(1)->PreProcess(list, intermediateRenderTarget_.at(0)->GetRTVHandle());
	

}
