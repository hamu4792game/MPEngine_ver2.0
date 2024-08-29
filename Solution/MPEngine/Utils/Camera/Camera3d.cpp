#include "Camera3d.h"
#include "ImGuiManager/ImGuiManager.h"

Camera3d* Camera3d::GetInstance() {
	static Camera3d instance;
	return &instance;
}

void Camera3d::Initialize(const float& farz) {
	camera_ = Camera(farz, true);
	camera_.transform.scale_ = Vector3::one;
	
}

Matrix4x4 Camera3d::GetViewProMat() {
	if (useDebugCamera) {
		camera_.transform = debugCamera_.Update();
	}
	return camera_.GetViewProMat();
}

void Camera3d::DrawImGui() {
#ifdef _DEBUG
	ImGui::Begin("MasterCamera");
	static bool checkCameraFrag = false;
	if (ImGui::Checkbox("UseDebugCamera", &useDebugCamera)) {
		checkCameraFrag = !checkCameraFrag;
	}
	useDebugCamera = checkCameraFrag;
	ImGui::DragFloat3("translate", &camera_.transform.translation_.x, 0.1f);
	ImGui::DragFloat3("rotation", &camera_.transform.rotation_.x, 0.1f);
	if (ImGui::TreeNode("Speed")) {
		debugCamera_.DrawImGui();
		ImGui::TreePop();
	}
	ImGui::End();
#endif // _DEBUG
}
