#pragma once
#include "math/Vector2.h"
#include "math/Vector3.h"

class Ease {
public:
	//イージングチートシートより
	enum EaseType {
		Constant,

		EaseInSine,
		EaseOutSine,
		EaseInOutSine,

		//Quadから
		//index乗で対応してる
		EaseIn,
		EaseOut,
		EaseInOut,
		//Quintまで

		EaseInExpo,
		EaseOutExpo,
		EaseInOutExpo,
		EaseInCirc,
		EaseOutCirc,
		EaseInOutCirc,
		EaseInBack,
		EaseOutBack,
		EaseInOutBack,
		EaseInElastic,
		EaseOutElastic,
		EaseInOutElastic,
		EaseInBounce,
		EaseOutBounce,
		EaseInOutBounce,
	};

	static float UseEase(float a, float b, float t);

	static Vector2 UseEase(const Vector2& a, const Vector2& b, float t);

	static Vector3 UseEase(const Vector3& a, const Vector3& b, float t);

	static float MakeEaseT(int nowFrame, int maxFrame, EaseType easeType, int index = 2);
	//返ってくるのはイージングに使うtの値
	static float MakeEaseT(float nowFrame, int maxFrame, EaseType easeType, int index = 2);
	//返ってくるのはイージングに使うtの値
	static float MakeEaseT(float nowFrame, float maxFrame, EaseType easeType, int index = 2);
	//返ってくるのはイージングに使うtの値
	static float MakeEaseT(float t, EaseType easeType, int index = 2);
	//返ってくるのはイージングに使うtの値

	//使うのはここから下4つだと思う。

	static float UseEase(float a, float b, int nowFrame, int maxFrame, EaseType easeType, int index = 2);

	static float UseEase(float a, float b, float nowFrame, int maxFrame, EaseType easeType, int index = 2);

	static float UseEase(float a, float b, float nowFrame, float maxFrame, EaseType easeType, int index = 2);

	static float UseEase(float a, float b, float t, EaseType easeType, int index = 2);

	static Vector2 UseEase(const Vector2& a, const Vector2& b, int nowFrame, int maxFrame, EaseType easeType, int index = 2);

	static Vector2 UseEase(const Vector2& a, const Vector2& b, float nowFrame, int maxFrame, EaseType easeType, int index = 2);

	static Vector2 UseEase(const Vector2& a, const Vector2& b, float nowFrame, float maxFrame, EaseType easeType, int index = 2);

	static Vector2 UseEase(const Vector2& a, const Vector2& b, float t, EaseType easeType, int index = 2);

	static Vector3 UseEase(const Vector3& a, const Vector3& b, int nowFrame, int maxFrame, EaseType easeType, int index = 2);

	static Vector3 UseEase(const Vector3& a, const Vector3& b, float nowFrame, int maxFrame, EaseType easeType, int index = 2);

	static Vector3 UseEase(const Vector3& a, const Vector3& b, float nowFrame, float maxFrame, EaseType easeType, int index = 2);

	static Vector3 UseEase(const Vector3& a, const Vector3& b, float t, EaseType easeType, int index = 2);

	//返ってくる値はイージングされた値
	//aにt=0のときの値、bにt=1の時の値
	//easeTypeには使用するイージング、上のenumの文字列を入力。EaseInBackとか。
	//indexにはEaseIn,Out,InOutのべき乗を入力

};

