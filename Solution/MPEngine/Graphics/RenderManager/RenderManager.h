#pragma once
#include "MPEngine/Graphics/SpriteRender/SpriteRender.h"
#include "MPEngine/Graphics/ModelRender/ModelRender.h"
#include "MPEngine/Graphics/ParticleRender/ParticleRender.h"
#include "MPEngine/Graphics/LineRender/LineRender.h"
#include "MPEngine/Graphics/SkyBoxRender/SkyBoxRender.h"
#include "Utils/Camera/Camera.h"
#include "MPEngine/Utils/Camera/Camera3d.h"

// MPEngineクラスでのみ実体化
class RenderManager {
public:
	RenderManager() = default;
	~RenderManager() = default;

	void Initialize();
	void Draw();

	enum class PostEffect {
		None,
		Grayscale,
		Sepiatone,
		Vignette,
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
};

