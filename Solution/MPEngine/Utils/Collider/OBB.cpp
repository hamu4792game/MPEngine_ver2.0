#include "OBB.h"
#include <cmath>
#include <algorithm>
#undef min
#undef max

OBB::OBB() {

}

void OBB::Update(WorldTransform& transform) {

}

bool OBB::IsCollision(const AABB* aabb2) const {
	return false;
}

bool OBB::IsCollision(const OBB* obb2, Vector3& minAxis, float& minOverlap) const {
	Vector3 axis[15]; // 分離軸候補

	// 各軸
	axis[0] = this->orientations[0];
	axis[1] = this->orientations[1];
	axis[2] = this->orientations[2];
	axis[3] = obb2->orientations[0];
	axis[4] = obb2->orientations[1];
	axis[5] = obb2->orientations[2];

	// 各辺のクロス積
	axis[6] = Cross(this->orientations[0], obb2->orientations[0]).Normalize();
	axis[7] = Cross(this->orientations[0], obb2->orientations[1]).Normalize();
	axis[8] = Cross(this->orientations[0], obb2->orientations[2]).Normalize();
	axis[9] = Cross(this->orientations[1], obb2->orientations[0]).Normalize();
	axis[10] = Cross(this->orientations[1], obb2->orientations[1]).Normalize();
	axis[11] = Cross(this->orientations[1], obb2->orientations[2]).Normalize();
	axis[12] = Cross(this->orientations[2], obb2->orientations[0]).Normalize();
	axis[13] = Cross(this->orientations[2], obb2->orientations[1]).Normalize();
	axis[14] = Cross(this->orientations[2], obb2->orientations[2]).Normalize();

	// 頂点
	Matrix4x4 obb1WorldMatrix;
	obb1WorldMatrix.m = {
		this->orientations[0].x, this->orientations[0].y, this->orientations[0].z, 0.0f,
		this->orientations[1].x, this->orientations[1].y, this->orientations[1].z, 0.0f,
		this->orientations[2].x, this->orientations[2].y, this->orientations[2].z, 0.0f,
		this->center.x,          this->center.y,          this->center.z,          1.0f 
	};
	Matrix4x4 obb2WorldMatrix;
	obb2WorldMatrix.m = {
		obb2->orientations[0].x, obb2->orientations[0].y, obb2->orientations[0].z, 0.0f,
		obb2->orientations[1].x, obb2->orientations[1].y, obb2->orientations[1].z, 0.0f,
		obb2->orientations[2].x, obb2->orientations[2].y, obb2->orientations[2].z, 0.0f,
		obb2->center.x,          obb2->center.y,          obb2->center.z,          1.0f 
	};


	Vector3 vertices1[] = {
		-this->size,
		{this->size.x,  -this->size.y, -this->size.z},
		{this->size.x,  -this->size.y, this->size.z },
		{-this->size.x, -this->size.y, this->size.z },
		{-this->size.x, this->size.y,  -this->size.z},
		{this->size.x,  this->size.y,  -this->size.z},
		this->size,
		{-this->size.x, this->size.y,  this->size.z }
	};

	Vector3 vertices2[] = {
		-obb2->size,
		{obb2->size.x,  -obb2->size.y, -obb2->size.z},
		{obb2->size.x,  -obb2->size.y, obb2->size.z },
		{-obb2->size.x, -obb2->size.y, obb2->size.z },
		{-obb2->size.x, obb2->size.y,  -obb2->size.z},
		{obb2->size.x,  obb2->size.y,  -obb2->size.z},
		obb2->size,
		{-obb2->size.x, obb2->size.y,  obb2->size.z }
	};

	for (int i = 0; i < 8; i++) {
		vertices1[i] = obb1WorldMatrix * vertices1[i];
		vertices2[i] = obb2WorldMatrix * vertices2[i];
	}

	float minoverlap = FLT_MAX;
	int axisIndex = 0;

	// 各軸
	for (int i = 0; i < 15; i++) {
		if (axis[i].x == 0.0f && axis[i].y == 0.0f && axis[i].z == 0.0f) {
			continue;
		}
		// 影の長さの合計
		float sumSpan;
		// 2つの影の両端の差分
		float longSpan;
		// 射影した最大値最小値
		float max1, min1, max2, min2;
		// 差分が形状を分離軸に射影した長さ
		float L1, L2;
		// すべての頂点を射影した値
		float Dot1[8];
		float Dot2[8];
		// 各頂点
		for (int j = 0; j < 8; j++) {
			Dot1[j] = Dot(axis[i], vertices1[j]);
		}
		for (int k = 0; k < 8; k++) {
			Dot2[k] = Dot(axis[i], vertices2[k]);
		}
		max1 = (std::max)({ Dot1[0], Dot1[1], Dot1[3], Dot1[4], Dot1[5], Dot1[6], Dot1[7] });
		min1 = (std::min)({ Dot1[0], Dot1[1], Dot1[3], Dot1[4], Dot1[5], Dot1[6], Dot1[7] });
		L1 = max1 - min1;
		max2 = (std::max)({ Dot2[0], Dot2[1], Dot2[3], Dot2[4], Dot2[5], Dot2[6], Dot2[7] });
		min2 = (std::min)({ Dot2[0], Dot2[1], Dot2[3], Dot2[4], Dot2[5], Dot2[6], Dot2[7] });
		L2 = max2 - min2;

		sumSpan = L1 + L2;
		longSpan = (std::max)(max1, max2) - (std::min)(min1, min2);

		if (sumSpan < longSpan) {
			return false;
		}
		float overlap = sumSpan - longSpan;
		if (overlap < minoverlap) {
			minoverlap = overlap;
			axisIndex = i;
		}
	}

	minAxis = axis[axisIndex];
	minOverlap = minoverlap;

	return true;
}
