#include "PlayerAnimation.h"
#include "Player.h"

void PlayerAnimation::SetPartsPtr(WorldTransform* parts) {
	partsTrans_ = parts;
	Initialize();
}

void PlayerAnimation::Request(uint32_t type) {
	type_ = type;
	switch (type_) {
	case Player::AnimationType::Normal:
		NormalInitialize();
		break;
	}
}

void PlayerAnimation::Update() {
	switch (type_) {
	case Player::AnimationType::Normal:
		NormalUpdate();
		break;
	}
}

void PlayerAnimation::Initialize() {
	normal_.offsets.resize(Player::Parts::kMaxParts);
	normal_.prePositions.resize(Player::Parts::kMaxParts);
}

void PlayerAnimation::NormalInitialize() {
	for (uint32_t index = 0u; index < normal_.offsets.size(); index++) {
		normal_.offsets.at(index) = partsTrans_[index].translation_;
		normal_.prePositions.at(index) = partsTrans_[index].translation_;
	}
	partsTrans_[Player::Parts::Tracking1].rotation_.y = AngleToRadian(0.0f);
	partsTrans_[Player::Parts::Tracking2].rotation_.y = AngleToRadian(120.0f);
	partsTrans_[Player::Parts::Tracking3].rotation_.y = AngleToRadian(240.0f);
}

void PlayerAnimation::NormalUpdate() {
	//partsTrans_[Player::Parts::Head].rotation_.y += AngleToRadian(1.0f);

	for (uint32_t index = Player::Parts::Tracking1; index <= Player::Parts::Tracking3; index++) {
		partsTrans_[index].rotation_.y += AngleToRadian(1.0f);
		if (partsTrans_[index].rotation_.y >= AngleToRadian(360.0f)) {
			partsTrans_[index].rotation_.y -= AngleToRadian(360.0f);
		}

		Vector3 lOffset = normal_.offsets[index];
		lOffset = TargetOffset(lOffset, partsTrans_[index].rotation_);

		// 遅延は武器用
		normal_.prePositions.at(index) = Lerp(normal_.prePositions.at(index), partsTrans_[Player::Parts::Body].GetPosition(), 1.0f);

		partsTrans_[index].translation_ = lOffset + normal_.prePositions.at(index);
	}	

}
