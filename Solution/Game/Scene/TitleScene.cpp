#include "TitleScene.h"
#include "Utils/Camera/Camera3d.h"
#include "Input/Input.h"
#include "Base/Manager/ResourceManager/ResourceManager.h"
#include "Input/Audio.h"
#include "ImGuiManager/ImGuiManager.h"

void TitleScene::Initialize() {
	titleUI_ = std::make_unique<TitleUI>();
	titleUI_->Initialize();
	auto rs = ResourceManager::GetInstance();
	Audio* titleAudio = rs->FindAudio("Title");
	titleAudio->SoundPlayWave(true);

	stage_ = std::make_unique<Stage>();
	stage_->Initialize("TitleStage");

	player_ = std::make_unique<Player>();
	player_->Initialize();

	cameraTransform_.translation_ = Vector3(0.0f, 300.0f, -51.0f);
	cameraTransform_.rotation_.x = AngleToRadian(90.0f);
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

	titleUI_->Update();
	stage_->Update();
	//player_->TitleUpdate();

	for (auto coll : stage_->GetCollision()) {
		//player_->OnCollisionStage(coll);
	}

	cameraTransform_.UpdateMatrix();
	Camera3d::GetInstance()->SetTransform(cameraTransform_);
}

void TitleScene::DrawImGui() {
#ifdef _DEBUG
	ImGui::Begin("camera");
	ImGui::DragFloat3("translate", &cameraTransform_.translation_.x, 0.1f);
	ImGui::DragFloat3("rotate", &cameraTransform_.rotation_.x, AngleToRadian(1.0f));
	ImGui::End();
#endif // _DEBUG
}

