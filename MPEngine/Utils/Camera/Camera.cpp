#include "Camera.h"
#include <numbers>
#include "MPEngine/MPEngine.h"

Camera::Camera(float farClip_, bool proType) {
	farZ = farClip_;
	type = proType;
	if (type) {
		projectionMatrix = MakePerspectiveFovMatrix(0.45f, float(MPEngine::GetInstance()->windowWidth_) / float(MPEngine::GetInstance()->windowHeight_), 0.1f, farZ);
	}
	else {
		projectionMatrix = MakeOrthographicMatrix(-float(MPEngine::GetInstance()->windowWidth_ / 2), float(MPEngine::GetInstance()->windowHeight_ / 2),
			float(MPEngine::GetInstance()->windowWidth_ / 2), -float(MPEngine::GetInstance()->windowHeight_ / 2), 0.01f, farZ);
	}
}

void Camera::CreateBillboard() {
	// 180度回転させる
	Matrix4x4 backToFrontMatrix = MakeRotateYMatrix(std::numbers::pi_v<float>);
	// 行列の生成
	billboardMatrix = backToFrontMatrix * cameraMatrix;
	billboardMatrix.m[3][0] = 0.0f;
	billboardMatrix.m[3][1] = 0.0f;
	billboardMatrix.m[3][2] = 0.0f;
}

Matrix4x4 Camera::GetViewProMat() {
	// 行列の計算
	cameraMatrix = transform.UpdateMatrix();
	viewMatrix = Inverse(cameraMatrix);
	viewProjectionMatrix = viewMatrix * projectionMatrix;
	// ビルボードも作る
	if (type) {
		CreateBillboard();
	}

	return viewProjectionMatrix;
}
