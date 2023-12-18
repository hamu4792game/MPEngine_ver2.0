#include "WorldTransform.h"

WorldTransform::WorldTransform() {
	Reset();
}

WorldTransform::WorldTransform(const WorldTransform& transform) {
	*this = transform;
}

WorldTransform& WorldTransform::operator=(const WorldTransform& trans) {
	this->translation_ = trans.translation_;
	this->rotation_ = trans.rotation_;
	this->scale_ = trans.rotation_;
	this->worldMatrix_ = trans.worldMatrix_;
	this->parent_ = trans.parent_;
	return *this;
}

Matrix4x4 WorldTransform::UpdateMatrix() {
	// 行列の生成
	this->worldMatrix_ = MakeAffineMatrix(scale_, rotation_, translation_);
	// 親があれば親のワールド行列を掛ける
	if (parent_) {
		this->worldMatrix_ = this->worldMatrix_ * parent_->worldMatrix_;
	}

	return this->worldMatrix_;
}

void WorldTransform::Reset() {
	scale_ = Vector3::one;
	rotation_ = Vector3::zero;
	translation_ = Vector3::zero;
	worldMatrix_ = MakeIdentity4x4();
	parent_ = nullptr;
}
