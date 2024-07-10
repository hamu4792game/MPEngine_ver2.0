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
	// 画面クリア
	radialBlur_->DrawCommand(list, 10u);
	grayscale_->DrawCommand(list, 10u);
}
