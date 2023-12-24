#include "RenderManager.h"
#include "MPEngine/Math/Matrix4x4.h"
#include "MPEngine/Base/Manager/ListManager/ListManager.h"
#include "MPEngine/Base/Manager/ResourceManager/ResourceManager.h"

void RenderManager::Initialize() {
	spriteRender.Initialize();
	modelRender.Initialize();
	camera = std::make_shared<Camera>();
	camera3d_ = Camera3d::GetInstance();
	camera3d_->Initialize(2000.0f);
}

void RenderManager::Draw() {
	auto list = ListManager::GetInstance()->GetList();
	auto rsManager = ResourceManager::GetInstance();
	auto srvHeap = rsManager->GetSRVHeap()->GetDescriptorHeap();

	list->SetDescriptorHeaps(1, &srvHeap);
	auto projectionMatrix = camera->GetViewProMat();
	spriteRender.DrawCommand(projectionMatrix);

	projectionMatrix = //MakePerspectiveFovMatrix(0.45f, 1280.0f / 720.0f, 0.1f, 2000.0f);
	camera3d_->GetViewProMat();

	modelRender.DrawCommand(projectionMatrix);

}
