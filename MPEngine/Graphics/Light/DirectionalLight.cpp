#include "DirectionalLight.h"
#include "ImGuiManager/ImGuiManager.h"

DirectionalLight::DirectionalLight() {
	cDirectionLight_->color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	cDirectionLight_->direction = Vector3(0.0f, -1.0f, 0.0f);
	cDirectionLight_->intensity = 1.0f;
}

void DirectionalLight::Update() {
#ifdef _DEBUG
	ImGui::Begin("DirectionLight");
	ImGui::DragFloat3("Direction", &cDirectionLight_->direction.x, 0.01f);
	ImGui::DragFloat("intensity", &cDirectionLight_->intensity, 0.01f);
	ImGui::End();
#endif // _DEBUG
}
