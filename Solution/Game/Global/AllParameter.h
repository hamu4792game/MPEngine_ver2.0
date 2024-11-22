#pragma once
#include "Math/MathUtl.h"
#include <algorithm>

// key入力用一時変数まとめ
struct InputParam {
	Vector3 move; // 入力方向ベクトル
	bool isJump = false; // ジャンプボタンが押されたか
	bool isWireMove = false; // ワイヤー移動ボタンが押されたか
	bool isDashMove = false; // ダッシュボタンが押されたか
	bool isSwingMove = false; // ウェブスイングをするか
	bool isPushSwing = false; // ウェブスイングを押した瞬間

	void Initialize() {
		move = Vector3::zero;
		isJump = false;
		isWireMove = false;
		isDashMove = false;
		isSwingMove = false;
		isPushSwing = false;
	}
};

// 速度パラメーター
struct SpeedParam {
	float acceleration = 0.0f; // 加速度
	float accelerationRate = 0.1f; // 加速率
	float kMinAcceleration = 0.0f; // 最低加速度
	float kMaxAcceleration = 5.0f; // 最大加速度

	SpeedParam& operator=(const SpeedParam& p) {
		this->acceleration = p.acceleration;
		this->accelerationRate = p.accelerationRate;
		this->kMinAcceleration = p.kMinAcceleration;
		this->kMaxAcceleration = p.kMaxAcceleration;
		return *this;
	}
	void Initialize(const float& acc, const float& rate, const float& min, const float& max) {
		acceleration = acc; // 初速度を与えてる
		accelerationRate = rate;
		kMinAcceleration = min;
		kMaxAcceleration = max;
	}
	void AccelInit(const float& acc = 0.0f) {
		acceleration = acc;
	}
	// 加算更新
	void AddUpdate() {
		acceleration += accelerationRate;
		acceleration = std::clamp(acceleration, kMinAcceleration, kMaxAcceleration);
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

// webswing用ステータス
struct WebSwingParam {
	Vector3 position; // playerの現在地
	Vector3 velocity; // ボールの速度
	Vector3 acceleration; // ボールの加速度
	float mass = 2.0f; // ボールの質量
};

// 移動に必要なパラメーターを渡すための構造体
struct MoveParam {
	SpeedParam inputMoveParam; // 通常移動時のパラメーター
	SpeedParam wireMoveParam; // ワイヤー移動のパラメーター
};