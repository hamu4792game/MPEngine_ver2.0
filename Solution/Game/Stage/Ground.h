#pragma once
#include "Graphics/Model/Model.h"
#include "Utils/Collider/Collider.h"
#include "Utils/Tool/LevelData.h"

class Ground {
public:
	Ground() = default;
	~Ground() = default;

	//void Initialize();
	void Initialize(const Vector3& scale, const Vector3& rotate, const Vector3& translate);
	void Initialize(const WorldTransform& transform);
	void Initialize(LevelData::ObjectData& objectdata);
	Collider* GetCollision() const { return collision_.get(); }
	const WorldTransform& GetTrans();
	void DrawImGui();
private:
	std::shared_ptr<Model> model_;
	WorldTransform transform_;
	WorldTransform collTransform_;
	std::unique_ptr<Collider> collision_;


};
