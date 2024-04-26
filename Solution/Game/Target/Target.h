#pragma once
#include "Graphics/Model/Model.h"
#include "Graphics/Animation/ModelAnimation.h"

class Target {
public:
	Target() = default;
	~Target() = default;

	void Initialize(const Vector3& translate);
	void DrawImGui();
	void Update();
	
	const WorldTransform& GetTransform() const { return transform_; }

private:
	std::unique_ptr<Model> model_;
	std::unique_ptr<ModelAnimation> animation_;
	WorldTransform transform_;
};
