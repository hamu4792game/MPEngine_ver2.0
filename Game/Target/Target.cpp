#include "Target.h"
#include "externals/imgui/imgui.h"

void Target::Initialize(const Vector3& translate) {
	auto rsManager = ResourceManager::GetInstance();
	model_ = std::make_shared<Model>();
	model_->SetModel(rsManager->FindObject3d("Box"));
	transform_.translation_ = translate;
	transform_.UpdateMatrix();
	model_->transform_ = transform_;
}

void Target::DrawImGui() {
#ifdef _DEBUG
	ImGui::DragFloat3("scale", &transform_.scale_.x, 0.1f);
	ImGui::DragFloat3("rotate", &transform_.rotation_.x, AngleToRadian(1.0f));
	ImGui::DragFloat3("position", &transform_.translation_.x, 0.1f);
	transform_.UpdateMatrix();
	model_->transform_ = transform_;
#endif // _DEBUG
}
