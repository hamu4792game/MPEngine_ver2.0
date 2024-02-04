#include "TitleUI.h"
#include "Base/Manager/ResourceManager/ResourceManager.h"
#include "externals/imgui/imgui.h"

void TitleUI::Initialize() {
	auto rsManager = ResourceManager::GetInstance();
	titleText_ = std::make_shared<Sprite>();
	titleText_->SetTexture(rsManager->FindTexture("TitleText"));
	titleText_->SetTranslate(Vector2(0.0f,110.0f));
	titleText_->SetScale(Vector2(730.0f, 310.0f));
	titleText_->SetIsActive(false);

	pushText_ = std::make_shared<Sprite>();
	pushText_->SetTexture(rsManager->FindTexture("PushText"));
	pushText_->SetTranslate(Vector2(0.0f,-250.0f));
	pushText_->SetScale(Vector2(900.0f, 80.0f));
	pushText_->SetIsActive(false);
	color_ = Vector4::one;
}

void TitleUI::Update() {
	DrawImGui();

	const float speed = 0.05f;
	if (isInOut_) {
		color_.w += speed;
		if (color_.w >= 1.0f) {
			color_.w = 1.0f;
			isInOut_ = false;
		}
	}
	else {
		color_.w -= speed;
		if (color_.w <= 0.0f) {
			color_.w = 0.0f;
			isInOut_ = true;
		}
	}
	pushText_->SetColor(color_);

}

void TitleUI::DrawImGui() {
#ifdef _DEBUG
	static Vector2 titlepos;
	static Vector2 scale = Vector2::one;
	ImGui::Begin("Title");
	ImGui::DragFloat2("titleTrans", &titlepos.x, 1.0f);
	ImGui::DragFloat2("titleScale", &scale.x, 1.0f);
	ImGui::End();

#endif // _DEBUG
}
