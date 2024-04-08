#pragma once
#include <d3d12.h>

#include <string>
#include <memory>
#include <wrl.h>
#include "MPEngine/Base/DetailSetting/DescriptorHandle/DescriptorHandle.h"
#include "MPEngine/Base/Manager/ResourceManager/ResourceManager.h"

class Texture;

// Textureデータ
class Object3d {
public:
	Object3d() = default;
	~Object3d();
public:
	void Load(const std::string& name, const std::string& filePath);

	DescriptorHandle GetHandle() const { return srvHandle_; }

	// ModelDataの取得
	ModelData GetModel() const { return modelData_; }
	Texture* GetTexture() const { return texture_; }

	std::string name_; // modelName
private:
	// モデルデータ構造体
	ModelData modelData_;
	DescriptorHandle srvHandle_;
	Texture* texture_ = nullptr;

};