#pragma once
#include "MPEngine/Graphics/Model/Model.h"
#include "Utils/WorldTransform/WorldTransform.h"
#include <memory>
#include <optional>
#include <vector>
#include <string>
#include "Math/AABB.h"
#include "Game/Camera/FollowCamera.h"
#include "Game/Camera/WireCamera.h"
#include "Game/Camera/AttackCamera.h"
#include "Game/Player/PlayerParticle.h"
#include "PlayerAnimation.h"
#include "PlayerAttack.h"


class sPlayer {
public:
	sPlayer() = default;
	~sPlayer() = default;

	void Initialize();
	void Update();
	void TitleUpdate();
	WorldTransform PostUpdate();
	
	const WorldTransform& GetTransform() const { return transform_; }

	void SetTargetTrans(const WorldTransform* transform) { targetTransform_ = transform; }

	bool OnCollision(const AABB* aabb);
	void OnCollisionStage(const AABB* aabb);

private:
	void DrawImGui();
	void Move();
	void TitleMove();
	void Jamp();
	void TransformUpdate();
	void LimitMoving(); // 移動制限用

	void InitializeAttack();
	void InitializeFall(); // 落下の初期化

	void BehaviorRootUpdate();
	void BehaviorAttackUpdate();
	void BehaviorDashUpdate();

	void DoWireMoving(); // ワイヤー移動

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
	std::vector<WorldTransform> partsTrans_;
	std::vector<std::shared_ptr<Model>> models_;

	// 落下用ステータス ジャンプも含む
	struct FallParam {
		float acceleration_ = 0.0f; // 落下時の加速度
		bool isJumpable_ = true; // ジャンプ可能かのフラグ
		bool isFalled_ = false; // 落下中かのフラグ true:落ちている/false:落ちていない
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
	std::shared_ptr<WireCamera> wireCamera_;

	// パーティクル
	std::shared_ptr<PlayerParticle> playerParticle_;

	// アニメーション
	std::unique_ptr<PlayerAnimation> animation_;
	bool isAnime_ = true;

	int moveVecFlag_ = 0;
	// 攻撃用クラス
	std::unique_ptr<PlayerAttack> playerAttack_;

};
