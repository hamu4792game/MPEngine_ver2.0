#include "Player.h"
#include "externals/imgui/imgui.h"
#include "Input/input.h"
#include "Utils/Camera/Camera3d.h"
#include "Utils/Easing/Easing.h"
#include "Utils/GlobalVariables/GlobalVariables.h"

// コンボ定数表
const std::array<Player::ConstAttack, Player::kComboNum> Player::kConstAttacks_ = {
	{
		{0,0,20,20,0.0f,0.0f,0.0f},
		{0,0,20,20,0.0f,0.0f,0.0f},
		{10,20,20,30,0.0f,0.0f,0.0f},
	}
};

void Player::Initialize() {
#pragma region Models
	auto rsManager = ResourceManager::GetInstance();
	for (uint32_t index = 0u; index < Parts::kMaxParts; index++) {
		auto& model = models_.emplace_back(std::make_shared<Model>());
	}
	partsTrans_.resize(models_.size());
	models_.at(Parts::Body)->SetModel(rsManager->FindObject3d("PlayerBody"));
	models_.at(Parts::Head)->SetModel(rsManager->FindObject3d("PlayerHead"));
	models_.at(Parts::L_Arm)->SetModel(rsManager->FindObject3d("PlayerLeftArm"));
	models_.at(Parts::R_Arm)->SetModel(rsManager->FindObject3d("PlayerRightArm"));
	models_.at(Parts::L_Legs)->SetModel(rsManager->FindObject3d("PlayerLegs"));
	models_.at(Parts::R_Legs)->SetModel(rsManager->FindObject3d("PlayerLegs"));

	partsTrans_.at(Parts::Body).parent_ = &transform_;
	partsTrans_.at(Parts::Head).parent_ = &partsTrans_.at(Parts::Body);
	partsTrans_.at(Parts::L_Arm).parent_ = &partsTrans_.at(Parts::Body);
	partsTrans_.at(Parts::R_Arm).parent_ = &partsTrans_.at(Parts::Body);
	partsTrans_.at(Parts::L_Legs).parent_ = &partsTrans_.at(Parts::Body);
	partsTrans_.at(Parts::R_Legs).parent_ = &partsTrans_.at(Parts::Body);

	auto global = GlobalVariables::GetInstance();
	global->LoadFile(itemName_);
	for (uint32_t index = 0u; index < Parts::kMaxParts; index++) {
		partsTrans_.at(index).scale_ = global->GetVector3Value(itemName_, ("PartsTrans : scale" + std::to_string(index)).c_str());
		partsTrans_.at(index).rotation_ = global->GetVector3Value(itemName_, ("PartsTrans : rotate" + std::to_string(index)).c_str());
		partsTrans_.at(index).translation_ = global->GetVector3Value(itemName_, ("PartsTrans : translate" + std::to_string(index)).c_str());
	}

#pragma endregion


	transform_.scale_ = Vector3::one;
	transform_.translation_.y = 1.0f;

	collision_ = std::make_shared<AABB>();
	TransformUpdate();

}

void Player::Update() {
	DrawImGui();

	// std::nullopt以外の時通る
	if (behaviorRequest_) {
		// 振る舞いを変更する
		behavior_ = behaviorRequest_.value();
		// 振る舞いごとの初期化を実行
		switch (behavior_) {
		case Behavior::kRoot:
			break;
		case Behavior::kAttack:
			InitializeAttack();
			break;
		case Behavior::kDash:
			break;
		}
		//	振る舞いリクエストをリセット
		behaviorRequest_ = std::nullopt;
	}

	switch (behavior_) {
	case Behavior::kRoot:
	default:
		// 通常行動
		BehaviorRootUpdate();
		break;
	case Behavior::kAttack:
		BehaviorAttackUpdate();
		break;
	case Behavior::kDash:
		break;
	}

	LimitMoving();
	TransformUpdate();
}

void Player::DrawImGui() {
#ifdef _DEBUG
	ImGui::Begin("Player",nullptr,ImGuiWindowFlags_MenuBar);
	if (ImGui::BeginMenuBar()) {
		if (ImGui::BeginMenu("initialize")) {
			if (ImGui::TreeNode("FileSave")) {
				if (ImGui::Button("Save")) {
					auto global = GlobalVariables::GetInstance();
					for (uint32_t index = 0u; index < Parts::kMaxParts; index++) {
						global->SetValue(itemName_, ("PartsTrans : scale" + std::to_string(index)).c_str(), partsTrans_.at(index).scale_);
						global->SetValue(itemName_, ("PartsTrans : rotate" + std::to_string(index)).c_str(), partsTrans_.at(index).rotation_);
						global->SetValue(itemName_, ("PartsTrans : translate" + std::to_string(index)).c_str(), partsTrans_.at(index).translation_);
					}
					global->SaveFile(itemName_);
				}
				ImGui::TreePop();
			}
			if (ImGui::TreeNode("FileLoad")) {
				if (ImGui::Button("Load")) {
					auto global = GlobalVariables::GetInstance();
					global->LoadFile(itemName_);
					for (uint32_t index = 0u; index < Parts::kMaxParts; index++) {
						partsTrans_.at(index).scale_ = global->GetVector3Value(itemName_, ("PartsTrans : scale" + std::to_string(index)).c_str());
						partsTrans_.at(index).rotation_ = global->GetVector3Value(itemName_, ("PartsTrans : rotate" + std::to_string(index)).c_str());
						partsTrans_.at(index).translation_ = global->GetVector3Value(itemName_, ("PartsTrans : translate" + std::to_string(index)).c_str());
					}
				}
				ImGui::TreePop();
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Adjustment")) {
			if (ImGui::TreeNode("Transform")) {
				ImGui::DragFloat3("position", &transform_.translation_.x, 0.1f);
				ImGui::DragFloat3("rotate", &transform_.rotation_.x, 0.1f);
				ImGui::DragFloat3("scale", &transform_.scale_.x, 0.1f);
				ImGui::TreePop();
			}

			for (uint32_t index = 0u; index < Parts::kMaxParts; index++) {
				if (ImGui::TreeNode(("Parts" + std::to_string(index)).c_str())) {
					ImGui::DragFloat3("position", &partsTrans_.at(index).translation_.x, 0.1f);
					ImGui::DragFloat3("rotate", &partsTrans_.at(index).rotation_.x, AngleToRadian(1.0f));
					ImGui::DragFloat3("scale", &partsTrans_.at(index).scale_.x, 0.1f);
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

void Player::Move() {
	auto input = Input::GetInstance();
	Vector3 move;

	const float speed = 0.2f;
	if (input->GetKey()->PressKey(DIK_W)) {
		move.z += speed;
	}
	if (input->GetKey()->PressKey(DIK_S)) {
		move.z -= speed;
	}
	if (input->GetKey()->PressKey(DIK_A)) {
		move.x -= speed;
	}
	if (input->GetKey()->PressKey(DIK_D)) {
		move.x += speed;
	}

	
	if (move != Vector3::zero) {
		move = Normalize(move);
		// 移動ベクトルをカメラの角度だけ回転させる
		move = TargetOffset(move, Camera3d::GetInstance()->GetTransform().rotation_);
		move.y = 0.0f;
		transform_.translation_ += move;
	}

}

void Player::Jamp() {
	if (Input::GetInstance()->GetKey()->TriggerKey(DIK_SPACE) && !isJamped_) {
		// 初速度を与える
		isJamped_ = true;
		acceleration_ = 1.0f;
	}

	if (isJamped_) {
		// ジャンプ更新処理
		// 重力
		const float gravity_ = 0.1f;
		// 重力を足していく
		acceleration_ -= gravity_;
		transform_.translation_.y += acceleration_;
	}
}

void Player::TransformUpdate() {
	transform_.UpdateMatrix();
	for (auto& transform : partsTrans_) {
		transform.UpdateMatrix();
	}
	for (uint32_t index = 0u; index < Parts::kMaxParts; index++) {
		models_.at(index)->transform_ = partsTrans_.at(index);
	}
	collision_->Update(transform_);
}

void Player::LimitMoving() {
	if (transform_.translation_.y < 1.0f) {
		transform_.translation_.y = 1.0f;
		isJamped_ = false;
	}
}

void Player::InitializeAttack() {
	// コンボ継続フラグをリセットする
	workAttack_.comboNext_ = false;
	// 攻撃のパラメーターリセット
	workAttack_.attackParameter_ = 0;
	workAttack_.inComboPhase_ = 0;
	workAttack_.comboIndex_ = 0;

}

void Player::BehaviorRootUpdate() {
	auto input = Input::GetInstance();
	if (input->GetKey()->TriggerKey(DIK_V)) { behaviorRequest_ = Behavior::kAttack; }

	Move();
	DoWireMoving();
	Jamp();
}

void Player::BehaviorAttackUpdate() {
	// 予備動作の時間
	auto input = Input::GetInstance();

	// コンボ上限に達していない
	if (workAttack_.comboIndex_ < kComboNum - 1) {
		// 攻撃ボタンをトリガーしたら
		if (input->GetKey()->TriggerKey(DIK_V)) {
			workAttack_.comboNext_ = true;
		}
	}

	// 既定の時間経過で通常行動に戻る
	uint32_t totalTime = kConstAttacks_[workAttack_.comboIndex_].anticipationTime + kConstAttacks_[workAttack_.comboIndex_].chargeTime
		+ kConstAttacks_[workAttack_.comboIndex_].recoveryTime + kConstAttacks_[workAttack_.comboIndex_].swingTime;
	if (++workAttack_.attackParameter_ >= totalTime) {
		// コンボ継続なら次のコンボに進む
		if (workAttack_.comboNext_) {
			// コンボ継続フラグをリセットする
			workAttack_.comboNext_ = false;
			workAttack_.comboIndex_++;
			if (workAttack_.comboIndex_ >= kComboNum) {
				workAttack_.comboIndex_ = 0;
			};
			// 攻撃のパラメーターリセット
			workAttack_.attackParameter_ = 0;
			workAttack_.inComboPhase_ = 0;
			Move();
		}
		// コンボ継続出ないなら終了
		else {
			behaviorRequest_ = Behavior::kRoot;
		}
	}

	GetPhase();

	// コンボ段階によってモーションを分岐
	switch (workAttack_.comboIndex_) {
	case 0:
		attackDamage_ = 20;
		if (workAttack_.inComboPhase_ == 2) {
			
		}
		break;
	case 1:
		attackDamage_ = 30;
		if (workAttack_.inComboPhase_ == 2) {
			
		}
		break;
	case 2:
		attackDamage_ = 100;
		if (workAttack_.inComboPhase_ == 0) {
			float a = 2.0f / kConstAttacks_[workAttack_.comboIndex_].anticipationTime;
			//parts_[2].rotation_.x += a;
			//parts_[3].rotation_.x += a;
			//parts_[4].rotation_.x += a;
			//playerTrans_.rotation_.y -= AngleToRadian(90.0f / kConstAttacks_[workAttack_.comboIndex_].anticipationTime);
		}
		else if (workAttack_.inComboPhase_ == 1) {
			//playerTrans_.rotation_.y += AngleToRadian(270.0f / kConstAttacks_[workAttack_.comboIndex_].chargeTime);
		}
		else if (workAttack_.inComboPhase_ == 2) {
			//float a = 1.5f / kConstAttacks_[workAttack_.comboIndex_].swingTime;
			//parts_[2].rotation_.x -= a;
			//parts_[3].rotation_.x -= a;
			//parts_[4].rotation_.x -= a;
			//playerTrans_.rotation_.y += AngleToRadian(180.0f / kConstAttacks_[workAttack_.comboIndex_].swingTime);
		}
		break;
	}
}

void Player::GetPhase() {
	uint32_t totalTime = kConstAttacks_[workAttack_.comboIndex_].anticipationTime;
	if (workAttack_.attackParameter_ < totalTime) {
		workAttack_.inComboPhase_ = 0;
		return;
	}
	totalTime += kConstAttacks_[workAttack_.comboIndex_].chargeTime;
	if (workAttack_.attackParameter_ < totalTime) {
		workAttack_.inComboPhase_ = 1;
		return;
	}
	totalTime += kConstAttacks_[workAttack_.comboIndex_].swingTime;
	if (workAttack_.attackParameter_ < totalTime) {
		workAttack_.inComboPhase_ = 2;
		return;
	}
	totalTime += kConstAttacks_[workAttack_.comboIndex_].recoveryTime;
	if (workAttack_.attackParameter_ < totalTime) {
		workAttack_.inComboPhase_ = 3;
		return;
	}
}

void Player::DoWireMoving() {
	auto input = Input::GetInstance();
	static bool flag = false;
	static Vector3 vec;
	static Vector3 prePosition;
	static float num = 0.0f;
	if (input->GetKey()->TriggerKey(DIK_B) && flag == false) {
		//vec = FindVector(transform_.translation_, targetTransform_.translation_);
		//vec = Normalize(vec);
		vec = targetTransform_.translation_;
		if (vec == Vector3::zero) { return; } // 早期リターン
		prePosition = transform_.translation_;
		num = 0.0f;
		flag = true;
		isJamped_ = false;
		acceleration_ = 2.0f;
	}
	// 一番近くのターゲットのベクトルを取得して加速度を上げた移動をさせたい
	// イージング使う方が良さそうなので打診
	if (flag) {
		/*const float resistanceValue = 0.05f;
		acceleration_ -= resistanceValue;
		transform_.translation_ += vec * acceleration_;
		if (acceleration_ <= 0.0f) {
			acceleration_ = 0.0f;
			flag = false;
		}*/
		float T = Easing::EaseOutQuart(num);
		T = std::clamp(T, 0.0f, 1.0f);
		transform_.translation_ = Lerp(prePosition, vec, T);
		if (T >= 1.0f) {
			flag = false;
			isJamped_ = true;
			acceleration_ = 0.0f;
		}
		else {
			num += 1.0f / 60.0f;
		}
	}
}
