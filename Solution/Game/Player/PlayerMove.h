#pragma once
#include "Utils/WorldTransform/WorldTransform.h"

// 移動系を纏めたクラス
class PlayerMove {
public:
	PlayerMove();
	~PlayerMove() = default;
	void Initialize(Vector3* modelRotate);

	bool Update(const Vector3& direction);

	// 移動量を取得
	const Vector3& GetMoveValue() const { return moveVector_; }

private:
	// 入力した向きにただ移動する処理
	void InputMove(const Vector3& direction);

	
private:
	Vector3* modelRotate_ptr = nullptr; // playerModel用回転変数
	bool isMoved_ = false; // 移動したか
	Vector3 moveVector_; // 移動量

	// 速度パラメーター
	struct SpeedParam {
		float acceleration = 0.0f; // 加速度
		float accelerationRate = 0.1f; // 加速率
		float kMinAcceleration = 0.0f; // 最低加速度
		float kMaxAcceleration = 5.0f; // 最大加速度
		void Initialize(const float& acc, const float& rate, const float& min, const float& max) {
			acceleration = acc; // 初速度を与えてる
			accelerationRate = rate;
			kMinAcceleration = min;
			kMaxAcceleration = max;
		}
	};

	// 落下用ステータス ジャンプも含む
	struct FallParam {
		SpeedParam fall; // 落下時の加速度等
		bool isFalled = false; // 落下中かのフラグ true:落ちている/false:落ちていない
		bool isJump = false; // ジャンプするかのフラグ。キー入力時に立つ
		bool isJumpable = true; // ジャンプ可能かのフラグ
		bool oldJumpable = false; // 前の状態フラグ
		// ジャンプ用の初期化処理
		void JumpInitialize() {
			isFalled = false;
			isJumpable = true;
			oldJumpable = false;
			fall.acceleration = 0.0f;
		}
	};

	FallParam fallParam_;

};
