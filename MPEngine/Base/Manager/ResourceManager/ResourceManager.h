#pragma once
//	DirextXtex
#include "externals/DirectXTex/DirectXTex.h"
#include "externals/DirectXTex/d3dx12.h"

#include "MPEngine/Base/DetailSetting/DescriptorHeap/DescriptorHeap.h"

#include <memory>
#include <map>
#include <cmath>
#include <string>
#include <vector>

#include "MPEngine/Math/MathUtl.h"

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
// モデルデータ構造体
struct ModelData {
	std::vector<VertexData> vertices;
	MaterialData material;
};

class Texture;
class Object3d;

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

	std::map<std::string, std::shared_ptr<Texture>> textureContainer_; // textureを纏めたコンテナ
	std::map<std::string, std::shared_ptr<Object3d>> object3dContainer_; // modelDataを纏めたコンテナ

	uint32_t textureCount_ = 10u; // 今のテクスチャが追加された数
public: // 取得関数
	DescriptorHeap* const GetSRVHeap() { return srvDescriptorHeap_.get(); }
	std::shared_ptr<Texture> FindTexture(const std::string& name) const { 
		return textureContainer_.at(name); }
	std::shared_ptr<Object3d> FindObject3d(const std::string& name) const { return object3dContainer_.at(name); }

public: // 追加関数
	void AddTexture(const std::string& name, const std::shared_ptr<Texture>& texture);
	void AddModel(const std::string& name, const std::shared_ptr<Object3d>& model);

	const uint32_t GetCount(); // 次のtexture番号を割り当て

public:
	// DirectX12のTextureResourceを作る
	DirectX::ScratchImage LoadTexture(const std::string& filePath);
	// TextureResourceにデータを転送する
	void UploadTextureData(ID3D12Resource* texture, const DirectX::ScratchImage& mipImages);
	// Resourceを生成してreturnする
	ID3D12Resource* CreateTextureResource(ID3D12Device* device, const DirectX::TexMetadata& metadata);
	// BufferResourceを作る関数
	ID3D12Resource* CreateBufferResource(ID3D12Device* device, size_t sizeInBytes);

	// オブジェクトファイルを読み込む関数
	ModelData LoadObjFile(const std::string& filename);
private:
	MaterialData LoadMaterialTemplateFile(const std::string& filename);

};
