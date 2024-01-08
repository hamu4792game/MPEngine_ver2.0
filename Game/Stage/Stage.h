#pragma once
#include "MPEngine/Graphics/Model/Model.h"
#include <memory>
#include "Game/Stage/Ground.h"
#include <vector>
#include "Math/AABB.h"
#include <string>
#include <list>
#include "Game/Target/Target.h"

class Stage {
public:
	Stage() = default;
	~Stage() = default;

	void Initialize();
	void Update();
	void DrawImGui();

	std::list<AABB*> GetCollision() const { return collisionList_; }
	std::list<std::shared_ptr<Target>> GetTargets() const;


private:
	std::shared_ptr<Ground> ground_;
	std::vector<std::shared_ptr<Ground>> boxes_;
	std::vector<std::shared_ptr<Target>> targets_;
	std::string itemName_ = "Stage";
	std::list<AABB*> collisionList_;
};