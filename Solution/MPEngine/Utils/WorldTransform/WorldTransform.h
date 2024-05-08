#pragma once
#include "MPEngine/Math/MathUtl.h"

class WorldTransform {
public:
	WorldTransform();
	~WorldTransform() = default;
	WorldTransform(const WorldTransform& transform);
	WorldTransform(const Vector3& scale, const Vector3& rotate, const Vector3& translate);
	WorldTransform(const Vector3& scale, const Quaternion& rotate, const Vector3& translate);

	WorldTransform& operator=(const WorldTransform& trans);

public:
	// ローカルスケール
	Vector3 scale_ = { 1.0f, 1.0f, 1.0f };
	// X,Y,Z軸回りのローカル回転角
	Vector3 rotation_ = { 0.0f, 0.0f, 0.0f };
	Quaternion rotationQuat_;
	bool isQuaternion_ = false; // Quaternionを使うか
	// ローカル座標
	Vector3 translation_ = { 0.0f, 0.0f, 0.0f };
	// ローカル → ワールド変換行列
	Matrix4x4 worldMatrix_;
	// 親となるワールド変換へのポインタ
	const WorldTransform* parent_ = nullptr;

	// 更新
	Matrix4x4 UpdateMatrix();
	// リセット
	void Reset();

	Vector3 GetPosition() const { return Vector3(worldMatrix_.m[3][0], worldMatrix_.m[3][1], worldMatrix_.m[3][2]); }

};