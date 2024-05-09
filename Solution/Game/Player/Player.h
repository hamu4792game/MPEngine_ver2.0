#pragma once
#include "MPEngine/Graphics/Model/Model.h"
#include "Utils/WorldTransform/WorldTransform.h"
#include <memory>
#include <optional>
#include <array>
#include <string>
#include "Game/Camera/FollowCamera.h"
#include "Math/AABB.h"
#include "Graphics/Animation/ModelAnimation.h"

class Player {
public:
	Player() = default;
	~Player() = default;

	void Initialize();
	void Update();
	WorldTransform PostUpdate(); // カメラ用更新処理 カメラ座標を返す

	const WorldTransform& GetTransform() const { return transform_; }

	//void SetTargetTrans(const WorldTransform* transform) { targetTransform_ = transform; }
	void OnCollisionStage(const AABB* aabb);

private:
	void DrawImGui(); // ImGui処理
	void Move(); // 移動処理
	void Jamp(); // 重力処理
	void TransformUpdate(); // 座標更新処理
	void LimitMoving(); // 移動制限用

	void BehaviorRootUpdate();

private:
	std::string itemName_ = "Player";
public:
	enum Parts {
		Body,
		Head,
		L_Arm,
		R_Arm,
		L_Legs,
		R_Legs,
		Weapon,
		Tracking1,
		Tracking2,
		Tracking3,

		kMaxParts,
	};

	enum AnimationType {
		Normal,

		kMaxNum
	};

private:
	WorldTransform transform_;
	std::array<WorldTransform, Parts::kMaxParts> partsTrans_;
	std::array<std::shared_ptr<Model>, Parts::kMaxParts> models_;
	std::unique_ptr<ModelAnimation> animation_;
	float animationTime_ = 0.0f;

	// 落下用ステータス ジャンプも含む
	struct FallParam {
		float acceleration_ = 0.0f; // 落下時の加速度
		bool isJumpable_ = true; // ジャンプ可能かのフラグ
		bool isFalled_ = false; // 落下中かのフラグ true:落ちている/false:落ちていない
		void Initialize() {
			isJumpable_ = true;
			isFalled_ = false;
			acceleration_ = 0.0f;
		}
	};
	FallParam fallParam_;

	bool wireMove_ = false;
	bool wireInitialize_ = true;

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