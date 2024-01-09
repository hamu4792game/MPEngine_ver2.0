#pragma once
#include <list>
#include "MPEngine/Graphics/Texture/Texture.h"
#include "MPEngine/Graphics/Object3d/Object3d.h"
#include "MPEngine/Base/ConstantBuffer.h"
#include "Utils/WorldTransform/WorldTransform.h"
#include "Base/DetailSetting/GraphicsPipeline/GraphicsPipeline.h"

class Particle {
	friend class ParticleRender;
public:
	Particle();
	~Particle();

	// Textureのセット
	void SetModel(const std::shared_ptr<Object3d>& model, const uint32_t& count);
	void SetTexture(const std::shared_ptr<Texture>& texture) { texture_ = texture; }
	void SetBlendMode(BlendMode type) { blendType_ = type; }
	//void SetColor(const Vector4& color) { color_ = color; }
	//void SetColor(const uint32_t& color) { color_ = ChangeColor(color); }
private:
	void Initialize();
	void CreateVertexResource();
	void CreateInstancingResource();
	void CreateInstancingSRV();
	void UploadVertexData();
	void UploadInstancingData(const Matrix4x4& viewProjectionMat, WorldTransform* transform = nullptr, Vector4* color = nullptr);

public:
	std::vector<WorldTransform> transform_;
	std::vector<Vector4> color_;
	bool isActive_ = true;

private:
	static std::list<Particle*> particleLists_;
	std::shared_ptr<Object3d> model_;
	std::shared_ptr<Texture> texture_;
	BlendMode blendType_ = BlendMode::Normal;

	uint32_t kNumInstance = 1u;
	// Instancing用のリソースを作る
	Microsoft::WRL::ComPtr<ID3D12Resource> instancingResource_ = nullptr;
	DescriptorHandle instancingSrvHandle_;

	// 描画必要情報
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource_ = nullptr;

	// instancing描画に必要な定数バッファをまとめたもの
	struct InstancingStruct {
		Matrix4x4 wvp;
		Vector4 color;
	};
	InstancingStruct instansingData_;


};
