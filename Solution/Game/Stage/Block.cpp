#include "Block.h"
#include "ImGuiManager/ImGuiManager.h"

void Block::Initialize(LevelData::ObjectData& objectdata) {
	model_ = std::make_shared<Model>();
	std::string textureName = objectdata.textureName;
	model_->SetModel(ResourceManager::GetInstance()->FindObject3d("Block"));
	model_->SetTexture(ResourceManager::GetInstance()->FindTexture(textureName));
	transform_ = objectdata.transform;
	transform_.UpdateMatrix();
	model_->SetTransform(transform_);

	// コライダー
	collision_ = std::make_unique<Collider>();
	collTransform_.parent_ = &transform_;
	collTransform_.scale_ = objectdata.collider.size;
	collTransform_.translation_ = objectdata.collider.center;
	collTransform_.UpdateMatrix();
	if (objectdata.collider.colliderType == "BOX") {
		collision_->Initialize(&collTransform_, Collider::Type::Box, name_);
	}
	collision_->Update();
}

bool Block::Update() {

	return true;
}

void Block::DrawImGui() {
#ifdef _DEBUG
	ImGui::DragFloat3("scale", &transform_.scale_.x, 0.1f);
	ImGui::DragFloat3("rotate", &transform_.rotation_.x, AngleToRadian(1.0f));
	ImGui::DragFloat3("position", &transform_.translation_.x, 0.1f);

	transform_.UpdateMatrix();
	model_->SetTransform(transform_);
	collision_->Update();
#endif // _DEBUG
}

const WorldTransform& Block::GetTransform() {
	transform_.UpdateMatrix();
	model_->SetUVMatrix(MakeAffineMatrix(Vector3::one, Vector3(0.0f, 0.0f, AngleToRadian(-90.0f)), Vector3::zero));
	collTransform_.UpdateMatrix();
	return transform_;
}

void Block::UpdateTransform() {
	transform_.UpdateMatrix();
	collTransform_.UpdateMatrix();
	collision_->Update();
	model_->SetTransform(transform_);
}
