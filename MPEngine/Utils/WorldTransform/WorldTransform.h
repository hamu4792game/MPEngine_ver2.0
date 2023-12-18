#pragma once
#include "MPEngine/Math/MathUtl.h"

class WorldTransform {
public:
	WorldTransform();
	~WorldTransform() = default;
	WorldTransform(const WorldTransform& transform);

	WorldTransform& operator=(const WorldTransform& trans);

public:
	// ローカルスケール
	Vector3 scale_ = { 1.0f, 1.0f, 1.0f };
	// X,Y,Z軸回りのローカル回転角
	Vector3 rotation_ = { 0.0f, 0.0f, 0.0f };
	//Matrix4x4 rotateMatrix;
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

};