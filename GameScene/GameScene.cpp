#include "GameScene.h"
#include "MPEngine/Base/Manager/ResourceManager/ResourceManager.h"
#include "MPEngine/Graphics/Texture/Texture.h"

void GameScene::Initialize() {
	auto rs = ResourceManager::GetInstance();
	std::shared_ptr<Texture> text;
	text = std::make_shared<Texture>();
	text->Load("Resources/uvChecker.png");
	rs->AddTexture("A", text);

	
	sprite = std::make_shared<Sprite>();
	auto a = rs->FindTexture("A");
	sprite->SetTexture(a);

}

void GameScene::Draw() {

}
