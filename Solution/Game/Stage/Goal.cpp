#include "Goal.h"
#include "ImGuiManager/ImGuiManager.h"

Goal::Goal() {
	model_ = std::make_unique<Model>();
	model_->SetModel(ResourceManager::GetInstance()->FindObject3d("Goal"));
	collision_ = std::make_unique<Collider>();
}

void Goal::Initialize(const Vector3& scale, const Vector3& rotate, const Vector3& translate) {
	transform_.scale_ = scale;
	transform_.rotation_ = rotate;
	transform_.translation_ = translate;
	transform_.UpdateMatrix();

	model_->SetTransform(transform_);
	collision_->Initialize(&transform_, Collider::Type::Box, "Goal");
	collision_->Update();
}

void Goal::Initialize(LevelData::ObjectData& objectdata) {
	transform_ = objectdata.transform;
	transform_.UpdateMatrix();
	model_->SetTransform(transform_);

	// コライダー
	collTransform_.parent_ = &transform_;
	collTransform_.scale_ = objectdata.collider.size;
	collTransform_.translation_ = objectdata.collider.center;
	collTransform_.UpdateMatrix();
	if (objectdata.collider.colliderType == "BOX") {
		collision_->Initialize(&collTransform_, Collider::Type::Box, "Goal");
	}
	collision_->Update();
}

void Goal::Update() {
	transform_.UpdateMatrix();
	model_->SetTransform(transform_);
	collTransform_.UpdateMatrix();
	collision_->Update();
}

const WorldTransform& Goal::GetTrans() {
	transform_.UpdateMatrix();
	collTransform_.UpdateMatrix();
	return transform_;
}

void Goal::DrawImGui() {
#ifdef _DEBUG
	ImGui::DragFloat3("scale", &transform_.scale_.x, 0.1f);
	ImGui::DragFloat3("rotate", &transform_.rotation_.x, AngleToRadian(1.0f));
	ImGui::DragFloat3("position", &transform_.translation_.x, 0.1f);
	transform_.UpdateMatrix();
	model_->SetTransform(transform_);
	collision_->Update();
#endif // _DEBUG
}
