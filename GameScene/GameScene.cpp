#include "GameScene.h"
#include "MPEngine/Base/Manager/ResourceManager/ResourceManager.h"
#include "MPEngine/Graphics/Texture/Texture.h"
#include "externals/imgui/imgui.h"

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
	sprite->SetUVSize(Vector2(1.0f, 1.0f));

	tex = std::make_shared<Sprite>();
	a = rs->FindTexture("ABCD");
	tex->SetTexture(a);

	tex->SetScale(Vector2(100.0f, 100.0f));
	tex->SetColor(Vector4(0.0f, 1.0f, 1.0f, 1.0f));
	tex->SetUVSize(Vector2(1.0f, 1.0f));

}

void GameScene::Draw() {
	static Vector2 pos;
	ImGui::DragFloat2("position", &sprite->uvTranslate_.x, 0.1f);
	ImGui::DragFloat2("scale", &sprite->uvScale_.x, 0.1f);
	//sprite->SetTranslate(pos);
}
