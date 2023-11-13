#pragma once
#include "math/Vector3.h"
#include "Engine/Texture/Line/Line.h"
#include "math/Sphere/Sphere.h"

//	AABB
class AABB
{
public:
	AABB() = default;
	~AABB() = default;

	Vector3 min;	// 最小点
	Vector3 max;	// 最大点
	Line line[12];
	Vector3 size{ 1.0f,1.0f,1.0f };

public:
	void Update(WorldTransform& transform);
	//	AABBと球の衝突判定
	bool IsCollision(Sphere* sphere);
	//	AABB同士の衝突判定
	bool IsCollision(AABB* aabb2);
	//	AABBの描画
	void DrawAABB(const Matrix4x4& viewProjectionMatrix, uint32_t color);

};

