#pragma once
#include "Math/MathUtl.h"
#include "Utils/WorldTransform/WorldTransform.h"
#include <vector>
#include <memory>

// 半直線
struct Ray {
	Vector3 origin; // 始点
	Vector3 diff;	// 終点への差分ベクトル
};

class LineCollider {
public:
	LineCollider() = default;
	~LineCollider() = default;

	void Update(const WorldTransform& transform);
	void LineUpdate(std::vector<std::shared_ptr<class Line>> lines);

	enum class Type {
		Ray,
	};
	Type GetType() const { return type_; }
	Ray GetRay() const { return ray_; }

private:
	Type type_;
	Ray ray_;


};