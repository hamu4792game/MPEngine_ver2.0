#include "Quaternion.h"
#include <cmath>
#include <functional>

Quaternion::Quaternion() {
	*this = IdentityQuaternion();
}

Quaternion::Quaternion(const float& x, const float& y, const float& z, const float& w) {
	this->x = x;
	this->y = y;
	this->z = z;
	this->w = w;
}

Quaternion Quaternion::operator*(const float& num) const {
	return Quaternion(this->x * num, this->y * num, this->z * num, this->w * num);
}

Quaternion Quaternion::operator+(const Quaternion& quaternion) const {
	return Quaternion(this->x + quaternion.x, this->y + quaternion.y, this->z + quaternion.z, this->w + quaternion.w);
}

Quaternion Quaternion::operator*(const Quaternion& quaternion) const {
	Quaternion result = this->Multiply(quaternion);
	return result;
}

Quaternion Quaternion::operator-() const {
	return Quaternion(-this->x, -this->y, -this->z, -this->w);
}

Quaternion& Quaternion::operator=(const Quaternion& quaternion) {
	this->x = quaternion.x;
	this->y = quaternion.y;
	this->z = quaternion.z;
	this->w = quaternion.w;
	return *this;
}

Quaternion& Quaternion::operator=(const Vector3& vec) {
	this->x = vec.x;
	this->y = vec.y;
	this->z = vec.z;
	return *this;
}

Quaternion Quaternion::Multiply(const Quaternion& quaternion) const {
	Quaternion result;
	Vector3 qVec(this->x, this->y, this->z);
	Vector3 rVec(quaternion.x, quaternion.y, quaternion.z);
	Vector3 resultVec = Cross(qVec, rVec) + (qVec * quaternion.w) + (rVec * this->w);
	result = resultVec;
	result.w = this->w * quaternion.w - (this->x * quaternion.x + this->y * quaternion.y + this->z * quaternion.z);
	return result;
}

Quaternion Quaternion::IdentityQuaternion() const {
	return Quaternion(0.0f,0.0f,0.0f,1.0f);
}

Quaternion Quaternion::Conjugate() const {
	Vector3 qVec(this->x, this->y, this->z);
	Quaternion result;
	result = -qVec;
	result.w = this->w;
	return result;
}

float Quaternion::Norm() const {
	float result;
	result = std::sqrtf(this->w * this->w + this->x * this->x + this->y * this->y + this->z * this->z);
	return result;
}

Quaternion Quaternion::Normalize() const {
	Quaternion result;
	float length = this->Norm();
	if (length == 0.0f) { result = IdentityQuaternion(); }
	length = 1.0f / length;
	result.x = this->x * length;
	result.y = this->y * length;
	result.z = this->z * length;
	result.w = this->w * length;

	return result;
}

Quaternion Quaternion::Inverse() const {
	Quaternion result;
	Quaternion conj = this->Conjugate();
	float length = this->Norm() * this->Norm();
	result.x = conj.x / length;
	result.y = conj.y / length;
	result.z = conj.z / length;
	result.w = conj.w / length;
	return result;
}

Quaternion Quaternion::MakeRotateAxisAngleQuaternion(const Vector3& vector, const float& angle) {
	float rad = angle / 2.0f;
	float cos = std::cosf(rad);	float sin = std::sinf(rad);
	Vector3 nVec = vector.Normalize() * sin;
	Quaternion result = Quaternion(nVec.x, nVec.y, nVec.z, cos);
	return result;
}

Vector3 Quaternion::RotateVector(const Vector3& vector, const Quaternion& quaternion) {
	Quaternion myVec = Quaternion(vector.x, vector.y, vector.z, 0.0f);
	myVec = (quaternion * myVec) * quaternion.Conjugate();
	Vector3 result = Vector3(myVec.x, myVec.y, myVec.z);
	return result;
}

Matrix4x4 Quaternion::MakeRotateMatrix(const Quaternion& quaternion) {
	Matrix4x4 result;
	result.m[0][0] = (quaternion.w * quaternion.w) + (quaternion.x * quaternion.x) - (quaternion.y * quaternion.y) - (quaternion.z * quaternion.z);
	result.m[0][1] = 2.0f * ((quaternion.x * quaternion.y) + (quaternion.w * quaternion.z));
	result.m[0][2] = 2.0f * ((quaternion.x * quaternion.z) - (quaternion.w * quaternion.y));
	result.m[1][0] = 2.0f * ((quaternion.x * quaternion.y) - (quaternion.w * quaternion.z));
	result.m[1][1] = (quaternion.w * quaternion.w) - (quaternion.x * quaternion.x) + (quaternion.y * quaternion.y) - (quaternion.z * quaternion.z);
	result.m[1][2] = 2.0f * ((quaternion.y * quaternion.z) + (quaternion.w * quaternion.x));
	result.m[2][0] = 2.0f * ((quaternion.x * quaternion.z) + (quaternion.w * quaternion.y));
	result.m[2][1] = 2.0f * ((quaternion.y * quaternion.z) - (quaternion.w * quaternion.x));
	result.m[2][2] = (quaternion.w * quaternion.w) - (quaternion.x * quaternion.x) - (quaternion.y * quaternion.y) + (quaternion.z * quaternion.z);
	result.m[3][3] = 1.0f;

	return result;
}

float Quaternion::Dot(const Quaternion& q0, const Quaternion& q1) {
	float dot = (q0.x * q1.x) + (q0.y * q1.y) + (q0.z* q1.z) + (q0.w * q1.w);
	return dot;
}

Quaternion Quaternion::Slerp(const Quaternion& q0, const Quaternion& q1, const float& t) {
	float dot = Quaternion::Dot(q0, q1);
	Quaternion localQ0 = q0;
	float ep = 1.0f - std::numeric_limits<float>::epsilon();
	if (dot < 0.0f) {
		localQ0 = (-q0); // もう片方の回転を利用する
		dot = -dot; // 内積も反転
	}
	if (dot >= 1.0f - std::numeric_limits<float>::epsilon()) {
		Vector3 resultVec = Lerp(Vector3(localQ0.x, localQ0.y, localQ0.z), Vector3(q1.x, q1.y, q1.z), t);
		float resultW = Lerp(localQ0.w, q1.w, t);
		return Quaternion(resultVec.x, resultVec.y, resultVec.z, resultW);
	}
	
	// なす角を求める
	float theta = std::acosf(dot);

	float scale0 = std::sinf((1.0f - t) * theta) * (1.0f / std::sinf(theta));
	float scale1 = std::sinf(t * theta) * (1.0f / std::sinf(theta));

	Quaternion result = (localQ0 * scale0) + (q1 * scale1);

	return result;
}
