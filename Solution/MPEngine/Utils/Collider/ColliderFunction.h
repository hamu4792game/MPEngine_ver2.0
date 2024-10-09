#pragma once
#include "Math/MathUtl.h"
#include "BoxCollider.h"

class ColliderFunction {
public:
	ColliderFunction() = default;
	~ColliderFunction() = default;

public:
	static float DistanceToAABB(const Vector3& cameraPos, const AABB& box);

private:

};

