#include "Player.h"
#include "externals/imgui/imgui.h"
#include "Input/input.h"
#include "Utils/Camera/Camera3d.h"
#include "Utils/Easing/Easing.h"
#include "Utils/GlobalVariables/GlobalVariables.h"
#undef min
#undef max

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

	dustParticle_ = std::make_shared<Particle>();
	dustParticle_->SetModel(rsManager->FindObject3d("PlayerBody"), 10u);
	float lIndex = 0.0f;
	dustParticle_->transform_.resize(10u);
	for (auto& dust : dustParticle_->transform_) {
		lIndex += 2.0f;
		dust.translation_.x += lIndex;
		dust.scale_ = Vector3(2.0f, 2.0f, 2.0f);
		dust.UpdateMatrix();
	}
	for (auto& dust : dustParticle_->color_) {
		dust = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	}

#pragma endregion


	transform_.scale_ = Vector3::one;
	transform_.translation_.z = -5.0f;
	transform_.translation_.y = 2.4f;
	transform_.UpdateMatrix();

	collision_ = std::make_shared<AABB>();
	collision_->size = Vector3(1.0f, 2.4f, 1.0f);
	InitializeFall();

	followCamera_ = std::make_shared<FollowCamera>();
	followCamera_->SetTarget(&transform_);
	followCamera_->Initialize();
	
	wireCamera_ = std::make_shared<WireCamera>();
	wireCamera_->SetTarget(&transform_);
	wireCamera_->Initialize(followCamera_->GetTransform());

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
	if (wireMove_) {
		wireCamera_->CameraMove();
	}
	else {
		followCamera_->CameraMove();
	}
	TransformUpdate();
}

WorldTransform Player::PostUpdate() {
	// カメラアップデート
	
	WorldTransform cameraTrans;
	if (wireMove_) {
		wireCamera_->Update();
		cameraTrans = wireCamera_->GetTransform();
	}
	else {
		followCamera_->Update();
		cameraTrans = followCamera_->GetTransform();
	}
	return cameraTrans;
}

void Player::OnCollision(const AABB* aabb) {
	// ローカル変数にrつけてるだけ
	bool iscoll = collision_->IsCollision(aabb);
	// 床との衝突判定
	if (iscoll) {
		Vector3 extrusionVector;
		collision_->boxModel_->SetColor(0x000000aa);
		// 地面と当たっているので初期化
		InitializeFall();

		Vector3 boxPositions[8]{
			Vector3(collision_->min.x,collision_->min.y,collision_->min.z), // 左下手前
			Vector3(collision_->min.x,collision_->min.y,collision_->max.z), // 左下奥
			Vector3(collision_->max.x,collision_->min.y,collision_->min.z), // 右下手前
			Vector3(collision_->min.x,collision_->min.y,collision_->max.z), // 右下奥

			Vector3(collision_->min.x,collision_->max.y,collision_->min.z), // 左上手前
			Vector3(collision_->min.x,collision_->max.y,collision_->max.z), // 左上奥
			Vector3(collision_->max.x,collision_->max.y,collision_->min.z), // 右上手前
			Vector3(collision_->max.x,collision_->max.y,collision_->max.z)  // 右下奥
		};

		// 最近接点を求める
		Vector3 crossPoint = Vector3(
			crossPoint.x = std::clamp(collision_->GetCenter().x, aabb->min.x, aabb->max.x),
			crossPoint.y = std::clamp(collision_->GetCenter().y, aabb->min.y, aabb->max.y),
			crossPoint.z = std::clamp(collision_->GetCenter().z, aabb->min.z, aabb->max.z)
		);
		
		for (auto& position : boxPositions) {
			if (collision_->IsCollision(position)) {
				extrusionVector = FindVector(crossPoint, position);
				extrusionVector = Vector3(std::fabs(extrusionVector.x), std::fabs(extrusionVector.y), std::fabs(extrusionVector.z));
				// ベクトルを求める
				float minDistance = std::min(extrusionVector.x, std::min(extrusionVector.y, extrusionVector.z));
				if (minDistance == extrusionVector.x) { extrusionVector = Vector3(extrusionVector.x, 0.0f, 0.0f); }
				else if (minDistance == extrusionVector.y) { extrusionVector = Vector3(0.0f, extrusionVector.y, 0.0f); }
				else if (minDistance == extrusionVector.z) { extrusionVector = Vector3(0.0f, 0.0f, extrusionVector.z); }

				Vector3 vec = FindVector(collision_->GetCenter(), aabb->GetCenter());
				if (vec * Vector3::one >= 0.0f) {
					transform_.translation_ += extrusionVector;
				}
				else {
					transform_.translation_ -= extrusionVector;
				}
				TransformUpdate();

			}
		}

		//extrusionVector = Normalize(extrusionVector);
		
		//// size同士
		//Vector3 rScale = collision_->boxModel_->transform_.scale_ + aabb->boxModel_->transform_.scale_;
		//// 距離 playerからboxの距離
		//Vector3 rPosA = collision_->boxModel_->transform_.GetPosition();
		//Vector3 rPosB = aabb->boxModel_->transform_.GetPosition();
		//Vector3 rDistance = rPosA - rPosB;
		//// 距離がsizeよりちいさかったらめり込んでいる 当たっている時点で通るがおまじない
		//// y軸
		//if (std::fabs(rScale.y) > std::fabs(rDistance.y)) {
		//	// size - 距離の差分を求めて指定の向きに足す
		//	float dis = std::fabs(rScale.y) - std::fabs(rDistance.y);
		//	// playerがboxよりも下にあったら
		//	if (rPosA.y <= rPosB.y) {
		//		extrusionVector.y -= dis;
		//	}
		//	// playerがboxよりも上にあったら
		//	else if (rPosA.y > rPosB.y) {
		//		extrusionVector.y += dis;
		//	}
		//}
		//// x軸
		//if (std::fabs(rScale.x) > std::fabs(rDistance.x)) {
		//	// size - 距離の差分を求めて指定の向きに足す
		//	float dis = std::fabs(rScale.x) - std::fabs(rDistance.x);
		//	// plaxerがboxよりも左にあったら
		//	if (rPosA.x <= rPosB.x) {
		//		extrusionVector.x -= dis;
		//	}
		//	// plaxerがboxよりも右にあったら
		//	else if (rPosA.x > rPosB.x) {
		//		extrusionVector.x += dis;
		//	}
		//}
		//// z軸
		//if (std::fabs(rScale.z) > std::fabs(rDistance.z)) {
		//	// size - 距離の差分を求めて指定の向きに足す
		//	float dis = std::fabs(rScale.z) - std::fabs(rDistance.z);
		//	// plazerがbozよりも下にあったら
		//	if (rPosA.z <= rPosB.z) {
		//		extrusionVector.z -= dis;
		//	}
		//	// plazerがbozよりも上にあったら
		//	else if (rPosA.z > rPosB.z) {
		//		extrusionVector.z += dis;
		//	}
		//}

		//float min = std::min(extrusionVector.x, std::min(extrusionVector.y, extrusionVector.z));
		//if (min == extrusionVector.x) { extrusionVector = Vector3(extrusionVector.x, 0.0f, 0.0f); }
		//else if (min == extrusionVector.y) { extrusionVector = Vector3(0.0f, extrusionVector.y, 0.0f); }
		//else if (min == extrusionVector.z) { extrusionVector = Vector3(0.0f, 0.0f, extrusionVector.z); }

		//transform_.translation_ += extrusionVector;
		//TransformUpdate();
	}
	else {
		collision_->boxModel_->SetColor(0xffffffaa);
	}

}

void Player::OnCollisionStage(const AABB* aabb) {
	// ローカル変数にrつけてるだけ
	bool iscoll = collision_->IsCollision(aabb);
	// 床との衝突判定
	if (iscoll) {
		Vector3 extrusionVector;
		collision_->boxModel_->SetColor(0x000000aa);
		// 地面と当たっているので初期化
		InitializeFall();
		// size同士
		Vector3 rScale = collision_->boxModel_->transform_.scale_ + aabb->boxModel_->transform_.scale_;
		// 距離 playerからboxの距離
		Vector3 rPosA = collision_->boxModel_->transform_.GetPosition();
		Vector3 rPosB = aabb->boxModel_->transform_.GetPosition();
		Vector3 rDistance = rPosA - rPosB;
		// 距離がsizeよりちいさかったらめり込んでいる 当たっている時点で通るがおまじない
		// y軸
		if (std::fabs(rScale.y) > std::fabs(rDistance.y)) {
			// size - 距離の差分を求めて指定の向きに足す
			float dis = std::fabs(rScale.y) - std::fabs(rDistance.y);
			// playerがboxよりも下にあったら
			if (rPosA.y <= rPosB.y) {
				extrusionVector.y -= dis;
			}
			// playerがboxよりも上にあったら
			else if (rPosA.y > rPosB.y) {
				extrusionVector.y += dis;
			}
		}
		
		transform_.translation_ += extrusionVector;
		TransformUpdate();
	}
	else {
		collision_->boxModel_->SetColor(0xffffffaa);
	}
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
				ImGui::DragFloat3("rotate", &transform_.rotation_.x, AngleToRadian(1.0f));
				ImGui::DragFloat3("scale", &transform_.scale_.x, 0.1f);
				ImGui::TreePop();
			}
			if (ImGui::TreeNode("Collision")) {
				ImGui::DragFloat3("size", &collision_->size.x, 0.1f);
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
	if (Input::GetInstance()->GetKey()->TriggerKey(DIK_SPACE) && fallParam_.isJumpable_ && !fallParam_.isFalled_) {
		// 初速度を与える
		fallParam_.isJumpable_ = false;
		fallParam_.acceleration_ = 1.0f;
		fallParam_.isFalled_ = true;
	}
	// 落下更新処理
	// 重力
	const float gravity_ = 0.05f;
	// 重力を足していく
	fallParam_.acceleration_ -= gravity_;
	transform_.translation_.y += fallParam_.acceleration_;

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
	if (transform_.translation_.y < 2.4f) {
		//transform_.translation_.y = 2.4f;
		//isJamped_ = false;
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

void Player::InitializeFall() {
	fallParam_.isJumpable_ = true;
	fallParam_.isFalled_ = false;
	fallParam_.acceleration_ = 0.0f;

}

void Player::BehaviorRootUpdate() {
	auto input = Input::GetInstance();
	if (input->GetKey()->TriggerKey(DIK_V) && wireMove_) { behaviorRequest_ = Behavior::kAttack; }
	if (input->GetKey()->TriggerKey(DIK_B) && !wireMove_) { wireMove_ = true; }

	Move();
	if (wireMove_) {
		DoWireMoving();
	}
	else {
		Jamp();
	}
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
	static Vector3 rVec;
	static float num = 0.0f;
	static bool flag = false;
	static float a = 0.0f;
	static Vector3 rTarget;
	if (wireInitialize_) {
		rVec = Normalize(FindVector(transform_.GetPosition(), targetTransform_.GetPosition()));
		// targetがいない場合は0を返すので早期リターン
		if (targetTransform_.GetPosition() == Vector3::zero) {
			wireMove_ = false;
			return;
		}
		num = 0.0f;
		rTarget = targetTransform_.GetPosition();
		wireInitialize_ = false;
		fallParam_.isJumpable_ = false;
		fallParam_.acceleration_ = 0.0f;
		fallParam_.isFalled_ = false;
		flag = false;
		wireCamera_->Initialize(followCamera_->GetTransform());
		wireCamera_->SetChangeFlag(true);
	}
	// 一番近くのターゲットのベクトルを取得して加速度を上げた移動をさせたい
	// イージング使う方が良さそうなので打診
	if (!wireInitialize_) {
		const float kMaxResistanceValue = 5.0f;

		// 減速する処理
		if (flag) {
			num += 1.0f / 20.0f;
			fallParam_.acceleration_ -= a;
			fallParam_.acceleration_ = std::clamp(fallParam_.acceleration_, 0.0f, kMaxResistanceValue);
			// 切替わり処理
			if (num >= 1.0f) {
				wireInitialize_ = true;
				wireMove_ = false;
				InitializeFall();
				followCamera_->Initialize(wireCamera_->GetTransform());
			}
		}
		else {
			num += 1.0f / 60.0f;
			// 2秒以上で加速開始
			if (num >= 1.0f) {
				if (num <= 1.1f) {
					fallParam_.acceleration_ = 1.0f;
					wireCamera_->SetChangeFlag(false);
				}
				const float kResistanceValue = 0.5f;
				fallParam_.acceleration_ += kResistanceValue;
				fallParam_.acceleration_ = std::clamp(fallParam_.acceleration_, 0.0f, kMaxResistanceValue);
				float distance = Distance(transform_.GetPosition(), rTarget);
				ImGui::Text("distance :%f", distance);
				// 距離が6以下の時にフラグを建てる
				if (distance < std::fabs(6.0f * kMaxResistanceValue)) {
					flag = true;
					num = 0.0f;
					a = (1.0f / 5.0f) * fallParam_.acceleration_;
				}
			}
		}

		transform_.translation_ += rVec * fallParam_.acceleration_;
	}
}
