#pragma once
#include <d3d12.h>

#include <string>
#include <memory>
#include <wrl.h>
#include "MPEngine/Math/MathUtl.h"
#include "MPEngine/Base/DetailSetting/DescriptorHandle/DescriptorHandle.h"

// Textureデータ
class Texture {
	friend class Object3d;
public:
	Texture() = default;
	~Texture();
public:
	void Load(const std::string& filePath);

	Vector2 GetTextureSize() const { return textureSize_; }
	DescriptorHandle GetHandle() const { return srvHandle_; }
public:
	std::string name_; // textureName
private:
	Microsoft::WRL::ComPtr<ID3D12Resource> resource_;
	Vector2 textureSize_;
	DescriptorHandle srvHandle_;

};