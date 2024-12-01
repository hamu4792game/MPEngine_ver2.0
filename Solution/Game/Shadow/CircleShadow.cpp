#include "CircleShadow.h"

CircleShadow::CircleShadow() {
	
}

void CircleShadow::Initialize() {
	model_ = std::make_shared<Model>();
	model_->SetModel(ResourceManager::GetInstance()->FindObject3d("Plane"));
	model_->SetTexture(ResourceManager::GetInstance()->FindTexture("CircleShadow"));
	model_->SetTransform(transform_);
	model_->isActive_ = true;
	model_->materials.enableLighting = false;
	model_->materials.phongLighing = false;
	transform_.rotation_.x = AngleToRadian(-90.0f);
}

void CircleShadow::Update(const Vector3& playerPos) {
	
	transform_.translation_ = playerPos;
	transform_.UpdateMatrix();
	model_->SetTransform(transform_);
}
