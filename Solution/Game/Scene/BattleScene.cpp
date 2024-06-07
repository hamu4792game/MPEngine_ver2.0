#include "BattleScene.h"
#include "Utils/Camera/Camera3d.h"
#include "Input/Input.h"
#include "ImGuiManager/ImGuiManager.h"
#include "ResourceManager/ResourceManager.h"
#include "Input/Audio.h"
#include "MPEngine/Graphics/RenderManager/RenderManager.h"
#include "Utils/Tool/LevelData.h"

void BattleScene::Initialize() {
	player_ = std::make_unique<PlayerManager>();
	player_->Initialize();
	cameraTrans_.scale_ = Vector3::one;
	cameraTrans_.translation_ = Vector3(0.0f, 0.0f, -10.0f);
	
	stage_ = std::make_unique<Stage>();
	//stage_->Initialize("Stage");
	LevelData data;
	auto leveldata = data.Load("box.json");
	stage_->LevelLoad(leveldata);

	enemy_ = std::make_unique<Enemy>();
	enemy_->Initialize();

	lockOn_ = std::make_unique<LockOn>();
	lockOn_->Initialize();

	gameUI_ = std::make_unique<GameUI>();
	gameUI_->Initialize();

	skybox_ = std::make_unique<SkyBox>();
	skybox_->SetTexture(ResourceManager::GetInstance()->FindTexture("Airport"));

	// 音
	auto rs = ResourceManager::GetInstance();
	Audio* bgm = rs->FindAudio("Battle");
	bgm->SoundPlayWave(true);

}

void BattleScene::Finalize() {
	
}

void BattleScene::Update() {

	if (time_++ <= kMaxTimer_ * 60.0f) {
		RenderManager::nowEffect = RenderManager::PostEffect::Grayscale;
	}
	else {
		RenderManager::nowEffect = RenderManager::PostEffect::None;
	}
	time_ = std::min(time_, 300.0f);

	DrawImGui();
	stage_->Update();
	std::list<std::shared_ptr<Target>> listData(stage_->GetTargets());

	lockOn_->Update(listData);
	auto handle = lockOn_->GetTargetTrans();
	if (handle) {
		RenderManager::nowEffect = RenderManager::PostEffect::Vignette;
	}
	player_->SetTargetTrans(handle);


	player_->Update();
	enemy_->Update();
	//player_->OnCollisionStage(enemy_->GetCollision());
	//if (player_->OnCollision(enemy_->GetCollision())) {
	//	endRequest_ = true;
	//}

#ifdef _DEBUG
	static Vector3 scale = Vector3(300.0f, 300.0f, 300.0f);
	static Vector3 translate = Vector3(0.0f, 0.0f, 0.0f);
	ImGui::DragFloat3("BoxScale", &scale.x, 1.0f);
	ImGui::DragFloat3("BoxTranslate", &translate.x, 1.0f);
	WorldTransform trans(scale, Vector3::zero, translate);
	skybox_->SetTransform(trans);
#endif // _DEBUG
	skybox_->Update();

	for (auto coll : stage_->GetCollision()) {
		player_->OnCollisionStage(coll);
	}
	gameUI_->Update();

	if (Input::GetInstance()->GetKey()->PressKey(DIK_P)) {
		endRequest_ = true;
	}

	SecondUpdate();
}

void BattleScene::SecondUpdate() {
	Camera3d::GetInstance()->SetTransform(player_->PostUpdate());
}

void BattleScene::DrawImGui() {
#ifdef _DEBUG

#endif // _DEBUG
}
