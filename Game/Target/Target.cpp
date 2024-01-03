#include "Target.h"

void Target::Initialize(const Vector3& translate) {
	auto rsManager = ResourceManager::GetInstance();
	model_ = std::make_shared<Model>();
	model_->SetModel(rsManager->FindObject3d("Box"));
	transform_.translation_ = translate;
	transform_.UpdateMatrix();
	model_->transform_ = transform_;
}
