#include "NetTitleScene.h"
#include "Input/Input.h"
#include "ResourceManager/ResourceManager.h"

void NetTitleScene::Initialize() {
	auto rs = ResourceManager::GetInstance();
	text_ = std::make_unique<Sprite>();
	text_->SetTexture(rs->FindTexture("PushText"));
	text_->SetScale(Vector2(900.0f, 80.0f));
}

void NetTitleScene::Finalize() {

}

void NetTitleScene::Update() {
	auto input = Input::GetInstance();
	if (input->GetKey()->TriggerKey(DIK_SPACE)) {
		endRequest_ = true;
	}
}

void NetTitleScene::SecondUpdate() {

}
