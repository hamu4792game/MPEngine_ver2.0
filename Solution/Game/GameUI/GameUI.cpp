#include "GameUI.h"
#include "Base/Manager/ResourceManager/ResourceManager.h"
#include "ImGuiManager/ImGuiManager.h"
#include "Utils/GlobalVariables/GlobalVariables.h"

void GameUI::Initialize(const uint32_t& collectionNum) {
	auto rsManager = ResourceManager::GetInstance();
	hud_ = std::make_shared<Sprite>();
	hud_->SetTexture(rsManager->FindTexture("ABText"));
	hud_->SetScale(Vector2(130.0f,100.0f));
	hud_->SetTranslate(Vector2(500.0f, -260.0f));

	collectionObjects_.resize(collectionNum);
	Vector2 scale = Vector2(64.0f, 64.0f);
	int index = 0;
	for (auto& object : collectionObjects_) {
		object = std::make_unique<Sprite>();
		object->SetTexture(rsManager->FindTexture("CollectionObject"));
		object->SetScale(scale);
		object->SetTranslate(Vector2(-570.0f + (scale.x * index++), 300.0f));
	}
}

void GameUI::Update(const uint32_t& collectionNum) {

	for (uint32_t index = 0; index < collectionObjects_.size(); index++) {
		// 今残ってる収集物以上なら色変える
		if (collectionNum <= index) {
			collectionObjects_.at(collectionObjects_.size() - 1u - index)->SetColor(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
		}
		else {
			collectionObjects_.at(collectionObjects_.size() - 1u - index)->SetColor(Vector4(0.0f, 0.0f, 0.0f, 1.0f));
		}
	}

#ifdef _DEBUG
	static Vector2 titlepos;
	static Vector2 scale = Vector2::one;
	ImGui::Begin("UI");
	ImGui::DragFloat2("uiTrans", &titlepos.x, 1.0f);
	ImGui::DragFloat2("uiScale", &scale.x, 1.0f);
	ImGui::End();

#endif // _DEBUG
}
