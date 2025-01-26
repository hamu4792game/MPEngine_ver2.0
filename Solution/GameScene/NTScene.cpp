#include "NTScene.h"
#include "ResourceManager/ResourceManager.h"

#include "NT/Scene/GameScene.h"

void NTScene::Initialize() {

	auto rs = ResourceManager::GetInstance();
	rs->AddTexture("white2x2", "Resources/Texture/white2x2.png");
	rs->AddTexture("number", "Resources/Texture/number.png");
	
	gameScene_ = std::make_unique<GameScene>();
	gameScene_->Initialize();
	
}

void NTScene::Finalize() {
	gameScene_->Finalize();
}

void NTScene::Update() {

	gameScene_->Update();

}

