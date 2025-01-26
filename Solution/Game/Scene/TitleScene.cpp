#include "TitleScene.h"
#include "Utils/Camera/Camera3d.h"
#include "Input/Input.h"
#include "Base/Manager/ResourceManager/ResourceManager.h"
#include "Input/Audio.h"
#include "ImGuiManager/ImGuiManager.h"
#include "TitleScene.h"

void TitleScene::Initialize() {
	titleUI_ = std::make_unique<TitleUI>();
	titleUI_->Initialize();
	auto rs = ResourceManager::GetInstance();
	Audio* titleAudio = rs->FindAudio("Battle");
	titleAudio->SoundPlayWave(true);
	titleAudio->SetVolume(0.25f);

	stage_ = std::make_unique<Stage>();
	LevelData data;
	auto leveldata = data.Load("title.json");
	stage_->LevelLoad(leveldata);

	player_ = std::make_unique<Player>();
	player_->Initialize(stage_->GetPlayerRespawnPoint());

	tutorial_ = std::make_shared<Sprite>();
	tutorial_->SetTexture(rs->FindTexture("tutorial"));
	tutorial_->SetScale(Vector2(1280.0f, 720.0f));
	tutorial_->SetIsActive(false);

	Input::GetInstance()->GetMouse()->SetLockCursorToCenter(false);
	Input::GetInstance()->GetMouse()->SetShowCursor(true);
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
	if (tutorialDraw_) {
		if (flag) {
			endRequest_ = true;
		}
	}
	if (flag) {
		tutorialDraw_ = true;
		tutorial_->SetIsActive(true);
	}

	if (input->GetKey()->TriggerKey(DIK_ESCAPE)) {
		endApplication_ = true;
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
