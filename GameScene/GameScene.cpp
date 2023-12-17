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
	sprite->SetIsActive(false);

	std::shared_ptr<Object3d> object;
	object = std::make_shared<Object3d>();
	object->Load("Box","Resources/box/box.obj");
	rs->AddModel("Box", object);

	model = std::make_shared<Model>();
	auto b = rs->FindObject3d("Box");
	model->SetModel(b);
	model->Initialize();

	model->translate_.z = 10.0f;
	model->scale_ = Vector3::one;

}

void GameScene::Draw() {
	static Vector2 pos;
	ImGui::DragFloat3("position", &model->translate_.x, 0.1f);
	ImGui::DragFloat3("rotate", &model->rotate_.x, 0.1f);
	ImGui::DragFloat3("scale", &model->scale_.x, 0.1f);
	//sprite->SetTranslate(pos);
}
