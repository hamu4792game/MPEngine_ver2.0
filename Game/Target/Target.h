#pragma once
#include "Graphics/Model/Model.h"

class Target {
public:
	Target() = default;
	~Target() = default;

	void Initialize(const Vector3& translate);
	
	const WorldTransform& GetTransform() const { return transform_; }
private:
	std::shared_ptr<Model> model_;
	WorldTransform transform_;

};
