#pragma once

/// <summary>
/// 3次元ベクトル
/// </summary>
///
class Vector3 final {
public:
	Vector3();
	Vector3(const float& X, const float& Y, const float& Z);

public:
	float x;
	float y;
	float z;

public:
	Vector3 operator+(const Vector3& num) const;
	Vector3 operator-(const Vector3& num) const;
	Vector3& operator+=(const Vector3& num);
	Vector3& operator-=(const Vector3& num);
	Vector3& operator=(const Vector3& num);

	Vector3 operator+(const float& num) const;

	//	ドット積(内積)
	float operator*(const Vector3& num) const;

	Vector3 operator*(const float& num) const;
	Vector3& operator*=(const float& num);

	bool operator!=(const Vector3& num) const;

public: // 定数
	static const Vector3 zero;
	static const Vector3 one;
	
};

//	長さ(外積)を求める
float Length(const Vector3& vec);
//	内積を求める
float Dot(const Vector3& vec1, const Vector3& vec2);
//	2点間の距離を求める
float Distance(const Vector3& vec1, const Vector3& vec2);
//	クロス積
Vector3 Cross(const Vector3& v1, const Vector3& v2);
//	正規化
Vector3 Normalize(Vector3 vec3);
//	絶対値を求める
Vector3 AbsoluteValue(const Vector3& num);

Vector3 NormalizeRad(const Vector3& vec);

//	線形補間
float Lerp(const float& a, const float& b, const float& t);
Vector3 Lerp(const Vector3& a, const Vector3& b, const float& t);

//	弧度法から度数法へ変換する
float AngleToRadian(float angle);

//	度数法から弧度法へ変換する
float RadianToAngle(float radian);
//	オフセットからの計算
Vector3 TargetOffset(const Vector3& offset, const Vector3& rotation);

/// <summary>
/// vec1からvec2のベクトルを求める
/// </summary>
/// <param name="vec1">引くベクトル</param>
/// <param name="vec2">引かれるベクトル</param>
/// <returns>正規化されていないベクトル</returns>
Vector3 FindVector(const Vector3& vec1, const Vector3& vec2);

/// <summary>
/// 指定した範囲で実数値を返す
/// </summary>
/// <param name="min">最小値</param>
/// <param name="max">最大値</param>
/// <returns>指定した範囲の乱数</returns>
int RandNum(int min, int max);
float RandNum(float min, float max);