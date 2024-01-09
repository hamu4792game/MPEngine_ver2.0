#include "GameScene.h"
#include "MPEngine/Base/Manager/ResourceManager/ResourceManager.h"
#include "MPEngine/Graphics/Texture/Texture.h"
#include "MPEngine/Graphics/Object3d/Object3d.h"
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
	std::shared_ptr<Texture> text;
	text = std::make_shared<Texture>();
	text->Load("Resources/uvChecker.png");
	rs->AddTexture("ABCD", text);

	text = std::make_shared<Texture>();
	text->Load("Resources/hud/target.png");
	rs->AddTexture("Target", text);

	text = std::make_shared<Texture>();
	text->Load("Resources/Texture/particle.png");
	rs->AddTexture("Circle", text);
	
	text = std::make_shared<Texture>();
	text->Load("Resources/Texture/ground.png");
	rs->AddTexture("Ground", text);

	text = std::make_shared<Texture>();
	text->Load("Resources/hud/titleText.png");
	rs->AddTexture("TitleText", text);

	text = std::make_shared<Texture>();
	text->Load("Resources/hud/pushText.png");
	rs->AddTexture("PushText", text);

	text = std::make_shared<Texture>();
	text->Load("Resources/hud/AB.png");
	rs->AddTexture("ABText", text);


	std::shared_ptr<Object3d> object;
	object = std::make_shared<Object3d>();
	object->Load("Box","Resources/box/box.obj");
	rs->AddModel("Box", object);

	object = std::make_shared<Object3d>();
	object->Load("Plane","Resources/plane/plane.obj");
	rs->AddModel("Plane", object);

	object = std::make_shared<Object3d>();
	object->Load("BoxSky","Resources/boxSky/boxSky.obj");
	rs->AddModel("BoxSky", object);

	object = std::make_shared<Object3d>();
	object->Load("PlayerHead","Resources/Player/Head/head.obj");
	rs->AddModel("PlayerHead", object);
	
	object = std::make_shared<Object3d>();
	object->Load("PlayerBody","Resources/Player/Body/body.obj");
	rs->AddModel("PlayerBody", object);

	object = std::make_shared<Object3d>();
	object->Load("PlayerLeftArm","Resources/Player/LArm/LArm.obj");
	rs->AddModel("PlayerLeftArm", object);
	
	object = std::make_shared<Object3d>();
	object->Load("PlayerRightArm","Resources/Player/RArm/RArm.obj");
	rs->AddModel("PlayerRightArm", object);

	object = std::make_shared<Object3d>();
	object->Load("PlayerRightArm","Resources/Player/Legs/limbs.obj");
	rs->AddModel("PlayerLegs", object);

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

