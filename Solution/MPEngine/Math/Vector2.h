#pragma once

/// <summary>
/// 2次元ベクトル
/// </summary>
class Vector2 final {
public:
	Vector2();
	Vector2(const float& X, const float& Y);

public:
	float x;
	float y;

	static Vector2 zero;
	static Vector2 one;
public:
	Vector2 operator+(const Vector2& num) const;
	Vector2 operator-(const Vector2& num) const;
	Vector2& operator+=(const Vector2& num);
	Vector2& operator-=(const Vector2& num);
	Vector2& operator=(const Vector2& num);

	Vector2 operator*(const float& num) const;
	Vector2& operator*=(const float& num);

	bool operator==(const Vector2& num) const;
	bool operator!=(const Vector2& num) const;

public:
	// 距離を求める
	friend float Distance(const Vector2& vecA, const Vector2& vecB);

};