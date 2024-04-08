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

bool OBB::IsCollision(const OBB* obb2) const {
	Vector3 axis[15]; // 分離軸候補

	// 各軸
	axis[0] = this->orientations[0];
	axis[1] = this->orientations[1];
	axis[2] = this->orientations[2];
	axis[3] = obb2->orientations[0];
	axis[4] = obb2->orientations[1];
	axis[5] = obb2->orientations[2];

	// 各辺のクロス積
	axis[6] = Cross(this->orientations[0], obb2->orientations[0]);
	axis[7] = Cross(this->orientations[0], obb2->orientations[1]);
	axis[8] = Cross(this->orientations[0], obb2->orientations[2]);
	axis[9] = Cross(this->orientations[1], obb2->orientations[0]);
	axis[10] = Cross(this->orientations[1], obb2->orientations[1]);
	axis[11] = Cross(this->orientations[1], obb2->orientations[2]);
	axis[12] = Cross(this->orientations[2], obb2->orientations[0]);
	axis[13] = Cross(this->orientations[2], obb2->orientations[1]);
	axis[14] = Cross(this->orientations[2], obb2->orientations[2]);

	// 面法線
	Vector3 k;



	return false;
}
