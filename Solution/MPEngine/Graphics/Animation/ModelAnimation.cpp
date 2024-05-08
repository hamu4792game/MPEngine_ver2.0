#include "ModelAnimation.h"
#include "Graphics/Model/Model.h"

void ModelAnimation::Load(const AnimationData& data, const Model* model) {
	data_ = data;
	model_ = model;
}

void ModelAnimation::Play(const bool& flag) {
	isPlay_ = flag;
	animationTime_ = 0.0f;
}

Matrix4x4 ModelAnimation::Update() {
	if (isPlay_ == true) {
		// 固定フレーム
		static float kFrame = 1.0f / 60.0f;
		animationTime_ += kFrame; // のちに可変フレームに対応
		animationTime_ = std::fmod(animationTime_, data_.duration); // 最後まで行ったらリピート再生
		NodeAnimation& rootNodeAnimation = data_.nodeAnimations[model_->model_->GetModel().rootNode.name]; // rootnodeのanimationを取得
		Vector3 translate = CalculateValue(rootNodeAnimation.translate.keyframes, animationTime_);
		Quaternion rotate = CalculateValue(rootNodeAnimation.rotate.keyframes, animationTime_);
		Vector3 scale = CalculateValue(rootNodeAnimation.scale.keyframes, animationTime_);
		localMatrix = MakeAffineMatrix(scale, rotate, translate);
	}	
	return localMatrix;
}

void ModelAnimation::Update(Skeleton& skeleton) {
	// 全てのJointを更新、親が若いので通常ループで処理可能になっている
	for (Joint& joint : skeleton.joints) {
		joint.localMatrix = joint.transform.UpdateMatrix();
		if (joint.parent) { /// 親がいれば親の行列を抜ける
			joint.skeletonSpaceMatrix = joint.localMatrix * skeleton.joints[*joint.parent].skeletonSpaceMatrix;
		}
		else { // 親がいないのでlocalMatrixとskeletonSpaceMatrixは一致する
			joint.skeletonSpaceMatrix = joint.localMatrix;
		}
	}
}

void ModelAnimation::ApplyAnimation(Skeleton& skeleton, const AnimationData& animation, float animationTime) {
	for (Joint& joint : skeleton.joints) {
		// 対象のJointのAnimationがあれば、相対の適応を行う。下のif文はC++17から可能になった初期化付きif文
		if (auto it = animation.nodeAnimations.find(joint.name); it != animation.nodeAnimations.end()) {
			const NodeAnimation& rootNodeAnimation = (*it).second;
			joint.transform.translation_ = CalculateValue(rootNodeAnimation.translate.keyframes, animationTime);
			joint.transform.rotationQuat_ = CalculateValue(rootNodeAnimation.rotate.keyframes, animationTime);
			joint.transform.scale_ = CalculateValue(rootNodeAnimation.scale.keyframes, animationTime);

		}
	}
}

Vector3 ModelAnimation::CalculateValue(const std::vector<Keyframe<Vector3>>& keyframes, float time) {
	assert(!keyframes.empty()); // キーがない場合
	// 特殊なケースを早期リターンすることで、のちの処理が纏まる
	if (keyframes.size() == 1 || time <= keyframes[0].time) { // キーが1つか、時刻がキーフレームなら最初の値を返す
		return keyframes[0].value;
	}
	
	for (size_t index = 0; index < keyframes.size() - 1; index++) {
		size_t nextIndex = index + 1;
		// indexとnextindexの二つをkeyframeを取得して範囲内に時刻がアルカを判定
		if (keyframes[index].time <= time && time <= keyframes[nextIndex].time) {
			// 範囲内を確認する
			float t = (time - keyframes[index].time / (keyframes[nextIndex].time - keyframes[index].time));
			// 線形補間で返す
			return Lerp(keyframes[index].value, keyframes[nextIndex].value, t);
		}
	}
	// ここまで来た場合は一番最後の時刻よりも後なため、最後の値を返す
	return (*keyframes.rbegin()).value;
}

Quaternion ModelAnimation::CalculateValue(const std::vector<Keyframe<Quaternion>>& keyframes, float time) {
	assert(!keyframes.empty()); // キーがない場合
	// 特殊なケースを早期リターンすることで、のちの処理が纏まる
	if (keyframes.size() == 1 || time <= keyframes[0].time) { // キーが1つか、時刻がキーフレームなら最初の値を返す
		return keyframes[0].value;
	}

	for (size_t index = 0; index < keyframes.size() - 1; index++) {
		size_t nextIndex = index + 1;
		// indexとnextindexの二つをkeyframeを取得して範囲内に時刻がアルカを判定
		if (keyframes[index].time <= time && time <= keyframes[nextIndex].time) {
			// 範囲内を確認する
			float t = (time - keyframes[index].time / (keyframes[nextIndex].time - keyframes[index].time));
			/// 球面補間で返す
			return Quaternion::Slerp(keyframes[index].value, keyframes[nextIndex].value, t);
		}
	}
	// ここまで来た場合は一番最後の時刻よりも後なため、最後の値を返す
	return (*keyframes.rbegin()).value;
}
