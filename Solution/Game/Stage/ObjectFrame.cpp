#include "ObjectFrame.h"

void ObjectFrame::Initialize(LevelData::ObjectData& objectdata) {
	model_ = std::make_shared<Model>();
	std::string textureName = objectdata.textureName;
	model_->SetModel(ResourceManager::GetInstance()->FindObject3d(name_));
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

const WorldTransform& ObjectFrame::GetTransform() {
	transform_.UpdateMatrix();
	collTransform_.UpdateMatrix();
	return transform_;
}

void ObjectFrame::UpdateTransform() {
	transform_.UpdateMatrix();
	collTransform_.UpdateMatrix();
	collision_->Update();
	model_->SetTransform(transform_);
}
