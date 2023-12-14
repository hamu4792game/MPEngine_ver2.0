#pragma once
#include "MPEngine/Graphics/SpriteRender/SpriteRender.h"

// MPEngineクラスでのみ実体化
class RenderManager {
public:
	RenderManager() = default;
	~RenderManager() = default;

	void Initialize();
	void Draw();
private:

	SpriteRender spriteRender;

};

