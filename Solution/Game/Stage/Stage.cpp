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

void Stage::initialize() {
	planeGround_.model_ = std::make_shared<Model>();
	planeGround_.model_->SetModel(ResourceManager::GetInstance()->FindObject3d("Plane"));
	planeGround_.model_->SetTexture(ResourceManager::GetInstance()->FindTexture("ground1"));
	planeGround_.transform_ = WorldTransform(Vector3(1000.0f, 1000.0f, 1.0f), Vector3(AngleToRadian(-90.0f), 0.0f, 0.0f), Vector3(0.0f, 0.01f, 0.0f));
	planeGround_.transform_.UpdateMatrix();

	planeGround_.model_->isActive_ = true;
	planeGround_.model_->SetTransform(planeGround_.transform_);
	planeGround_.model_->materials.enableLighting = false;
	planeGround_.model_->materials.phongLighing = false;
	Matrix4x4 mat = MakeScaleMatrix(Vector3(1000.0f, 1000.0f, 0.1f));
	planeGround_.model_->SetUVMatrix(mat);
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
	
	for (auto& i : targets_) {
		i->Update();
	}

	for (uint32_t index = 0u; index < static_cast<uint32_t>(collectionObject_.size()); index++) {
		bool isAlive = collectionObject_.at(index)->Update();
		if (!isAlive) {
			// 要素の削除
			collectionObject_.erase(collectionObject_.begin() + index);
			maxCollectionNum_--;
			if (maxCollectionNum_ <= 0u) {
				// 全てのオブジェクトを取ったらゴール可能に
				isCanGoal_ = true;
			}
		}
		else {
			auto& handle = collisionList_.emplace_back(collectionObject_.at(index)->GetCollision());
		}
	}

	// ゴール可能ならの処理
	if (isCanGoal_) {
		goal_->Update();
		collisionList_.emplace_back(goal_->GetCollision());
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
		if (ImGui::BeginMenu("CollectionObject")) {
			for (int i = 0; i < collectionObject_.size(); i++) {
				if (ImGui::TreeNode(("CollectionObjectNumber : " + std::to_string(i)).c_str())) {
					collectionObject_.at(i)->DrawImGui();
					if (ImGui::Button("Delete")) {
						collectionObject_.erase(collectionObject_.begin() + i);
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

std::list<std::shared_ptr<Ground>> Stage::GetGrounds() const {
	return std::list<std::shared_ptr<Ground>>(boxes_.begin(), boxes_.end());
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

			if (objectData.typeName.find("15") != std::string::npos) {
				auto size = boxes_.size();
			}

			// 見つかれば
			boxes_.emplace_back(std::make_shared<Ground>())->Initialize(objectData);
			// 絶対後で消す
			if (boxes_.size() == 8 || boxes_.size() == 9) {
				boxes_.back()->Set(0.6f);
			}
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
		else if (objectData.typeName.find("Camera") != std::string::npos) {
			cameraTransform_ = objectData.transform;
		}
		else if (objectData.typeName.find("CollectionObject") != std::string::npos) {
			collectionObject_.emplace_back(std::make_shared<CollectionObject>())->Initialize(objectData);
		}
	}
	maxCollectionNum_ = static_cast<uint32_t>(collectionObject_.size());
}

void Stage::OnCollition(const Collider& coll) {
	for (auto& object : collectionObject_) {
		Vector3 pushbackVec;
		bool iscollision = object->GetCollision()->OnCollision(coll, pushbackVec);

		if (iscollision) {
			// 衝突していたら
			object->Death();
		}
	}
}
