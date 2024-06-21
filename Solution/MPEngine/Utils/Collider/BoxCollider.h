#pragma once
#include "Math/MathUtl.h"
#include "Utils/WorldTransform/WorldTransform.h"

struct AACBB {
	Vector3 min; // 最小点
	Vector3 max; // 最大点
	Vector3 size = Vector3::one;
};

struct OBB {
	Vector3 center; // 中心点
	Vector3 orientations[3]; // 座標軸。正規化・直公必須
	Vector3 size = Vector3::one; // 座標軸方向の長さの半分。中心から面までの距離
};

class BoxCollider {
public:
	BoxCollider() = default;
	~BoxCollider();

	void Update(const WorldTransform& transform);

	bool IsCollision(const BoxCollider& coll, Vector3& minAxis, float& minOverlap);

private:
	void AABBUpdate();
	void OBBUpdate();

private:

	enum class Type {
		AABB,
		OBB
	};
	Type type_ = Type::AABB;
	AACBB aabb_;
	OBB obb_;

	WorldTransform transform_;

private:
	static bool IsCollision(const OBB& obb1, const OBB& obb2, Vector3& minAxis, float& minOverlap);
};
