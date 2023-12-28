#include"Ease.h"
#define _USE_MATH_DEFINES
#include<math.h>

float Ease::UseEase(float a, float b, float t) {
	float result = (1.0f - t) * a + t * b;
	return result;
}

Vector2 Ease::UseEase(const Vector2& a, const Vector2& b, float t) {
	Vector2 result = {};

	result.x = UseEase(a.x, b.x, t);
	result.y = UseEase(a.y, b.y, t);

	return result;
}

Vector3 Ease::UseEase(const Vector3& a, const Vector3& b, float t) {
	Vector3 result = {};

	result.x = UseEase(a.x, b.x, t);
	result.y = UseEase(a.y, b.y, t);
	result.z = UseEase(a.z, b.z, t);

	return result;
}

float Ease::MakeEaseT(int nowFrame, int maxFrame, EaseType easeType, int index) {

	float t = 1.0f / maxFrame * nowFrame;
	float result = 0.0f;

	float c1 = 1.70158f;
	float c2 = c1 * 1.525f;
	float c3 = c1 + 1;
	float c4 = static_cast<float>((2.0f * M_PI) / 3.0f);
	float c5 = static_cast<float>((2.0f * M_PI) / 4.5f);
	float n1 = 7.5625f;
	float d1 = 2.75f;

	switch (easeType) {
	case EaseType::Constant:
		result = t;
		break;

	case EaseType::EaseInSine:
		result = 1.0f - cosf(static_cast<float>(M_PI * t) / 2.0f);
		break;

	case EaseType::EaseOutSine:
		result = sinf(static_cast<float>(M_PI * t) / 2.0f);
		break;

	case EaseType::EaseInOutSine:
		result = -(cosf(static_cast<float>(M_PI * t)) - 1.0f) / 2.0f;
		break;

	case EaseType::EaseIn:
		result = powf(t, static_cast<float>(index));
		break;

	case EaseType::EaseOut:
		result = 1.0f - powf(1.0f - t, static_cast<float>(index));
		break;

	case EaseType::EaseInOut:
		result = t < 0.5f ? powf(2.0f, static_cast<float>(index) - 1.0f) * powf(t, static_cast<float>(index)) : 1.0f - powf(-2.0f * t + 2.0f, static_cast<float>(index)) / 2.0f;
		break;

	case EaseType::EaseInExpo:
		result = t == 0.0f ? 0.0f : powf(2.0f, 10.0f * t - 10.0f);
		break;

	case EaseType::EaseOutExpo:
		result = t == 1.0f ? 1.0f : 1.0f - powf(2.0f, -10.0f * t);
		break;

	case EaseType::EaseInOutExpo:
		result = t == 0.0f ? 0.0f : t == 1.0f ? 1.0f : t < 0.5f ? powf(2.0f, 20.0f * t - 10.0f) / 2.0f : (2.0f - powf(2.0f, -20.0f * t + 10.0f)) / 2.0f;
		break;

	case EaseType::EaseInCirc:
		result = 1.0f - sqrtf(1.0f - powf(t, 2.0f));
		break;

	case EaseType::EaseOutCirc:
		result = sqrtf(1.0f - powf(t - 1.0f, 2.0f));
		break;

	case EaseType::EaseInOutCirc:
		result = t < 0.5f ? (1.0f - sqrtf(1.0f - powf(2.0f * t, 2.0f))) / 2.0f : (sqrtf(1.0f - powf(-2.0f * t + 2.0f, 2.0f)) + 1.0f) / 2.0f;
		break;

	case EaseType::EaseInBack:
		result = c3 * t * t * t - c1 * t * t;
		break;

	case EaseType::EaseOutBack:
		result = 1.0f + c3 * powf(t - 1.0f, 3.0f) + c1 * powf(t - 1.0f, 2.0f);
		break;

	case EaseType::EaseInOutBack:
		result = t < 0.5f ? (powf(2.0f * t, 2.0f) * ((c2 + 1.0f) * 2.0f * t - c2)) / 2.0f : (powf(2.0f * t - 2.0f, 2.0f) * ((c2 + 1.0f) * (t * 2.0f - 2.0f) + c2) + 2.0f) / 2.0f;
		break;

	case EaseType::EaseInElastic:
		result = t == 0.0f ? 0.0f : t == 1.0f ? 1.0f : -powf(2.0f, 10.0f * t - 10.0f) * sinf((t * 10.0f - 10.75f) * c4);
		break;

	case EaseType::EaseOutElastic:
		result = t == 0.0f ? 0.0f : t == 1.0f ? 1.0f : powf(2.0f, -10.0f * t) * sinf((t * 10.0f - 0.75f) * c4) + 1.0f;
		break;

	case EaseType::EaseInOutElastic:
		result = t == 0.0f ? 0.0f : t == 1.0f ? 1.0f : t < 0.5f ? -(powf(2.0f, 20.0f * t - 10.0f) * sinf((20.0f * t - 11.125f) * c5)) / 2.0f : (powf(2.0f, -20.0f * t + 10.0f) * sinf((20.0f * t - 11.125f) * c5)) / 2.0f + 1.0f;
		break;

	case EaseType::EaseInBounce:
		result = 1.0f - MakeEaseT(1.0f - t, EaseType::EaseOutBounce, 0);
		break;

	case EaseType::EaseOutBounce:
		if (t < 1.0f / d1) {
			result = n1 * t * t;
		}
		else if (t < 2 / d1) {
			result = n1 * (t -= 1.5f / d1) * t + 0.75f;
		}
		else if (t < 2.5 / d1) {
			result = n1 * (t -= 2.25f / d1) * t + 0.9375f;
		}
		else {
			result = n1 * (t -= 2.625f / d1) * t + 0.984375f;
		}
		break;

	case EaseType::EaseInOutBounce:
		result = t < 0.5f ? (1.0f - MakeEaseT(1.0f - 2.0f * t, EaseType::EaseOutBounce, 0)) / 2.0f : (1.0f + MakeEaseT(2.0f * t - 1.0f, EaseType::EaseOutBounce, 0)) / 2.0f;
		break;

	}

	return result;
}

float Ease::MakeEaseT(float nowFrame, int maxFrame, EaseType easeType, int index) {

	float t = 1.0f / maxFrame * nowFrame;
	float result = 0.0f;

	float c1 = 1.70158f;
	float c2 = c1 * 1.525f;
	float c3 = c1 + 1;
	float c4 = static_cast<float>((2.0f * M_PI) / 3.0f);
	float c5 = static_cast<float>((2.0f * M_PI) / 4.5f);
	float n1 = 7.5625f;
	float d1 = 2.75f;

	switch (easeType) {
	case EaseType::Constant:
		result = t;
		break;

	case EaseType::EaseInSine:
		result = 1.0f - cosf(static_cast<float>(M_PI * t) / 2.0f);
		break;

	case EaseType::EaseOutSine:
		result = sinf(static_cast<float>(M_PI * t) / 2.0f);
		break;

	case EaseType::EaseInOutSine:
		result = -(cosf(static_cast<float>(M_PI * t)) - 1.0f) / 2.0f;
		break;

	case EaseType::EaseIn:
		result = powf(t, static_cast<float>(index));
		break;

	case EaseType::EaseOut:
		result = 1.0f - powf(1.0f - t, static_cast<float>(index));
		break;

	case EaseType::EaseInOut:
		result = t < 0.5f ? powf(2.0f, static_cast<float>(index) - 1.0f) * powf(t, static_cast<float>(index)) : 1.0f - powf(-2.0f * t + 2.0f, static_cast<float>(index)) / 2.0f;
		break;

	case EaseType::EaseInExpo:
		result = t == 0.0f ? 0.0f : powf(2.0f, 10.0f * t - 10.0f);
		break;

	case EaseType::EaseOutExpo:
		result = t == 1.0f ? 1.0f : 1.0f - powf(2.0f, -10.0f * t);
		break;

	case EaseType::EaseInOutExpo:
		result = t == 0.0f ? 0.0f : t == 1.0f ? 1.0f : t < 0.5f ? powf(2.0f, 20.0f * t - 10.0f) / 2.0f : (2.0f - powf(2.0f, -20.0f * t + 10.0f)) / 2.0f;
		break;

	case EaseType::EaseInCirc:
		result = 1.0f - sqrtf(1.0f - powf(t, 2.0f));
		break;

	case EaseType::EaseOutCirc:
		result = sqrtf(1.0f - powf(t - 1.0f, 2.0f));
		break;

	case EaseType::EaseInOutCirc:
		result = t < 0.5f ? (1.0f - sqrtf(1.0f - powf(2.0f * t, 2.0f))) / 2.0f : (sqrtf(1.0f - powf(-2.0f * t + 2.0f, 2.0f)) + 1.0f) / 2.0f;
		break;

	case EaseType::EaseInBack:
		result = c3 * t * t * t - c1 * t * t;
		break;

	case EaseType::EaseOutBack:
		result = 1.0f + c3 * powf(t - 1.0f, 3.0f) + c1 * powf(t - 1.0f, 2.0f);
		break;

	case EaseType::EaseInOutBack:
		result = t < 0.5f ? (powf(2.0f * t, 2.0f) * ((c2 + 1.0f) * 2.0f * t - c2)) / 2.0f : (powf(2.0f * t - 2.0f, 2.0f) * ((c2 + 1.0f) * (t * 2.0f - 2.0f) + c2) + 2.0f) / 2.0f;
		break;

	case EaseType::EaseInElastic:
		result = t == 0.0f ? 0.0f : t == 1.0f ? 1.0f : -powf(2.0f, 10.0f * t - 10.0f) * sinf((t * 10.0f - 10.75f) * c4);
		break;

	case EaseType::EaseOutElastic:
		result = t == 0.0f ? 0.0f : t == 1.0f ? 1.0f : powf(2.0f, -10.0f * t) * sinf((t * 10.0f - 0.75f) * c4) + 1.0f;
		break;

	case EaseType::EaseInOutElastic:
		result = t == 0.0f ? 0.0f : t == 1.0f ? 1.0f : t < 0.5f ? -(powf(2.0f, 20.0f * t - 10.0f) * sinf((20.0f * t - 11.125f) * c5)) / 2.0f : (powf(2.0f, -20.0f * t + 10.0f) * sinf((20.0f * t - 11.125f) * c5)) / 2.0f + 1.0f;
		break;

	case EaseType::EaseInBounce:
		result = 1.0f - MakeEaseT(1.0f - t, EaseType::EaseOutBounce, 0);
		break;

	case EaseType::EaseOutBounce:
		if (t < 1.0f / d1) {
			result = n1 * t * t;
		}
		else if (t < 2 / d1) {
			result = n1 * (t -= 1.5f / d1) * t + 0.75f;
		}
		else if (t < 2.5 / d1) {
			result = n1 * (t -= 2.25f / d1) * t + 0.9375f;
		}
		else {
			result = n1 * (t -= 2.625f / d1) * t + 0.984375f;
		}
		break;

	case EaseType::EaseInOutBounce:
		result = t < 0.5f ? (1.0f - MakeEaseT(1.0f - 2.0f * t, EaseType::EaseOutBounce, 0)) / 2.0f : (1.0f + MakeEaseT(2.0f * t - 1.0f, EaseType::EaseOutBounce, 0)) / 2.0f;
		break;

	}

	return result;
}

float Ease::MakeEaseT(float nowFrame, float maxFrame, EaseType easeType, int index) {

	float t = 1.0f / maxFrame * nowFrame;
	float result = 0.0f;

	float c1 = 1.70158f;
	float c2 = c1 * 1.525f;
	float c3 = c1 + 1;
	float c4 = static_cast<float>((2.0f * M_PI) / 3.0f);
	float c5 = static_cast<float>((2.0f * M_PI) / 4.5f);
	float n1 = 7.5625f;
	float d1 = 2.75f;

	switch (easeType) {
	case EaseType::Constant:
		result = t;
		break;

	case EaseType::EaseInSine:
		result = 1.0f - cosf(static_cast<float>(M_PI * t) / 2.0f);
		break;

	case EaseType::EaseOutSine:
		result = sinf(static_cast<float>(M_PI * t) / 2.0f);
		break;

	case EaseType::EaseInOutSine:
		result = -(cosf(static_cast<float>(M_PI * t)) - 1.0f) / 2.0f;
		break;

	case EaseType::EaseIn:
		result = powf(t, static_cast<float>(index));
		break;

	case EaseType::EaseOut:
		result = 1.0f - powf(1.0f - t, static_cast<float>(index));
		break;

	case EaseType::EaseInOut:
		result = t < 0.5f ? powf(2.0f, static_cast<float>(index) - 1.0f) * powf(t, static_cast<float>(index)) : 1.0f - powf(-2.0f * t + 2.0f, static_cast<float>(index)) / 2.0f;
		break;

	case EaseType::EaseInExpo:
		result = t == 0.0f ? 0.0f : powf(2.0f, 10.0f * t - 10.0f);
		break;

	case EaseType::EaseOutExpo:
		result = t == 1.0f ? 1.0f : 1.0f - powf(2.0f, -10.0f * t);
		break;

	case EaseType::EaseInOutExpo:
		result = t == 0.0f ? 0.0f : t == 1.0f ? 1.0f : t < 0.5f ? powf(2.0f, 20.0f * t - 10.0f) / 2.0f : (2.0f - powf(2.0f, -20.0f * t + 10.0f)) / 2.0f;
		break;

	case EaseType::EaseInCirc:
		result = 1.0f - sqrtf(1.0f - powf(t, 2.0f));
		break;

	case EaseType::EaseOutCirc:
		result = sqrtf(1.0f - powf(t - 1.0f, 2.0f));
		break;

	case EaseType::EaseInOutCirc:
		result = t < 0.5f ? (1.0f - sqrtf(1.0f - powf(2.0f * t, 2.0f))) / 2.0f : (sqrtf(1.0f - powf(-2.0f * t + 2.0f, 2.0f)) + 1.0f) / 2.0f;
		break;

	case EaseType::EaseInBack:
		result = c3 * t * t * t - c1 * t * t;
		break;

	case EaseType::EaseOutBack:
		result = 1.0f + c3 * powf(t - 1.0f, 3.0f) + c1 * powf(t - 1.0f, 2.0f);
		break;

	case EaseType::EaseInOutBack:
		result = t < 0.5f ? (powf(2.0f * t, 2.0f) * ((c2 + 1.0f) * 2.0f * t - c2)) / 2.0f : (powf(2.0f * t - 2.0f, 2.0f) * ((c2 + 1.0f) * (t * 2.0f - 2.0f) + c2) + 2.0f) / 2.0f;
		break;

	case EaseType::EaseInElastic:
		result = t == 0.0f ? 0.0f : t == 1.0f ? 1.0f : -powf(2.0f, 10.0f * t - 10.0f) * sinf((t * 10.0f - 10.75f) * c4);
		break;

	case EaseType::EaseOutElastic:
		result = t == 0.0f ? 0.0f : t == 1.0f ? 1.0f : powf(2.0f, -10.0f * t) * sinf((t * 10.0f - 0.75f) * c4) + 1.0f;
		break;

	case EaseType::EaseInOutElastic:
		result = t == 0.0f ? 0.0f : t == 1.0f ? 1.0f : t < 0.5f ? -(powf(2.0f, 20.0f * t - 10.0f) * sinf((20.0f * t - 11.125f) * c5)) / 2.0f : (powf(2.0f, -20.0f * t + 10.0f) * sinf((20.0f * t - 11.125f) * c5)) / 2.0f + 1.0f;
		break;

	case EaseType::EaseInBounce:
		result = 1.0f - MakeEaseT(1.0f - t, EaseType::EaseOutBounce, 0);
		break;

	case EaseType::EaseOutBounce:
		if (t < 1.0f / d1) {
			result = n1 * t * t;
		}
		else if (t < 2 / d1) {
			result = n1 * (t -= 1.5f / d1) * t + 0.75f;
		}
		else if (t < 2.5 / d1) {
			result = n1 * (t -= 2.25f / d1) * t + 0.9375f;
		}
		else {
			result = n1 * (t -= 2.625f / d1) * t + 0.984375f;
		}
		break;

	case EaseType::EaseInOutBounce:
		result = t < 0.5f ? (1.0f - MakeEaseT(1.0f - 2.0f * t, EaseType::EaseOutBounce, 0)) / 2.0f : (1.0f + MakeEaseT(2.0f * t - 1.0f, EaseType::EaseOutBounce, 0)) / 2.0f;
		break;

	}

	return result;
}

float Ease::MakeEaseT(float t, EaseType easeType, int index) {

	float result = 0.0f;

	float c1 = 1.70158f;
	float c2 = c1 * 1.525f;
	float c3 = c1 + 1.0f;
	float c4 = static_cast<float>(2.0f * M_PI) / 3.0f;
	float c5 = static_cast<float>(2.0f * M_PI) / 4.5f;
	float n1 = 7.5625f;
	float d1 = 2.75f;

	switch (easeType) {
	case EaseType::Constant:
		result = t;
		break;

	case EaseType::EaseInSine:
		result = 1.0f - cosf(static_cast<float>(M_PI * t) / 2.0f);
		break;

	case EaseType::EaseOutSine:
		result = sinf(static_cast<float>(M_PI * t) / 2.0f);
		break;

	case EaseType::EaseInOutSine:
		result = -(cosf(static_cast<float>(M_PI * t)) - 1.0f) / 2.0f;
		break;

	case EaseType::EaseIn:
		result = powf(t, static_cast<float>(index));
		break;

	case EaseType::EaseOut:
		result = 1.0f - powf(1.0f - t, static_cast<float>(index));
		break;

	case EaseType::EaseInOut:
		result = t < 0.5f ? powf(2.0f, static_cast<float>(index) - 1.0f) * powf(t, static_cast<float>(index)) : 1.0f - powf(-2.0f * t + 2.0f, static_cast<float>(index)) / 2.0f;
		break;

	case EaseType::EaseInExpo:
		result = t == 0.0f ? 0.0f : powf(2.0f, 10.0f * t - 10.0f);
		break;

	case EaseType::EaseOutExpo:
		result = t == 1.0f ? 1.0f : 1.0f - powf(2.0f, -10.0f * t);
		break;

	case EaseType::EaseInOutExpo:
		result = t == 0.0f ? 0.0f : t == 1.0f ? 1.0f : t < 0.5f ? powf(2.0f, 20.0f * t - 10.0f) / 2.0f : (2.0f - powf(2.0f, -20.0f * t + 10.0f)) / 2.0f;
		break;

	case EaseType::EaseInCirc:
		result = 1.0f - sqrtf(1.0f - powf(t, 2.0f));
		break;

	case EaseType::EaseOutCirc:
		result = sqrtf(1.0f - powf(t - 1.0f, 2.0f));
		break;

	case EaseType::EaseInOutCirc:
		result = t < 0.5f ? (1.0f - sqrtf(1 - powf(2.0f * t, 2.0f))) / 2.0f : (sqrtf(1.0f - powf(-2.0f * t + 2.0f, 2.0f)) + 1.0f) / 2.0f;
		break;

	case EaseType::EaseInBack:
		result = c3 * t * t * t - c1 * t * t;
		break;

	case EaseType::EaseOutBack:
		result = 1.0f + c3 * powf(t - 1.0f, 3.0f) + c1 * powf(t - 1.0f, 2.0f);
		break;

	case EaseType::EaseInOutBack:
		result = t < 0.5f ? (powf(2.0f * t, 2.0f) * ((c2 + 1.0f) * 2.0f * t - c2)) / 2.0f : (powf(2.0f * t - 2.0f, 2.0f) * ((c2 + 1.0f) * (t * 2.0f - 2.0f) + c2) + 2.0f) / 2.0f;
		break;

	case EaseType::EaseInElastic:
		result = t == 0.0f ? 0.0f : t == 1.0f ? 1.0f : -powf(2.0f, 10.0f * t - 10.0f) * sinf((t * 10.0f - 10.75f) * c4);
		break;

	case EaseType::EaseOutElastic:
		result = t == 0.0f ? 0.0f : t == 1.0f ? 1 : powf(2.0f, -10.0f * t) * sinf((t * 10.0f - 0.75f) * c4) + 1.0f;
		break;

	case EaseType::EaseInOutElastic:
		result = t == 0.0f ? 0.0f : t == 1.0f ? 1.0f : t < 0.5f ? -(powf(2.0f, 20.0f * t - 10.0f) * sinf((20.0f * t - 11.125f) * c5)) / 2.0f : (powf(2.0f, -20.0f * t + 10.0f) * sinf((20.0f * t - 11.125f) * c5)) / 2.0f + 1.0f;
		break;

	case EaseType::EaseInBounce:
		result = 1 - MakeEaseT(1.0f - t, EaseType::EaseOutBounce, 0);
		break;

	case EaseType::EaseOutBounce:
		if (t < 1.0f / d1) {
			result = n1 * t * t;
		}
		else if (t < 2.0f / d1) {
			result = n1 * (t -= 1.5f / d1) * t + 0.75f;
		}
		else if (t < 2.5f / d1) {
			result = n1 * (t -= 2.25f / d1) * t + 0.9375f;
		}
		else {
			result = n1 * (t -= 2.625f / d1) * t + 0.984375f;
		}
		break;

	case EaseType::EaseInOutBounce:
		result = t < 0.5f ? (1.0f - MakeEaseT(1.0f - 2.0f * t, EaseType::EaseOutBounce, 0)) / 2.0f : (1.0f + MakeEaseT(2.0f * t - 1.0f, EaseType::EaseOutBounce, 0)) / 2.0f;
		break;

	}

	return result;
}

float Ease::UseEase(float a, float b, int nowFrame, int maxFrame, EaseType easeType, int index) {
	float t = MakeEaseT(nowFrame, maxFrame, easeType, index);

	float result = UseEase(a, b, t);

	return result;
}

float Ease::UseEase(float a, float b, float nowFrame, int maxFrame, EaseType easeType, int index) {
	float t = MakeEaseT(nowFrame, maxFrame, easeType, index);

	float result = UseEase(a, b, t);

	return result;
}

float Ease::UseEase(float a, float b, float nowFrame, float maxFrame, EaseType easeType, int index) {
	float t = MakeEaseT(nowFrame, maxFrame, easeType, index);

	float result = UseEase(a, b, t);

	return result;
}

float Ease::UseEase(float a, float b, float t, EaseType easeType, int index) {
	float easeT = MakeEaseT(t, easeType, index);

	float result = UseEase(a, b, easeT);

	return result;
}

Vector2 Ease::UseEase(const Vector2& a, const Vector2& b, int nowFrame, int maxFrame, EaseType easeType, int index) {
	float t = MakeEaseT(nowFrame, maxFrame, easeType, index);

	Vector2 result = UseEase(a, b, t);

	return result;
}

Vector2 Ease::UseEase(const Vector2& a, const Vector2& b, float nowFrame, int maxFrame, EaseType easeType, int index) {
	float t = MakeEaseT(nowFrame, maxFrame, easeType, index);

	Vector2 result = UseEase(a, b, t);

	return result;
}

Vector2 Ease::UseEase(const Vector2& a, const Vector2& b, float nowFrame, float maxFrame, EaseType easeType, int index) {
	float t = MakeEaseT(nowFrame, maxFrame, easeType, index);

	Vector2 result = UseEase(a, b, t);

	return result;
}

Vector2 Ease::UseEase(const Vector2& a, const Vector2& b, float t, EaseType easeType, int index) {
	float easeT = MakeEaseT(t, easeType, index);

	Vector2 result = UseEase(a, b, easeT);

	return result;
}

Vector3 Ease::UseEase(const Vector3& a, const Vector3& b, int nowFrame, int maxFrame, EaseType easeType, int index) {
	float t = MakeEaseT(nowFrame, maxFrame, easeType, index);

	Vector3 result = UseEase(a, b, t);

	return result;
}

Vector3 Ease::UseEase(const Vector3& a, const Vector3& b, float nowFrame, int maxFrame, EaseType easeType, int index) {
	float t = MakeEaseT(nowFrame, maxFrame, easeType, index);

	Vector3 result = UseEase(a, b, t);

	return result;
}

Vector3 Ease::UseEase(const Vector3& a, const Vector3& b, float nowFrame, float maxFrame, EaseType easeType, int index) {
	float t = MakeEaseT(nowFrame, maxFrame, easeType, index);

	Vector3 result = UseEase(a, b, t);

	return result;
}

Vector3 Ease::UseEase(const Vector3& a, const Vector3& b, float t, EaseType easeType, int index) {
	float easeT = MakeEaseT(t, easeType, index);

	Vector3 result = UseEase(a, b, easeT);

	return result;
}

