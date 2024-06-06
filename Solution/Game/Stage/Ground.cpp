#include "Ground.h"
#include "externals/imgui/imgui.h"

//void Ground::Initialize() {
//	model_ = std::make_shared<Model>();
//	model_->SetModel(ResourceManager::GetInstance()->FindObject3d("Box"));
//	model_->SetTexture(ResourceManager::GetInstance()->FindTexture("ABCD"));
//	transform_.scale_ = Vector3(100.0f, 30.0f, 100.0f);
//	transform_.translation_.y = -30.0f;
//	transform_.UpdateMatrix();
//
//	model_->transform_ = transform_;
//	collision_ = std::make_shared<AABB>();
//	collision_->Update(transform_);
//}

void Ground::Initialize(const Vector3& scale, const Vector3& rotate, const Vector3& translate) {
	model_ = std::make_shared<Model>();
	model_->SetModel(ResourceManager::GetInstance()->FindObject3d("Box"));
	model_->SetTexture(ResourceManager::GetInstance()->FindTexture("Ground"));
	transform_.scale_ = scale;
	transform_.rotation_ = rotate;
	transform_.translation_ = translate;
	transform_.UpdateMatrix();

	model_->SetTransform(transform_);
	collision_ = std::make_shared<AABB>();
	collision_->Update(transform_);
}

void Ground::Initialize(const WorldTransform& transform) {
	model_ = std::make_shared<Model>();
	model_->SetModel(ResourceManager::GetInstance()->FindObject3d("Box"));
	model_->SetTexture(ResourceManager::GetInstance()->FindTexture("Ground"));
	transform_ = transform;
	transform_.UpdateMatrix();

	model_->SetTransform(transform_);
	collision_ = std::make_shared<AABB>();
	collision_->Update(transform_);
}

const WorldTransform& Ground::GetTrans() {
	transform_.UpdateMatrix();
	return transform_;
}

void Ground::DrawImGui() {
	ImGui::DragFloat3("scale", &transform_.scale_.x, 0.1f);
	ImGui::DragFloat3("rotate", &transform_.rotation_.x, AngleToRadian(1.0f));
	ImGui::DragFloat3("position", &transform_.translation_.x, 0.1f);
	transform_.UpdateMatrix();
	model_->SetTransform(transform_);
	collision_->Update(transform_);
}
