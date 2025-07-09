#pragma once
#include "Utils/Tool/LevelData.h"
#include "Graphics/Model/Model.h"
#include "Utils/Collider/Collider.h"
#include <memory>


class Block {
public:
	Block() = default;
	~Block() = default;

	void Initialize(LevelData::ObjectData& objectdata);

	bool Update();

	void DrawImGui();

	Collider* GetCollision() const { return collision_.get(); }
	const WorldTransform& GetTransform();

private:
	void UpdateTransform();

private:
	std::string name_ = "Block";
	std::shared_ptr<Model> model_;
	WorldTransform transform_;
	WorldTransform collTransform_;
	std::unique_ptr<Collider> collision_;

};
