#pragma once
#include "MPEngine/Graphics/Model/Model.h"
#include "Utils/WorldTransform/WorldTransform.h"
#include <memory>
#include <optional>

class Player {
public:
	Player() = default;
	~Player() = default;

	void Initialize();
	void Update();
	
	const WorldTransform& GetTransform() const { return transform_; }

private:
	void DrawImGui();
	void Move();
	void Jamp();
	void TransformUpdate();
	void LimitMoving(); // 移動制限用

	void BehaviorRootUpdate();

private:
	WorldTransform transform_;
	std::shared_ptr<Model> model_;

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

};
