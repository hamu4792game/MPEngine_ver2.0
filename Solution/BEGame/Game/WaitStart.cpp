#include "WaitStart.h"
#include "Input/Input.h"
#include "ImGuiManager/ImGuiManager.h"
#include <string>
#include "Communication/httpClient.h"
#include "externals/nlohmann/json.hpp"

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
	static char name[128] = "";
	static char password[128] = "";
	ImGui::InputText("UserName", name, IM_ARRAYSIZE(name));
	ImGui::InputText("Password", password, IM_ARRAYSIZE(password));
	if (ImGui::Button("サインアップ")) {
		// サインアップ
		RegisterUserAsync(name, password);
	}
	if (ImGui::Button("ログイン")) {
		// ログイン
		std::string login = LoginAsync(name, password).get();
		nlohmann::json loginJson = nlohmann::json::parse(login);
		TokenManager::GetToken() = loginJson["token"];
	}

	ImGui::End();
#endif // _DEBUG
}
