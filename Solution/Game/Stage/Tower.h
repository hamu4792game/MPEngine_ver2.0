#pragma once
#include "Graphics/Model/Model.h"

class Tower {
public:
	Tower() = default;
	~Tower() = default;

	void Initialize(const Vector3& scale, const Vector3& rotate, const Vector3& translate);
	void DrawImGui();

	const WorldTransform& GetTrans() const { return transform_; }
private:
	std::shared_ptr<Model> model_;
	WorldTransform transform_;

};
