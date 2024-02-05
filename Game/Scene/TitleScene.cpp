#include "TitleScene.h"
#include "Utils/Camera/Camera3d.h"
#include "Input/Input.h"
#include "Base/Manager/ResourceManager/ResourceManager.h"
#include "Input/Audio.h"
#include "ImGuiManager/ImGuiManager.h"

void TitleScene::Initialize() {
	
	auto rs = ResourceManager::GetInstance();
	
	monsterBall_ = std::make_unique<Model>();
	monsterBall_->SetModel(rs->FindObject3d("Sphere"));
	monsterBall_->SetTexture(rs->FindTexture("MonsterBall"));
	monsterBall_->isActive_ = false;

	ballTrans_.rotation_.y = AngleToRadian(90.0f);
	cameraTransform_.translation_ = Vector3(0.0f, 0.0f, -20.0f);

	model_ = std::make_unique<ModelsControl>();

}

void TitleScene::Finalize() {
	//auto rs = ResourceManager::GetInstance();
	//std::shared_ptr<Audio> titleAudio = rs->FindAudio("Title");
	//titleAudio->SoundStop();
}

void TitleScene::Update() {
	DrawImGui();
	auto input = Input::GetInstance();
	// 終了処理
	bool flag = false;
	if (input->GetKey()->TriggerKey(DIK_SPACE)) {
		flag = true;
	}
	if (input->GetPad()->GetPadConnect()) {
		if (input->GetPad()->GetPadButtonDown(XINPUT_GAMEPAD_A)) {
			flag = true;
		}
	}
	if (flag) {
		endRequest_ = true;
	}

	ballTrans_.UpdateMatrix();
	monsterBall_->transform_ = ballTrans_;

	model_->Update();

	cameraTransform_.UpdateMatrix();
	Camera3d::GetInstance()->SetTransform(cameraTransform_);
}

void TitleScene::DrawImGui() {
#ifdef _DEBUG
	ImGui::Begin("State", nullptr, ImGuiWindowFlags_MenuBar);
	if (ImGui::BeginMenuBar()) {
		if (ImGui::BeginMenu("Camera")) {
			ImGui::DragFloat3("translate", &cameraTransform_.translation_.x, 0.1f);
			ImGui::DragFloat3("rotate", &cameraTransform_.rotation_.x, AngleToRadian(1.0f));
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Sphere")) {
			ImGui::DragFloat3("scale", &ballTrans_.scale_.x, 0.1f);
			ImGui::DragFloat3("rotate", &ballTrans_.rotation_.x, AngleToRadian(1.0f));
			ImGui::DragFloat3("translate", &ballTrans_.translation_.x, 0.01f);
			ImGui::EndMenu();
		}
		model_->ImGuiProcess();

		ImGui::EndMenuBar();
	}
	ImGui::End();
#endif // _DEBUG
}

