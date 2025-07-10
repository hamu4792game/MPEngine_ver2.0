#include "Ranking.h"
#include "Input/Input.h"
#include "ImGuiManager/ImGuiManager.h"
#include "BEGame/Game/Communication/httpClient.h"
#include "externals/nlohmann/json.hpp"

void Ranking::Initialize() {
	rankingText_.clear();

	std::string postRes = PostScoreAsync(pdata_->score).get();
	std::string allScoresJson = GetAllScoresAsync().get();

	try {
		nlohmann::json j = nlohmann::json::parse(allScoresJson);
		rankingText_ = "Ranking top 5:\n";
		int i = 0;
		for (const auto& entry : j) {
			int rankScore = entry["score"];
			rankingText_ += std::to_string(i + 1) + ". " + std::to_string(rankScore) + "\n";
			++i;
		}
	}
	catch (const std::exception& e) {
		e;
		rankingText_ = "[Ranking Error]";
	}
}

void Ranking::Update() {
	
	if (Input::GetInstance()->GetKey()->TriggerKey(DIK_SPACE)) {
		RequestStateChange(GameState::WAIT_START);
	}
	ImGuiProc();
}

void Ranking::ImGuiProc() {
#ifdef _DEBUG
	ImGui::Begin("ストップウォッチ");

	std::istringstream iss(rankingText_);
	std::string line;
	while (std::getline(iss, line)) {
		ImGui::Text("%s", line.c_str());
	}

	ImGui::Text("SPACEを押してタイトルへ");
	ImGui::End();
#endif // _DEBUG
}
