#include "Ranking.h"
#include "Input/Input.h"
#include "ImGuiManager/ImGuiManager.h"

void Ranking::Update() {
	ImGuiProc();
	if (Input::GetInstance()->GetKey()->TriggerKey(DIK_SPACE)) {
		RequestStateChange(GameState::WAIT_START);
	}
}

void Ranking::ImGuiProc() {
#ifdef _DEBUG
	ImGui::Begin("ストップウォッチ");
	ImGui::Text("SPACEを押してタイトルへ");
	ImGui::End();
#endif // _DEBUG
}
