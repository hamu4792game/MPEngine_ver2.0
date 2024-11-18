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
	LevelData data;
	auto leveldata = data.Load("test.json");
	stage_->LevelLoad(leveldata);

	player_ = std::make_unique<PlayerManager>();
	player_->Initialize(stage_->GetPlayerRespawnPoint());

	enemy_ = std::make_unique<Enemy>();
	enemy_->Initialize();

	lockOn_ = std::make_unique<LockOn>();
	lockOn_->Initialize();

	gameUI_ = std::make_unique<GameUI>();
	gameUI_->Initialize();

	skybox_ = std::make_unique<SkyBox>();
	skybox_->SetTexture(ResourceManager::GetInstance()->FindTexture("Airport"));
	WorldTransform trans(Vector3(300.0f, 300.0f, 300.0f), Vector3::zero, Vector3::zero);
	skybox_->SetTransform(trans);

	// 音
	auto rs = ResourceManager::GetInstance();
	Audio* bgm = rs->FindAudio("Battle");
	bgm->SoundPlayWave(true);

}

void BattleScene::Finalize() {
	
}

void BattleScene::Update() {

	DrawImGui();
	stage_->Update();
	std::list<std::shared_ptr<Target>> listData(stage_->GetTargets());

	lockOn_->Update(listData);
	auto handle = lockOn_->GetTargetTrans();
	
	time_ = std::min(time_, 300.0f);

	player_->SetTargetTrans(handle);


	player_->Update();
	enemy_->Update();
	
	static Vector3 scale = Vector3(300.0f, 300.0f, 300.0f);
	WorldTransform trans(scale, Vector3::zero, player_->GetTransform().GetPosition());
	skybox_->SetTransform(trans);
	skybox_->Update();

	bool gameclear = false;
	for (auto& coll : stage_->GetCollision()) {
		gameclear = player_->OnCollisionStage(*coll);
		if (gameclear) { break; }
	}
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
	WorldTransform cameraTransform = player_->PostUpdate();
	// 一旦うまくいかないのでしない
	for (auto& coll : stage_->GetCollision()) {
		cameraTransform = player_->OnCollisionCameraToStage(*coll);
	}
	camera->SetTransform(cameraTransform);

}

void BattleScene::DrawImGui() {
#ifdef _DEBUG

#endif // _DEBUG
}
