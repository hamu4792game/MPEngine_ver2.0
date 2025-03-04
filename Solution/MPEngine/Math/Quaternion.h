#pragma once

class Vector3;
class Matrix4x4;

class Quaternion {
public:
	Quaternion();
	Quaternion(const float& x, const float& y, const float& z, const float& w);
	Quaternion(const Matrix4x4& matrix);
	~Quaternion() = default;

public:
	float x = 0.0f, y = 0.0f, z = 0.0f, w = 1.0f;

public:
	Quaternion operator*(const float& num) const;
	Quaternion operator+(const Quaternion& quaternion) const;
	Quaternion operator*(const Quaternion& quaternion) const;
	Vector3 operator*(const Vector3& quaternion) const;
	Quaternion operator-() const;
	Quaternion& operator=(const Quaternion& quaternion);
	Quaternion& operator=(const Vector3& vec);
	bool operator==(const Quaternion& quaternion) const;
	bool operator!=(const Quaternion& quaternion) const;

public:
	// 積
	Quaternion Multiply(const Quaternion& quaternion) const;
	// 単位Quaternionを返す
	static Quaternion IdentityQuaternion();
	// 共役Quaternionを返す
	Quaternion Conjugate() const;
	// Quaternionのnormを返す
	float Norm() const;
	// 正規化したQuaternionを返す
	Quaternion Normalize() const;
	// 逆Quaternionを返す
	Quaternion Inverse() const;
	// 任意軸回転を表すQuaternionの生成
	static Quaternion MakeRotateAxisAngleQuaternion(const Vector3& vector, const float& angle);
	// ベクトルをQuaternionで回転させた結果のベクトルを求める
	static Vector3 RotateVector(const Vector3& vector, const Quaternion& quaternion);
	// Quaternionから回転行列を求める
	static Matrix4x4 MakeQuaternionRotateMatrix(const Quaternion& quaternion);
	// 内積
	static float Dot(const Quaternion& q0, const Quaternion& q1);
	// 球面補間
	static Quaternion Slerp(const Quaternion& q0, const Quaternion& q1, const float& t);
	/// <summary>
	/// 2つのベクトルから回転を求める
	/// </summary>
	/// <param name="from">基準となるベクトル Upとか</param>
	/// <param name="to">もう一つのベクトル　移動ベクトルとか</param>
	/// <returns>Quaternion</returns>
	static Quaternion MakeFromTwoVector(const Vector3& from, const Vector3& to);

	// Quaternionからオイラー角へ変換
	static Vector3 QuaternionToEuler(const Quaternion& quaternion);

	// 回転行列からクオータニオンへ変換
	static Quaternion FromRotationMatrix4x4(const Matrix4x4& matrix);

	// オイラー角からQuaternionへ変換
	static Quaternion EulerToQuaternion(const Vector3& euler);


};