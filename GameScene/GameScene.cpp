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
	rs->AddTexture("white2x2", "Resources/Texture/white2x2.png");
	rs->AddTexture("UVChecker", "Resources/Texture/uvChecker.png");

	rs->AddModel("Box", "Resources/Model/box/box.obj");
	rs->AddModel("Plane", "Resources/Model/plane/plane.obj");
	rs->AddModel("Sphere", "Resources/Model/sphere/sphere.obj");
	
	rs->AddTexture("Ground", "Resources/Texture/ground.png");
	rs->AddTexture("Circle", "Resources/Texture/particle.png");
	rs->AddTexture("ABText", "Resources/Texture/hud/AB.png");
	
	rs->AddTexture("MonsterBall", "Resources/Texture/monsterBall.png");
	rs->AddTexture("CheckerBoard", "Resources/Texture/checkerBoard.png");

	rs->AddTexture("TitleText", "Resources/hud/titleText.png");
	rs->AddTexture("PushText", "Resources/hud/pushText.png");
	rs->AddTexture("ABCD", "Resources/uvChecker.png");
	rs->AddTexture("Target", "Resources/hud/target.png");
	rs->AddTexture("Circle", "Resources/Texture/particle.png");
	rs->AddTexture("Ground", "Resources/Texture/ground.png");
	rs->AddTexture("ABText", "Resources/hud/AB.png");
	rs->AddTexture("Block", "Resources/Texture/ground.png");


	rs->AddModel("Axis", "Resources/Model/axis/axis.obj");
	rs->AddModel("MultiMesh", "Resources/Model/multiMesh/multiMesh.obj");
	rs->AddModel("Teapot", "Resources/Model/teapot/teapot.obj");

	rs->AddModel("BoxSky", "Resources/boxSky/boxSky.obj");
	rs->AddModel("PlayerHead", "Resources/Player/Head/head.obj");
	rs->AddModel("PlayerBody", "Resources/Player/Body/body.obj");
	rs->AddModel("PlayerLeftArm", "Resources/Player/LArm/LArm.obj");
	rs->AddModel("PlayerRightArm", "Resources/Player/RArm/RArm.obj");
	rs->AddModel("PlayerLegs", "Resources/Player/Legs/limbs.obj");
	rs->AddModel("PlayerWeapon", "Resources/Player/weapon/weapon.obj");

	rs->AddAudio("Title", "./Resources/Sound/w006.wav");

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

