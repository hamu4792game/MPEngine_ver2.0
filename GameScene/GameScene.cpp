#include "GameScene.h"
#include "MPEngine/Base/Manager/ResourceManager/ResourceManager.h"
#include "MPEngine/Graphics/Texture/Texture.h"
#include "MPEngine/Graphics/Object3d/Object3d.h"
#include "MPEngine/Input/Audio.h"
#include "externals/imgui/imgui.h"

void (GameScene::* GameScene::SceneUpdateTable[])() = {
	&GameScene::TitleUpdate,
	&GameScene::BattleUpdate,
	&GameScene::ResultUpdate,
};

void (GameScene::* GameScene::SceneInitializeTable[])() = {
	&GameScene::TitleInitialize,
	&GameScene::BattleInitialize,
	&GameScene::ResultInitialize,
};

void GameScene::Initialize() {
	auto rs = ResourceManager::GetInstance();
	rs->AddTexture("TitleText", "Resources/hud/titleText.png");
	rs->AddTexture("PushText", "Resources/hud/pushText.png");
	rs->AddTexture("ABCD", "Resources/uvChecker.png");
	rs->AddTexture("Target", "Resources/hud/target.png");
	rs->AddTexture("Circle", "Resources/Texture/particle.png");
	rs->AddTexture("Ground", "Resources/Texture/ground.png");
	rs->AddTexture("ABText", "Resources/hud/AB.png");


	rs->AddModel("Box", "Resources/box/box.obj");
	rs->AddModel("Plane", "Resources/plane/plane.obj");
	rs->AddModel("BoxSky", "Resources/boxSky/boxSky.obj");
	rs->AddModel("PlayerHead", "Resources/Player/Head/head.obj");
	rs->AddModel("PlayerBody", "Resources/Player/Body/body.obj");
	rs->AddModel("PlayerLeftArm", "Resources/Player/LArm/LArm.obj");
	rs->AddModel("PlayerRightArm", "Resources/Player/RArm/RArm.obj");
	rs->AddModel("PlayerLegs", "Resources/Player/Legs/limbs.obj");

	rs->AddAudio("Title", "./Resources/Sound/w006.wav");


	boxSky_ = std::make_unique<BoxSky>();
	boxSky_->Initialize();
	sceneRequest_ = Scene::TITLE;
}

void GameScene::Finalize() {
	if (titleScene) { delete titleScene; }
	if (battleScene) { delete battleScene; }
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
	titleScene = new TitleScene;
	titleScene->Initialize();
}

void GameScene::BattleInitialize() {
	battleScene = new BattleScene;
	battleScene->Initialize();
}

void GameScene::ResultInitialize() {

}

void GameScene::TitleUpdate() {
	titleScene->Update();
	if (titleScene->IsEndRequest()) {
		sceneRequest_ = Scene::BATTLE;
		titleScene->Finalize();
		delete titleScene;
	}
}

void GameScene::BattleUpdate() {
	battleScene->Update();
	if (battleScene->IsEndRequest()) {
		sceneRequest_ = Scene::TITLE;
		battleScene->Finalize();
		delete battleScene;
	}
}

void GameScene::ResultUpdate() {

}

