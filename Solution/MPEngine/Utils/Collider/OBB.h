#pragma once
#include "Math/MathUtl.h"
#include "Utils/WorldTransform/WorldTransform.h"
#include "Graphics/Model/Model.h"
#include <memory>
#include "Graphics/Line/Line.h"
#include <array>

class AABB;

class OBB {
public:
	OBB();
	~OBB() = default;

private:
	Vector3 center; // 中心点
	Vector3 orientations[3]; // 座標軸。正規化・直公必須
	Vector3 size = Vector3::one; // 座標軸方向の長さの半分。中心から面までの距離

	std::array<std::unique_ptr<Line>, 12> line;

public:
	void Update(class WorldTransform& transform);

	bool IsCollision(const AABB* aabb2) const;
	bool IsCollision(const OBB* obb2, Vector3& minAxis, float& minOverlap) const;

private:

};