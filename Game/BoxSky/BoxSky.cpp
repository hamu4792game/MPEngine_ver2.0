#include "BoxSky.h"

void BoxSky::Initialize() {
	model_ = std::make_shared<Model>();
	model_->SetModel(ResourceManager::GetInstance()->FindObject3d("BoxSky"));
	transform_.scale_ = Vector3(300.0f, 300.0f, 300.0f);
	transform_.UpdateMatrix();

	model_->transform_ = transform_;
}
