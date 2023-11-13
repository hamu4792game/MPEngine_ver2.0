#include "Vector4.h"


Vector4::Vector4() {
	x = 0.0f, y = 0.0f, z = 0.0f, w = 0.0f;
}

Vector4::Vector4(const float& X, const float& Y, const float& Z, const float& W) {
	x = X, y = Y, z = Z, w = W;
}

Vector4 ChangeColor(uint32_t color)
{
	static const float nNum = 1.0f / 255.0f;

	float red = static_cast<float>((color & 0xff000000) >> 24) * nNum;
	float blue = static_cast<float>((color & 0x00ff0000) >> 16) * nNum;
	float green = static_cast<float>((color & 0x0000ff00) >> 8) * nNum;
	float alpha = static_cast<float>((color & 0x000000ff)) * nNum;

	return Vector4(red, blue, green, alpha);
}

uint32_t ChangeColor(Vector4 color)
{
	uint32_t result = 0x00;
	result += static_cast<uint32_t>(color.x) << 24;
	result += static_cast<uint32_t>(color.y) << 16;
	result += static_cast<uint32_t>(color.z) << 8;
	result += static_cast<uint32_t>(color.w);

	return result;
}
