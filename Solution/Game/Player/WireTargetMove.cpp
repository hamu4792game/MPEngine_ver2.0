#include "WireTargetMove.h"
#include <algorithm>
#include <limits>

void WireTargetMove::Execute(const Vector3& target, const Vector3& player) {
	// playerからtargetのベクトルを求めて、
	direction_ = FindVector(player, target).Normalize();
	// 圧倒的な大きさに初期化
	oldDistance_ = std::numeric_limits<float>::max();
	target_ = target;
	param_->AccelInit(3.0f);
}

bool WireTargetMove::Update(const Vector3& player, Vector3& result) {
	// のちのち距離が長ければ力が強く、短ければ弱くに変更する,また、距離で測るのではなく時間で失速させた方が安全なので、そこも考える

	bool flag = false;
	// 現在の距離がいくつ以下になったら速度を緩める
	float distance = Distance(player, target_);


	if (distance > oldDistance_ || param_->acceleration <= 0.0f) {
		flag = true;
	}

	// 加速
	param_->acceleration += param_->accelerationRate;
	param_->acceleration = std::clamp(param_->acceleration, param_->kMinAcceleration, param_->kMaxAcceleration);

	result = direction_ * param_->acceleration;
	oldDistance_ = distance;
	return flag;
}

