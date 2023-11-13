#include "OBB.h"

void OBB::Update(WorldTransform& transform) {
	Matrix4x4 mat = transform.UpdateMatrix();
	this->orientations[0].x = mat.m[0][0];
	this->orientations[0].y = mat.m[0][1];
	this->orientations[0].z = mat.m[0][2];

	this->orientations[1].x = mat.m[1][0];
	this->orientations[1].y = mat.m[1][1];
	this->orientations[1].z = mat.m[1][2];

	this->orientations[2].x = mat.m[2][0];
	this->orientations[2].y = mat.m[2][1];
	this->orientations[2].z = mat.m[2][2];

	this->center.x = mat.m[3][0];
	this->center.y = mat.m[3][1];
	this->center.z = mat.m[3][2];
}

void OBB::DrawOBB(const Matrix4x4& viewProjectionMatrix, uint32_t color) {
	Matrix4x4 worldMat;
	worldMat.m[0][0] = orientations[0].x; worldMat.m[0][1] = orientations[1].x; worldMat.m[0][2] = orientations[2].x;
	worldMat.m[1][0] = orientations[0].y; worldMat.m[1][1] = orientations[1].y; worldMat.m[1][2] = orientations[2].y;
	worldMat.m[2][0] = orientations[0].z; worldMat.m[2][1] = orientations[1].z; worldMat.m[2][2] = orientations[2].z;
	worldMat.m[3][0] = center.x; worldMat.m[3][1] = center.y; worldMat.m[3][2] = center.z; worldMat.m[3][3] = 1.0f;

	obbLocal_.min = Vector3(0.0f, 0.0f, 0.0f) - size;
	obbLocal_.max = size;
	obbLocal_.DrawAABB(worldMat * viewProjectionMatrix, color);
}
