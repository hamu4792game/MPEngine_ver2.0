#include "DirectionalLight.h"
#include "ImGuiManager/ImGuiManager.h"
#include "Utils/GlobalVariables/GlobalVariables.h"

DirectionalLight::DirectionalLight() {
	auto gv = GlobalVariables::GetInstance();
	std::string groupname = "Utils";
	gv->CreateGroup(groupname);
	gv->AddItem(groupname, "DirectionalLight_Direction", cDirectionLight_->direction);
	gv->AddItem(groupname, "DirectionalLight_Intensity", cDirectionLight_->intensity);
	
	gv->LoadFile(groupname);

	cDirectionLight_->color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	cDirectionLight_->direction = gv->GetVector3Value(groupname, "DirectionalLight_Direction");
	cDirectionLight_->intensity = gv->GetFloatValue(groupname, "DirectionalLight_Intensity");
}

void DirectionalLight::Update() {
#ifdef _DEBUG
	ImGui::Begin("DirectionLight");
	ImGui::DragFloat4("Color", &cDirectionLight_->color.x, 0.01f);
	ImGui::DragFloat3("Direction", &cDirectionLight_->direction.x, 0.01f);
	ImGui::DragFloat("intensity", &cDirectionLight_->intensity, 0.01f);
	if (ImGui::Button("Save")) {
		auto gv = GlobalVariables::GetInstance();
		std::string groupname = "Utils";
		gv->SetValue(groupname, "DirectionalLight_Direction", cDirectionLight_->direction);
		gv->SetValue(groupname, "DirectionalLight_Intensity", cDirectionLight_->intensity);

		gv->SaveFile(groupname);
	}
	ImGui::End();
#endif // _DEBUG

	cDirectionLight_->direction = Normalize(cDirectionLight_->direction);

}
