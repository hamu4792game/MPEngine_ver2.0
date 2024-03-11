#pragma once
#include "Utils/WorldTransform/WorldTransform.h"

class FollowCamera {
public:
	FollowCamera();
	~FollowCamera() = default;
	void Initialize(const WorldTransform& transform = WorldTransform());
	void SetParam(const Vector3& offset, const Vector3& rotate, float lerpSpeed = 0.2f);
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
	Vector3 preOffset_;
	Vector3 preRotate_;
	Vector3 postRotate_;
	float lerpSpeed_ = 0.0f;
};
