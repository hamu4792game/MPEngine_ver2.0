#include "WorldTransform.h"
#include "DirectXMath.h"

WorldTransform::WorldTransform() {
	Reset();
}

WorldTransform::WorldTransform(const WorldTransform& transform) {
	*this = transform;
}

WorldTransform::WorldTransform(const Vector3& scale, const Vector3& rotate, const Vector3& translate) {
	scale_ = scale;
	rotation_ = rotate;
	translation_ = translate;
	UpdateMatrix();
}

WorldTransform::WorldTransform(const Vector3& scale, const Quaternion& rotate, const Vector3& translate) {
	scale_ = scale;
	rotationQuat_ = rotate;
	translation_ = translate;
	isQuaternion_ = true;
	UpdateMatrix();
}

WorldTransform& WorldTransform::operator=(const WorldTransform& trans) {
	this->translation_ = trans.translation_;
	this->rotation_ = trans.rotation_;
	this->isQuaternion_ = trans.isQuaternion_;
	this->rotationQuat_ = trans.rotationQuat_;
	this->scale_ = trans.scale_;
	this->worldMatrix_ = trans.worldMatrix_;
	this->parent_ = trans.parent_;
	return *this;
}

Matrix4x4 WorldTransform::UpdateMatrix() {
	// 行列の生成
	if (isQuaternion_) {
		// quaternionを使う場合
		this->worldMatrix_ = MakeScaleMatrix(scale_) * Quaternion::MakeQuaternionRotateMatrix(rotationQuat_) * MakeTranslateMatrix(translation_);
	}
	else {
		this->worldMatrix_ = MakeAffineMatrix(scale_, rotation_, translation_);

	}

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

Quaternion WorldTransform::GetQuaternion() const {
	// XMMATRIX に変換
	DirectX::XMMATRIX wm = DirectX::XMLoadFloat4x4(reinterpret_cast<const DirectX::XMFLOAT4X4*>(&this->worldMatrix_));

	// スケールを抽出 (各軸の長さ)
	DirectX::XMVECTOR scaleX = DirectX::XMVector3Length(DirectX::XMVectorSet(this->worldMatrix_.m[0][0], this->worldMatrix_.m[0][1], this->worldMatrix_.m[0][2], 0.0f));
	DirectX::XMVECTOR scaleY = DirectX::XMVector3Length(DirectX::XMVectorSet(this->worldMatrix_.m[1][0], this->worldMatrix_.m[1][1], this->worldMatrix_.m[1][2], 0.0f));
	DirectX::XMVECTOR scaleZ = DirectX::XMVector3Length(DirectX::XMVectorSet(this->worldMatrix_.m[2][0], this->worldMatrix_.m[2][1], this->worldMatrix_.m[2][2], 0.0f));

	// 行列の各軸ベクトルを正規化
	DirectX::XMVECTOR row0 = DirectX::XMVectorDivide(DirectX::XMLoadFloat3(reinterpret_cast<const DirectX::XMFLOAT3*>(&this->worldMatrix_.m[0])), scaleX);
	DirectX::XMVECTOR row1 = DirectX::XMVectorDivide(DirectX::XMLoadFloat3(reinterpret_cast<const DirectX::XMFLOAT3*>(&this->worldMatrix_.m[1])), scaleY);
	DirectX::XMVECTOR row2 = DirectX::XMVectorDivide(DirectX::XMLoadFloat3(reinterpret_cast<const DirectX::XMFLOAT3*>(&this->worldMatrix_.m[2])), scaleZ);

	// 回転行列を構築
	DirectX::XMMATRIX rotationMatrix = DirectX::XMMATRIX(row0, row1, row2, DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f));
	// Quaternionに変換
	DirectX::XMVECTOR xmQuaternion = DirectX::XMQuaternionRotationMatrix(rotationMatrix);
	// 独自クラスに代入
	Quaternion result;
	result.x = DirectX::XMVectorGetX(xmQuaternion);
	result.y = DirectX::XMVectorGetY(xmQuaternion);
	result.z = DirectX::XMVectorGetZ(xmQuaternion);
	result.w = DirectX::XMVectorGetW(xmQuaternion);

	return result;
}
