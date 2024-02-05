#include "ModelsControl.h"
#include "Base/Manager/ResourceManager/ResourceManager.h"
#include "ImGuiManager/ImGuiManager.h"

ModelsControl::ModelsControl() {
	auto rs = ResourceManager::GetInstance();
	targetModel_ = std::make_unique<Model>();
	targetModel_->SetModel(rs->FindObject3d("Sphere"));
}

void ModelsControl::Update() {

	transform_.UpdateMatrix();
	targetModel_->transform_ = transform_;
}

void ModelsControl::ImGuiProcess() {
#ifdef _DEBUG
	if (ImGui::BeginMenu("Model")) {
		ImGui::DragFloat3("scale", &transform_.scale_.x, 0.1f);
		ImGui::DragFloat3("rotate", &transform_.rotation_.x, AngleToRadian(1.0f));
		ImGui::DragFloat3("translate", &transform_.translation_.x, 0.01f);
		transform_.UpdateMatrix();
		ImGui::EndMenu();
	}
#endif // _DEBUG
}
