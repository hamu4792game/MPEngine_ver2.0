#pragma once
#include "Math/MathUtl.h"
#include "Utils/WorldTransform/WorldTransform.h"
#include "Graphics/Model/Model.h"
#include <memory>

//	AABB
class AABB {
public:
	AABB();
	~AABB() = default;

	Vector3 min;	// 最小点
	Vector3 max;	// 最大点
	Vector3 size{ 1.0f,1.0f,1.0f };

	std::shared_ptr<Model> boxModel_;

public:
	void Update(const WorldTransform& transform);
	//	AABBと球の衝突判定
	bool IsCollision(class Sphere* sphere) const;
	//	AABB同士の衝突判定
	bool IsCollision(const AABB* aabb2) const;

};
