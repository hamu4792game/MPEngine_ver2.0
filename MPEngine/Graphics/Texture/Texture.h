#pragma once
#include <d3d12.h>

#include <string>
#include <memory>
#include <wrl.h>
#include "MPEngine/Math/Vector2.h"
#include "MPEngine/Base/DetailSetting/DescriptorHandle/DescriptorHandle.h"

// Textureデータ
class Texture {
public:
	Texture() = default;
	~Texture() = default;
public:
	void Load(const std::string& filePath);

	Vector2 GetTextureSize() const { return textureSize_; }
	DescriptorHandle GetHandle() const { return srvHandle_; }

private:
	Microsoft::WRL::ComPtr<ID3D12Resource> resource_;
	Vector2 textureSize_;
	DescriptorHandle srvHandle_;
	std::string name_; // textureName

	const uint32_t kFirstNumber_ = 20u;


};