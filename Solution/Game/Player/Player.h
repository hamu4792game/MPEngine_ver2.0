#pragma once
#include <memory>
#include "PlayerAnimation.h"
#include "Utils/WorldTransform/WorldTransform.h"
#include "Game/Camera/FollowCamera.h"
#include "WebSwing.h"
#include "Utils/Collider/Collider.h"
#include "Game/Global/AllParameter.h"
#include "MoveCommand.h"
#include "Game/Shadow/CircleShadow.h"
#include "Game/Wire/Wire.h"

class Player {
public:
	Player();
	~Player() = default;

	void Initialize(const WorldTransform& respawnpoint);
	void Update();
	WorldTransform PostUpdate(); // カメラ用更新処理 カメラ座標を返す

	const WorldTransform& GetTransform() const { return transform_; }

	void SetTargetTrans(const WorldTransform* transform) { targetTransform_ = transform; }
	// true ゲームクリア
	bool OnCollisionStage(const Collider& coll);
	// 下向きのレイとステージの衝突判定
	bool OnCollisionDownRayToStage(const Collider& coll, Vector3& hitPoint);
	WorldTransform OnCollisionCameraToStage(const Collider& coll);

	const uint32_t GetEffectNumber() const { return static_cast<uint32_t>(postEffectNum_); }
	Collider* GetCollision() const { return collision_.get(); }

private:
	const std::string itemName_ = "Player";
private:
	void DrawImGui(); // ImGui処理
	void TransformUpdate(); // 座標更新処理
	void LimitMoving(); // 移動制限用
	void KeyInput(); // プレイヤー入力処理まとめ

	void BehaviorRootUpdate();

	void SetGlobalVariables(MoveParam& param); // データの値を外部ファイルからセット
	void AddGlobalVariables(const MoveParam& param); // データを外部ファイルに出力

private:
	std::unique_ptr<PlayerAnimation> animation_;
	PlayerAnimation::BehaviorFlag behaviorFlag_;
	WorldTransform transform_;
	Vector3 moveVector_; // 移動ベクトル
	std::unique_ptr<MoveCommand> moveCom_; // 移動コマンド

	Vector3 velocity_; // 現在の速度
	Vector3 oldPosition_; // 1つ前のプレイヤーワールド座標
	InputParam inputParam_; // 入力パラメータ

	// 移動関係速度
	SpeedParam moveParameter_;

	Vector3 oldMoveVector; // 1f前の移動ベクトル

	FallParam fallParam_;

	// 振る舞い
	enum class Behavior {
		kRoot,	// 通常行動
		kWebSwing,	// ウェブスイング
		kDash,	// ダッシュ中
		kWireMove,	// ワイヤー移動中
	};
	Behavior behavior_ = Behavior::kRoot;
	// 次の振る舞いリクエスト
	std::optional<Behavior> behaviorRequest_ = std::nullopt;

	const WorldTransform* targetTransform_ = nullptr;

	std::unique_ptr<Collider> collision_;
	WorldTransform collTrans_;

	// 下向きのレイ
	std::unique_ptr<Collider> downCollision_;

	std::unique_ptr<WebSwing> webswing_;

	// カメラ関係
	std::shared_ptr<FollowCamera> followCamera_;

	WorldTransform respawnpoint_; // リス地

	// ポストエフェクト用
	enum PostEffectNum {
		None,
		RadialBlur,
		GrayScale,
	};
	PostEffectNum postEffectNum_ = PostEffectNum::None;

	float masterSpeed_ = 1.0f;
	struct FrameCount {
		float maxFrame = 1.0f;
		float count = 0.0f;
	};
	FrameCount frameCount_;
	FrameCount dashCount_;

	Vector3 hittingObjectNormal_; // 衝突している物体の法線ベクトル

	Vector3 temporarySaveMoveVector_; // 一時保存用移動ベクトル、壁移動前の姿勢に戻すために使用

	// ウェブスイング用
	bool isWebSwing_ = false;

	std::unique_ptr<CircleShadow> circleShadow_; // 落ち影
	float minDistance_ = 0.0f; // 最小の距離を記録するための変数

	// 壁走り用
	bool isWallRunning_ = false;
	bool isDashing_ = false;

	// ワイヤー描画用
	std::unique_ptr<Wire> wire_;

};
