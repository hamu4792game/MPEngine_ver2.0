#include "TitleScene.h"
#include "Utils/Camera/Camera3d.h"
#include "Input/Input.h"
#include "Base/Manager/ResourceManager/ResourceManager.h"
#include "Input/Audio.h"

void TitleScene::Initialize() {
	titleUI_ = std::make_unique<TitleUI>();
	titleUI_->Initialize();
	auto rs = ResourceManager::GetInstance();
	std::shared_ptr<Audio> titleAudio = rs->FindAudio("Title");
	titleAudio->SoundPlayWave(true);
}

void TitleScene::Finalize() {
	auto rs = ResourceManager::GetInstance();
	std::shared_ptr<Audio> titleAudio = rs->FindAudio("Title");
	titleAudio->SoundStop();
}

void TitleScene::Update() {
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

	cameraTransform_.UpdateMatrix();
	Camera3d::GetInstance()->SetTransform(cameraTransform_);
}

void TitleScene::DrawImGui() {

}
