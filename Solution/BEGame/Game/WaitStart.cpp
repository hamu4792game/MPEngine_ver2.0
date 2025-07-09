#include "WaitStart.h"
#include "Input/Input.h"
#include "ImGuiManager/ImGuiManager.h"

void WaitStart::Update() {
	ImGuiProc();
	if (Input::GetInstance()->GetKey()->TriggerKey(DIK_SPACE)) {
		RequestStateChange(GameState::RUNNING);
	}
}

void WaitStart::ImGuiProc() {
#ifdef _DEBUG
	ImGui::Begin("ストップウォッチ");
	ImGui::Text("Press SPACE to Start");
	ImGui::End();
#endif // _DEBUG
}
