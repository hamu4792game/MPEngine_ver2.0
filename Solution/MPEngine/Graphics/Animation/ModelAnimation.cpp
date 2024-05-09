#include "ModelAnimation.h"
#include "Graphics/Model/Model.h"

void ModelAnimation::Load(const AnimationData* data, const Model* model) {
	data_ = data;
	model_ = model;
	skeleton_ = CreateSkeleton(model_->model_->GetModel().rootNode);
	for (Joint& joint : skeleton_.joints) {
		if (!joint.parent && lines_.empty()) { continue; }
		auto& line = lines_.emplace_back(std::make_unique<Line>());
	}
}

void ModelAnimation::Play(const bool& flag) {
	isPlay_ = flag;
	animationTime_ = 0.0f;
}

/*Matrix4x4 ModelAnimation::Update() {
	if (isPlay_ == true) {
		// 固定フレーム
		static float kFrame = 1.0f / 60.0f;
		animationTime_ += kFrame; // のちに可変フレームに対応
		animationTime_ = std::fmod(animationTime_, data_->duration); // 最後まで行ったらリピート再生
		NodeAnimation rootNodeAnimation = data_->nodeAnimations.at(model_->model_->GetModel().rootNode.name); // rootnodeのanimationを取得
		Vector3 translate = CalculateValue(rootNodeAnimation.translate.keyframes, animationTime_);
		Quaternion rotate = CalculateValue(rootNodeAnimation.rotate.keyframes, animationTime_);
		Vector3 scale = CalculateValue(rootNodeAnimation.scale.keyframes, animationTime_);
		localMatrix = MakeAffineMatrix(scale, rotate, translate);
	}	
	return localMatrix;
}*/

void ModelAnimation::Update(const WorldTransform& transform) {
	// 全てのJointを更新、親が若いので通常ループで処理可能になっている
	for (Joint& joint : skeleton_.joints) {
		joint.localMatrix = joint.transform.UpdateMatrix();
		if (joint.parent) { /// 親がいれば親の行列を抜ける
			joint.skeletonSpaceMatrix = joint.localMatrix * skeleton_.joints[*joint.parent].skeletonSpaceMatrix;
		}
		else { // 親がいないのでlocalMatrixとskeletonSpaceMatrixは一致する
			joint.skeletonSpaceMatrix = joint.localMatrix;
		}
	}
	Draw(transform);
}

void ModelAnimation::ApplyAnimation(float animationTime) {
	for (Joint& joint : skeleton_.joints) {
		// 対象のJointのAnimationがあれば、相対の適応を行う。下のif文はC++17から可能になった初期化付きif文
		if (auto it = data_->nodeAnimations.find(joint.name); it != data_->nodeAnimations.end()) {
			const NodeAnimation& rootNodeAnimation = (*it).second;
			joint.transform.translation_ = CalculateValue(rootNodeAnimation.translate.keyframes, animationTime);
			joint.transform.rotationQuat_ = CalculateValue(rootNodeAnimation.rotate.keyframes, animationTime);
			joint.transform.scale_ = CalculateValue(rootNodeAnimation.scale.keyframes, animationTime);
		}
	}
}

void ModelAnimation::Draw(const WorldTransform& transform) {
	/*for (Joint& joint : skeleton_.joints) {
		if (!joint.parent && lines_.empty()) { continue; }
		auto& line = lines_.emplace_back(std::make_unique<Line>());
		int32_t handle = joint.parent.value();
		line->SetLine(skeleton_.joints[handle].skeletonSpaceMatrix.GetPosition(), skeleton_.joints[joint.index].skeletonSpaceMatrix.GetPosition());
	}*/
	int index = 0;
	for (Joint& joint : skeleton_.joints) {
		if (!joint.parent.has_value()) { continue; }
		int32_t handle = joint.parent.value();
		Matrix4x4 mat1 = transform.worldMatrix_ * skeleton_.joints[handle].skeletonSpaceMatrix;
		Matrix4x4 mat2 = transform.worldMatrix_ * skeleton_.joints[joint.index].skeletonSpaceMatrix;
		lines_.at(index++)->SetLine(mat1.GetPosition(), mat2.GetPosition());
	}
}

Skeleton ModelAnimation::CreateSkeleton(const Node& rootNode) {
	Skeleton skeleton;
	skeleton.root = CreateJoint(rootNode, {}, skeleton.joints);
	// 名前とindexのマッピングを行いアクセスしやすくする
	for (const Joint& joint : skeleton.joints) {
		skeleton.jointMap.emplace(joint.name, joint.index);
	}
	return skeleton;
}

int32_t ModelAnimation::CreateJoint(const Node& node, const std::optional<int32_t>& parent, std::vector<Joint>& joints) {
	Joint joint;
	joint.name = node.name;
	joint.localMatrix = node.localMatrix;
	joint.skeletonSpaceMatrix = MakeIdentity4x4();
	joint.transform = node.transform;
	joint.index = int32_t(joints.size()); // 現在登録されている数をindexに
	joint.parent = parent;
	joints.push_back(joint); // SkeletonのJoint列に追加
	for (const Node& child : node.children) {
		// 子Jointを作成し、そおのIndexを登録
		int32_t childIndex = CreateJoint(child, joint.index, joints);
		joints[joint.index].children.push_back(childIndex);
	}
	// 自信のindexを返す
	return joint.index;
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
