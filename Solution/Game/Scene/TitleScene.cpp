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
	Audio* titleAudio = rs->FindAudio("Battle");
	titleAudio->SoundPlayWave(true);

	stage_ = std::make_unique<Stage>();
	LevelData data;
	auto leveldata = data.Load("title.json");
	stage_->LevelLoad(leveldata);

	player_ = std::make_unique<PlayerManager>();
	player_->Initialize(stage_->GetPlayerRespawnPoint());

}

void TitleScene::Finalize() {
	
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
	player_->PostUpdate();

	Camera3d::GetInstance()->SetTransform(stage_->GetCameraRespawnPoint());
}

void TitleScene::SecondUpdate() {

}

void TitleScene::DrawImGui() {
#ifdef _DEBUG
	
#endif // _DEBUG
}

