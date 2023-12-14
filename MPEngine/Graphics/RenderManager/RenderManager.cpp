#include "RenderManager.h"
#include "MPEngine/Math/Matrix4x4.h"
#include "MPEngine/Base/Manager/ListManager/ListManager.h"
#include "MPEngine/Base/Manager/ResourceManager/ResourceManager.h"

void RenderManager::Initialize() {
	spriteRender.Initialize();
}

void RenderManager::Draw() {
	auto list = ListManager::GetInstance()->GetList();
	auto rsManager = ResourceManager::GetInstance();
	auto srvHeap = rsManager->GetSRVHeap()->GetDescriptorHeap();

	list->SetDescriptorHeaps(1, &srvHeap);
	spriteRender.DrawCommand(MakeIdentity4x4());
}
