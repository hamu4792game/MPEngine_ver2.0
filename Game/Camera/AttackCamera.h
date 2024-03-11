#pragma once
#include "Utils/WorldTransform/WorldTransform.h"

class AttackCamera {
public:
	AttackCamera() = default;
	~AttackCamera() = default;
	void Initialize(const WorldTransform& transform = WorldTransform());
	void Update(const Vector3& lockon);
	void DrawImGui();

	void SetTarget(const WorldTransform* target) { target_ = target; }
	const WorldTransform& GetTransform() { return transform_; }

private:
	WorldTransform transform_;
	const WorldTransform* target_ = nullptr;
	Vector3 offset_;

};
