#pragma once
#include "Math/MathUtl.h"
#include "Utils/WorldTransform/WorldTransform.h"
#include <vector>
#include <memory>
#include "LineCollider.h"

struct AABB {
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
	void LineUpdate(std::vector<std::shared_ptr<class Line>> lines);

	bool IsCollision(const BoxCollider& coll, Vector3& minAxis, float& minOverlap) const;
	bool IsCollision(const LineCollider& coll, Vector3& minAxis, float& minOverlap) const;

private:
	void AABBUpdate();
	void OBBUpdate();

private:

	enum class Type {
		AABB,
		OBB,
	};
	Type type_ = Type::AABB;
	AABB aabb_;
	OBB obb_;

	WorldTransform transform_;

private:
	
	// AABBとAABBの衝突判定
	static bool IsCollision(const AABB& aabb1, const AABB& aabb2, Vector3& minAxis, float& minOverlap);
	// AABBとOBBの衝突判定
	static bool IsCollision(const AABB& aabb, const OBB& obb, Vector3& minAxis, float& minOverlap);
	// OBBとOBBの衝突判定
	static bool IsCollision(const OBB& obb1, const OBB& obb2, Vector3& minAxis, float& minOverlap);
	

	// OBBとRayの衝突判定
	static bool IsCollision(const OBB& obb, const Ray& ray, Vector3& hitPoint);

	
};
