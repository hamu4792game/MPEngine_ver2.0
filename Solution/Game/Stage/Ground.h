#pragma once
#include "Graphics/Model/Model.h"
#include "Math/AABB.h"

class Ground {
public:
	Ground() = default;
	~Ground() = default;

	//void Initialize();
	void Initialize(const Vector3& scale, const Vector3& rotate, const Vector3& translate);
	AABB* GetCollision() const { return collision_.get(); }
	const WorldTransform& GetTrans() const { return transform_; }
	void DrawImGui();
private:
	std::shared_ptr<Model> model_;
	WorldTransform transform_;
	std::shared_ptr<AABB> collision_;


};
