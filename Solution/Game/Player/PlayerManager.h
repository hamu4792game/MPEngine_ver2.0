#pragma once
#include <memory>
#include "PlayerAnimation.h"
#include "Utils/WorldTransform/WorldTransform.h"
#include "Game/Camera/FollowCamera.h"
#include "Math/AABB.h"

class PlayerManager {
public:
	PlayerManager();
	~PlayerManager() = default;

	void Initialize();
	void Update();
	WorldTransform PostUpdate(); // カメラ用更新処理 カメラ座標を返す

	const WorldTransform& GetTransform() const { return transform_; }

	void SetTargetTrans(const WorldTransform* transform) { targetTransform_ = transform; }
	void OnCollisionStage(const AABB* aabb);

private:
	const std::string itemName_ = "Player";
private:
	void DrawImGui(); // ImGui処理
	void Move(); // 移動処理
	void Jamp(); // 重力処理
	void TransformUpdate(); // 座標更新処理
	void LimitMoving(); // 移動制限用

	void BehaviorRootUpdate();

private:
	std::unique_ptr<PlayerAnimation> animation_;
	PlayerAnimation::BehaviorFlag behaviorFlag_;
	WorldTransform transform_;

	// 落下用ステータス ジャンプも含む
	struct FallParam {
		float acceleration_ = 0.0f; // 落下時の加速度
		bool isJumpable_ = true; // ジャンプ可能かのフラグ
		bool oldJumpable_ = false; // ジャンプ可能かのフラグ
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
	};
	Behavior behavior_ = Behavior::kRoot;
	// 次の振る舞いリクエスト
	std::optional<Behavior> behaviorRequest_ = std::nullopt;

	const WorldTransform* targetTransform_ = nullptr;

	std::shared_ptr<AABB> collision_;

	// カメラ関係
	std::shared_ptr<FollowCamera> followCamera_;

};
