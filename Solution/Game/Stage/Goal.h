#pragma once
#include "Graphics/Model/Model.h"
#include "Utils/Collider/Collider.h"
#include "Utils/Tool/LevelData.h"

class Goal {
public:
	Goal();
	~Goal() = default;
	void Initialize(const Vector3& scale, const Vector3& rotate, const Vector3& translate);
	void Initialize(LevelData::ObjectData& objectdata);
	Collider* GetCollision() const { return collision_.get(); }
	void Update();
	const WorldTransform& GetTrans();
	void DrawImGui();
private:
	std::unique_ptr<Model> model_;
	WorldTransform transform_;
	WorldTransform collTransform_;
	std::unique_ptr<Collider> collision_;

};