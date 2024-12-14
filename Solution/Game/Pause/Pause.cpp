#include "Pause.h"
#include "Input/Input.h"
#include "ImGuiManager/ImGuiManager.h"
#include "MPEngine.h"

Pause::Pause() {
	auto rsManager = ResourceManager::GetInstance();
	backGround_ = std::make_shared<Sprite>();
	for (auto& menu : menus_) {
		menu = std::make_shared<Sprite>();
	}
	backGround_->SetTexture(rsManager->FindTexture("white2x2"));
	backGround_->SetScale(Vector2(1280.0f, 720.0f));
	backGround_->SetColor(Vector4(0.0f, 0.0f, 0.0f, 0.7f));

	menus_.at(Restart)->SetTexture(rsManager->FindTexture("Restart"));
	menus_.at(Restart)->SetScale(Vector2(480.0f, 90.0f));
	menus_.at(Restart)->SetTranslate(Vector2(0.0f, -40.0f));
	menus_.at(End)->SetTexture(rsManager->FindTexture("End"));
	menus_.at(End)->SetScale(Vector2(480.0f, 90.0f));
	menus_.at(End)->SetTranslate(Vector2(0.0f, -200.0f));
}

void Pause::Initialize() {
	backGround_->SetIsActive(false);
	for (auto& menu : menus_) {
		menu->SetIsActive(false);
	}
}

Pause::Menu Pause::Update(bool& flag) {
	backGround_->SetIsActive(true);
	for (auto& menu : menus_) {
		menu->SetIsActive(true);
	}
	InputKey(flag);
#ifdef _DEBUG

	ImGui::Begin("MENU");

	/*struct Pam {
		Vector2 scale;
		Vector2 translate;
	};
	static Pam restart;
	static Pam end;

	ImGui::DragFloat2("restartScale", &restart.scale.x, 1.0f);
	ImGui::DragFloat2("restartTranslate", &restart.translate.x, 1.0f);
	ImGui::DragFloat2("endScale", &end.scale.x, 1.0f);
	ImGui::DragFloat2("endTranslate", &end.translate.x, 1.0f);

	restart_->SetScale(restart.scale);
	restart_->SetTranslate(restart.translate);
	end_->SetScale(end.scale);
	end_->SetTranslate(end.translate);*/
	
	ImGui::End();
#endif // _DEBUG

	switch (nowMenu_) {
	case Pause::Restart:
		menus_.at(Restart)->SetColor(Vector4(1.0f, 0.0f, 0.0f, 1.0f));
		menus_.at(End)->SetColor(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
		break;
	case Pause::End:
		menus_.at(Restart)->SetColor(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
		menus_.at(End)->SetColor(Vector4(1.0f, 0.0f, 0.0f, 1.0f));
		break;
	}

	// 
	return nowMenu_;
}

void Pause::InputKey(bool& flag) {
	auto input = Input::GetInstance();

	int index = 0;
	for (auto& menu : menus_) {
		if (HitTestTexture(*menu, input->GetMouse()->GetScreenPosition(true))) {
			nowMenu_ = static_cast<Menu>(index);
			if (nowMenu_ >= Menu::kMaxNum) {
				nowMenu_ = Menu::Restart;
			}
		}
		index++;
	}

	if (input->GetKey()->TriggerKey(DIK_SPACE) || input->GetMouse()->TriggerMouse(MouseInput::LEFT)) {
		flag = true;
	}
}

bool Pause::HitTestTexture(const Sprite& texture, const Vector2& mousePos) {
	// 1/2のサイズを取得
	Vector2 textureScale = texture.GetScale() * 0.5f;
	Vector2 textureTranslate = texture.GetTranslate();

	// マウスの座標をスクリーン中心を(0,0)になるよう変換
	Vector2 mousePosition = mousePos - Vector2(float(MPEngine::GetInstance()->windowWidth_) * 0.5f, float(MPEngine::GetInstance()->windowHeight_) * 0.5f);
	mousePosition.y = -mousePosition.y;

	/*static Matrix4x4 projectionMatrix = MakePerspectiveFovMatrix(0.45f, float(MPEngine::GetInstance()->windowWidth_) / float(MPEngine::GetInstance()->windowHeight_), 0.1f, 1.0f);
	Matrix4x4 a = projectionMatrix * WorldTransform().worldMatrix_;
	Matrix4x4 in = Inverse(a);
	Vector3 handle = Transform(Vector3(mousePos.x, mousePos.y, 0.0f), in);
	mousePosition.x = handle.x;
	mousePosition.y = handle.y;*/


	Vector2 texMin = textureTranslate - textureScale;
	Vector2 texMax = textureTranslate + textureScale;

	// x軸判定
	if (texMin.x > mousePosition.x || texMax.x < mousePosition.x) {
		return false;
	}
	if (texMin.y > mousePosition.y || texMax.y < mousePosition.y) {
		return false;
	}
	return true;
}
