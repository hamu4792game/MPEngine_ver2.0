#pragma once
#include "Graphics/Sprite/Sprite.h"
#include <vector>

class GameUI {
public:
	GameUI() = default;
	~GameUI() = default;

	void Initialize(const uint32_t& collectionNum);
	void Update(const uint32_t& collectionNum);

private:
	std::shared_ptr<Sprite> hud_;
	std::vector<std::shared_ptr<Sprite>> collectionObjects_;


};
