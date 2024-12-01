#include "Ground.h"
#include "ImGuiManager/ImGuiManager.h"

//void Ground::Initialize() {
//	model_ = std::make_shared<Model>();
//	model_->SetModel(ResourceManager::GetInstance()->FindObject3d("Box"));
//	model_->SetTexture(ResourceManager::GetInstance()->FindTexture("ABCD"));
//	transform_.scale_ = Vector3(100.0f, 30.0f, 100.0f);
//	transform_.translation_.y = -30.0f;
//	transform_.UpdateMatrix();
//
//	model_->transform_ = transform_;
//	collision_ = std::make_shared<AABB>();
//	collision_->Update(transform_);
//}

void Ground::Initialize(const Vector3& scale, const Vector3& rotate, const Vector3& translate) {
	model_ = std::make_shared<Model>();
	model_->SetModel(ResourceManager::GetInstance()->FindObject3d("Box"));
	model_->SetTexture(ResourceManager::GetInstance()->FindTexture("ground"));
	transform_.scale_ = scale;
	transform_.rotation_ = rotate;
	transform_.translation_ = translate;
	transform_.UpdateMatrix();

	model_->SetTransform(transform_);
	collision_ = std::make_unique<Collider>();
	collision_->Initialize(&transform_, Collider::Type::Box, "Ground");
	collision_->Update();
}

void Ground::Initialize(const WorldTransform& transform) {
	model_ = std::make_shared<Model>();
	model_->SetModel(ResourceManager::GetInstance()->FindObject3d("Box"));
	model_->SetTexture(ResourceManager::GetInstance()->FindTexture("ground"));
	transform_ = transform;
	transform_.UpdateMatrix();

	model_->SetTransform(transform_);
	collision_ = std::make_unique<Collider>();
	collision_->Initialize(&transform_, Collider::Type::Box, "Ground");
	collision_->Update();
}

void Ground::Initialize(LevelData::ObjectData& objectdata) {
	model_ = std::make_shared<Model>();
	std::string textureName = objectdata.textureName;
	model_->SetModel(ResourceManager::GetInstance()->FindObject3d("Box"));
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
		collision_->Initialize(&collTransform_, Collider::Type::Box, "Ground");
	}
	collision_->Update();
}

const WorldTransform& Ground::GetTrans() {
	transform_.UpdateMatrix();
	model_->SetUVMatrix(MakeAffineMatrix(transform_.scale_, Vector3::zero, Vector3::zero));
	collTransform_.UpdateMatrix();
	return transform_;
}

void Ground::DrawImGui() {
#ifdef _DEBUG
	ImGui::DragFloat3("scale", &transform_.scale_.x, 0.1f);
	ImGui::DragFloat3("rotate", &transform_.rotation_.x, AngleToRadian(1.0f));
	ImGui::DragFloat3("position", &transform_.translation_.x, 0.1f);

	ImGui::DragFloat("明るさ", &model_->materials.environmentCoefficient, 0.01f, 0.0f, 10.0f);

	transform_.UpdateMatrix();
	model_->SetTransform(transform_);
	collision_->Update();
#endif // _DEBUG
}
