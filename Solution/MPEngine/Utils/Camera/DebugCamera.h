#pragma once
#include "Camera.h"
#include "Utils/WorldTransform/WorldTransform.h"

class DebugCamera {
public:
	DebugCamera();
	~DebugCamera() = default;

	WorldTransform Update();

	void DrawImGui();
private:
	WorldTransform transform_;
	float moveSpeed_ = 0.01f;
	float rotSpeed_ = 0.2f;
	float offsetSpeed_ = 0.5f;

};
