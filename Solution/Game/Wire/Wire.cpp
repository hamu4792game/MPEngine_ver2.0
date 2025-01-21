#include "Wire.h"
#include "ResourceManager/ResourceManager.h"
#include <algorithm>

Wire::Wire() {
	wireTipModels_ = std::make_unique<Particle>();
	auto rsManager = ResourceManager::GetInstance();
	wireTipNum_ = 10u;
	wireTipModels_->SetModel(rsManager->FindObject3d("Box"), wireTipNum_);
	wireTipModels_->SetTexture(rsManager->FindTexture("UVChecker"));
}

void Wire::Initialize(const Vector3& playerPos, const Vector3& anchor) {
	// 2点の座標から距離を求める
	float distance = Length(playerPos - anchor);

	// 距離からオブジェクトの数を求める
	const float oneSize = 3.0f;
	float objectCount = ceilf(distance / oneSize);

	// オブジェクトの数が足りていなければresizeする
	if (wireTipNum_ < static_cast<uint32_t>(objectCount)) {
		wireTipNum_ = static_cast<uint32_t>(objectCount);
		SetWireTipModel();
	}

	// 色
	for (uint32_t index = 0u; index < wireTipModels_->color_.size(); index++) {
		if (index >= objectCount) {
			wireTipModels_->color_.at(index) = Vector4(1.0f, 1.0f, 1.0f, 0.0f);
		}
		else {
			wireTipModels_->color_.at(index) = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
		}
	}

	// オブジェクトに座標を割り当てる
	Vector3 direction = FindVector(playerPos, anchor).Normalize();
	uint32_t index = 0u;
	Quaternion quat = Quaternion::MakeFromTwoVector(Vector3::front, direction);
	for (auto& trans : wireTipModels_->transform_) {
		// player座標から敷いていく
		trans.translation_ = playerPos + (direction * (oneSize * index++));
		trans.rotationQuat_ = quat;
		trans.scale_ = Vector3::one;
		trans.isQuaternion_ = true;
		trans.UpdateMatrix();
	}

}

void Wire::SetWireTipModel() {
	auto rsManager = ResourceManager::GetInstance();
	wireTipModels_->SetModel(rsManager->FindObject3d("Box"), wireTipNum_);
}
