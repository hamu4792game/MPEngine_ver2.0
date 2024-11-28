#include "Pause.h"
#include "Input/Input.h"
#include "ImGuiManager/ImGuiManager.h"

Pause::Menu Pause::Update(bool& flag) {
	InputKey(flag);

	ImGui::Begin("MENU");
	
	switch (nowMenu_) {
	case Pause::Restart:
		ImGui::Text("Restart");
		break;
	case Pause::End:
		ImGui::Text("End");
		break;
	}

	ImGui::End();

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
