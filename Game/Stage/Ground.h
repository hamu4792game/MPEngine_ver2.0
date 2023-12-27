#pragma once
#include "Graphics/Model/Model.h"

class Ground {
public:
	Ground() = default;
	~Ground() = default;

	void Initialize();
private:
	std::shared_ptr<Model> model_;
	WorldTransform transform_;

};
