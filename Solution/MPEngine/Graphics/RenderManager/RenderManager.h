#pragma once
#include "MPEngine/Graphics/SpriteRender/SpriteRender.h"
#include "MPEngine/Graphics/ModelRender/ModelRender.h"
#include "MPEngine/Graphics/ParticleRender/ParticleRender.h"
#include "MPEngine/Graphics/LineRender/LineRender.h"
#include "MPEngine/Graphics/SkyBoxRender/SkyBoxRender.h"
#include "Utils/Camera/Camera.h"
#include "MPEngine/Utils/Camera/Camera3d.h"
#include "MPEngine/Graphics/PostEffect/RadialBlur.h"
#include "MPEngine/Graphics/PostEffect/Grayscale.h"
#include "MPEngine/Graphics/PostEffect/IntermediateRenderTarget.h"

// MPEngineクラスでのみ実体化
class RenderManager {
public:
	RenderManager() = default;
	~RenderManager() = default;

	void Initialize(class SwapChain* swapchain);
	void Draw(SwapChain* swapchain);
	void PostDraw(SwapChain* swapchain);

	enum class PostEffect {
		None,
		Grayscale,
		Sepiatone,
		Vignette,
		GaussianFilter,
		RadialBlur,
		kMaxNum
	};
	static PostEffect nowEffect;
private:

	SpriteRender spriteRender;
	ModelRender modelRender;
	ParticleRender particleRender;
	LineRender lineRender;
	SkyBoxRender skyBoxRender;
	std::shared_ptr<Camera> camera;
	Camera3d* camera3d_ = nullptr;

	// posteffect
	RadialBlur* radialBlur_ = nullptr;
	Grayscale* grayscale_ = nullptr;
	std::array<std::unique_ptr<IntermediateRenderTarget>, 2u> intermediateRenderTarget_;

};

