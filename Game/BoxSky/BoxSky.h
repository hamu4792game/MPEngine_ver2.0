#pragma once
#include "Graphics/Model/Model.h"

class BoxSky {
public:
	BoxSky() = default;
	~BoxSky() = default;

	void Initialize();
private:
	std::shared_ptr<Model> model_;
	WorldTransform transform_;

};
