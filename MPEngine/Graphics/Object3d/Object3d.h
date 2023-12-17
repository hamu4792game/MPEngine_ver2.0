#pragma once
#include <d3d12.h>

#include <string>
#include <memory>
#include <wrl.h>
#include "MPEngine/Base/DetailSetting/DescriptorHandle/DescriptorHandle.h"
#include "MPEngine/Graphics/Texture/Texture.h"

// Textureデータ
class Object3d {
public:
	Object3d() = default;
	~Object3d();
public:
	void Load(const std::string& filePath);

	DescriptorHandle GetHandle() const { return srvHandle_; }

	std::string name_; // modelName
private:
	// モデルデータ構造体
	ModelData modelData_;
	Microsoft::WRL::ComPtr<ID3D12Resource> resource_;
	DescriptorHandle srvHandle_;
	std::shared_ptr<Texture> texture_;

};