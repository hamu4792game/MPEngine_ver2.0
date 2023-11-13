#pragma once
#include "Vector2.h"
#include <cstdint>

/// <summary>
/// 4次元ベクトル
/// </summary>
///
class Vector4 final {
public:
	Vector4();
	Vector4(const float& X, const float& Y, const float& Z,const float& W);

public:
	float x;
	float y;
	float z;
	float w;

};

Vector4 ChangeColor(uint32_t color);

uint32_t ChangeColor(Vector4 color);