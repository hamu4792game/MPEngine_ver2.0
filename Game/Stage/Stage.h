#pragma once
#include "MPEngine/Graphics/Model/Model.h"
#include <memory>
#include "Game/Stage/Ground.h"
#include "Game/Stage/Tower.h"
#include <vector>

class Stage {
public:
	Stage() = default;
	~Stage() = default;

	void Initialize();
	void Update();

private:
	std::shared_ptr<Ground> ground_;
	std::vector<std::shared_ptr<Tower>> towers_;
};