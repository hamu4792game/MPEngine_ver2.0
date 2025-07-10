#include "Result.h"
#include "ImGuiManager/ImGuiManager.h"
#include "Input/Input.h"
#include <cmath>
#include <algorithm>

void Result::Initialize() {
	double elapsed = std::round(pdata_->time * 100.0) / 100.0; // 小数第二位で丸める
	int score = 0;
	if (elapsed >= 7.0 && elapsed <= 10.01) {
		double ratio = (elapsed - 7.0) / 2.0; // 7.0～10.0 → 0.0～1.0 に正規化
		score = static_cast<int>(ratio * 1000.0);
	}
	score_ = std::clamp<int>(score, 0, 1000); // 念のため
	pdata_->score = score_;
}

void Result::Update() {
	ImGuiProc();
	if (Input::GetInstance()->GetKey()->TriggerKey(DIK_SPACE)) {
		RequestStateChange(GameState::RANKING);
	}
}

void Result::ImGuiProc() {
#ifdef _DEBUG
	ImGui::Begin("ストップウォッチ");
	ImGui::Text("結果　タイム：%lf / スコア：%d", pdata_->time, score_);
	ImGui::Text("ENTERを押してスコアを送信");
	ImGui::End();
#endif // _DEBUG
}
