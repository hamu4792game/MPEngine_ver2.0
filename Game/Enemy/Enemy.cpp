#include "Enemy.h"
#include "externals/imgui/imgui.h"
#include "Base/Manager/ResourceManager/ResourceManager.h"

void Enemy::Initialize() {
#pragma region Models
	auto rsManager = ResourceManager::GetInstance();
	for (uint32_t index = 0u; index < Parts::kMaxParts; index++) {
		models_.emplace_back(std::make_shared<Model>());
	}
	partsTrans_.resize(models_.size());
	models_.at(Parts::Body)->SetModel(rsManager->FindObject3d("PlayerBody"));
	models_.at(Parts::Head)->SetModel(rsManager->FindObject3d("PlayerHead"));
#pragma endregion
	collision_ = std::make_shared<AABB>();

	TransformUpdate();
}

void Enemy::Update() {
	DrawImGui();
	TransformUpdate();
}

void Enemy::DrawImGui() {
#ifdef _DEBUG
	ImGui::Begin("Enemy");
	if (ImGui::TreeNode("Transform")) {
		ImGui::DragFloat3("position", &transform_.translation_.x, 0.1f);
		ImGui::DragFloat3("rotate", &transform_.rotation_.x, AngleToRadian(1.0f));
		ImGui::DragFloat3("scale", &transform_.scale_.x, 0.1f);
		ImGui::TreePop();
	}
	ImGui::End();
#endif // _DEBUG
}

void Enemy::TransformUpdate() {
	transform_.UpdateMatrix();
	for (auto& transform : partsTrans_) {
		transform.UpdateMatrix();
	}
	for (uint32_t index = 0u; index < Parts::kMaxParts; index++) {
		models_.at(index)->transform_ = partsTrans_.at(index);
	}
	collision_->Update(transform_);
}
