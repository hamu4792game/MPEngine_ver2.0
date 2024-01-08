#pragma once
#include "Utils/WorldTransform/WorldTransform.h"

class FollowCamera {
public:
	FollowCamera() = default;
	~FollowCamera() = default;
	void Initialize(const WorldTransform& transform = WorldTransform());
	void Update();
	void DrawImGui();

	void SetTarget(const WorldTransform* target) { target_ = target; }
	const WorldTransform& GetTransform() { return transform_; }
	void CameraMove();
private:
	WorldTransform transform_;
	const WorldTransform* target_ = nullptr;
	Vector3 preTranslate_;
	Vector3 offset_;
};
