#include "PlayerAttack.h"
#include "Input/Input.h"
#include "Player.h"
#include "Game/Camera/FollowCamera.h"

// コンボ定数表
const std::array<PlayerAttack::ConstAttack, PlayerAttack::kComboNum> PlayerAttack::kConstAttacks_ = {
	{
		{0,0,20,20,0.0f,0.0f,0.0f},
		{0,0,20,20,0.0f,0.0f,0.0f},
		{10,20,20,30,0.0f,0.0f,0.0f},
	}
};

void PlayerAttack::SetPtr(WorldTransform* playerTransPtr, WorldTransform* partsTransPtr, FollowCamera* followCameraPtr) {
	playerTrans = playerTransPtr;
	partsTrans = partsTransPtr;
	followCamera = followCameraPtr;
}

void PlayerAttack::Initialize() {
	// コンボ継続フラグをリセットする
	workAttack_.comboNext_ = false;
	// 攻撃のパラメーターリセット
	workAttack_.attackParameter_ = 0;
	workAttack_.inComboPhase_ = 0;
	workAttack_.comboIndex_ = 0;
	partsTrans[Player::Parts::Weapon].rotation_ = Vector3::zero;
}

bool PlayerAttack::Update(const WorldTransform* target) {
	// ダッシュアタックが出来れば攻撃不可
	if (DashAttack(target)) { return true; }
	// コンボが出来なければ終了
	if (!AttackProcess()) { return false; }
	GetPhase();
	AttackAnimation();
	return true;
}

void PlayerAttack::GetPhase() {
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

bool PlayerAttack::AttackProcess() {
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
			
		}
		// コンボ継続出ないなら終了
		else {
			return false;
		}
	}
	return true;
}

void PlayerAttack::AttackAnimation() {
	// コンボ段階によってモーションを分岐
	switch (workAttack_.comboIndex_) {
	case AttackPattern::LeftSwingAttack:
		attackDamage_ = 20;
		if (workAttack_.inComboPhase_ == 2) {
			partsTrans[Player::Parts::Weapon].rotation_.y += AngleToRadian(2.0f);
			partsTrans[Player::Parts::Weapon].rotation_.x = AngleToRadian(90.0f);
			followCamera->SetTarget(playerTrans);
			followCamera->SetParam(Vector3(0.0f, 2.0f, -15.0f), Vector3(AngleToRadian(5.0f), followCamera->GetTransform().rotation_.y, followCamera->GetTransform().rotation_.z), 0.1f);
		}
		break;
	case AttackPattern::RightSwingAttack:
		attackDamage_ = 30;
		if (workAttack_.inComboPhase_ == 2) {
			partsTrans[Player::Parts::Weapon].rotation_.y -= AngleToRadian(4.0f);
			followCamera->SetParam(Vector3(0.0f, 1.0f, -10.0f), Vector3(AngleToRadian(2.0f), followCamera->GetTransform().rotation_.y, followCamera->GetTransform().rotation_.z), 0.1f);
		}
		break;
	case AttackPattern::SwingStrongAttack:
		attackDamage_ = 100;
		if (workAttack_.inComboPhase_ == 0) {
			partsTrans[Player::Parts::Weapon].rotation_.y += AngleToRadian(4.0f);
			partsTrans[Player::Parts::Weapon].rotation_.x -= AngleToRadian(4.0f);
			playerTrans->translation_.y += 1.0f;
			followCamera->SetParam(Vector3(0.0f, -2.0f, -30.0f), Vector3(AngleToRadian(5.0f), followCamera->GetTransform().rotation_.y, followCamera->GetTransform().rotation_.z), 0.3f);
		}
		else if (workAttack_.inComboPhase_ == 1) {
			//playerTrans_.rotation_.y += AngleToRadian(270.0f / kConstAttacks_[workAttack_.comboIndex_].chargeTime);

		}
		else if (workAttack_.inComboPhase_ == 2) {
			partsTrans[Player::Parts::Weapon].rotation_.x += AngleToRadian(2.0f);
			playerTrans->translation_.y -= 0.5f;
			followCamera->SetParam(Vector3(0.0f, 0.0f, -10.0f), Vector3(AngleToRadian(5.0f), playerTrans->rotation_.y + AngleToRadian(-45.0f), followCamera->GetTransform().rotation_.z), 0.2f);
		}
		break;
	}
}

bool PlayerAttack::DashAttack(const WorldTransform* target) {

	// 早期リターン
	if (!target) { return false; }

	float distance = Distance(target->GetPosition(), playerTrans->GetPosition());
	if (distance <= 10.0f) { return false; } // 距離が近ければ通常攻撃へ

	float speed = 2.0f;
	Vector3 vec = FindVector(playerTrans->GetPosition(), target->GetPosition());
	vec = Normalize(vec) * speed;

	playerTrans->translation_ += vec;
	playerTrans->UpdateMatrix();

	// カメラ関係の更新
	followCamera->Update();
	followCamera->transform_.translation_ = followCamera->GetTransform().GetPosition();
	//followCamera->transform_.rotation_.y = FindAngle(target->GetPosition(), playerTrans->GetPosition());
	followCamera->SetTarget(nullptr);

	return true;

}

void PlayerAttack::InputProcess() {
	AttackButton handle;
	// 予備動作の時間
	auto input = Input::GetInstance();

	attackButton_ = handle;
}
