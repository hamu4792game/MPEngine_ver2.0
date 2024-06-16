#pragma once
#include "Utils/WorldTransform/WorldTransform.h"

// playerとtarget間の移動ベクトルを渡すクラス
class WireTargetMove {
public:
	WireTargetMove();
	~WireTargetMove() = default;

	void Initialize(const Vector3& target, const Vector3& player);
	// resultに移動ベクトルを渡し、移動処理がおわったらtrueを返す
	bool Update(const Vector3& player, Vector3& result);

private:
	void MoveUpdate();

private:
	Vector3 direction_; // 向きベクトル
	Vector3 target_;
	
	// 速度パラメーター
	struct SpeedParam {
		float acceleration = 0.0f; // 加速度
		float accelerationRate = 0.1f; // 加速率
		const float kMaxAcceleration = 5.0f; // 最大加速度
		const float kMinAcceleration = 0.0f; // 最低加速度
		void Initialize(const float& acc,const float& accRate) {
			acceleration = acc; // 初速度を与えてる
			accelerationRate = accRate;
		}
	};
	SpeedParam speed_;


};
