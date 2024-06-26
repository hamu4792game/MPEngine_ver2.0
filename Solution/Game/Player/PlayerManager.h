#pragma once
#include <memory>
#include "PlayerAnimation.h"
#include "Utils/WorldTransform/WorldTransform.h"
#include "Game/Camera/FollowCamera.h"
#include "WebSwing.h"
#include "WireTargetMove.h"
#include "PlayerMove.h"
#include "Utils/Collider/Collider.h"

class PlayerManager {
public:
	PlayerManager();
	~PlayerManager() = default;

	void Initialize(const WorldTransform& respawnpoint);
	void Update();
	WorldTransform PostUpdate(); // カメラ用更新処理 カメラ座標を返す

	const WorldTransform& GetTransform() const { return transform_; }

	void SetTargetTrans(const WorldTransform* transform) { targetTransform_ = transform; }
	void OnCollisionStage(const Collider& coll);

private:
	const std::string itemName_ = "Player";
private:
	void DrawImGui(); // ImGui処理
	void Move(); // 移動処理
	void Jamp(); // 重力処理
	void TransformUpdate(); // 座標更新処理
	void LimitMoving(); // 移動制限用
	void KeyInput(); // プレイヤー入力処理まとめ

	void BehaviorRootUpdate();

private:
	std::unique_ptr<PlayerAnimation> animation_;
	PlayerAnimation::BehaviorFlag behaviorFlag_;
	WorldTransform transform_;
	float velocity_ = 1.0f;
	Vector3 moveVector_; // 移動ベクトル

	

	// key入力用一時変数まとめ
	struct InputParam {
		Vector3 move; // 入力方向ベクトル
		bool isJump = false; // ジャンプボタンが押されたか
		bool isWireMove = false; // ワイヤー移動ボタンが押されたか

		void Initialize() {
			move = Vector3::zero;
			isJump = false;
			isWireMove = false;
		}
	};
	InputParam inputParam_;

	// 落下用ステータス ジャンプも含む
	struct FallParam {
		float acceleration_ = 0.0f; // 落下時の加速度
		bool isJump_ = false; // ジャンプするかのフラグ。キー入力時に立つ
		bool isJumpable_ = true; // ジャンプ可能かのフラグ
		bool oldJumpable_ = false; // 前の状態フラグ
		bool isFalled_ = false; // 落下中かのフラグ true:落ちている/false:落ちていない
		void Initialize() {
			isJumpable_ = true;
			isFalled_ = false;
			oldJumpable_ = false;
			acceleration_ = 0.0f;
		}
	};
	FallParam fallParam_;

	// 振る舞い
	enum class Behavior {
		kRoot,	// 通常行動
		kAttack,// 攻撃中
		kDash,	// ダッシュ中
		kSwing,	// スイング中
	};
	Behavior behavior_ = Behavior::kRoot;
	// 次の振る舞いリクエスト
	std::optional<Behavior> behaviorRequest_ = std::nullopt;

	const WorldTransform* targetTransform_ = nullptr;

	std::unique_ptr<Collider> collision_;
	WorldTransform collTrans_;

	std::unique_ptr<WebSwing> webswing_;
	std::unique_ptr<WireTargetMove> wireTargetMove_;

	// カメラ関係
	std::shared_ptr<FollowCamera> followCamera_;

	std::unique_ptr<PlayerMove> playerMove_;

};
