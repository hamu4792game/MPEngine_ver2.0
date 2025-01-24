#include "Wire.h"
#include "ResourceManager/ResourceManager.h"
#include <algorithm>

// のちに外部に書き出す用 一旦ここでグローバル変数化させている
// フェードアウト速度
const float kFadeOutSpeed = 0.01f;
// 一つのモデルの大きさ のちに調整項目として書き出す
const float kOneSize = 0.2f;

Wire::Wire() {
	wireTipModels_ = std::make_unique<Particle>();
	auto rsManager = ResourceManager::GetInstance();
	wireTipNum_ = 10u;
	wireTipModels_->SetModel(rsManager->FindObject3d("Box"), wireTipNum_);
	wireTipModels_->SetTexture(rsManager->FindTexture("white2x2"));
}

void Wire::Initialize(const Vector3& playerPos, const Vector3& anchor) {
	// 2点の座標から距離を求める
	float distance = Length(playerPos - anchor);
	// 距離からオブジェクトの数を求める
	float objectCount = ceilf(distance / kOneSize);

	// オブジェクトの数が足りていなければresizeする
	if (wireTipNum_ < static_cast<uint32_t>(objectCount)) {
		wireTipNum_ = static_cast<uint32_t>(objectCount);
		SetWireTipModel();
	}

	// 色
	for (uint32_t index = 0u; index < wireTipModels_->color_.size(); index++) {
		if (index >= objectCount) {
			// 透明にして見えなくする
			wireTipModels_->color_.at(index) = Vector4(1.0f, 1.0f, 1.0f, 0.0f);
		}
		else {
			// 透明度問題
			wireTipModels_->color_.at(index) = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
		}
	}

	// オブジェクトに座標を割り当てる
	Vector3 direction = FindVector(playerPos, anchor).Normalize();
	uint32_t index = 0u;
	Quaternion quat = Quaternion::MakeFromTwoVector(Vector3::front, direction);
	for (auto& trans : wireTipModels_->transform_) {
		// player座標から敷いていく
		trans.translation_ = playerPos + (direction * (kOneSize * index++));
		trans.rotationQuat_ = quat;
		trans.scale_ = Vector3(kOneSize, kOneSize, kOneSize);
		trans.isQuaternion_ = true;
		trans.UpdateMatrix();
	}
	isFadeOut_ = false;
}

void Wire::FadeOutUpdate() {
	if (!isFadeOut_) { return; }
	for (uint32_t index = 0u; index < wireTipModels_->color_.size(); index++) {
		if (wireTipModels_->color_.at(index).w != 0.0f) {
			// 透明ではなかったら透明にして見えなくする
			wireTipModels_->color_.at(index).w -= kFadeOutSpeed;
		}
		wireTipModels_->color_.at(index).w = std::max(wireTipModels_->color_.at(index).w, 0.0f);
	}
}

void Wire::SetWireTipModel() {
	auto rsManager = ResourceManager::GetInstance();
	wireTipModels_->SetModel(rsManager->FindObject3d("Box"), wireTipNum_);
}
