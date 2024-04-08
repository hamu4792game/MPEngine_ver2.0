#pragma once
#include "math/Vector3.h"
#include <array>

class Sphere {
public:
	Sphere() = default;
	~Sphere() = default;

public:
	Vector3 center_;
	Vector3 rotate_;
	float radius_ = 0.0f;

public:
	void Initialize(const float& radius);
	void Update(WorldTransform& transform);
	void Draw(const Matrix4x4& viewProjection, uint32_t color);
};