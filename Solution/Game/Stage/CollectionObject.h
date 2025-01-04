#pragma once
#include "Game/Particle/DustParticle.h"
#include "Utils/Tool/LevelData.h"
#include "Graphics/Model/Model.h"
#include "Utils/Collider/Collider.h"
#include <memory>

class CollectionObject {
public:
	CollectionObject();
	~CollectionObject() = default;

	void Initialize(LevelData::ObjectData& objectdata);

	bool Update();

	void DrawImGui();

	void Death() { isAnimation_ = true; }
	Collider* GetCollision() const { return collision_.get(); }
	const WorldTransform& GetTransform();

private:
	void Animation();
	void UpdateTransform();

	std::string name_;
	std::shared_ptr<Model> model_;
	WorldTransform transform_;
	WorldTransform collTransform_;
	std::unique_ptr<Collider> collision_;
	std::shared_ptr<DustParticle> dustParticle_;
private:
	bool isAlive_ = true;
	bool isAnimation_ = false;
	float time_ = 0.0f;

};

