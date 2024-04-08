#pragma once
#include "Math/MathUtl.h"
#include "Utils/WorldTransform/WorldTransform.h"
#include "Graphics/Model/Model.h"
#include <memory>
#include "Graphics/Line/Line.h"
#include <array>

//	AABB
class AABB {
public:
	AABB();
	~AABB() = default;

	Vector3 min;	// 最小点
	Vector3 max;	// 最大点
	Vector3 size{ 1.0f,1.0f,1.0f };
	
	WorldTransform boxModel_;
private:
	Vector3 center_; // 中点
	struct Direction {
		Vector3 upper; // 上
		Vector3 lower; // 下
		Vector3 left; // 左
		Vector3 right; // 右
	};
	Direction direction_;
	std::array<std::unique_ptr<Line>, 12> line_;
public:
	void Update(const WorldTransform& transform);
	//	AABBと球の衝突判定
	bool IsCollision(class Sphere* sphere) const;
	//	AABB同士の衝突判定
	bool IsCollision(const AABB* aabb2) const;
	// AABBと点の衝突判定
	bool IsCollision(const Vector3& vector) const;

	static float CalculatePushBackVector(const AABB* aabb1, const AABB* aabb2);
	const Vector3& GetCenter() const { return center_; }

};
