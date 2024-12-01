#include "BattleScene.h"
#include "Utils/Camera/Camera3d.h"
#include "Input/Input.h"
#include "ImGuiManager/ImGuiManager.h"
#include "ResourceManager/ResourceManager.h"
#include "Input/Audio.h"
#include "MPEngine/Graphics/RenderManager/RenderManager.h"
#include "Utils/Tool/LevelData.h"

void BattleScene::Initialize() {
	stage_ = std::make_unique<Stage>();
	stage_->initialize();
	LevelData data;
	auto leveldata = data.Load("stage.json");
	stage_->LevelLoad(leveldata);

	player_ = std::make_unique<PlayerManager>();
	player_->Initialize(stage_->GetPlayerRespawnPoint());

	lockOn_ = std::make_unique<LockOn>();
	lockOn_->Initialize();
	
	pointOfGazeSearch_ = std::make_unique<PointOfGazeSearch>();
	pointOfGazeSearch_->Initialize();


	gameUI_ = std::make_unique<GameUI>();
	gameUI_->Initialize();

	skybox_ = std::make_unique<SkyBox>();
	skybox_->SetTexture(ResourceManager::GetInstance()->FindTexture("Airport"));
	WorldTransform trans(Vector3(300.0f, 300.0f, 300.0f), Vector3::zero, Vector3::zero);
	skybox_->SetTransform(trans);

	pause_ = std::make_unique<Pause>();

	// 音
	auto rs = ResourceManager::GetInstance();
	Audio* bgm = rs->FindAudio("Battle");
	bgm->SoundPlayWave(true);
	bgm->SetVolume(0.02f);

}

void BattleScene::Finalize() {
	
}

void BattleScene::Update() {

	DrawImGui();

	if (Input::GetInstance()->GetKey()->TriggerKey(DIK_ESCAPE)) {
		isPause_ = !isPause_;
	}

	// ポーズ状態の時
	if (isPause_) {
		PauseMenu();

		//　早期リターン
		return;
	}

	stage_->Update();
	std::list<std::shared_ptr<Target>> listData(stage_->GetTargets());
	std::list<std::shared_ptr<Ground>> groundListData(stage_->GetGrounds());

	// ターゲットの更新
	lockOn_->Update(listData);
	WorldTransform* handle = nullptr;
	handle = lockOn_->GetTargetTrans();
	//Vector3* target = pointOfGazeSearch_->Update(groundListData, player_->GetTransform().GetPosition(), cameraTransform_);
	//if (target) {
	//	WorldTransform tg = WorldTransform(Vector3::one, Vector3::zero, *target);
	//	handle = &tg;
	//}
	
	player_->SetTargetTrans(handle);


	time_ = std::min(time_, 300.0f);

	player_->Update();
	
	static Vector3 scale = Vector3(300.0f, 300.0f, 300.0f);
	WorldTransform trans(scale, Vector3::zero, player_->GetTransform().GetPosition());
	skybox_->SetTransform(trans);
	skybox_->Update();

	bool gameclear = false;
	for (auto& coll : stage_->GetCollision()) {
		gameclear = player_->OnCollisionStage(*coll);
		Vector3 hitPoint;
		if (player_->OnCollisionDownRayToStage(*coll, hitPoint)) {
			
		}
		if (gameclear) { break; }
	}
	stage_->OnCollition(*player_->GetCollision());
	gameUI_->Update();

	uint32_t num = player_->GetEffectNumber();
	if (num == 1u) {
		RadialBlur::GetInstance()->SetUsed(true);
	}
	else {
		RadialBlur::GetInstance()->SetUsed(false);
	}
	
	if (num == 2u) {
		Grayscale::GetInstance()->SetUsed(true);
	}
	else {
		Grayscale::GetInstance()->SetUsed(false);
	}

	if (Input::GetInstance()->GetKey()->PressKey(DIK_P) || gameclear) {
		endRequest_ = true;
	}

	SecondUpdate();
}

void BattleScene::SecondUpdate() {
	// ここでレイキャストする
	Camera3d* camera = Camera3d::GetInstance();
	cameraTransform_ = player_->PostUpdate();
	// 一旦うまくいかないのでしない
	for (auto& coll : stage_->GetCollision()) {
		cameraTransform_ = player_->OnCollisionCameraToStage(*coll);
	}
	camera->SetTransform(cameraTransform_);

}

void BattleScene::DrawImGui() {
#ifdef _DEBUG

#endif // _DEBUG
}

void BattleScene::PauseMenu() {
	bool flag = false;
	Pause::Menu menu = pause_->Update(flag);
	// フラグが立った時
	if (flag) {
		if (menu == Pause::Menu::Restart) {
			player_->Initialize(stage_->GetPlayerRespawnPoint());
			isPause_ = false;
		}
		else if (menu == Pause::Menu::End) {
			endRequest_ = true;
		}
	}

}
