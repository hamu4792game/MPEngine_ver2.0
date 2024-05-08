#include "Target.h"
#include "ImGuiManager/ImGuiManager.h"

void Target::Initialize(const Vector3& translate) {
	auto rsManager = ResourceManager::GetInstance();
	model_ = std::make_unique<Model>();
	model_->SetModel(rsManager->FindObject3d("Target"));
	transform_.translation_ = translate;
	transform_.UpdateMatrix();
	model_->SetTransform(transform_);
	animation_ = std::make_unique<ModelAnimation>();
	animation_->Load(rsManager->FindAnimation("Target"), model_.get());
	model_->SetAnimation(animation_.get());
}

void Target::DrawImGui() {
#ifdef _DEBUG
	ImGui::DragFloat3("scale", &transform_.scale_.x, 0.1f);
	ImGui::DragFloat3("rotate", &transform_.rotation_.x, AngleToRadian(1.0f));
	ImGui::DragFloat3("position", &transform_.translation_.x, 0.1f);
	transform_.UpdateMatrix();
	model_->SetTransform(transform_);
#endif // _DEBUG
}

void Target::Update() {
	transform_.UpdateMatrix();
	model_->SetTransform(transform_);
	//animation_->Update();
}
