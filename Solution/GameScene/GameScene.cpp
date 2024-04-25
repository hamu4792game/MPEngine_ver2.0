#include "GameScene.h"
#include "MPEngine/Base/Manager/ResourceManager/ResourceManager.h"
#include "MPEngine/Graphics/Texture/Texture.h"
#include "MPEngine/Graphics/Object3d/Object3d.h"
#include "MPEngine/Input/Audio.h"
#include "ImGuiManager/ImGuiManager.h"

void (GameScene::* GameScene::SceneUpdateTable[])() = {
	&GameScene::TitleUpdate,
	&GameScene::BattleUpdate,
};

void (GameScene::* GameScene::SceneInitializeTable[])() = {
	&GameScene::TitleInitialize,
	&GameScene::BattleInitialize,
};

void GameScene::Initialize() {
	auto rs = ResourceManager::GetInstance();
	rs->AddTexture("white2x2", "white2x2.png");
	rs->AddTexture("UVChecker", "uvChecker.png");

	rs->AddModel("Box", "box/box.obj");
	rs->AddModel("Plane", "plane/plane.obj");
	rs->AddModel("Sphere", "sphere/sphere.obj");
	
	rs->AddTexture("Ground", "ground.png");
	rs->AddTexture("Circle", "particle.png");
	rs->AddTexture("ABText", "hud/AB.png");
	
	rs->AddTexture("MonsterBall", "monsterBall.png");
	rs->AddTexture("CheckerBoard", "checkerBoard.png");

	rs->AddTexture("TitleText", "hud/titleText.png");
	rs->AddTexture("PushText", "hud/pushText.png");
	rs->AddTexture("ABCD", "uvChecker.png");
	rs->AddTexture("Target", "hud/target.png");
	rs->AddTexture("Block", "ground.png");

	rs->AddModel("BoxSky", "boxSky/boxSky.obj");
	rs->AddModel("PlayerHead", "AnimatedCube/AnimatedCube.gltf");
	rs->AddModel("PlayerBody", "multiMesh/multiMaterial.obj");
	rs->AddModel("PlayerLeftArm", "Player/LArm/LArm.obj");
	rs->AddModel("PlayerRightArm", "Player/RArm/RArm.obj");
	rs->AddModel("PlayerLegs", "Player/Legs/limbs.obj");
	rs->AddModel("PlayerWeapon", "Player/weapon/weapon.obj");

	rs->LoadAnimationFile("AnimatedCube/AnimatedCube.gltf");

	//rs->AddAudio("Title", "./Resources/Sound/w006.wav");
	rs->AddAudio("Battle", "mLoop1.mp3");

	sceneRequest_ = Scene::BATTLE;
	transition_ = std::make_unique<Transition>();
}

void GameScene::Finalize() {
	if (titleScene_) {
		titleScene_->Finalize();
	}
	if (battleScene_) {
		battleScene_->Finalize();
	}
}

void GameScene::Update() {

	//Scene初期化
	if (sceneRequest_) {
		scene_ = sceneRequest_.value();
		(this->*SceneInitializeTable[static_cast<size_t>(scene_)])();
		sceneRequest_ = std::nullopt;
	}
	//SceneUpdate
	(this->*SceneUpdateTable[static_cast<size_t>(scene_)])();

}

void GameScene::TitleInitialize() {
	titleScene_.reset(new TitleScene);
	titleScene_->Initialize();
}

void GameScene::BattleInitialize() {
	battleScene_.reset(new BattleScene);
	battleScene_->Initialize();
}

void GameScene::TitleUpdate() {
	if (titleScene_->IsEndRequest()) {
		transition_->StartTransition(Transition::Type::BlackOut);
	}
	else {
		titleScene_->Update();
	}

	if (transition_->Update()) {
		sceneRequest_ = Scene::BATTLE;
		titleScene_->Finalize();
		titleScene_.reset();
	}
}

void GameScene::BattleUpdate() {
	if (battleScene_->IsEndRequest()) {
		transition_->StartTransition(Transition::Type::Spin);
	}
	else {
		battleScene_->Update();
	}

	if (transition_->Update()) {
		sceneRequest_ = Scene::TITLE;
		battleScene_->Finalize();
		battleScene_.reset();
	}
}

