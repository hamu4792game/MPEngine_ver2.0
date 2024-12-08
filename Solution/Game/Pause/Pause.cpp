#include "Pause.h"
#include "Input/Input.h"
#include "ImGuiManager/ImGuiManager.h"

Pause::Pause() {
	auto rsManager = ResourceManager::GetInstance();
	backGround_ = std::make_shared<Sprite>();
	restart_ = std::make_shared<Sprite>();
	end_ = std::make_shared<Sprite>();
	backGround_->SetTexture(rsManager->FindTexture("white2x2"));
	backGround_->SetScale(Vector2(1280.0f, 720.0f));
	backGround_->SetColor(Vector4(0.0f, 0.0f, 0.0f, 0.7f));
	restart_->SetTexture(rsManager->FindTexture("Restart"));
	restart_->SetScale(Vector2(480.0f, 90.0f));
	restart_->SetTranslate(Vector2(0.0f, -40.0f));
	end_->SetTexture(rsManager->FindTexture("End"));
	end_->SetScale(Vector2(480.0f, 90.0f));
	end_->SetTranslate(Vector2(0.0f, -200.0f));
}

void Pause::Initialize() {
	backGround_->SetIsActive(false);
	restart_->SetIsActive(false);
	end_->SetIsActive(false);
}

Pause::Menu Pause::Update(bool& flag) {
	backGround_->SetIsActive(true);
	restart_->SetIsActive(true);
	end_->SetIsActive(true);
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
		restart_->SetColor(Vector4(1.0f, 0.0f, 0.0f, 1.0f));
		end_->SetColor(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
		break;
	case Pause::End:
		restart_->SetColor(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
		end_->SetColor(Vector4(1.0f, 0.0f, 0.0f, 1.0f));
		break;
	}

	// 
	return nowMenu_;
}

void Pause::InputKey(bool& flag) {
	auto input = Input::GetInstance();

	if (input->GetKey()->TriggerKey(DIK_S)) {
		nowMenu_ = static_cast<Menu>(static_cast<int>(nowMenu_) + 1);
		if (nowMenu_ >= Menu::kMaxNum) {
			nowMenu_ = Menu::Restart;
		}
	}
	else if (input->GetKey()->TriggerKey(DIK_W)) {
		nowMenu_ = static_cast<Menu>(static_cast<int>(nowMenu_) - 1);
		if (nowMenu_ < Menu::Restart) {
			nowMenu_ = Menu::End;
		}
	}

	if (input->GetKey()->TriggerKey(DIK_SPACE)) {
		flag = true;
	}

}
