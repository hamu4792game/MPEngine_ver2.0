#pragma once
#include "Utils/WorldTransform/WorldTransform.h"

class FollowCamera {
public:
	FollowCamera() = default;
	~FollowCamera() = default;
	void Update();
	void DrawImGui();

	void SetTarget(const WorldTransform* target) { target_ = target; }
	const WorldTransform& GetTransform() { return transform_; }
private:
	WorldTransform transform_;
	const WorldTransform* target_ = nullptr;

};
