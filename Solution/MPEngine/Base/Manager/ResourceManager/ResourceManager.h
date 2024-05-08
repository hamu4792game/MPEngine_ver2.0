#pragma once
//	DirextXtex
#include <d3d12.h>
#include "externals/DirectXTex/DirectXTex.h"

#include "MPEngine/Base/DetailSetting/DescriptorHeap/DescriptorHeap.h"

#include <memory>
#include <unordered_map>
#include <cmath>
#include <string>
#include <vector>
#include <map>
#include <optional>

#include "MPEngine/Math/MathUtl.h"
#include "Math/Quaternion.h"
#include "Utils/WorldTransform/WorldTransform.h"


// 頂点データ構造体
struct VertexData {
	Vector4 position;
	Vector2 texcoord;
	Vector3 normal;
};
// mtl
struct MaterialData {
	std::string textureFilePath;
};
// gltfのnode
struct Node {
	WorldTransform transform;
	Matrix4x4 localMatrix;
	std::string name;
	std::vector<Node> children;
};
// モデルデータ構造体
struct ModelData {
	std::vector<VertexData> vertices;
	MaterialData material;
	Node rootNode;
};

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

struct AnimationData {
	float duration = 0.0f; // アニメーション全体の尺(単位は秒)
	// NodeAnimationの集合。Nodeメイでひけるようにしておく
	std::map<std::string, NodeAnimation> nodeAnimations;
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

class Texture;
class Object3d;
class Audio;
class ModelAnimation;

class ResourceManager {
private:
	ResourceManager() = default;
	~ResourceManager() = default;

public:
	static ResourceManager* GetInstance();
	void Initialize();
	void Finalize();

private:
	std::unique_ptr<DescriptorHeap> srvDescriptorHeap_;
	const uint32_t kDescriptorHeapSize_ = 4096u;

	std::unordered_map<std::string, std::unique_ptr<Texture>> textureContainer_; // textureを纏めたコンテナ
	std::unordered_map<std::string, std::unique_ptr<Object3d>> object3dContainer_; // modelDataを纏めたコンテナ
	std::unordered_map<std::string, std::unique_ptr<Audio>> audioContainer_; // audioを纏めたコンテナ
	std::unordered_map<std::string, AnimationData> animationContainer_; // animationDataを纏めたコンテナ

	uint32_t textureCount_ = 10u; // 今のテクスチャが追加された数
public: // 取得関数
	DescriptorHeap* const GetSRVHeap() { return srvDescriptorHeap_.get(); }
	Texture* FindTexture(const std::string& name) const { return textureContainer_.at(name).get(); }
	Object3d* FindObject3d(const std::string& name) const { return object3dContainer_.at(name).get(); }
	Audio* FindAudio(const std::string& name) const { return audioContainer_.at(name).get(); }
	AnimationData FindAnimation(const std::string& name) const { return animationContainer_.at(name); }

public: // 追加関数
	void AddTexture(const std::string& name, const std::string& fileName);
	void AddModel(const std::string& name, const std::string& fileName);
	void AddAudio(const std::string& name, const std::string& fileName);
	void AddAnimation(const std::string& name, const std::string& fileName);

	const uint32_t GetCount(); // 次のtexture番号を割り当て

public:
	// DirectX12のTextureResourceを作る
	DirectX::ScratchImage LoadTexture(const std::string& filePath);
	// TextureResourceにデータを転送する
	void UploadTextureData(ID3D12Resource* texture, const DirectX::ScratchImage& mipImages);
	ID3D12Resource* UploadTextureData(ID3D12Resource* texture, const DirectX::ScratchImage& mipImages, ID3D12Device* device, ID3D12GraphicsCommandList* commandList);
	// Resourceを生成してreturnする
	ID3D12Resource* CreateTextureResource(ID3D12Device* device, const DirectX::TexMetadata& metadata);
	// BufferResourceを作る関数
	ID3D12Resource* CreateBufferResource(ID3D12Device* device, size_t sizeInBytes);
	// RenderTextureを作る関数
	ID3D12Resource* CreateRenderTextureResource(ID3D12Device* device, D3D12_RESOURCE_DESC resDesc, DXGI_FORMAT format, const Vector4& color);
	// モデルファイルを読み込む関数
	ModelData LoadModelFile(const std::string& filename);
	// アニメーションファイルを読み込む関数
	AnimationData LoadAnimationFile(const std::string& filename);
private:
	MaterialData LoadMaterialTemplateFile(const std::string& filename);
	Node ReadNode(struct aiNode* node);
	Skeleton CreateSkeleton(const Node& rootNode);
	int32_t CreateJoint(const Node& node, const std::optional<int32_t>& parent, std::vector<Joint>& joints);
	

};
