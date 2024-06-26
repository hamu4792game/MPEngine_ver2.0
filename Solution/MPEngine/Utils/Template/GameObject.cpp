#include "GameObject.h"

void GameObject::Initialize(LevelData::ObjectData& objectdata) {
	model_ = std::make_unique<Model>();
	std::string modelName = objectdata.fileName;
	model_->SetModel(ResourceManager::GetInstance()->FindObject3d("Box"));
	model_->SetTexture(ResourceManager::GetInstance()->FindTexture("Ground"));
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
		collision_->Initialize(&collTransform_, Collider::Type::Box);
	}
	else if (objectdata.collider.colliderType == "SPHERE")
	collision_->Update();
}
