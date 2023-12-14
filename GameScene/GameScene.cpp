#include "GameScene.h"
#include "MPEngine/Base/Manager/ResourceManager/ResourceManager.h"
#include "MPEngine/Graphics/Texture/Texture.h"

void GameScene::Initialize() {
	auto rs = ResourceManager::GetInstance();
	std::shared_ptr<Texture> text;
	text = std::make_shared<Texture>();
	text->Load("Resources/uvChecker.png");
	rs->AddTexture("ABCD", text);

	
	sprite = std::make_shared<Sprite>();
	auto a = rs->FindTexture("ABCD");
	sprite->SetTexture(a);

	sprite->SetScale(Vector2(600.0f, 600.0f));
	sprite->SetColor(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
	sprite->SetUVSize(Vector2(512.0f, 512.0f));

}

void GameScene::Draw() {

}
