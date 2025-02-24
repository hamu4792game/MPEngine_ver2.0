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
#include "MPEngine/Graphics/PostEffect/HSVFilter.h"
#include "MPEngine/Graphics/PostEffect/IntermediateRenderTarget.h"
#include "MPEngine/Graphics/PostEffect/GaussianBlur.h"
#include "MPEngine/Graphics/PostEffect/HighLumi.h"
#include "MPEngine/Base/DetailSetting/RenderTarget/RenderTarget.h"

// MPEngineクラスでのみ実体化
class RenderManager {
public:
	RenderManager() = default;
	~RenderManager() = default;

	void Initialize(class SwapChain* swapchain);
	void Draw();
	void PostDraw(SwapChain* swapchain);
	void HudDraw();

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

	/// <summary>
	/// ポストエフェクト用
	/// </summary>
	/// <param name="texhandle">読み込むテクスチャのhandle</param>
	/// <param name="effect">エフェクト</param>
	/// <param name="frontNum">書き込む中間ファイル</param>
	/// <returns></returns>
	uint32_t ChangeProc(const int& texhandle, BaseEffect* effect,const int& frontNum);

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
	HSVFilter* hsvFilter_ = nullptr;
	GaussianBlur* gaussianBlur_ = nullptr;
	HighLumi* highLumi_ = nullptr;
	std::array<std::unique_ptr<IntermediateRenderTarget>, 2u> intermediateRenderTarget_;

	// 一時避難用のPostEffectRender

};

