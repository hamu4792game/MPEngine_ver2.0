#include "AABB.h"
#include <cmath>
#include <algorithm>
#include "Sphere.h"
#include "Base/Manager/ResourceManager/ResourceManager.h"
#undef min
#undef max

AABB::AABB() {
	for (auto& i : line_) {
		i = std::make_unique<Line>();
	}
}

void AABB::Update(const WorldTransform& transform) {
	//	座標 - scale * size
	this->min = Vector3(transform.GetPosition() - Vector3(transform.scale_.x * size.x, transform.scale_.y * size.y, transform.scale_.z * size.z));
	this->max = Vector3(transform.GetPosition() + Vector3(transform.scale_.x * size.x, transform.scale_.y * size.y, transform.scale_.z * size.z));
	center_ = transform.GetPosition();
	boxModel_.scale_ = Vector3(transform.scale_.x * size.x, transform.scale_.y * size.y, transform.scale_.z * size.z);
	boxModel_.translation_ = center_;
	boxModel_.UpdateMatrix();
#ifdef _DEBUG
	//boxModel_->isActive_ = true;
#endif // _DEBUG

	direction_.upper.y = boxModel_.translation_.y + (boxModel_.scale_.y);
	direction_.lower.y = boxModel_.translation_.y - (boxModel_.scale_.y);

	// フレーム描画
	Vector3 ver[8]{};
	// 左下手前
	ver[0] = { min.x,min.y,min.z };
	// 右下手前
	ver[1] = { max.x,min.y,min.z };
	// 左上手前
	ver[2] = { min.x,max.y,min.z };
	// 右上手前
	ver[3] = { max.x,max.y,min.z };
	// 左下奥
	ver[4] = { min.x,min.y,max.z };
	// 右下奥
	ver[5] = { max.x,min.y,max.z };
	// 左上奥
	ver[6] = { min.x,max.y,max.z };
	// 右上奥
	ver[7] = { max.x,max.y,max.z };

	// 描画
	// 下手前
	line_[0]->SetLine(ver[0], ver[1]);
	// 上手前
	line_[1]->SetLine(ver[2], ver[3]);
	// 下奥
	line_[2]->SetLine(ver[4], ver[5]);
	// 上奥
	line_[3]->SetLine(ver[6], ver[7]);
	// 左手前
	line_[4]->SetLine(ver[0], ver[2]);
	// 左奥
	line_[5]->SetLine(ver[4], ver[6]);
	// 右手前
	line_[6]->SetLine(ver[1], ver[3]);
	// 右奥
	line_[7]->SetLine(ver[5], ver[7]);
	// 左下
	line_[8]->SetLine(ver[0], ver[4]);
	// 左上
	line_[9]->SetLine(ver[2], ver[6]);
	// 右下
	line_[10]->SetLine(ver[1], ver[5]);
	// 右上
	line_[11]->SetLine(ver[3], ver[7]);


}

bool AABB::IsCollision(Sphere* sphere) const {
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

bool AABB::IsCollision(const AABB* aabb2) const {
	Vector3 aMin; Vector3 aMax;
	Vector3 bMin; Vector3 bMax;
	aMin.x = (std::min)(this->min.x, this->max.x); aMax.x = (std::max)(this->min.x, this->max.x);
	aMin.y = (std::min)(this->min.y, this->max.y); aMax.y = (std::max)(this->min.y, this->max.y);
	aMin.z = (std::min)(this->min.z, this->max.z); aMax.z = (std::max)(this->min.z, this->max.z);
	bMin.x = (std::min)(aabb2->min.x, aabb2->max.x); bMax.x = (std::max)(aabb2->min.x, aabb2->max.x);
	bMin.y = (std::min)(aabb2->min.y, aabb2->max.y); bMax.y = (std::max)(aabb2->min.y, aabb2->max.y);
	bMin.z = (std::min)(aabb2->min.z, aabb2->max.z); bMax.z = (std::max)(aabb2->min.z, aabb2->max.z);

	if ((aMin.x <= bMax.x && aMax.x >= bMin.x) &&
		(aMin.y <= bMax.y && aMax.y >= bMin.y) &&
		(aMin.z <= bMax.z && aMax.z >= bMin.z))
	{
		return true;
	}
	return false;
}

bool AABB::IsCollision(const Vector3& vector) const {
	if (this->min.x <= vector.x && vector.x <= this->max.x) {
		if (this->min.y <= vector.y && vector.y <= this->max.y) {
			if (this->min.z <= vector.z && vector.z <= this->max.z) {
				return true;
			}
		}
	}
	return false;
}

float AABB::CalculatePushBackVector(const AABB* aabb1, const AABB* aabb2) {
	Vector3 overlap;
	// めり込み分を計算 負の場合は重なっていない
	overlap.x = std::max(0.0f, std::min(aabb1->max.x, aabb2->max.x) - std::max(aabb1->min.x, aabb2->min.x));
	overlap.y = std::max(0.0f, std::min(aabb1->max.y, aabb2->max.y) - std::max(aabb1->min.y, aabb2->min.y));
	overlap.z = std::max(0.0f, std::min(aabb1->max.z, aabb2->max.z) - std::max(aabb1->min.z, aabb2->min.z));
	
	float result = std::min(overlap.x, std::min(overlap.y, overlap.z));
	return result;
}
