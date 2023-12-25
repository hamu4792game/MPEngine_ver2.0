#include "GameScene.h"
#include "MPEngine/Base/Manager/ResourceManager/ResourceManager.h"
#include "MPEngine/Graphics/Texture/Texture.h"
#include "MPEngine/Graphics/Object3d/Object3d.h"
#include "externals/imgui/imgui.h"

void GameScene::Initialize() {
	auto rs = ResourceManager::GetInstance();
	std::shared_ptr<Texture> text;
	text = std::make_shared<Texture>();
	text->Load("Resources/uvChecker.png");
	rs->AddTexture("ABCD", text);

	std::shared_ptr<Object3d> object;
	object = std::make_shared<Object3d>();
	object->Load("Box","Resources/box/box.obj");
	rs->AddModel("Box", object);

	object = std::make_shared<Object3d>();
	object->Load("BoxSky","Resources/boxSky/boxSky.obj");
	rs->AddModel("BoxSky", object);

	battleScene = new BattleScene;
	battleScene->Initialize();

	boxSky_ = std::make_unique<BoxSky>();
	boxSky_->Initialize();

}

void GameScene::Finalize() {
	battleScene->Finalize();
	delete battleScene;
}

void GameScene::Update() {
	battleScene->Update();
}

