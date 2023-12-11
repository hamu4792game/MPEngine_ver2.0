#pragma once
//	DirextXtex
#include "externals/DirectXTex/DirectXTex.h"
#include "externals/DirectXTex/d3dx12.h"

#include <string>
#include <memory>
#include <wrl.h>
#include "MPEngine/Math/Vector2.h"

// Textureデータ
class Texture {
public:
	Texture() = default;
	~Texture() = default;
public:
	void Load(const std::string& filePath);

	Vector2 GetTextureSize() const { return textureSize_; }

private:
	Microsoft::WRL::ComPtr<ID3D12Resource> resource_;
	Vector2 textureSize_;
};