#pragma once
#include "MPEngine/Graphics/Model/Model.h"
#include <memory>
#include "Game/Stage/Ground.h"
#include <vector>
#include "Utils/Collider/Collider.h"
#include <string>
#include <list>
#include "Game/Target/Target.h"
#include "Goal.h"

#include "MPEngine/Utils/Tool/LevelData.h"

class Stage {
public:
	Stage();
	~Stage() = default;

	void Initialize(std::string fileName);
	void Update();
	void DrawImGui();

	std::list<Collider*> GetCollision() const { return collisionList_; }
	std::list<std::shared_ptr<Target>> GetTargets() const;
	const WorldTransform& GetPlayerRespawnPoint() const { return playerRespawnpoint_; }
	const WorldTransform& GetCameraRespawnPoint() const { return cameraTransform_; }

	void LevelLoad(LevelData* data);

private:
	std::vector<std::shared_ptr<Ground>> boxes_;
	std::vector<std::shared_ptr<Target>> targets_;
	std::unique_ptr<Goal> goal_;
	std::string itemName_ = "Stage";
	std::list<Collider*> collisionList_;
	WorldTransform playerRespawnpoint_;
	WorldTransform cameraTransform_;
};