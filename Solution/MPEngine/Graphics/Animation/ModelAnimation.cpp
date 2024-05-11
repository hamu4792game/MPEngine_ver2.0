#include "ModelAnimation.h"
#include "Graphics/Model/Model.h"
#include "ResourceManager/ResourceManager.h"
#include "DeviceManager/DeviceManager.h"
#include <algorithm>

ModelAnimation::~ModelAnimation() {
	
}

void ModelAnimation::Load(const AnimationData* data, const Model* model) {
	data_ = data;
	model_ = model;
	skeleton_ = CreateSkeleton(model_->model_->GetModel().rootNode);
	/*for (Joint& joint : skeleton_.joints) {
		if (!joint.parent && lines_.empty()) { continue; }
		auto& line = lines_.emplace_back(std::make_unique<Line>());
	}*/
	skinCluster_ = CreateSkinCluster(skeleton_, model_->model_->GetModel());

	joints_.resize(skeleton_.joints.size());
	for (auto& joint : joints_) {
		joint = std::make_unique<Model>();
		joint->SetModel(ResourceManager::GetInstance()->FindObject3d("Box"));
		joint->SetTexture(ResourceManager::GetInstance()->FindTexture("UVChecker"));
		joint->transform_.scale_ = Vector3(0.1f, 0.1f, 0.1f);
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
	Update(skeleton_);
	Update(skinCluster_, skeleton_);
	transform;
	Draw(transform);
}

void ModelAnimation::ApplyAnimation(float animationTime) {
	animationTime = std::fmod(animationTime, data_->duration);
	int index = 0;
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
		// JointのWorldMatrixは skeletonSpaceMatrix * worldMatrix のため、変換色はこうなる
		Matrix4x4 mat1 = skeleton_.joints[handle].skeletonSpaceMatrix * transform.worldMatrix_;
		//Matrix4x4 mat2 = skeleton_.joints[joint.index].skeletonSpaceMatrix * transform.worldMatrix_;
		//lines_.at(index++)->SetLine(mat1.GetPosition(), mat2.GetPosition());
		joints_.at(index++)->transform_.parent_ = &transform;
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
		// 子Jointを作成し、そのIndexを登録
		int32_t childIndex = CreateJoint(child, joint.index, joints);
		joints[joint.index].children.push_back(childIndex);
	}
	// 自信のindexを返す
	return joint.index;
}

SkinCluster ModelAnimation::CreateSkinCluster(const Skeleton& skeleton, const ModelData& modelData) {
	SkinCluster skinCluster;
	auto rsManager = ResourceManager::GetInstance();

	// paletteのresourceを確保
	skinCluster.paletteResource = rsManager->CreateBufferResource(DeviceManager::GetInstance()->GetDevice(), sizeof(WellForGPU) * skeleton.joints.size());
	WellForGPU* mappedPalette = nullptr;
	skinCluster.paletteResource->Map(0, nullptr, reinterpret_cast<void**>(&mappedPalette));
	skinCluster.mappedPalette = { mappedPalette,skeleton.joints.size() };
	//skinCluster.paletteSrvHandle.first = rsManager->GetSRVHeap()->GetCPUDescriptorHandle(rsManager->GetCount());
	//skinCluster.paletteSrvHandle.second = rsManager->GetSRVHeap()->GetGPUDescriptorHandle(rsManager->GetCount());
	skinCluster.paletteSrvHandle.CreateView(rsManager->GetSRVHeap(), rsManager->GetCount());

	// palette用のsrvを作成
	D3D12_SHADER_RESOURCE_VIEW_DESC paletteSrvDesc{};
	paletteSrvDesc.Format = DXGI_FORMAT_UNKNOWN;
	paletteSrvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	paletteSrvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	paletteSrvDesc.Buffer.FirstElement = 0;
	paletteSrvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
	paletteSrvDesc.Buffer.NumElements = UINT(skeleton.joints.size());
	paletteSrvDesc.Buffer.StructureByteStride = sizeof(WellForGPU);
	DeviceManager::GetInstance()->GetDevice()->CreateShaderResourceView(skinCluster.paletteResource.Get(), &paletteSrvDesc, skinCluster.paletteSrvHandle.GetCPU());

	// influence用のresourceを確保
	skinCluster.influenceResource = rsManager->CreateBufferResource(DeviceManager::GetInstance()->GetDevice(), sizeof(VertexInfluence) * modelData.vertices.size());
	VertexInfluence* mappedInfluence = nullptr;
	skinCluster.influenceResource->Map(0, nullptr, reinterpret_cast<void**>(&mappedInfluence));
	std::memset(mappedInfluence, 0, sizeof(VertexInfluence) * modelData.vertices.size());
	skinCluster.mappedInfluence = { mappedInfluence,modelData.vertices.size() };

	// influence用のVBVを作成
	skinCluster.influenceBufferView.BufferLocation = skinCluster.influenceResource->GetGPUVirtualAddress();
	skinCluster.influenceBufferView.SizeInBytes = UINT(sizeof(VertexInfluence) * modelData.vertices.size());
	skinCluster.influenceBufferView.StrideInBytes = sizeof(VertexInfluence);

	// InverseBindPoseMatrixの保存領域を作成
	skinCluster.inverseBindPoseMatrices.resize(skeleton.joints.size());
	//std::generate(skinCluster.inverseBindPoseMatrices.begin(), skinCluster.inverseBindPoseMatrices.end(), MakeIdentity4x4());
	for (auto& matrix : skinCluster.inverseBindPoseMatrices) {
		matrix = MakeIdentity4x4();
	}

	// ModelDataのskinCluster情報を解析してInfluenceの中身を埋める
	for (const auto& jointWeight : modelData.skinClusterData) { // ModelのSkinCluesterの情報を解析
		auto it = skeleton.jointMap.find(jointWeight.first); // JointWeight.firstはjoint名なので、skeletonに対象となるjointが含まれるか判断
		if (it == skeleton.jointMap.end()) {
			continue;
		}
		// (*it).secondにはjointのindexが入っているので、街頭のindexのinverseBindPoseMatrixを代入
		skinCluster.inverseBindPoseMatrices[(*it).second] = jointWeight.second.inverseBindPoseMatrix;
		for (const auto& vertexWeight : jointWeight.second.vertexWeights) {
			auto& currentInfluence = skinCluster.mappedInfluence[vertexWeight.vertexIndex]; // 街頭のvertexInfluence情報を参照しておく
			for (uint32_t index = 0; index < kNumMaxInfluence; index++) {
				if (currentInfluence.weights[index] == 0.0f) {
					currentInfluence.weights[index] = vertexWeight.weight;
					currentInfluence.jointIndices[index] = (*it).second;
					break;
				}
			}
		}
	}

	return skinCluster;
}

void ModelAnimation::Update(Skeleton& skeleton) {
	// 全てのJointを更新、親が若いので通常ループで処理可能になっている
	for (Joint& joint : skeleton_.joints) {
		joint.transform.isQuaternion_ = true;
		//joint.localMatrix = joint.transform.UpdateMatrix();
		joint.localMatrix = MakeAffineMatrix(joint.transform.scale_, joint.transform.rotationQuat_, joint.transform.translation_);
		if (joint.parent) { /// 親がいれば親の行列を抜ける
			joint.skeletonSpaceMatrix = joint.localMatrix * skeleton_.joints[*joint.parent].skeletonSpaceMatrix;
		}
		else { // 親がいないのでlocalMatrixとskeletonSpaceMatrixは一致する
			joint.skeletonSpaceMatrix = joint.localMatrix;
		}
	}
}

void ModelAnimation::Update(SkinCluster& skinCluster, const Skeleton& skeleton) {
	for (size_t jointIndex = 0; jointIndex < skeleton.joints.size(); jointIndex++) {
		assert(jointIndex < skinCluster.inverseBindPoseMatrices.size());
		skinCluster.mappedPalette[jointIndex].skeletonSpaceMatrix =
			skinCluster.inverseBindPoseMatrices[jointIndex] * skeleton.joints[jointIndex].skeletonSpaceMatrix;
		skinCluster.mappedPalette[jointIndex].skeletonSpaceInverseTransposeMatrix =
		Transpose(Inverse(skinCluster.mappedPalette[jointIndex].skeletonSpaceMatrix));
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
