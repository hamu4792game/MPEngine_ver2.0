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
