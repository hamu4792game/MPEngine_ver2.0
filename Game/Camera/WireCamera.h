#pragma once
#include "Utils/WorldTransform/WorldTransform.h"

class WireCamera {
public:
	WireCamera() = default;
	~WireCamera() = default;
	void Initialize(const WorldTransform& transform);
	void Update();
	void DrawImGui();

	void SetTarget(const WorldTransform* target) { target_ = target; }
	const WorldTransform& GetTransform() { return transform_; }
	void CameraMove();
	void SetChangeFlag(const bool& flag);
private:
	WorldTransform transform_;
	const WorldTransform* target_ = nullptr;
	Vector3 preTranslate_;
	Vector3 offset_;
	bool changeFlag_ = false;
	float easeNum_ = 0.0f;
};
