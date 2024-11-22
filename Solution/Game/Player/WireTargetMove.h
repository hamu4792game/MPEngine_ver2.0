#pragma once
#include "Utils/WorldTransform/WorldTransform.h"
#include "Game/Global/AllParameter.h"

// playerとtarget間の移動ベクトルを渡すクラス
class WireTargetMove {
public:
	WireTargetMove() = default;
	WireTargetMove(SpeedParam* param) {
		param_ = param;
	};
	~WireTargetMove() = default;

	void Execute(const Vector3& target, const Vector3& player);
	// resultに移動ベクトルを渡し、移動処理がおわったらtrueを返す
	bool Update(const Vector3& player, Vector3& result);


private:
	Vector3 direction_; // 向きベクトル
	Vector3 target_;
	float oldDistance_ = 0.0f;
	SpeedParam* param_ = nullptr;

};
