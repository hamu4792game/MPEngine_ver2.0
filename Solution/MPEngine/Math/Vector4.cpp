#include "Vector4.h"

const Vector4 Vector4::zero(0.0f, 0.0f, 0.0f,0.0f);
const Vector4 Vector4::one(1.0f, 1.0f, 1.0f,1.0f);

Vector4::Vector4() {
	x = 0.0f, y = 0.0f, z = 0.0f, w = 0.0f;
}

Vector4::Vector4(const float& X, const float& Y, const float& Z, const float& W) {
	x = X, y = Y, z = Z, w = W;
}

Vector4::Vector4(const float* num) {
	x = num[0], y = num[1], z = num[2], w = num[3];
}

Vector4 Vector4::operator+(const Vector4& num) const {
	Vector4 result;
	result.x = this->x + num.x;
	result.y = this->y + num.y;
	result.z = this->z + num.z;
	result.w = this->w + num.w;
	return result;
}

Vector4 Vector4::operator-(const Vector4& num) const {
	Vector4 result;
	result.x = this->x - num.x;
	result.y = this->y - num.y;
	result.z = this->z - num.z;
	result.w = this->w - num.w;
	return result;
}

Vector4 Vector4::operator-() const {
	return Vector4(-this->x, -this->y, -this->z, -this->w);
}

Vector4& Vector4::operator+=(const Vector4& num) {
	*this = *this + num;

	return *this;
}

Vector4& Vector4::operator-=(const Vector4& num) {
	*this = *this - num;

	return *this;
}

Vector4& Vector4::operator=(const Vector4& num) {
	this->x = num.x;
	this->y = num.y;
	this->z = num.z;
	this->w = num.w;
	return *this;
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
