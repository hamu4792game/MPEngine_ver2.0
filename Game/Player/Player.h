#pragma once
#include "MPEngine/Graphics/Model/Model.h"
#include "Utils/WorldTransform/WorldTransform.h"
#include <memory>

class Player {
public:
	Player() = default;
	~Player() = default;

	void Initialize();
	void Update();
	
	const WorldTransform& GetTransform() const { return transform_; }

private:
	void DrawImGui();
	void Move();

private:
	WorldTransform transform_;
	std::shared_ptr<Model> model_;


};
