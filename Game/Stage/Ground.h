#pragma once
#include "Graphics/Model/Model.h"
#include "Math/AABB.h"

class Ground {
public:
	Ground() = default;
	~Ground() = default;

	void Initialize();
	AABB* GetCollision() const { return collision_.get(); }
private:
	std::shared_ptr<Model> model_;
	WorldTransform transform_;
	std::shared_ptr<AABB> collision_;

};
