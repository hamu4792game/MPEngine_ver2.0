#include "CircleShadow.h"
#include <algorithm>

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

void CircleShadow::Update(const Vector3& hitpoint, const float& distance) {
	float sc = (distance - 1.0f) / (8.0f - 1.0f);;
	sc = Lerp(0.5f, 1.5f, sc);
	sc = std::clamp(sc, 0.0f, 1.5f);

	transform_.scale_ = Vector3(sc, sc, 1.0f);
	transform_.translation_ = hitpoint;
	transform_.UpdateMatrix();
	model_->SetTransform(transform_);
}
