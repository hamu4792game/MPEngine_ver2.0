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
	auto leveldata = data.Load("stage.json");
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
	
	if (time_++ <= kMaxTimer_ * 60.0f) {
		RenderManager::nowEffect = RenderManager::PostEffect::GaussianFilter;
	}
	else if (handle) {
		RenderManager::nowEffect = RenderManager::PostEffect::Vignette;
	}
	else {
		RenderManager::nowEffect = RenderManager::PostEffect::None;
	}
	time_ = std::min(time_, 300.0f);

	player_->SetTargetTrans(handle);


	player_->Update();
	enemy_->Update();
	
#ifdef _DEBUG
	static Vector3 scale = Vector3(300.0f, 300.0f, 300.0f);
	static Vector3 translate = Vector3(0.0f, 0.0f, 0.0f);
	ImGui::DragFloat3("BoxScale", &scale.x, 1.0f);
	ImGui::DragFloat3("BoxTranslate", &translate.x, 1.0f);
	WorldTransform trans(scale, Vector3::zero, translate);
	skybox_->SetTransform(trans);
#endif // _DEBUG
	skybox_->Update();

	bool gameclear = false;
	for (auto& coll : stage_->GetCollision()) {
		gameclear = player_->OnCollisionStage(*coll);
		if (gameclear) { break; }
	}
	gameUI_->Update();

	uint32_t num = player_->GetEffectNumber();
	if (num == 1u) {
		RenderManager::nowEffect = RenderManager::PostEffect::RadialBlur;
	}
	else if (num == 2u) {
		RenderManager::nowEffect = RenderManager::PostEffect::Grayscale;
	}

	if (Input::GetInstance()->GetKey()->PressKey(DIK_P) || gameclear) {
		endRequest_ = true;
	}
	RenderManager::nowEffect = RenderManager::PostEffect::None;

	SecondUpdate();
}

void BattleScene::SecondUpdate() {
	Camera3d::GetInstance()->SetTransform(player_->PostUpdate());
}

void BattleScene::DrawImGui() {
#ifdef _DEBUG

#endif // _DEBUG
}
