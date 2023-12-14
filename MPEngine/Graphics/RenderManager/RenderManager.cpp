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
	auto projectionMatrix = MakeOrthographicMatrix(-float(1280.0f / 2.0f), float(720.0f / 2.0f),
		float(1280.0f / 2.0f), -float(720.0f / 2.0f), 0.01f, 1000.0f);
	spriteRender.DrawCommand(projectionMatrix);
}
