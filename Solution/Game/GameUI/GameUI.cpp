#include "GameUI.h"
#include "Base/Manager/ResourceManager/ResourceManager.h"
#include "ImGuiManager/ImGuiManager.h"
#include "Utils/GlobalVariables/GlobalVariables.h"

void GameUI::Initialize(const uint32_t& collectionNum) {
	auto rsManager = ResourceManager::GetInstance();
	auto global = GlobalVariables::GetInstance();
	global->CreateGroup("GameUI");

	

	int index = 0;
	for (auto& hud : huds_) {
		hud = std::make_shared<Sprite>();
		hud->SetTexture(rsManager->FindTexture(textureNames.at(index)));
		hud->SetScale(spriteParam.at(index).scale);
		hud->SetTranslate(spriteParam.at(index).translate);
		hud->SetUVSize(spriteParam.at(index).uvScale);
		hud->SetUVPosition(spriteParam.at(index).uvTranslate);
		index++;
	}

	collectionObjects_.resize(collectionNum);
	Vector2 scale = Vector2(64.0f, 64.0f);
	index = 0;
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
	ImGui::Begin("UI");
	int index = 0;
	for (auto& hud : huds_) {
		if (ImGui::TreeNode(textureNames.at(index).c_str())) {
			ImGui::DragFloat2("Scale", &spriteParam.at(index).scale.x, 1.0f);
			ImGui::DragFloat2("Translate", &spriteParam.at(index).translate.x, 1.0f);
			ImGui::DragFloat2("UVScale", &spriteParam.at(index).uvScale.x, 0.01f);
			ImGui::DragFloat2("UVTranslate", &spriteParam.at(index).uvTranslate.x, 0.01f);
			ImGui::TreePop();
		}
		hud->SetScale(spriteParam.at(index).scale);
		hud->SetTranslate(spriteParam.at(index).translate);
		hud->SetUVSize(spriteParam.at(index).uvScale);
		hud->SetUVPosition(spriteParam.at(index).uvTranslate);
		index++;
	}
	ImGui::End();

#endif // _DEBUG
}
