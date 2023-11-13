#pragma once
#include "Vector3.h"
#include <array>

class Matrix4x4
{
public:
	Matrix4x4();
	~Matrix4x4() = default;

public:
	//	mの多重配列 固定長配列
	std::array<std::array<float, 4>, 4> m;

private:
	
public:
	//	加算
	Matrix4x4 operator+(const Matrix4x4& mat) const;
	Matrix4x4& operator+=(const Matrix4x4& mat);
	//	減算
	Matrix4x4 operator-(const Matrix4x4& mat) const;
	Matrix4x4& operator-=(const Matrix4x4& mat);
	//	行列の積
	Matrix4x4 operator*(const Matrix4x4& mat) const;
	Matrix4x4& operator*=(const Matrix4x4& mat);
	//	行列の代入
	Matrix4x4& operator=(const Matrix4x4& mat);
	
};


//	逆行列
Matrix4x4 Inverse(const Matrix4x4& m);
//	転置行列
Matrix4x4 Transpose(const Matrix4x4& m);
//	単位行列の作成
Matrix4x4 MakeIdentity4x4();
//	平行移動行列
Matrix4x4 MakeTranslateMatrix(const Vector3& translate);
//	拡大縮小行列
Matrix4x4 MakeScaleMatrix(const Vector3& scale);
//	座標変換
Vector3 Transform(const Vector3& vector, const Matrix4x4& matrix);
//	平行移動を加味しない座標変換
Vector3 TransformNormal(const Vector3& vector, const Matrix4x4& matrix);
//	X軸回転行列
Matrix4x4 MakeRotateXMatrix(float radian);
//	Y軸回転行列
Matrix4x4 MakeRotateYMatrix(float radian);
//	Z軸回転行列
Matrix4x4 MakeRotateZMatrix(float radian);
//	XYZの回転行列をまとめて計算する
Matrix4x4 MakeRotateMatrix(const Vector3& rotate);
//	3次元アフィン変換行列
Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Vector3& rotate, const Vector3& translate);

//	透視投影行列
Matrix4x4 MakePerspectiveFovMatrix(float fovY, float aspectRatio, float nearClip, float farClip);
//	正射影行列
Matrix4x4 MakeOrthographicMatrix(float left, float top, float right, float bottom, float nearClip, float farClip);
//	ビューポート変換行列
Matrix4x4 MakeViewportMatrix(float left, float top, float width, float height, float minDepth, float maxDepth);
//
Matrix4x4 DirectionToDirection(const Vector3& from, const Vector3& to);