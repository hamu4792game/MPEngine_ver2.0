#pragma once
#include <memory>
#include "Graphics/Model/Model.h"

class CircleShadow {
public:
	CircleShadow();
	~CircleShadow() = default;
	void Initialize();
	void Update(const Vector3& hitpoint, const float& distance);

private:
	std::shared_ptr<Model> model_;
	WorldTransform transform_;

};
