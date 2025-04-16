#include "Target.h"
#include "ImGuiManager/ImGuiManager.h"

void Target::Initialize(const Vector3& translate) {
	auto rsManager = ResourceManager::GetInstance();
	model_ = std::make_unique<Model>();
	model_->SetModel(rsManager->FindObject3d("Target"));
	model_->SetTexture(rsManager->FindTexture("white2x2"));
	transform_.translation_ = translate;
	transform_.UpdateMatrix();
	model_->SetTransform(transform_);
	model_->materials.color = Vector4(1.0f, 1.0f, 0.0f, 1.0f);
	model_->materials.environmentCoefficient = 0.21f;

	plane_ = std::make_unique<Model>();
	plane_->SetModel(rsManager->FindObject3d("Plane"));
	plane_->SetTexture(rsManager->FindTexture("Circle"));
	plane_->IsUseBillboard(true);
	plane_->materials.color = Vector4(1.0f, 1.0f, 0.0f, 1.0f);
	planeTransform_.scale_ = Vector3::one * 2.0f;
	planeTransform_.translation_ = transform_.translation_;
	plane_->materials.shininess = 0.5f;
	plane_->SetBlendType(BlendMode::Screen);

}

void Target::DrawImGui() {
#ifdef _DEBUG
	ImGui::DragFloat3("scale", &transform_.scale_.x, 0.1f);
	ImGui::DragFloat3("rotate", &transform_.rotation_.x, AngleToRadian(1.0f));
	ImGui::DragFloat3("position", &transform_.translation_.x, 0.1f);
	ImGui::DragFloat("明るさ", &model_->materials.environmentCoefficient, 0.01f, 0.0f, 1.0f);
	ImGui::DragFloat4("color", &model_->materials.color.x, 0.01f, 0.0f, 1.0f);
	ImGui::DragFloat3("planescale", &planeTransform_.scale_.x, 0.1f);
	ImGui::DragFloat3("planerotate", &planeTransform_.rotation_.x, AngleToRadian(1.0f));
	ImGui::DragFloat3("planeposition", &planeTransform_.translation_.x, 0.1f);
	ImGui::DragFloat("plane明るさ", &plane_->materials.environmentCoefficient, 0.01f, 0.0f, 1.0f);
	ImGui::DragFloat4("planecolor", &plane_->materials.color.x, 0.01f, 0.0f, 1.0f);
	transform_.UpdateMatrix();
	model_->SetTransform(transform_);
#endif // _DEBUG
}

void Target::Update() {
	transform_.UpdateMatrix();
	model_->SetTransform(transform_);
	planeTransform_.UpdateMatrix();
	plane_->SetTransform(planeTransform_);
}
