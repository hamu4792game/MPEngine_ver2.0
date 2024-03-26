#include "ModelsControl.h"
#include "Base/Manager/ResourceManager/ResourceManager.h"
#include "ImGuiManager/ImGuiManager.h"

ModelsControl::ModelsControl() {
	auto rs = ResourceManager::GetInstance();
	targetModel_ = std::make_unique<Model>();
	targetModel_->SetModel(rs->FindObject3d("Sphere"));

	transform_.rotation_.y = AngleToRadian(180.0f);
}

void ModelsControl::Update() {

	transform_.UpdateMatrix();
	model_->SetTransform(transform_);
	targetModel_->SetColor(color_);
}
template<typename T>
void ModelButton(const T& name,Model* model) {
	if (ImGui::Button(name)) {
		auto rs = ResourceManager::GetInstance();
		model->SetModel(rs->FindObject3d(name));
	}
}
template<typename T>
void TextureButton(const T& name,Model* model) {
	if (ImGui::Button(name)) {
		auto rs = ResourceManager::GetInstance();
		model->SetTexture(rs->FindTexture(name));
	}
}

void ModelsControl::ImGuiProcess() {
#ifdef _DEBUG
	if (ImGui::BeginMenu("ModelSRT")) {
		ImGui::DragFloat3("scale", &transform_.scale_.x, 0.1f);
		ImGui::DragFloat3("rotate", &transform_.rotation_.x, AngleToRadian(1.0f));
		ImGui::DragFloat3("translate", &transform_.translation_.x, 0.01f);
		transform_.UpdateMatrix();
		ImGui::ColorEdit4("Color", &color_.x);
		ImGui::EndMenu();
	}
	if (ImGui::BeginMenu("ModelChange")) {
		ModelButton("Box",targetModel_.get());
		ModelButton("Plane",targetModel_.get());
		ModelButton("Sphere", targetModel_.get());
		ModelButton("Axis", targetModel_.get());
		ModelButton("MultiMesh", targetModel_.get());
		ModelButton("Teapot", targetModel_.get());
		ImGui::EndMenu();
	}
	if (ImGui::BeginMenu("TextureChange")) {
		TextureButton("white2x2", targetModel_.get());
		TextureButton("UVChecker", targetModel_.get());
		TextureButton("Ground", targetModel_.get());
		TextureButton("Circle", targetModel_.get());
		TextureButton("MonsterBall", targetModel_.get());
		TextureButton("CheckerBoard", targetModel_.get());
		ImGui::EndMenu();
	}
	

#endif // _DEBUG
}
