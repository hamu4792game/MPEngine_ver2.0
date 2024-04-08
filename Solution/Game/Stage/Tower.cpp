#include "Tower.h"
#include "externals/imgui/imgui.h"

void Tower::Initialize(const Vector3& scale, const Vector3& rotate, const Vector3& translate) {
	model_ = std::make_shared<Model>();
	model_->SetModel(ResourceManager::GetInstance()->FindObject3d("Box"));
	model_->SetTexture(ResourceManager::GetInstance()->FindTexture("ABCD"));
	transform_.scale_ = scale;
	transform_.rotation_ = rotate;
	transform_.translation_ = translate;
	transform_.UpdateMatrix();

	model_->SetTransform(transform_);
}

void Tower::DrawImGui() {
	ImGui::DragFloat3("scale", &transform_.scale_.x, 0.1f);
	ImGui::DragFloat3("rotate", &transform_.rotation_.x, AngleToRadian(1.0f));
	ImGui::DragFloat3("position", &transform_.translation_.x, 0.1f);
	model_->SetTransform(transform_);
}
