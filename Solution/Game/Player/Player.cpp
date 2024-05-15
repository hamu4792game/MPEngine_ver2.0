#include "Player.h"
#include "ImGuiManager/ImGuiManager.h"
#include "Input/input.h"
#include "Utils/Camera/Camera3d.h"
#include "Utils/Easing/Easing.h"
#include "Utils/GlobalVariables/GlobalVariables.h"
#undef min
#undef max
#include "MPEngine/Graphics/RenderManager/RenderManager.h"

void Player::Initialize() {
#pragma region Models
	auto rsManager = ResourceManager::GetInstance();
	for (auto& model : models_) {
		model = std::make_shared<Model>();
	}

	models_.at(Parts::Body)->SetModel(rsManager->FindObject3d("Human"));
	models_.at(Parts::Head)->SetModel(rsManager->FindObject3d("PlayerHead"));
	models_.at(Parts::L_Arm)->SetModel(rsManager->FindObject3d("PlayerLeftArm"));
	models_.at(Parts::R_Arm)->SetModel(rsManager->FindObject3d("PlayerRightArm"));
	models_.at(Parts::L_Legs)->SetModel(rsManager->FindObject3d("PlayerLegs"));
	models_.at(Parts::R_Legs)->SetModel(rsManager->FindObject3d("PlayerLegs"));
	models_.at(Parts::Weapon)->SetModel(rsManager->FindObject3d("PlayerWeapon"));
	models_.at(Parts::Tracking1)->SetModel(rsManager->FindObject3d("PlayerLegs"));
	models_.at(Parts::Tracking2)->SetModel(rsManager->FindObject3d("PlayerLegs"));
	models_.at(Parts::Tracking3)->SetModel(rsManager->FindObject3d("PlayerLegs"));
	for (auto& model : models_) {
		model->isActive_ = false;
	}

	models_.at(Parts::Body)->isActive_ = true;

	for (auto& anime : animation_) {
		anime = std::make_unique<ModelAnimation>();
	}
	animation_.at(0)->Load(rsManager->FindAnimation("HumanWait"), models_.at(Parts::Body).get());
	animation_.at(1)->Load(rsManager->FindAnimation("HumanWalk"), models_.at(Parts::Body).get());
	models_.at(Parts::Body)->SetAnimation(animation_.at(0).get());

	partsTrans_.at(Parts::Body).parent_ = &transform_;
	partsTrans_.at(Parts::Head).parent_ = &partsTrans_.at(Parts::Body);
	partsTrans_.at(Parts::L_Arm).parent_ = &partsTrans_.at(Parts::Body);
	partsTrans_.at(Parts::R_Arm).parent_ = &partsTrans_.at(Parts::Body);
	partsTrans_.at(Parts::L_Legs).parent_ = &partsTrans_.at(Parts::Body);
	partsTrans_.at(Parts::R_Legs).parent_ = &partsTrans_.at(Parts::Body);
	partsTrans_.at(Parts::Weapon).parent_ = &partsTrans_.at(Parts::Body);

	auto global = GlobalVariables::GetInstance();
	global->LoadFile(itemName_);
	for (uint32_t index = 0u; index < Parts::kMaxParts; index++) {
		partsTrans_.at(index).scale_ = global->GetVector3Value(itemName_, ("PartsTrans : scale" + std::to_string(index)).c_str());
		partsTrans_.at(index).rotation_ = global->GetVector3Value(itemName_, ("PartsTrans : rotate" + std::to_string(index)).c_str());
		partsTrans_.at(index).translation_ = global->GetVector3Value(itemName_, ("PartsTrans : translate" + std::to_string(index)).c_str());
	}
#pragma endregion

	transform_.scale_ = Vector3::one;
	transform_.translation_ = Vector3(120.0f, 22.0f, -100.0f);
	//transform_.translation_ = Vector3(0.0f, 22.0f, 0.0f);
	transform_.UpdateMatrix();

	collision_ = std::make_shared<AABB>();
	collision_->size = Vector3(1.0f, 2.4f, 1.0f);
	fallParam_.Initialize();

	followCamera_ = std::make_shared<FollowCamera>();
	followCamera_->SetTarget(&transform_);
	followCamera_->Initialize();
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
			followCamera_->SetTarget(&transform_);
			followCamera_->SetParam(Vector3(0.0f, 2.0f, -10.0f), Vector3(AngleToRadian(5.0f), transform_.rotation_.y, followCamera_->GetTransform().rotation_.z), 0.05f);
			break;
		case Behavior::kAttack:
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
		break;
	case Behavior::kDash:
		break;
	}

	LimitMoving();
	followCamera_->CameraMove();
	TransformUpdate();

	//animationTime_ = 0.0f;
	animationTime_ = models_.at(Parts::Body)->GetAnimation()->ApplyAnimation(animationTime_);
	//animationTime_ = animation_->ApplyAnimation(animationTime_);
	static const float frameSpeed = 1.0f / 60.0f;
	animationTime_ += frameSpeed;
	models_.at(Parts::Body)->GetAnimation()->Update(models_.at(Parts::Body)->GetTransform());
}

WorldTransform Player::PostUpdate() {
	WorldTransform cameraTrans;
	followCamera_->Update();
	cameraTrans = followCamera_->GetTransform();
	return cameraTrans;
}

void Player::OnCollisionStage(const AABB* aabb) {
	// ローカル変数にrつけてるだけ
	bool iscoll = collision_->IsCollision(aabb);
	// 床との衝突判定
	if (iscoll) {
		Vector3 extrusionVector;
		// 地面と当たっているので初期化
		fallParam_.Initialize();
		// size同士
		Vector3 rScale = collision_->boxModel_.scale_ + aabb->boxModel_.scale_;
		// 距離 playerからboxの距離
		Vector3 rPosA = collision_->boxModel_.GetPosition();
		Vector3 rPosB = aabb->boxModel_.GetPosition();
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
		//collision_->boxModel_->SetColor(0xffffffaa);
	}
}

void Player::DrawImGui() {
#ifdef _DEBUG
	ImGui::Begin(itemName_.c_str(), nullptr, ImGuiWindowFlags_MenuBar);
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
			/*if (ImGui::TreeNode("Collision")) {
				ImGui::DragFloat3("size", &collision_->size.x, 0.1f);
				ImGui::TreePop();
			}*/

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
		/*if (ImGui::BeginMenu("Animation")) {
			if (ImGui::Button("Animation")) {
				isAnime_ = !isAnime_;
			}
			ImGui::EndMenu();
		}*/
		ImGui::EndMenuBar();

	}
	ImGui::End();

	auto input = Input::GetInstance()->GetMouse();
	ImGui::Begin("Mouse");
	ImGui::Text("while %f", input->GetMouseWheel());
	ImGui::Text("move %02f : %02f", input->GetMouseMove().x, input->GetMouseMove().y);
	ImGui::Text("screenpos %02f : %02f", input->GetScreenPosition().x,input->GetScreenPosition().y);
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

	if (input->GetPad()->GetPadConnect()) {
		Vector2 pMove(0.0f, 0.0f);
		pMove = input->GetPad()->GetPadLStick();
		//	移動があれば代入する
		if (pMove.x != 0.0f || pMove.y != 0.0f)
		{
			move.x = pMove.x;
			move.z = pMove.y;
		}
	}
	models_.at(Parts::Body)->SetAnimation(animation_.at(0).get());
	if (move != Vector3::zero) {
		move = Normalize(move);
		// 移動ベクトルをカメラの角度だけ回転させる
		move = TargetOffset(move, Camera3d::GetInstance()->GetTransform().rotation_);
		move.y = 0.0f;
		transform_.translation_ += move;
		//partsTrans_[Parts::Body].rotation_.y = FindAngle(move, Vector3(0.0f, 0.0f, 1.0f));
		transform_.rotation_.y = FindAngle(move, Vector3(0.0f, 0.0f, 1.0f));

		models_.at(Parts::Body)->SetAnimation(animation_.at(1).get());
	}
}

void Player::Jamp() {
	bool flag = false;
	auto input = Input::GetInstance();
	if (input->GetKey()->TriggerKey(DIK_SPACE)) {
		flag = true;
	}
	if (input->GetPad()->GetPadConnect()) {
		if (input->GetPad()->GetPadButtonDown(XINPUT_GAMEPAD_A)) {
			flag = true;
		}
	}
	
	if (flag && fallParam_.isJumpable_ && !fallParam_.isFalled_) {
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
		models_.at(index)->SetTransform(partsTrans_.at(index));
	}
	collision_->Update(transform_);
}

void Player::LimitMoving() {
	transform_.translation_.y = std::clamp(transform_.translation_.y, 15.0f, 10000.0f);
	if (transform_.translation_.y < 15.0f) {
		transform_.translation_ = Vector3(0.0f, 22.0f, -100.0f);
	}
}

void Player::BehaviorRootUpdate() {
	Move();
	Jamp();
}
