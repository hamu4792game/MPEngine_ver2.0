#include "CollectionObject.h"
#include "ImGuiManager/ImGuiManager.h"

CollectionObject::CollectionObject() {
	name_ = "CollectionObject";
}

void CollectionObject::Initialize(LevelData::ObjectData& objectdata) {
	model_ = std::make_shared<Model>();
	std::string textureName = objectdata.textureName;
	model_->SetModel(ResourceManager::GetInstance()->FindObject3d("Target"));
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

	isAlive_ = true;
	isAnimation_ = false;
	time_ = 0.0f;

	dustParticle_ = std::make_shared<DustParticle>();
	dustParticle_->Initialize(transform_.GetPosition());

}

bool CollectionObject::Update() {
	if (!isAlive_) {
		model_->isActive_ = false;
	}
	Animation();
	UpdateTransform();
	dustParticle_->Update(transform_.GetPosition());

	return isAlive_;
}

void CollectionObject::DrawImGui() {
#ifdef _DEBUG
	ImGui::DragFloat3("scale", &transform_.scale_.x, 0.1f);
	ImGui::DragFloat3("rotate", &transform_.rotation_.x, AngleToRadian(1.0f));
	ImGui::DragFloat3("position", &transform_.translation_.x, 0.1f);
#endif // _DEBUG
}

const WorldTransform& CollectionObject::GetTransform() {
	transform_.UpdateMatrix();
	collTransform_.UpdateMatrix();
	return transform_;
}

void CollectionObject::UpdateTransform() {
	transform_.UpdateMatrix();
	collTransform_.UpdateMatrix();
	collision_->Update();
	model_->SetTransform(transform_);
}

void CollectionObject::Animation() {
	transform_.rotation_.y += AngleToRadian(1.0f);
	if (isAnimation_) {
		time_++;
		transform_.translation_.y += 0.5f;
		if (time_ >= 60.0f) {
			isAlive_ = false;
		}
	}
}
