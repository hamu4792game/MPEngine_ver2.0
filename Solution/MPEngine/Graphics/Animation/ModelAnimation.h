#pragma once
#include <vector>
#include "Math/MathUtl.h"
#include "Math/Quaternion.h"
#include <map>
#include <string>
#include "AnimationData.h"
#include "ResourceManager/ResourceManager.h"

class Model;

class ModelAnimation {
	friend class ResourceManager;
public:
	ModelAnimation() = default;
	~ModelAnimation() = default;

	enum AnimationType {
		None,
		Rigid,
		Skin,
		kMaxNum
	};

	void Load(const AnimationData* data, const Model* model);
	void Play(const bool& flag);
	Matrix4x4 Update();
	const Matrix4x4& GetMatrix() const { return localMatrix; }
	void Update(Skeleton& skeleton);
	void ApplyAnimation(Skeleton& skeleton, const AnimationData& animation, float animationTime);

private:
	static Skeleton CreateSkeleton(const Node& rootNode);
	static int32_t CreateJoint(const Node& node, const std::optional<int32_t>& parent, std::vector<Joint>& joints);

private:
	// 任意時刻の値を取得する
	Vector3 CalculateValue(const std::vector<Keyframe<Vector3>>& keyframes, float time);
	Quaternion CalculateValue(const std::vector<Keyframe<Quaternion>>& keyframes, float time);
	float animationTime_ = 0.0f;
	const AnimationData* data_ = nullptr;
	const Model* model_ = nullptr;
	Matrix4x4 localMatrix;
	bool isPlay_ = true;

};
