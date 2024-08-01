#include "Vector2.h"

decltype(Vector2::zero) Vector2::zero = Vector2(0.0f, 0.0f);
decltype(Vector2::one) Vector2::one = Vector2(1.0f, 1.0f);

Vector2::Vector2() {
	this->x = 0.0f;
	this->y = 0.0f;
}

Vector2::Vector2(const float& X, const float& Y) {
	this->x = X;
	this->y = Y;
}

Vector2 Vector2::operator+(const Vector2& num) const {
	Vector2 result;
	result.x = this->x + num.x;
	result.y = this->y + num.y;
	return result;
}

Vector2 Vector2::operator-(const Vector2& num) const {
	Vector2 result;
	result.x = this->x - num.x;
	result.y = this->y - num.y;
	return result;
}

Vector2& Vector2::operator+=(const Vector2& num) {
	*this = *this + num;

	return *this;
}

Vector2& Vector2::operator-=(const Vector2& num) {
	*this = *this - num;

	return *this;
}

Vector2& Vector2::operator=(const Vector2& num)
{
	this->x = num.x;
	this->y = num.y;
	return *this;
}

Vector2 Vector2::operator*(const float& num) const
{
	Vector2 result;
	result.x = this->x * num;
	result.y = this->y * num;
	return result;
}

Vector2& Vector2::operator*=(const float& num)
{
	*this = *this * num;

	return *this;
}

bool Vector2::operator==(const Vector2& num) const {
	return this->x == num.x && this->y == num.y;
}

bool Vector2::operator!=(const Vector2& num) const {
	return !(*this == num);
}
