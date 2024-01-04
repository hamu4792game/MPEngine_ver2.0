#pragma once
#include "MPEngine/Graphics/Model/Model.h"
#include "Utils/WorldTransform/WorldTransform.h"
#include <memory>
#include <optional>
#include <vector>
#include <string>

class Player {
public:
	Player() = default;
	~Player() = default;

	void Initialize();
	void Update();
	
	const WorldTransform& GetTransform() const { return transform_; }

	void SetTargetTrans(const WorldTransform& transform) { targetTransform_ = transform; }

private:
	void DrawImGui();
	void Move();
	void Jamp();
	void TransformUpdate();
	void LimitMoving(); // 移動制限用

	void InitializeAttack();

	void BehaviorRootUpdate();
	void BehaviorAttackUpdate();

	void GetPhase();
	void DoWireMoving(); // ワイヤー移動

private:
	std::string itemName_ = "Player";
	enum Parts {
		Body,
		Head,
		L_Arm,
		R_Arm,
		L_Legs,
		R_Legs,

		kMaxParts,
	};
	WorldTransform transform_;
	std::vector<WorldTransform> partsTrans_;
	std::vector<std::shared_ptr<Model>> models_;

	float acceleration_ = 0.0f; // 落下時の加速度
	bool isJamped_ = false; // ジャンプ中かのフラグ

	// 振る舞い
	enum class Behavior {
		kRoot,	// 通常行動
		kAttack,// 攻撃中
		kDash,	// ダッシュ中
	};
	Behavior behavior_ = Behavior::kRoot;
	// 次の振る舞いリクエスト
	std::optional<Behavior> behaviorRequest_ = std::nullopt;

	WorldTransform targetTransform_;

	// 攻撃用定数
	struct ConstAttack {
		uint32_t anticipationTime; // 振りかぶりの時間<frame>
		uint32_t chargeTime; // ため時間<frame>
		uint32_t swingTime; // 攻撃振りの時間
		uint32_t recoveryTime; // 硬直時間
		float anticipationSpeed; // 振りかぶりの移動速度
		float chargeSpeed; // ための移動速度
		float swingSpeed; // 攻撃振りの移動速度
	};
	// 攻撃用ワーク
	struct WorkAttack {
		// 攻撃ギミックの媒介変数
		uint32_t attackParameter_ = 0;
		int32_t comboIndex_ = 0;
		int32_t inComboPhase_ = 0;
		bool comboNext_ = false;
	};
	WorkAttack workAttack_;
	uint32_t attackDamage_ = 0u; // 攻撃力
	static const int kComboNum = 3; // 最大コンボ定数
	// コンボ定数表
	static const std::array<ConstAttack, kComboNum> kConstAttacks_;

};
