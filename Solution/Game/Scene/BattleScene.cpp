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

	player_ = std::make_unique<Player>();
	player_->Initialize(stage_->GetPlayerRespawnPoint());

	lockOn_ = std::make_unique<LockOn>();
	lockOn_->Initialize();
	
	pointOfGazeSearch_ = std::make_unique<PointOfGazeSearch>();
	pointOfGazeSearch_->Initialize();


	gameUI_ = std::make_unique<GameUI>();
	gameUI_->Initialize(stage_->GetCollectionNum());

	skybox_ = std::make_unique<SkyBox>();
	skybox_->SetTexture(ResourceManager::GetInstance()->FindTexture("Airport"));
	WorldTransform trans(Vector3(300.0f, 300.0f, 300.0f), Vector3::zero, Vector3::zero);
	skybox_->SetTransform(trans);

	pause_ = std::make_unique<Pause>();
	pause_->Initialize();

	// 音
	auto rs = ResourceManager::GetInstance();
	Audio* bgm = rs->FindAudio("Battle");
	bgm->SoundPlayWave(true);
	bgm->SetVolume(0.25f);

}

void BattleScene::Finalize() {
	RadialBlur::GetInstance()->SetUsed(false);
	Grayscale::GetInstance()->SetUsed(false);
}

void BattleScene::Update() {

	DrawImGui();

	// escキーを押された場合ポーズ画面にする
	if (Input::GetInstance()->GetKey()->TriggerKey(DIK_ESCAPE)) {
		isPause_ = !isPause_;
	}

	// ポーズ状態の時
	if (isPause_) {
		PauseMenu();

		Input::GetInstance()->GetMouse()->SetLockCursorToCenter(false);
		Input::GetInstance()->GetMouse()->SetShowCursor(true);

		//　早期リターン
		return;
	}
	else {
		pause_->Initialize();
#ifndef _DEBUG
		Input::GetInstance()->GetMouse()->SetLockCursorToCenter(true);
		Input::GetInstance()->GetMouse()->SetShowCursor(false);
#endif // !_DEBUG
	}
	
	// チュートリアルを行っていない状態で一つ目の収集物を取ったら通る
	if (stage_->GetCollectionNum() < stage_->GetMaxCollectionNum() && isTutorial_) {
		// 演出処理が終わったら
		if (gameUI_->FirstUpdate(stage_->GetCollectionNum())) {
			// フラグを折り通らないようにする
			isTutorial_ = false;
		}
		// のちを更新しないため早期リターン
		return;
	}
	
	// stageを更新し必要なものをリストに格納
	stage_->Update();
	std::list<std::shared_ptr<Target>> listData(stage_->GetTargets());
	std::list<std::shared_ptr<Ground>> groundListData(stage_->GetGrounds());

	player_->Update();

	// 天球の更新
	static Vector3 scale = Vector3(300.0f, 300.0f, 300.0f);
	WorldTransform trans(scale, Vector3::zero, player_->GetTransform().GetPosition());
	skybox_->SetTransform(trans);
	skybox_->Update();

	// ターゲットの更新
	lockOn_->Update(listData);
	static WorldTransform* handle = nullptr;
	handle = lockOn_->GetTargetTrans();

	bool gameclear = false;
	for (auto& coll : stage_->GetCollision()) {
		// プレイヤーとステージとの衝突判定
		gameclear = player_->OnCollisionStage(*coll);
		// 床の影用の衝突判定
		Vector3 hitPoint;
		if (player_->OnCollisionDownRayToStage(*coll, hitPoint)) {
			
		}

		if (gameclear) { break; }
	}

	player_->SetTargetTrans(handle);

	// playerとstageとの判定更新
	stage_->OnCollition(*player_->GetCollision());

	// UIの更新、バグがあったためチュートリアルの際は最大数を渡すようにして修正
	uint32_t collectNum = stage_->GetCollectionNum();
	if (isTutorial_) {
		collectNum = stage_->GetMaxCollectionNum();
	}
	gameUI_->Update(collectNum);

	// ポストエフェクト
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

	if (endRequest_) {
		Finalize();
	}
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
			Finalize();
		}
	}

}
