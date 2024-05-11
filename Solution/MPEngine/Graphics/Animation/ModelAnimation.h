#pragma once
#include <vector>
#include "Math/MathUtl.h"
#include "Math/Quaternion.h"
#include <map>
#include <string>
#include "AnimationData.h"
#include "ResourceManager/ResourceManager.h"
#include "Graphics/Line/Line.h"
#include "MPEngine/Base/DetailSetting/DescriptorHandle/DescriptorHandle.h"
#include "Graphics/Particle/Particle.h"


const uint32_t kNumMaxInfluence = 4;
struct VertexInfluence {
	std::array<float, kNumMaxInfluence> weights;
	std::array<int32_t, kNumMaxInfluence> jointIndices;
};

struct WellForGPU {
	Matrix4x4 skeletonSpaceMatrix; // 位置用
	Matrix4x4 skeletonSpaceInverseTransposeMatrix; // 法線用
};

struct SkinCluster {
	std::vector<Matrix4x4> inverseBindPoseMatrices;
	Microsoft::WRL::ComPtr<ID3D12Resource> influenceResource;
	D3D12_VERTEX_BUFFER_VIEW influenceBufferView;
	std::span<VertexInfluence> mappedInfluence;
	Microsoft::WRL::ComPtr<ID3D12Resource> paletteResource;
	std::span<WellForGPU> mappedPalette;
	//std::pair<D3D12_CPU_DESCRIPTOR_HANDLE, D3D12_GPU_DESCRIPTOR_HANDLE> paletteSrvHandle;
	DescriptorHandle paletteSrvHandle;
	~SkinCluster() {
		if (influenceResource) {
			influenceResource->Release();
			influenceResource.Reset();
		}
		if (paletteResource) {
			paletteResource->Release();
			paletteResource.Reset();
		}
	}
};
class Model;

class ModelAnimation {
	friend class ResourceManager;
	friend class ModelRender;
public:
	ModelAnimation() = default;
	~ModelAnimation();

	enum AnimationType {
		None,
		Rigid,
		Skin,
		kMaxNum
	};

	void Load(const AnimationData* data, const Model* model);
	void Play(const bool& flag);
	//Matrix4x4 Update();
	const Matrix4x4& GetMatrix() const { return localMatrix; }
	void Update(const WorldTransform& transform);
	void ApplyAnimation(float animationTime);
	void Draw(const WorldTransform& transform);

private:
	static Skeleton CreateSkeleton(const Node& rootNode);
	static int32_t CreateJoint(const Node& node, const std::optional<int32_t>& parent, std::vector<Joint>& joints);
	static SkinCluster CreateSkinCluster(const Skeleton& skeleton, const ModelData& modelData);

	void Update(Skeleton& skeleton);
	void Update(SkinCluster& skinCluster, const Skeleton& skeleton);

private:
	// 任意時刻の値を取得する
	Vector3 CalculateValue(const std::vector<Keyframe<Vector3>>& keyframes, float time);
	Quaternion CalculateValue(const std::vector<Keyframe<Quaternion>>& keyframes, float time);
	float animationTime_ = 0.0f;
	const AnimationData* data_ = nullptr;
	const Model* model_ = nullptr;
	Skeleton skeleton_;
	SkinCluster skinCluster_;
	Matrix4x4 localMatrix;
	bool isPlay_ = true;

	std::vector<std::unique_ptr<Model>> joints_;
};
