#include "AABB.h"
#include <cmath>
#include <algorithm>

void AABB::Update(WorldTransform& transform)
{
	//	座標 - scale * size
	this->min = Vector3(transform.GetTranslate() - Vector3(transform.scale_.x * size.x, transform.scale_.y * size.y, transform.scale_.z * size.z));
	this->max = Vector3(transform.GetTranslate() + Vector3(transform.scale_.x * size.x, transform.scale_.y * size.y, transform.scale_.z * size.z));
}

bool AABB::IsCollision(Sphere* sphere) {
	Vector3 min;
	min.x = (std::min)(this->min.x, this->max.x);
	min.y = (std::min)(this->min.y, this->max.y);
	min.z = (std::min)(this->min.z, this->max.z);
	Vector3 max;
	max.x = (std::max)(this->min.x, this->max.x);
	max.y = (std::max)(this->min.y, this->max.y);
	max.z = (std::max)(this->min.z, this->max.z);

	//	最近接点を求める
	Vector3 closestPoint{ std::clamp(sphere->center_.x,min.x,max.x),
		std::clamp(sphere->center_.y,min.y,max.y),
		std::clamp(sphere->center_.z,min.z,max.z) };
	//	最近接点と球の中心との距離を求める
	float distance = Length(closestPoint - sphere->center_);
	//	距離が半径よりも小さければ衝突
	if (distance <= std::fabs(sphere->radius_))
	{
		return true;
	}
	return false;
}

bool AABB::IsCollision(AABB* aabb2) {
	Vector3 aMin; Vector3 aMax;
	Vector3 bMin; Vector3 bMax;
	aMin.x = (std::min)(this->min.x, this->max.x);	aMax.x = (std::max)(this->min.x, this->max.x);
	aMin.y = (std::min)(this->min.y, this->max.y);	aMax.y = (std::max)(this->min.y, this->max.y);
	aMin.z = (std::min)(this->min.z, this->max.z);	aMax.z = (std::max)(this->min.z, this->max.z);
	bMin.x = (std::min)(aabb2->min.x, aabb2->max.x);	bMax.x = (std::max)(aabb2->min.x, aabb2->max.x);
	bMin.y = (std::min)(aabb2->min.y, aabb2->max.y);	bMax.y = (std::max)(aabb2->min.y, aabb2->max.y);
	bMin.z = (std::min)(aabb2->min.z, aabb2->max.z);	bMax.z = (std::max)(aabb2->min.z, aabb2->max.z);

	if ((aMin.x <= bMax.x && aMax.x >= bMin.x) &&
		(aMin.y <= bMax.y && aMax.y >= bMin.y) &&
		(aMin.z <= bMax.z && aMax.z >= bMin.z))
	{
		return true;
	}
	return false;
}

void AABB::DrawAABB(const Matrix4x4& viewProjectionMatrix, uint32_t color)
{
	Vector3 ver[8]{};
	//	左下手前
	ver[0] = { min.x,min.y,min.z };
	//	右下手前
	ver[1] = { max.x,min.y,min.z };
	//	左上手前
	ver[2] = { min.x,max.y,min.z };
	//	右上手前
	ver[3] = { max.x,max.y,min.z };
	//	左下奥
	ver[4] = { min.x,min.y,max.z };
	//	右下奥
	ver[5] = { max.x,min.y,max.z };
	//	左上奥
	ver[6] = { min.x,max.y,max.z };
	//	右上奥
	ver[7] = { max.x,max.y,max.z };

	//	描画
	//	下手前
	line[0].DrawLine(ver[0], ver[1], viewProjectionMatrix, color);
	//	上手前
	line[1].DrawLine(ver[2], ver[3], viewProjectionMatrix, color);
	//	下奥
	line[2].DrawLine(ver[4], ver[5], viewProjectionMatrix, color);
	//	上奥
	line[3].DrawLine(ver[6], ver[7], viewProjectionMatrix, color);
	//	左手前
	line[4].DrawLine(ver[0], ver[2], viewProjectionMatrix, color);
	//	左奥
	line[5].DrawLine(ver[4], ver[6], viewProjectionMatrix, color);
	//	右手前
	line[6].DrawLine(ver[1], ver[3], viewProjectionMatrix, color);
	//	右奥
	line[7].DrawLine(ver[5], ver[7], viewProjectionMatrix, color);
	//	左下
	line[8].DrawLine(ver[0], ver[4], viewProjectionMatrix, color);
	//	左上
	line[9].DrawLine(ver[2], ver[6], viewProjectionMatrix, color);
	//	右下
	line[10].DrawLine(ver[1], ver[5], viewProjectionMatrix, color);
	//	右上
	line[11].DrawLine(ver[3], ver[7], viewProjectionMatrix, color);
}
