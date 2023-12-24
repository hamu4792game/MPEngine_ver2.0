#pragma once
#include "Math/MathUtl.h"
#include "Utils/WorldTransform/WorldTransform.h"

class Camera {
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="farClip_">深度値</param>
	/// <param name="proType">false = 正射影 true = 透視投影</param>
	Camera(float farClip_ = 1000.0f, bool proType = false);
	~Camera() = default;

private:
	//	深度値
	float farZ;
	//	正射影か透視投影か false = 正射影 true = 透視投影
	bool type;
	//	行列の生成
	Matrix4x4 cameraMatrix{};
	Matrix4x4 viewMatrix{};
	Matrix4x4 projectionMatrix{};
	Matrix4x4 viewProjectionMatrix{};
	Matrix4x4 billboardMatrix;

private: // メンバ関数
	void CreateBillboard();

public:
	WorldTransform transform;
	//	viewProjectionMatrixの取得
	Matrix4x4 GetViewProMat();
	//	ビルボードの取得
	Matrix4x4 GetBillboardMat() const { return billboardMatrix; }
	Matrix4x4 GetViewMat() const { return viewMatrix; }
	Matrix4x4 GetProMat() const { return projectionMatrix; }

public:
	//ワールドトランスフォームの親ポインタのセット
	void SetParent(WorldTransform* parent) { transform.parent_ = parent; }

};

