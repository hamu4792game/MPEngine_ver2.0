#include "Running.h"
#include "Input/Input.h"
#include "ImGuiManager/ImGuiManager.h"

void Running::Initialize() {
	stopwatch_ = std::make_unique<StopWatch>();
}

void Running::Finalize() {
	pdata_->time = stopwatch_->ElapsedSeconds();
}

void Running::Update() {
	if (!stopwatch_) { return; }

	if (Input::GetInstance()->GetKey()->TriggerKey(DIK_SPACE)) {
		stopwatch_->Push();
	}

	pdata_->time = stopwatch_->ElapsedSeconds();

	if (stopwatch_->GetFinish()) {
		RequestStateChange(GameState::RESULT);
	}
	ImGuiProc();
}

void Running::ImGuiProc() {
#ifdef _DEBUG
	ImGui::Begin("ストップウォッチ");
	ImGui::Text("SPACEを押して計測開始・停止");
	if (pdata_->time >= 7.0) {
		ImGui::Text("**********");
	}
	else {
		ImGui::Text("%lf", pdata_->time);
	}
	ImGui::End();
#endif // _DEBUG
}
