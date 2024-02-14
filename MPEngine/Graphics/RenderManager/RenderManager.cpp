#include "RenderManager.h"
#include "MPEngine/Math/Matrix4x4.h"
#include "MPEngine/Base/Manager/ListManager/ListManager.h"
#include "MPEngine/Base/Manager/ResourceManager/ResourceManager.h"

void RenderManager::Initialize() {
	spriteRender.Initialize();
	modelRender.Initialize();
	particleRender.Initialize();
	camera = std::make_shared<Camera>();
	camera3d_ = Camera3d::GetInstance();
	camera3d_->Initialize(2000.0f);
}

void RenderManager::Draw() {
	auto list = ListManager::GetInstance()->GetList();
	auto rsManager = ResourceManager::GetInstance();
	auto srvHeap = rsManager->GetSRVHeap()->GetDescriptorHeap();
	Matrix4x4 projectionMatrix;

	list->SetDescriptorHeaps(1, &srvHeap);

	projectionMatrix = camera3d_->GetViewProMat();
	modelRender.DrawCommand(camera3d_);
	
	Matrix4x4 billboardMat = camera3d_->GetBillboardMat();
	particleRender.DrawCommand(projectionMatrix, billboardMat);

	projectionMatrix = camera->GetViewProMat();
	spriteRender.DrawCommand(projectionMatrix);

}
