#pragma once
#include "Graphics/Model/Model.h"
#include "Utils/Collider/Collider.h"
#include "Utils/Tool/LevelData.h"

class Tower {
public:
	Tower() = default;
	~Tower() = default;

	void Initialize(const Vector3& scale, const Vector3& rotate, const Vector3& translate);
	void Initialize(LevelData::ObjectData& objectdata);
	void Set(const float& param) { model_->materials.environmentCoefficient = param; }
	Collider* GetCollision() const { return collision_.get(); }
	const WorldTransform& GetTrans();
	void DrawImGui();

	const WorldTransform& GetTrans() const { return transform_; }
private:
	std::shared_ptr<Model> model_;
	WorldTransform transform_;
	WorldTransform collTransform_;
	std::unique_ptr<Collider> collision_;
};
