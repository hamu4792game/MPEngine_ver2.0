#include "Pause.h"
#include "Input/Input.h"
#include "ImGuiManager/ImGuiManager.h"

void Pause::Initialize() {
	auto rsManager = ResourceManager::GetInstance();
	backGround_ = std::make_shared<Sprite>();
	backGround_->SetTexture(rsManager->FindTexture("while2x2"));
	restart_ = std::make_shared<Sprite>();
	end_ = std::make_shared<Sprite>();
}

Pause::Menu Pause::Update(bool& flag) {
	InputKey(flag);
#ifdef _DEBUG

	ImGui::Begin("MENU");
	
	switch (nowMenu_) {
	case Pause::Restart:
		//restart_->SetColor(Vector4(1.0f, 0.0f, 0.0f, 1.0f));
		//end_->SetColor(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
		ImGui::Text("Restart");
		break;
	case Pause::End:
		///restart_->SetColor(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
		///end_->SetColor(Vector4(1.0f, 0.0f, 0.0f, 1.0f));
		ImGui::Text("End");
		break;
	}

	ImGui::End();
#endif // _DEBUG

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
