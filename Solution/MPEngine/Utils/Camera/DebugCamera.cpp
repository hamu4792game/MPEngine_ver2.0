#include "DebugCamera.h"
#include "MPEngine/Input/Input.h"
#include <numbers>
#include "ImGuiManager/ImGuiManager.h"

DebugCamera::DebugCamera() {
	transform_.scale_ = Vector3::one;
}

WorldTransform DebugCamera::Update() {
	auto mouseInput = Input::GetInstance()->GetMouse();

	Vector2 mouseMove = mouseInput->GetMouseMove();
	if (mouseInput->PressMouse(MouseInput::RIGHT)) {
		float rot = AngleToRadian(1.0f);
		transform_.rotation_.x += rot * mouseMove.y * rotSpeed_;
		transform_.rotation_.y += rot * mouseMove.x * rotSpeed_;
	}
	else if (mouseInput->PressMouse(MouseInput::CENTER)) {
		Matrix4x4 rotMat = MakeRotateMatrix(transform_.rotation_);
		Vector3 cameraX = GetXAxis(rotMat) * static_cast<float>(-mouseMove.x) * moveSpeed_;
		Vector3 cameraY = GetYAxis(rotMat) * static_cast<float>(mouseMove.y) * moveSpeed_;
		transform_.translation_ += cameraX + cameraY;
	}

	float wheel = mouseInput->GetMouseWheel();
	if (wheel != 0.0f) {
		Matrix4x4 rotMat = MakeRotateMatrix(transform_.rotation_);
		Vector3 cameraZ = GetZAxis(rotMat) * ((wheel / 120.0f) * offsetSpeed_);
		transform_.translation_ += cameraZ;
	}
	transform_.UpdateMatrix();
	return transform_;
}

void DebugCamera::DrawImGui() {
#ifdef _DEBUG
	ImGui::DragFloat("RotateSpeed", &rotSpeed_, 0.01f, 0.0f, 1.0f);
	ImGui::DragFloat("MoveSpeed", &moveSpeed_, 0.01f, 0.0f, 1.0f);
	ImGui::DragFloat("OffsetSpeed", &offsetSpeed_, 0.01f, 0.0f, 1.0f);
#endif // _DEBUG
}
