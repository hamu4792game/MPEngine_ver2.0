#pragma once
#include "Utils/Camera/Camera.h"
#include <memory>

class Camera3d {
private:
	Camera3d() = default;
	~Camera3d() = default;

public:
	static Camera3d* GetInstance();

	void Initialize(const float& farz);
	// viewProjectionMatrixの取得
	Matrix4x4 GetViewProMat();
	Matrix4x4 GetBillboardMat() const { return camera_.GetBillboardMat(); }
	// Set
	void SetTransform(const WorldTransform& transform) { camera_.transform = transform; }
	const WorldTransform& GetTransform() const { return camera_.transform; }
private:
	Camera camera_;

};