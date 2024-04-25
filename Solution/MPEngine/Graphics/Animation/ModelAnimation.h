#pragma once
#include "ResourceManager/ResourceManager.h"

class ModelAnimation {
public:
	ModelAnimation() = default;
	~ModelAnimation() = default;

	void Play();
	void Update();

private:
	// 任意時刻の値を取得する
	Vector3 CalculateValue(const std::vector<Keyframe<Vector3>>& keyframes, float time);
	Quaternion CalculateValue(const std::vector<Keyframe<Quaternion>>& keyframes, float time);
	float animationTime_ = 0.0f;
	Animation animation_;

};
