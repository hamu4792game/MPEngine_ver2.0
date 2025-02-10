#include "NTScene.h"
#include "ResourceManager/ResourceManager.h"

#include "NT/Scene/NetTitleScene.h"
#include "NT/Scene/GameScene.h"

void (NTScene::* NTScene::SceneUpdateTable[])() = {
	&NTScene::TitleUpdate,
	&NTScene::GameUpdate,
};

void (NTScene::* NTScene::SceneInitializeTable[])() = {
	&NTScene::TitleInitialize,
	&NTScene::GameInitialize,
};


void NTScene::Initialize() {

	auto rs = ResourceManager::GetInstance();
	rs->AddTexture("white2x2", "Resources/Texture/white2x2.png");
	rs->AddTexture("number", "Resources/Texture/number.png");
	rs->AddTexture("PushText", "Resources/Texture/hud/pushText.png");
	rs->AddTexture("Circle", "Resources/Texture/circle.png");

	transition_ = std::make_unique<Transition>();
	sceneRequest_ = GAME;
}

void NTScene::Finalize() {
	if (titleScene_) {
		titleScene_->Finalize();
	}
	if (gameScene_) {
		gameScene_->Finalize();
	}
}

void NTScene::Update() {

	//Scene初期化
	if (sceneRequest_) {
 		scene_ = sceneRequest_.value();
		(this->*SceneInitializeTable[static_cast<size_t>(scene_)])();
		sceneRequest_ = std::nullopt;
	}
	//SceneUpdate
	(this->*SceneUpdateTable[static_cast<size_t>(scene_)])();
}

void NTScene::TitleInitialize() {
	titleScene_.reset(new NetTitleScene);
	titleScene_->Initialize();
}

void NTScene::GameInitialize() {
	gameScene_.reset(new GameScene);
	gameScene_->Initialize();
}

void NTScene::TitleUpdate() {
	titleScene_->Update();
	if (titleScene_->IsEndRequest()) {
		transition_->StartTransition(Transition::Type::Spin);
	}
	if (transition_->Update()) {
		sceneRequest_ = GAME;
		titleScene_->Finalize();
		titleScene_.reset();
	}
}

void NTScene::GameUpdate() {
	gameScene_->Update();
	if (gameScene_->IsEndRequest()) {
		transition_->StartTransition(Transition::Type::BlackOut);
	}
	if (transition_->Update()) {
		sceneRequest_ = TITLE;
		gameScene_->Finalize();
		gameScene_.reset();
	}
}

