#pragma once
#include "Graphics/Model/Model.h"
#include "Utils/Collider/Collider.h"
#include "Utils/Tool/LevelData.h"

class ObjectFrame {
public:
	ObjectFrame() = default;
	~ObjectFrame() = default;

	virtual void Initialize(LevelData::ObjectData& objectdata);

	Collider* GetCollision() const { return collision_.get(); }
	const WorldTransform& GetTransform();
	virtual void DrawImGui() = 0;
protected:
	virtual void UpdateTransform();

	std::string name_;
	std::shared_ptr<Model> model_;
	WorldTransform transform_;
	WorldTransform collTransform_;
	std::unique_ptr<Collider> collision_;

};