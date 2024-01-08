#pragma once
#include "MPEngine/Graphics/Model/Model.h"
#include <memory>
#include "Game/Stage/Ground.h"
#include "Game/Stage/Tower.h"
#include <vector>
#include "Math/AABB.h"

class Stage {
public:
	Stage() = default;
	~Stage() = default;

	void Initialize();
	void Update();

	AABB* GetCollision() const { return ground_->GetCollision(); }

private:
	std::shared_ptr<Ground> ground_;
	std::vector<std::shared_ptr<Tower>> towers_;
};