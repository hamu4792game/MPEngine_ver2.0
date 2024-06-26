#pragma once
#include "Math/MathUtl.h"
#include "Utils/Tool/LevelData.h"
#include "Graphics/Model/Model.h"
#include "Utils/Collider/Collider.h"

class GameObject {
public:
	GameObject();
	~GameObject();

	// レベルエディタを使った場合の初期化
	virtual void Initialize(LevelData::ObjectData& objectdata);
	Collider* GetCollision() const { return collision_.get(); }
	const WorldTransform& GetTrans();

private:
	std::unique_ptr<Model> model_;
	WorldTransform transform_;
	std::unique_ptr<Collider> collision_;
	WorldTransform collTransform_;

};
