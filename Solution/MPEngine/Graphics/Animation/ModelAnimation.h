#pragma once
#include <vector>
#include "Math/MathUtl.h"
#include "Math/Quaternion.h"
#include <map>
#include <string>
#include "ResourceManager/ResourceManager.h"

class ModelAnimation {
	friend class ResourceManager;
public:
	ModelAnimation() = default;
	~ModelAnimation() = default;

	void Load(const AnimationData& data, const class Model* model);
	void Play(const bool& flag);
	Matrix4x4 Update();
	const Matrix4x4& GetMatrix() const { return localMatrix; }
	void Update(Skeleton& skeleton);
	void ApplyAnimation(Skeleton& skeleton, const AnimationData& animation, float animationTime);

private:
	// 任意時刻の値を取得する
	Vector3 CalculateValue(const std::vector<Keyframe<Vector3>>& keyframes, float time);
	Quaternion CalculateValue(const std::vector<Keyframe<Quaternion>>& keyframes, float time);
	float animationTime_ = 0.0f;
	AnimationData data_;
	const class Model* model_ = nullptr;
	Matrix4x4 localMatrix;
	bool isPlay_ = true;

};
