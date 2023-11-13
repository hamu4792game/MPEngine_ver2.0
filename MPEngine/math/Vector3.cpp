#include "Vector3.h"
#include <cmath>
#include <cassert>
#include <numbers>
#include <random>
#include "Matrix4x4.h"

const Vector3 Vector3::zero(0.0f, 0.0f, 0.0f);
const Vector3 Vector3::one(1.0f, 1.0f, 1.0f);

Vector3::Vector3() {
	this->x = 0.0f;
	this->y = 0.0f;
	this->z = 0.0f;
}

Vector3::Vector3(const float& X, const float& Y, const float& Z) {
	this->x = X;
	this->y = Y;
	this->z = Z;
}

Vector3 Vector3::operator+(const Vector3& num) const {
	Vector3 result;
	result.x = this->x + num.x;
	result.y = this->y + num.y;
	result.z = this->z + num.z;
	return result;
}

Vector3 Vector3::operator-(const Vector3& num) const {
	Vector3 result;
	result.x = this->x - num.x;
	result.y = this->y - num.y;
	result.z = this->z - num.z;
	return result;
}

Vector3& Vector3::operator+=(const Vector3& num) {
	*this = *this + num;

	return *this;
}

Vector3& Vector3::operator-=(const Vector3& num) {
	*this = *this - num;

	return *this;
}

Vector3& Vector3::operator=(const Vector3& num) {
	this->x = num.x;
	this->y = num.y;
	this->z = num.z;
	return *this;
}

Vector3 Vector3::operator+(const float& num) const {
	Vector3 result;
	result.x = this->x + num;
	result.y = this->y + num;
	result.z = this->z + num;
	return result;
}

float Vector3::operator*(const Vector3& num) const {
	return (this->x * num.x) + (this->y * num.y) + (this->z * num.z);
}

Vector3 Vector3::operator*(const float& num) const
{
	Vector3 result;
	result.x = this->x * num;
	result.y = this->y * num;
	result.z = this->z * num;
	return result;
}

Vector3& Vector3::operator*=(const float& num) {
	*this = *this * num;

	return *this;
}

bool Vector3::operator!=(const Vector3& num) const {
	if (this->x == num.x) { return false; }
	if (this->y == num.y) { return false; }
	if (this->z == num.z) { return false; }
	return true;
}

float Length(const Vector3& vec) {
	return sqrtf((vec.x * vec.x) + (vec.y * vec.y) + (vec.z * vec.z));
}

float Dot(const Vector3& vec1, const Vector3& vec2) {
	return vec1.x * vec2.x + vec1.y * vec2.y + vec1.z * vec2.z;
}

float Distance(const Vector3& vec1, const Vector3& vec2) {
	Vector3 v = vec2 - vec1;
	return Dot(v, v);
}

Vector3 Cross(const Vector3& v1, const Vector3& v2) {
	Vector3 resultVec = { 0.0f,0.0f,0.0f };
	resultVec.x = (v1.y * v2.z) - (v1.z * v2.y);
	resultVec.y = (v1.z * v2.x) - (v1.x * v2.z);
	resultVec.z = (v1.x * v2.y) - (v1.y * v2.x);
	return resultVec;
}

Vector3 Normalize(Vector3 vec3) {
	Vector3 result;
	float w = Length(vec3);
	if (w == 0)	{
		return Vector3(0.0f, 0.0f, 0.0f);
	}
	w = 1.0f / w;
	result.x = vec3.x * w;
	result.y = vec3.y * w;
	result.z = vec3.z * w;
	return result;
}

Vector3 AbsoluteValue(const Vector3& num) {
	Vector3 result{};
	result.x = fabsf(num.x);
	result.y = fabsf(num.y);
	result.z = fabsf(num.z);

	return result;
}

Vector3 NormalizeRad(const Vector3& vec) {
	Vector3 result;
	result.x = std::atan2f(vec.y, vec.z);
	result.y = std::atan2f(vec.z, vec.x);
	result.z = std::atan2f(vec.y, vec.x);
	return result;
}

float Lerp(const float& a, const float& b, const float& t) {
	return a + t * (b - a);
}

Vector3 Lerp(const Vector3& a, const Vector3& b, const float& t) {
	Vector3 result;
	result.x = Lerp(a.x, b.x, t);
	result.y = Lerp(a.y, b.y, t);
	result.z = Lerp(a.z, b.z, t);
	return result;
}

float AngleToRadian(float angle) {
	return angle * (std::numbers::pi_v<float> / 180.0f);
}

float RadianToAngle(float radian) {
	return radian * (180.0f / std::numbers::pi_v<float>);
}

Vector3 TargetOffset(const Vector3& offset, const Vector3& rotation) {
	Matrix4x4 rotate = MakeRotateMatrix(rotation);
	Vector3 result = TransformNormal(offset, rotate);
	return result;
}

Vector3 FindVector(const Vector3& vec1, const Vector3& vec2) {
	return vec2 - vec1;
}

int RandNum(int min, int max) {
	//	ハードウェア乱数をシードにして初期化
	std::random_device seedGen;
	//	メルセンヌツイスター法で疑似乱数生成器の作成
	std::mt19937_64 randNum(seedGen());
	//	(min,max)の範囲で等間隔に乱数を生成 一様実数分布
	std::uniform_real_distribution<> dist(min, max);

	return static_cast<int>(dist(randNum));
}
float RandNum(float min, float max) {
	//	ハードウェア乱数をシードにして初期化
	std::random_device seedGen;
	//	メルセンヌツイスター法で疑似乱数生成器の作成
	std::mt19937_64 randNum(seedGen());
	//	(min,max)の範囲で等間隔に乱数を生成 一様実数分布
	std::uniform_real_distribution<> dist(min, max);

	return static_cast<float>(dist(randNum));
}