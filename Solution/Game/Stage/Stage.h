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
#include "CollectionObject.h"

#include "MPEngine/Utils/Tool/LevelData.h"

class Stage {
public:
	Stage();
	~Stage() = default;

	void Initialize(std::string fileName);
	void initialize();
	void Update();
	void DrawImGui();

	std::list<Collider*> GetCollision() const { return collisionList_; }
	std::list<std::shared_ptr<Ground>> GetGrounds() const;
	std::list<std::shared_ptr<Target>> GetTargets() const;
	const WorldTransform& GetPlayerRespawnPoint() const { return playerRespawnpoint_; }
	const WorldTransform& GetCameraRespawnPoint() const { return cameraTransform_; }
	const uint32_t& GetMaxCollectionNum() const { return maxCollectionNum_; }
	const uint32_t GetCollectionNum() const { return static_cast<uint32_t>(collectionObject_.size()); }

	void LevelLoad(LevelData* data);

	// 衝突時の処理
	void OnCollition(const Collider& coll);

private:
	std::vector<std::shared_ptr<Ground>> boxes_;
	std::vector<std::shared_ptr<Target>> targets_;
	std::unique_ptr<Goal> goal_;
	std::vector<std::shared_ptr<CollectionObject>> collectionObject_;
	std::string itemName_ = "Stage";
	std::list<Collider*> collisionList_;
	WorldTransform playerRespawnpoint_;
	WorldTransform cameraTransform_;


	uint32_t maxCollectionNum_ = 0u;
	bool isCanGoal_ = false; // ゴール可能かどうか

	// 床のためのもの
	struct PlaneGround {
		std::shared_ptr<Model> model_;
		WorldTransform transform_;
	};
	PlaneGround planeGround_;

};