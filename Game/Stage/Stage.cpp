#include "Stage.h"
#include "externals/imgui/imgui.h"
#include <string>

void Stage::Initialize() {
	ground_ = std::make_shared<Ground>();
	ground_->Initialize();

    Vector3 IniTowerPos[4]{
        Vector3(90.0f,30.0f,90.0f),
        Vector3(-90.0f,30.0f,-90.0f),
        Vector3(-90.0f,30.0f,90.0f),
        Vector3(90.0f,30.0f,-90.0f),
    };

    for (uint32_t index = 0u; index < 4; index++) {
        towers_.emplace_back(std::make_shared<Tower>())->Initialize(IniTowerPos[index]);
    }
}

void Stage::Update() {
#ifdef _DEBUG
    ImGui::Begin("Tower");
    for (int i = 0; i < towers_.size(); i++) {
        if (ImGui::TreeNode(("TowerNumber : " + std::to_string(i)).c_str())) {
            towers_[i]->DrawImGui();
            ImGui::TreePop();
        }
    }
    ImGui::End();
#endif // _DEBUG
}
