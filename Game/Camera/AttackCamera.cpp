#include "AttackCamera.h"
#include "ImGuiManager/ImGuiManager.h"

void AttackCamera::Initialize(const WorldTransform& transform) {
	transform_ = transform;
	offset_ = Vector3(0.0f, 2.0f, -20.0f);
}

void AttackCamera::Update(const Vector3& lockon) {
	//transform_.rotation_.y = FindAngle(lockon)
	transform_.UpdateMatrix();
}

void AttackCamera::DrawImGui() {
#ifdef _DEBUG
	ImGui::Begin("camera");
	ImGui::DragFloat3("position", &transform_.translation_.x, 0.1f);
	ImGui::DragFloat3("rotate", &transform_.rotation_.x, 0.1f);
	ImGui::DragFloat3("scale", &transform_.scale_.x, 0.1f);
	ImGui::DragFloat3("offset", &offset_.x, 0.1f);
	ImGui::End();
#endif // _DEBUG
}
