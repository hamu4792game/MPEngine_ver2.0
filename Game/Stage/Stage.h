#pragma once
#include "MPEngine/Graphics/Model/Model.h"
#include <memory>

class Stage {
public:
	Stage();
	~Stage();

	void Initialize();
	void Update();

private:
	std::shared_ptr<Model> ground_;
};