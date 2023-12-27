#pragma once
#include "Graphics/Model/Model.h"

class Tower {
public:
	Tower() = default;
	~Tower() = default;

	void Initialize(const Vector3& position);
	void DrawImGui();
private:
	std::shared_ptr<Model> model_;
	WorldTransform transform_;

};
