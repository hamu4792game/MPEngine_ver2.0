#pragma once
#include "Math/MathUtl.h"
#include <algorithm>
#include "ImGuiManager/ImGuiManager.h"
#include "Utils/GlobalVariables/GlobalVariables.h"

// パラメーターをまとめる用のhpp、基本publicで使用するためstructで記載

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

	// データの値を外部ファイルからセット
	void SetGlobalVariables(const std::string& itemName, const std::string& keyName) {
		GlobalVariables* gv = GlobalVariables::GetInstance();
		accelerationRate = gv->GetFloatValue(itemName, keyName + "_accelerationRate");
		kMinAcceleration = gv->GetFloatValue(itemName, keyName + "_kMinAcceleration");
		kMaxAcceleration = gv->GetFloatValue(itemName, keyName + "_kMaxAcceleration");
	}

	// データを外部ファイルに出力
	const void AddGlobalVariables(const std::string& itemName, const std::string& keyName) const {
		GlobalVariables* gv = GlobalVariables::GetInstance();
		gv->SetValue(itemName, keyName + "_accelerationRate", accelerationRate);
		gv->SetValue(itemName, keyName + "_kMinAcceleration", kMinAcceleration);
		gv->SetValue(itemName, keyName + "_kMaxAcceleration", kMaxAcceleration);
	}
	// imguiでの値編集
	const void ImGuiProc() {
#ifdef _DEBUG
		ImGui::DragFloat("accelerationRate", &accelerationRate, 0.01f);
		ImGui::DragFloat("kMinAcceleration", &kMinAcceleration, 0.01f);
		ImGui::DragFloat("kMaxAcceleration", &kMaxAcceleration, 0.01f);
#endif // _DEBUG
	}

};

// 落下用ステータス ジャンプも含む
struct FallParam {
	SpeedParam fall; // 落下時の加速度等
	bool isFalled = false; // 落下中かのフラグ true:落ちている/false:落ちていない
	bool isJumped = false; // 上昇中か
	bool isJumpable = true; // ジャンプ可能かのフラグ
	bool oldJumpable = false; // 前の状態フラグ
	// ジャンプ用の初期化処理
	void JumpInitialize() {
		isFalled = false;
		isJumped = true;
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

// ダッシュ用のステータス
struct DashParam {
	SpeedParam speed; // 基本的な加速度情報
	float accelerationTime = 0.0f; // 加速時間
	float decelerationTime = 0.0f; // 減速時間
};


// 移動に必要なパラメーターを渡すための構造体
struct MoveParam {
	SpeedParam inputMoveParam; // 通常移動時のパラメーター
	SpeedParam wireMoveParam; // ワイヤー移動のパラメーター
	SpeedParam fallParam; // 重力系のパラメーター
	DashParam dashParam; // ダッシュ用パラメーター
	float jumpFirstVelocity = 0.0f; // ジャンプする際の初速度
	float airMoveVelocity = 0.0f; // 滞空時にWASDで動かせる速度
};