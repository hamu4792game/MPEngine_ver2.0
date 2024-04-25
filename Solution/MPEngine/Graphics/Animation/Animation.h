#pragma once
#include <vector>
#include "Math/MathUtl.h"
#include <map>
#include <string>

// アニメーション用構造体
template <typename tValue>
struct Keyframe {
	float time; // キーフレームの時刻(単位は秒)
	tValue value; // キーフレームの値
};
template <typename tValue>
struct AnimationCurve {
	std::vector<Keyframe<tValue>> keyframes;
};
// node毎のアニメーションとしてまとめる
struct NodeAnimation {
	AnimationCurve<Vector3> translate;
	AnimationCurve<Quaternion> rotate;
	AnimationCurve<Vector3> scale;
};

class Animation {
	friend class ResourceManager;
public:
	Animation();
	~Animation();

	void Update();

private:
	float duration; // アニメーション全体の尺(単位は秒)
	// NodeAnimationの集合。Nodeメイでひけるようにしておく
	std::map<std::string, NodeAnimation> nodeAnimations;

	// 任意時刻の値を取得する
	Vector3 CalculateValue(const std::vector<Keyframe<Vector3>>& keyframes, float time);
	Quaternion CalculateValue(const std::vector<Keyframe<Quaternion>>& keyframes, float time);
	float animationTime_ = 0.0f;

};
