#include "Tower.h"
#include "externals/imgui/imgui.h"

void Tower::Initialize(const Vector3& scale, const Vector3& rotate, const Vector3& translate) {
	model_ = std::make_shared<Model>();
	model_->SetModel(ResourceManager::GetInstance()->FindObject3d("Box"));
	model_->SetTexture(ResourceManager::GetInstance()->FindTexture("ABCD"));
	transform_.scale_ = scale;
	transform_.rotation_ = rotate;
	transform_.translation_ = translate;
	transform_.UpdateMatrix();

	model_->SetTransform(transform_);
}

void Tower::Initialize(LevelData::ObjectData& objectdata) {
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
		collision_->Initialize(&collTransform_, Collider::Type::Box, "Tower");
	}
	collision_->Update();
}

const WorldTransform& Tower::GetTrans() {
	transform_.UpdateMatrix();
	model_->SetUVMatrix(MakeAffineMatrix(-transform_.scale_, Vector3::zero, Vector3::zero));
	collTransform_.UpdateMatrix();
	return transform_;
}

void Tower::DrawImGui() {
#ifdef _DEBUG
	ImGui::DragFloat3("scale", &transform_.scale_.x, 0.1f);
	ImGui::DragFloat3("rotate", &transform_.rotation_.x, AngleToRadian(1.0f));
	ImGui::DragFloat3("position", &transform_.translation_.x, 0.1f);
	model_->SetTransform(transform_);
#endif // _DEBUG
}
