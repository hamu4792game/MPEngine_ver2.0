#include "GameUI.h"
#include "Base/Manager/ResourceManager/ResourceManager.h"
#include "externals/imgui/imgui.h"

void GameUI::Initialize() {
	auto rsManager = ResourceManager::GetInstance();
	hud_ = std::make_shared<Sprite>();
	hud_->SetTexture(rsManager->FindTexture("ABText"));
	hud_->SetScale(Vector2(130.0f,100.0f));
	hud_->SetTranslate(Vector2(500.0f, -260.0f));

}

void GameUI::Update() {
#ifdef _DEBUG
	static Vector2 titlepos;
	static Vector2 scale = Vector2::one;
	ImGui::Begin("Title");
	ImGui::DragFloat2("titleTrans", &titlepos.x, 1.0f);
	ImGui::DragFloat2("titleScale", &scale.x, 1.0f);
	ImGui::End();

#endif // _DEBUG
}
