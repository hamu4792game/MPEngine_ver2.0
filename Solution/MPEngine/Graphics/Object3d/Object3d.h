#pragma once
#include <d3d12.h>

#include <string>
#include <memory>
#include <wrl.h>
#include "MPEngine/Base/DetailSetting/DescriptorHandle/DescriptorHandle.h"
#include "MPEngine/Base/Manager/ResourceManager/ResourceManager.h"
#include <vector>

class Texture;

// Textureデータ
class Object3d {
	friend class ModelRender;
public:
	Object3d() = default;
	~Object3d();
public:
	void Load(const std::string& name, const std::string& filePath);

	DescriptorHandle GetHandle() const { return srvHandle_; }

	// ModelDataの取得
	ModelData GetModel() const { return modelDatas_.front(); }
	std::vector<ModelData> GetModels() const { return modelDatas_; }
	Texture* GetTexture() const { return texture_; }

	std::string name_; // modelName
private:
	void Initialize();
	void CreateVertexResource();
	void UploadVertexData();

private:
	// モデルデータ構造体
	std::vector<ModelData> modelDatas_;
	DescriptorHandle srvHandle_;
	Texture* texture_ = nullptr;

	// 描画必要情報
	std::vector<D3D12_VERTEX_BUFFER_VIEW> vertexBufferView_{};
	std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> vertexResource_;
	std::vector<D3D12_INDEX_BUFFER_VIEW> indexBufferView_{};
	std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> indexResource_;

};