#include "Ground.h"

void Ground::Initialize() {
	model_ = std::make_shared<Model>();
	model_->SetModel(ResourceManager::GetInstance()->FindObject3d("Box"));
	model_->SetTexture(ResourceManager::GetInstance()->FindTexture("ABCD"));
	transform_.scale_ = Vector3(100.0f, 30.0f, 100.0f);
	transform_.translation_.y = -30.0f;
	transform_.UpdateMatrix();

	model_->transform_ = transform_;
}
