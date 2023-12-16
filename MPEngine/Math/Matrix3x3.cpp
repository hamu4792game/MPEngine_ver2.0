#include "Matrix3x3.h"
#include <cmath>
#include <numbers>

Matrix3x3 Matrix3x3::MakeAffineMatrix(Vector2 scale, float rotate, Vector2 translate) {
	rotate *= std::numbers::pi_v<float> / 180.0f;
	Matrix3x3 result;

	//	代入
	result.m[0][0] = scale.x * cosf(rotate);
	result.m[0][1] = scale.x * sinf(rotate);
	result.m[1][0] = scale.y * -sinf(rotate);
	result.m[1][1] = scale.y * cosf(rotate);
	result.m[2][0] = translate.x;
	result.m[2][1] = translate.y;
	result.m[2][2] = 1.0f;

	return result;
}
