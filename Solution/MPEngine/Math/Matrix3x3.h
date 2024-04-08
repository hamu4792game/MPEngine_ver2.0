#pragma once
#include "Vector2.h"
#include <array>

class Matrix3x3 {
public:
	Matrix3x3() = default;
	~Matrix3x3() = default;
public:
	//	mの多重配列 固定長配列
	std::array<std::array<float, 3>, 3> m = {};

	// アフィン変換行列の作成関数 (SRT)
	static Matrix3x3 MakeAffineMatrix(Vector2 scale, float rotate, Vector2 translate);

private:
};