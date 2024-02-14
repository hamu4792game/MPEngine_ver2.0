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

	rs->AddTexture("Ground", "Resources/Texture/ground.png");
	rs->AddTexture("Circle", "Resources/Texture/particle.png");
	rs->AddTexture("ABText", "Resources/Texture/hud/AB.png");
	
	rs->AddTexture("MonsterBall", "Resources/Texture/monsterBall.png");
	rs->AddTexture("CheckerBoard", "Resources/Texture/checkerBoard.png");


	rs->AddModel("Box", "Resources/Model/box/box.obj");
	rs->AddModel("Plane", "Resources/Model/plane/plane.obj");
	rs->AddModel("Sphere", "Resources/Model/sphere/sphere.obj");
	rs->AddModel("Axis", "Resources/Model/axis/axis.obj");
	rs->AddModel("MultiMesh", "Resources/Model/multiMesh/multiMesh.obj");
	//rs->AddModel("Suzanne", "Resources/Model/suzanne/suzanne.obj");
	rs->AddModel("Teapot", "Resources/Model/teapot/teapot.obj");

	rs->AddAudio("Title", "./Resources/Sound/w006.wav");

	sceneRequest_ = Scene::TITLE;
	transition_ = std::make_unique<Transition>();
}

void GameScene::Finalize() {
	if (titleScene_) {
		titleScene_->Finalize();
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

}

void GameScene::TitleUpdate() {
	/*if (titleScene_->IsEndRequest()) {
		transition_->StartTransition(Transition::Type::BlackOut);
	}
	else {
		
	}*/
	titleScene_->Update();

	/*if (transition_->Update()) {
		sceneRequest_ = Scene::BATTLE;
		titleScene_->Finalize();
		titleScene_.reset();
	}*/
}

void GameScene::BattleUpdate() {
	
}

