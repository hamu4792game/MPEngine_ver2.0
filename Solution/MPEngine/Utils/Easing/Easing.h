#pragma once

#include <algorithm>
#include <numbers>

/*　イージングチートシート参照（左上[1]から右下[30]にかけて）　*/

//		使い方：例文
//		if(true){
//			num += spd;
//			T = Easing::EaseInSine(num);		Easing(クラス名)	EaseInSine(クラス内変数名)
//			pos_x = (1 - T) * 始点 + T * 終点;
//			pos_y = (1 - T) * 始点 + T * 終点;
//		}


class Easing {
public:

	//	1番
	static inline float EaseInSine(float x) {
		return 1.0f - cosf((x * std::numbers::pi_v<float>) / 2.0f);
	}
	//	2
	static inline float EaseOutSine(float x) {
		return sinf((x * std::numbers::pi_v<float>) / 2.0f);
	}
	//	3
	static inline float EaseInOutSine(float x) {
		return -1.0f * (cosf(std::numbers::pi_v<float> * x) - 1.0f) / 2.0f;
	}
	//	4
	static inline float EaseInQuad(float x) {
		return powf(x, 2.0f);
	}
	//	5
	static inline float EaseOutQuad(float x) {
		return 1.0f - (1.0f - x) * (1.0f - x);
	}
	//	6
	static inline float EaseInOutQuad(float x) {

		if (x < 0.5f)
		{
			return 2.0f * powf(x, 2.0f);
		}
		else
		{
			return 1.0f - powf(-2.0f * x + 2.0f, 2.0f) / 2.0f;
		}
	}
	//	7
	static inline float EaseInCubic(float x) {
		return powf(x, 3.0f);
	}
	//	8
	static inline float EaseOutCubic(float x) {
		return 1.0f - powf(1.0f - x, 3.0f);
	}
	//	9
	static inline float EaseInOutCubic(float x) {
		if (x < 0.5f)
		{
			return 4.0f * powf(x, 4.0f);
		}
		else
		{
			return 1.0f - powf(-2.0f * x + 2.0f, 3.0f) / 2.0f;
		}
	}
	//	10
	static inline float EaseInQuart(float x) {
		return powf(x, 4.0f);
	}
	//	11
	static inline float EaseOutQuart(float x) {
		return 1.0f - powf(1.0f - x, 4.0f);
	}
	//	12
	static inline float EaseInOutQuart(float x) {
		if (x < 0.5f)
		{
			return 8.0f * powf(x, 4.0f);
		}
		else
		{
			return 1.0f - powf(-2.0f * x + 2.0f, 4.0f) / 2.0f;
		}
	}
	//	13
	static inline float EaseInQuint(float x) {
		return powf(x, 5.0f);
	}
	//	14
	static inline float EaseOutQuint(float x) {
		return 1.0f - powf(1.0f - x, 5.0f);
	}
	//	15
	static inline float EaseInOutQuint(float x) {
		if (x < 0.5f)
		{
			return 16.0f * powf(x, 5.0f);
		}
		else
		{
			return 1.0f - powf(-2.0f * x + 2.0f, 5.0f) / 2.0f;
		}
	}
	//	16
	static inline float EaseInExpo(float x) {
		if (x == 0.0f)
		{
			return 0.0f;
		}
		else
		{
			return powf(2.0f, 10.0f * x - 10.0f);
		}
	}
	//	17
	static inline float EaseOutExpo(float x) {
		if (x == 1.0f)
		{
			return 1.0f;
		}
		else
		{
			return (1.0f - powf(2.0f, -10.0f * x));
		}
	}
	//	18
	static inline float EaseInOutExpo(float x) {
		if (x == 0.0f)
		{
			return 0.0f;
		}
		else if (x == 1.0f)
		{
			return 1.0f;
		}
		else if (x < 0.5f)
		{
			return powf(2.0f, 20.0f * x - 10.0f) / 2.0f;
		}
		else
		{
			return (2.0f - powf(2.0f, -20.0f * x + 10.0f)) / 2.0f;
		}
	}
	//	19
	static inline float EaseInCirc(float x) {
		return 1.0f - sqrtf(1.0f - powf(x, 2.0f));
	}
	//	20
	static inline float EaseOutCirc(float x) {
		return sqrtf(1.0f - powf(x - 1.0f, 2.0f));
	}
	//	21
	static inline float EaseInOutCirc(float x) {
		if (x < 0.5f)
		{
			return (1.0f - sqrtf(1.0f - powf(2.0f * x, 2.0f))) / 2.0f;
		}
		else
		{
			return (sqrtf(1.0f - powf(-2.0f * x + 2.0f, 2.0f)) + 1.0f) / 2.0f;
		}
	}
	//	22
	static inline float EaseInBack(float x) {
		const float c1 = 1.70158f;
		const float c3 = c1 + 1.0f;

		return (c3 * powf(x, 3.0f)) - (c1 * powf(x, 2.0f));
	}
	//	23
	static inline float EaseOutBack(float x) {
		const float c1 = 1.70158f;
		const float c3 = c1 + 1.0f;

		return 1 + c3 * powf(x - 1.0f, 3.0f) + c1 * powf(x - 1.0f, 2.0f);
	}
	//	24
	static inline float EaseInOutBack(float x) {
		const float c1 = 1.70158f;
		const float c2 = c1 * 1.525f;

		if (x < 0.5f)
		{
			return (powf(2.0f * x, 2.0f) * ((c2 + 1.0f) * 2.0f * x - c2)) / 2.0f;
		}
		else
		{
			return (powf(2.0f * x - 2.0f, 2.0f) * ((c2 + 1.0f) * (x * 2.0f - 2.0f) + c2) + 2.0f) / 2.0f;
		}
	}
	//	25
	static inline float EaseInElastic(float x) {
		const float c4 = (2.0f * std::numbers::pi_v<float>) / 3.0f;

		if (x == 0.0f)
		{
			return 0.0f;
		}
		else if (x == 1.0f)
		{
			return 1.0f;
		}
		else
		{
			return -powf(2.0f, 10.0f * x - 10.0f) * sinf((x * 10.0f - 10.75f) * c4);
		}
	}
	//	26
	static inline float EaseOutElastic(float x) {
		const float c4 = (2.0f * std::numbers::pi_v<float>) / 3.0f;

		if (x == 0.0f) {
			return 0.0f;
		}
		else if (x >= 1.0f) {
			return 1.0f;
		}
		else {
			return powf(2.0f, -10.0f * x) * sinf((x * 10.0f - 0.75f) * c4) + 1.0f;
		}
	}
	//	27
	static inline float EaseInOutElastic(float x) {
		const float c5 = (2.0f * std::numbers::pi_v<float>) / 4.5f;

		if (x == 0.0f)
		{
			return 0.0f;
		}
		else if (x == 1.0f)
		{
			return 1.0f;
		}
		else if (x < 0.5f)
		{
			return -1.0f * (powf(2.0f, 20.0f * x - 10.0f) * sinf((20.0f * x - 11.125f) * c5)) / 2.0f;
		}
		else
		{
			return (powf(2.0f, -20.0f * x + 10.0f) * sinf((20.0f * x - 11.125f) * c5)) / 2.0f + 1.0f;
		}
	}
	//	28
	static inline float EaseInBounce(float x) {
		return 1.0f - EaseOutBounce(1.0f - x);
	}
	//	29
	static inline float EaseOutBounce(float x) {
		const float n1 = 7.5625f;
		const float d1 = 2.75f;

		if (x < 1.0f / d1) {
			return n1 * powf(x, 2.0f);
		}
		else if (x < 2.0f / d1) {
			return n1 * (x -= 1.5f / d1) * x + 0.75f;
		}
		else if (x < 2.5f / d1) {
			return n1 * (x -= 2.25f / d1) * x + 0.9375f;
		}
		else {
			return n1 * (x -= 2.625f / d1) * x + 0.984375f;
		}
	}
	//	30
	static inline float EaseInOutBounce(float x) {
		if (x < 0.5f)
		{
			return (1.0f - EaseOutBounce(1.0f - 2.0f * x)) / 2.0f;
		}
		else
		{
			return (1.0f + EaseOutBounce(2.0f * x - 1.0f)) / 2.0f;
		}
	}
};

