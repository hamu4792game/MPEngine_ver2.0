#include "Stage.h"
#include "externals/imgui/imgui.h"
#include <string>
#include "Utils/GlobalVariables/GlobalVariables.h"

Stage::Stage() {
	goal_ = std::make_unique<Goal>();
}

void Stage::Initialize(std::string fileName) {
	auto global = GlobalVariables::GetInstance();
	global->LoadFile(fileName);
	itemName_ = fileName;
	boxes_.clear();
	int num = global->GetIntValue(itemName_, "BoxConfirmation");
	for (int i = 0; i < num; i++) {
		Vector3 scale = global->GetVector3Value(itemName_, ("BoxNumber : " + std::to_string(i) + " : Scale").c_str());
		Vector3 rotate = global->GetVector3Value(itemName_, ("BoxNumber : " + std::to_string(i) + " : Rotate").c_str());
		Vector3 translate = global->GetVector3Value(itemName_, ("BoxNumber : " + std::to_string(i) + " : Translate").c_str());
		boxes_.emplace_back(std::make_shared<Ground>())->Initialize(scale, rotate, translate);
	}

	targets_.clear();
	num = global->GetIntValue(itemName_, "TargetConfirmation");
	for (int index = 0u; index < num; index++) {
		Vector3 translate = global->GetVector3Value(itemName_, ("TargetNumber : " + std::to_string(index) + " : Translate").c_str());
		targets_.emplace_back(std::make_shared<Target>())->Initialize(translate);
	}
	goal_->Initialize(Vector3::one, Vector3::zero, Vector3::zero);

}

void Stage::Update() {
	DrawImGui();
	collisionList_.clear();

	for (auto& box : boxes_) {
		WorldTransform tra = box->GetTrans();
		
		if (box->GetTrans().parent_) {
			bool is = true;
		}

	}

	const uint32_t maxCount = static_cast<uint32_t>(boxes_.size());
	for (uint32_t index = 0u; index < maxCount; index++) {
		auto& handle = collisionList_.emplace_back(boxes_.at(index)->GetCollision());
	}
	
	if (goal_) {
		goal_->Update();
	}
	collisionList_.emplace_back(goal_->GetCollision());

	for (auto& i : targets_) {
		i->Update();
	}
}

void Stage::DrawImGui() {
#ifdef _DEBUG
	ImGui::Begin("Stage", nullptr, ImGuiWindowFlags_MenuBar);
	if (ImGui::BeginMenuBar()) {
		if (ImGui::BeginMenu("initialize")) {
			if (ImGui::TreeNode("FileSave")) {
				if (ImGui::Button("Save")) {
					auto global = GlobalVariables::GetInstance();
					if (!boxes_.empty()) {
						global->SetValue(itemName_, "BoxConfirmation" + std::string(), static_cast<int>(boxes_.size()));
						for (int i = 0; i < boxes_.size(); i++) {
							global->SetValue(itemName_, ("BoxNumber : " + std::to_string(i) + " : Scale").c_str(), boxes_.at(i)->GetTrans().scale_);
							global->SetValue(itemName_, ("BoxNumber : " + std::to_string(i) + " : Rotate").c_str(), boxes_.at(i)->GetTrans().rotation_);
							global->SetValue(itemName_, ("BoxNumber : " + std::to_string(i) + " : Translate").c_str(), boxes_.at(i)->GetTrans().translation_);
						}
					}
					if (!targets_.empty()) {
						global->SetValue(itemName_, "TargetConfirmation" + std::string(), static_cast<int>(targets_.size()));
						for (int i = 0; i < targets_.size(); i++) {
							global->SetValue(itemName_, ("TargetNumber : " + std::to_string(i) + " : Translate").c_str(), targets_.at(i)->GetTransform().translation_);
						}
					}

					global->SaveFile(itemName_);
				}
				ImGui::TreePop();
			}
			if (ImGui::TreeNode("FileLoad")) {
				if (ImGui::Button("Load")) {
					auto global = GlobalVariables::GetInstance();
					global->LoadFile(itemName_);
					boxes_.clear();
					int num = global->GetIntValue(itemName_, "BoxConfirmation");
					for (int i = 0; i < num; i++) {
						Vector3 scale = global->GetVector3Value(itemName_, ("BoxNumber : " + std::to_string(i) + " : Scale").c_str());
						Vector3 rotate = global->GetVector3Value(itemName_, ("BoxNumber : " + std::to_string(i) + " : Rotate").c_str());
						Vector3 translate = global->GetVector3Value(itemName_, ("BoxNumber : " + std::to_string(i) + " : Translate").c_str());
						boxes_.emplace_back(std::make_shared<Ground>())->Initialize(scale,rotate,translate);
					}

					num = global->GetIntValue(itemName_, "TargetConfirmation");
					for (int i = 0; i < num; i++) {
						Vector3 translate = global->GetVector3Value(itemName_, ("TargetNumber : " + std::to_string(i) + " : Translate").c_str());
						targets_.emplace_back(std::make_shared<Target>())->Initialize(translate);
					}

				}
				ImGui::TreePop();
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Box")) {
			if (ImGui::Button("Create")) {
				boxes_.emplace_back(std::make_shared<Ground>())->Initialize(Vector3::one, Vector3::zero, Vector3::zero);
			}
			for (int i = 0; i < boxes_.size(); i++) {
				if (ImGui::TreeNode(("BoxNumber : " + std::to_string(i)).c_str())) {
					boxes_[i]->DrawImGui();
					if (ImGui::Button("Delete")) {
						boxes_.erase(boxes_.begin() + i);
					}
					ImGui::TreePop();
				}
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Target")) {
			if (ImGui::Button("Create")) {
				targets_.emplace_back(std::make_shared<Target>())->Initialize(Vector3::zero);
			}
			for (int i = 0; i < targets_.size(); i++) {
				if (ImGui::TreeNode(("TargetNumber : " + std::to_string(i)).c_str())) {
					targets_[i]->DrawImGui();
					if (ImGui::Button("Delete")) {
						targets_.erase(targets_.begin() + i);
					}
					ImGui::TreePop();
				}
			}
			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}

	ImGui::End();
#endif // _DEBUG
}

std::list<std::shared_ptr<Target>> Stage::GetTargets() const {
	return std::list<std::shared_ptr<Target>>(targets_.begin(),targets_.end());
}

void Stage::LevelLoad(LevelData* data) {
	boxes_.clear();
	targets_.clear();
	for (LevelData::ObjectData& objectData : data->GetData()) {
		// 座標の取得
		WorldTransform transform;
		transform = objectData.transform;
		// ここからはタイプ別に生成
		if (objectData.typeName.find("Ground") != std::string::npos) {
			// 見つかれば
			boxes_.emplace_back(std::make_shared<Ground>())->Initialize(objectData);
		}
		else if (objectData.typeName.find("Player") != std::string::npos) {
			playerRespawnpoint_ = objectData.transform;
			playerRespawnpoint_.UpdateMatrix();
		}
		else if (objectData.typeName.find("Target") != std::string::npos) {
			targets_.emplace_back(std::make_shared<Target>())->Initialize(objectData.transform.GetPosition());
		}
		else if (objectData.typeName.find("Goal") != std::string::npos) {
			goal_->Initialize(objectData);
		}
	}
}
