#pragma once
#include "Math/MathUtl.h"
#include "Utils/WorldTransform/WorldTransform.h"
#include <vector>
#include <map>
#include <string>
#include <optional>

// アニメーション用構造体
template <typename tValue>
struct Keyframe {
	float time = 0.0f; // キーフレームの時刻(単位は秒)
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

struct Joint {
	WorldTransform transform;
	Matrix4x4 localMatrix;
	Matrix4x4 skeletonSpaceMatrix; // skeletonSpaceでの変換行列
	std::string name; // 名前
	std::vector<int32_t> children; // 子jointのindexリスト。居なければ空
	int32_t index = 0; // 自信のindex
	std::optional<int32_t> parent; // 親jointのindex。いなければnull
};

struct Skeleton {
	int32_t root = 0; // rootJointのindex
	std::map<std::string, int32_t> jointMap; // Joint名とIndexとの辞書
	std::vector<Joint> joints; // 所属しているJoint
};

class AnimationData {
	friend class ResourceManager;
public:
	AnimationData() = default;
private:
	AnimationData operator=(const AnimationData& animation);
	void Load(const std::string& name, const std::string& filename);

public:
	std::string name_;
	float duration = 0.0f; // アニメーション全体の尺(単位は秒)
	// NodeAnimationの集合。Nodeメイでひけるようにしておく
	std::map<std::string, NodeAnimation> nodeAnimations;


};
