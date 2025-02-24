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

	auto device = DeviceManager::GetInstance();
	auto rs = ResourceManager::GetInstance();

	radialBlur_ = RadialBlur::GetInstance();
	radialBlur_->CreateRenderTexture(device, swapchain, rs);
	
	grayscale_ = Grayscale::GetInstance();
	grayscale_->CreateRenderTexture(device, swapchain, rs);

	hsvFilter_ = HSVFilter::GetInstance();
	hsvFilter_->CreateRenderTexture(device, swapchain, rs);

	gaussianBlur_ = GaussianBlur::GetInstance();
	gaussianBlur_->CreateRenderTexture(device, swapchain, rs);

	highLumi_ = HighLumi::GetInstance();
	highLumi_->CreateRenderTexture(device, swapchain, rs);

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
	gaussianBlur_->PreProcess();
	highLumi_->PreProcess();

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
	
	// 2枚管理面倒だからつくったやつ
	struct FlontNumber {
		int num = 0;
		int Count() {
			num++;
			if (num > 1) {
				num = 0;
			}
			return num;
		}
	};
	FlontNumber flontNum;

	uint32_t handleNum = 10u; // 最初はRenderのSRV さすがに危険なのでどこかで取得できるよう修正必須
	// 以降ポストエフェクト

	// swapchainのrenderをテクスチャにし、中間レンダーの0番に書き込む
	handleNum = ChangeProc(handleNum, highLumi_, flontNum.num);
	// 中間レンダーの0番をテクスチャにし、中間レンダーの1へ書き込む。
	handleNum = ChangeProc(handleNum, gaussianBlur_, flontNum.Count());
	

	handleNum = ChangeProc(handleNum, grayscale_, flontNum.Count());
	handleNum = ChangeProc(handleNum, radialBlur_, flontNum.Count());

	// 最後だけswapchainに書き込み
	uint32_t index = swapchain->GetSwapChain()->GetCurrentBackBufferIndex();
	intermediateRenderTarget_.at(flontNum.num)->PreProcess(list, index, false);

	hsvFilter_->DrawCommand(list, handleNum);

	// hud用描画。後で奥にtextureが描画できるようにもする
	HudDraw();

}

void RenderManager::HudDraw() {
	Matrix4x4 projectionMatrix2D; 
	projectionMatrix2D = camera->GetViewProMat();
	spriteRender.DrawCommand(projectionMatrix2D);

}

uint32_t RenderManager::ChangeProc(const int& texhandle, BaseEffect* effect, const int& frontNum) {
	auto list = ListManager::GetInstance()->GetList();
	// n枚目をRenderとして使用
	intermediateRenderTarget_.at(frontNum)->PreProcess(list, intermediateRenderTarget_.at(frontNum)->GetRTVHandle());

	// n枚目をテクスチャとして使用し、m枚目に書き込み
	effect->DrawCommand(list, texhandle);

	// 状態をテクスチャに
	return intermediateRenderTarget_.at(frontNum)->PostProcess();
}
