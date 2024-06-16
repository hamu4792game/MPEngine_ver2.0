#include "WireTargetMove.h"
#include <algorithm>

WireTargetMove::WireTargetMove() {

}

void WireTargetMove::Initialize(const Vector3& target, const Vector3& player) {
	// playerからtargetのベクトルを求めて、
	direction_ = FindVector(player, target).Normalize();
	target_ = target;
	speed_.Initialize(3.0f, 0.1f);
}

bool WireTargetMove::Update(const Vector3& player, Vector3& result) {
	// のちのち距離が長ければ力が強く、短ければ弱くに変更する,また、距離で測るのではなく時間で失速させた方が安全なので、そこも考える

	bool flag = false;
	// 現在の距離がいくつ以下になったら速度を緩める
	float distance = Distance(player, target_);
	const float kDistanceLimit = 10.0f; // 距離制限
	
	if (distance <= 3.0f || speed_.acceleration <= 0.0f) {
		flag = true;
	}
	else if (distance <= kDistanceLimit) {
		// 距離が一定以下なら
		speed_.accelerationRate = -0.1f;
	}

	// 加速
	speed_.acceleration += speed_.accelerationRate;
	speed_.acceleration = std::clamp(speed_.acceleration, speed_.kMinAcceleration, speed_.kMaxAcceleration);

	result = direction_ * speed_.acceleration;
	return flag;
}

void WireTargetMove::MoveUpdate() {
	

}
