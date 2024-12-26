#pragma once
#include <list>
#include <memory>
#include "Math/MathUtl.h"
#include "Graphics/Sprite/Sprite.h"
#include "Game/Target/Target.h"
#include "Utils/Collider/Collider.h"

// ターゲットロックオン用のクラス
class LockOn {
public:
	LockOn();
	~LockOn() = default;

	void Initialize();
	void Update(const std::list<std::shared_ptr<Target>>& targets);
	void OnCollisionStageToRay(const Collider& coll);
	
	WorldTransform* GetTargetTrans() const;

private:
	Vector2 ChangeScreen(const Vector3& worldPos);
	void Search(const std::list<std::shared_ptr<Target>>& targets);

	std::shared_ptr<Sprite> lockOnMark_;
	std::shared_ptr<Sprite> frame_;
	Vector2 framePos_;
	// 目標
	std::list<std::pair<float, const Target*>> targets_;
	// ロックオン対象
	const Target* target_ = nullptr;

	std::unique_ptr<Collider> collision_;
	WorldTransform collTrans_;

};